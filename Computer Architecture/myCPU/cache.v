module cache
(
    input            clk_g    ,
    input            resetn   ,
    //cache and CPU
    input            valid    ,
    input            op       ,
    input  [ 7:0]    index    ,
    input  [19:0]    tag      ,
    input  [ 3:0]    offset   ,
    input  [ 3:0]    wstrb    ,
    input  [31:0]    wdata    ,

    output           addr_ok  ,
    output           data_ok  ,
    output [31:0]    rdata    ,

    //cache and axi
    output           rd_req   ,
    output [ 2:0]    rd_type  ,
    output [31:0]    rd_addr  ,
    input            rd_rdy   ,
    input            ret_valid,
    input            ret_last ,
    input  [31:0]    ret_data ,

    output           wr_req   ,
    output  [ 2:0]   wr_type  ,
    output  [31:0]   wr_addr  ,
    output  [ 3:0]   wr_wstrb ,
    output  [127:0]  wr_data  ,
    input            wr_rdy  
);
`define  IDLE    6'b00001
`define  LOOKUP  6'b00010
`define  MISS    6'b00100
`define  REPLACE 6'b01000
`define  REFILL  6'b10000
`define  WRITE   2'b10
`define  IDLE_W  2'b01

//data path d
reg  [255 : 0] way0_d;
reg  [255 : 0] way1_d;
//data path tagv and bank
wire [7   : 0] d_waddr;
wire [7   : 0] d_raddr;
wire           d0_wen;
wire           d1_wen;
wire           d0_rdata;
wire           d1_rdata;
wire           d_wdata;

wire [7   : 0] tagv_addr;
wire           tagv0_wen;
wire           tagv1_wen;
wire [20  : 0] tagv_wdata;
wire [20  : 0] tagv0_rdata;
wire [20  : 0] tagv1_rdata;

wire [3   : 0] bank0_wstrb;
wire [3   : 0] bank1_wstrb;
wire [1   : 0] CS;
wire [31  : 0] bank_wdata;
wire [7   : 0] bank_addr;

wire [31  : 0] way0_bank0_rdata;
wire [31  : 0] way0_bank1_rdata;
wire [31  : 0] way0_bank2_rdata;
wire [31  : 0] way0_bank3_rdata;
wire [31  : 0] way1_bank0_rdata;
wire [31  : 0] way1_bank1_rdata;
wire [31  : 0] way1_bank2_rdata;
wire [31  : 0] way1_bank3_rdata;

// state machine
reg  [ 4 : 0]state_main;
wire [ 4 : 0]next_state_main;
reg  [ 1 : 0]state_write_buffer;
wire [ 1 : 0]state_write_buffer_next;

//tag compare
wire way0_hit;
wire way1_hit;
wire cache_hit;
wire way0_v;
wire way1_v;
wire [19 : 0] way0_tag;
wire [19 : 0] way1_tag;

//Request Buffer
reg             reg_op;
reg  [ 7:0]     reg_index;
reg  [19:0]     reg_tag;
reg  [ 3:0]     reg_offset;
reg  [ 3:0]     reg_wstrb;
reg  [31:0]     reg_wdata;

//LFSR
reg [7 : 0] LFSR;
wire [7 : 0] LFSR_next;
wire         feedback;

//data select
wire [31  : 0]way0_load_word;
wire [31  : 0]way1_load_word;
wire [31  : 0]load_res;
wire [127 : 0] replace_data;
wire [127 : 0] way0_data;
wire [127 : 0] way1_data;

//miss buffer
reg replace_way_r;
reg [1:0]axi_num;
reg last;

//write buffer
reg [  7: 0]   write_buffer_index;
reg            write_buffer_way;
reg [  3: 0]   write_buffer_wstrb;
reg [ 31: 0]   write_buffer_wdata;
reg [  3: 0]   write_buffer_offset;

wire hit_write_conflict;
wire hit_write_conflict_exp;
reg wr_req_r;
wire hit_write;
wire v_d0;
wire v_d1;
wire [3 : 0]w0b0_we;
wire [3 : 0]w0b1_we;
wire [3 : 0]w0b2_we;
wire [3 : 0]w0b3_we;
wire [3 : 0]w1b1_we;
wire [3 : 0]w1b2_we;
wire [3 : 0]w1b3_we;
wire [3 : 0]w1b0_we;

wire [31: 0]ret_data1;

//ram implemention
tagv way0_tagv (
    .clka (clk_g     ),
    .wea  (tagv0_wen   ),
    .addra(tagv_addr ),
    .dina (tagv_wdata),
    .douta(tagv0_rdata )
);

tagv way1_tagv (
    .clka (clk_g     ),
    .wea  (tagv1_wen   ),
    .addra(tagv_addr  ),
    .dina (tagv_wdata),
    .douta(tagv1_rdata ) 
);

bank way0_bank0 (
    .clka (clk_g     ),
    .wea  (w0b0_we),
    .addra(bank_addr  ),
    .dina (bank_wdata ),
    .douta(way0_bank0_rdata )
);
bank way0_bank1 (
    .clka (clk_g     ),
    .wea  (w0b1_we),
    .addra(bank_addr  ),
    .dina (bank_wdata ),
    .douta(way0_bank1_rdata )
);
bank way0_bank2 (
    .clka (clk_g     ),
    .wea  (w0b2_we),
    .addra(bank_addr  ),
    .dina (bank_wdata ),
    .douta(way0_bank2_rdata ) 
);
bank way0_bank3 (
    .clka (clk_g     ),
    .wea  (w0b3_we),
    .addra(bank_addr ),
    .dina (bank_wdata ),
    .douta(way0_bank3_rdata )
);

bank way1_bank0 (
    .clka (clk_g     ),
    .wea  (w1b0_we),
    .addra(bank_addr ),
    .dina (bank_wdata ),
    .douta(way1_bank0_rdata ) 
);
bank way1_bank1 (
    .clka (clk_g     ),
    .wea  (w1b1_we),
    .addra(bank_addr),
    .dina (bank_wdata ),
    .douta(way1_bank1_rdata )
);
bank way1_bank2 (
    .clka (clk_g     ),
    .wea  (w1b2_we),
    .addra(bank_addr  ),
    .dina (bank_wdata ),
    .douta(way1_bank2_rdata) 
);
bank way1_bank3 (
    .clka (clk_g     ),
    .wea  (w1b3_we),
    .addra(bank_addr    ),
    .dina (bank_wdata ),
    .douta(way1_bank3_rdata )
);

//d write & read
always@(posedge clk_g)begin
    if(~resetn)
        way0_d<=0;
    else if(d0_wen)
        way0_d[d_waddr]<=d_wdata;
end

always@(posedge clk_g)begin
    if(~resetn)
        way1_d<=0;
    else if(d1_wen)
        way1_d[d_waddr]<=d_wdata;
end

assign d0_rdata = way0_d[d_raddr];
assign d1_rdata = way1_d[d_raddr];

//request buffer

always @ (posedge clk_g)begin
    if((state_main == `IDLE && valid && !hit_write_conflict_exp) || (state_main == `LOOKUP && cache_hit && valid && !hit_write_conflict_exp))begin//++
        reg_op <= op;
        reg_index <= index;
        reg_tag <= tag;
        reg_offset <= offset;
        reg_wstrb <= wstrb;
        reg_wdata <= wdata;
    end
end

//Tag Compare
assign way0_v = tagv0_rdata[0];
assign way1_v = tagv1_rdata[0];
assign way0_tag = tagv0_rdata[20:1];
assign way1_tag = tagv1_rdata[20:1];
    //valid & dirty used in replace
assign v_d0 = way0_v & d0_rdata;
assign v_d1 = way1_v & d1_rdata;

assign way0_hit = way0_v && (way0_tag == reg_tag);
assign way1_hit = way1_v && (way1_tag == reg_tag);
assign cache_hit = way0_hit || way1_hit;

//LFSR
always @ (posedge clk_g)begin
    if(~resetn)begin
        LFSR <= 8'b00000000;
    end else
        LFSR <= LFSR_next;
end

assign feedback     = LFSR[7]^(~|LFSR[6:0]);
assign LFSR_next[7] = LFSR[6];
assign LFSR_next[6] = LFSR[5];
assign LFSR_next[5] = LFSR[4];
assign LFSR_next[4] = LFSR[3]^feedback;
assign LFSR_next[3] = LFSR[2]^feedback;
assign LFSR_next[2] = LFSR[1]^feedback;
assign LFSR_next[1] = LFSR[0];
assign LFSR_next[0] = feedback;

//Data Select
assign way0_data = {way0_bank3_rdata,way0_bank2_rdata, way0_bank1_rdata,way0_bank0_rdata};
assign way1_data = {way1_bank3_rdata,way1_bank2_rdata, way1_bank1_rdata,way1_bank0_rdata};

assign way0_load_word = way0_data[reg_offset[3:2]*32 +: 32];
assign way1_load_word = way1_data[reg_offset[3:2]*32 +: 32];
assign load_res = {32{way0_hit}} & way0_load_word | {32{way1_hit}} & way1_load_word; // return to rdata if hit

assign replace_data = replace_way_r ? way1_data : way0_data; //data write to axi

//Miss Buffer
always @(posedge clk_g)begin
    if(state_main == `MISS && wr_rdy)begin 
        replace_way_r <= LFSR % 2;
    end
end

always @(posedge clk_g)begin
    if(state_main == `REPLACE && rd_rdy) // work in refill
        axi_num <= 0;
    else if(ret_valid)
        axi_num <= axi_num + 1;
end

//Write Buffer
always @ (posedge clk_g)begin
    if(state_main == `LOOKUP && reg_op && cache_hit)begin
        write_buffer_index <= reg_index;
        write_buffer_way   <= way1_hit; 
        write_buffer_wstrb <= reg_wstrb;
        write_buffer_wdata <= reg_wdata;
        write_buffer_offset <= reg_offset;
    end
end

// main machine
always @ (posedge clk_g)begin
    if(~resetn)begin
        state_main <= `IDLE;
    end else begin
        state_main <= next_state_main;
    end
end

assign hit_write_conflict = ((state_main == `LOOKUP) && valid && !op && reg_op && cache_hit && reg_offset[3:2]==offset[3:2]) 
                          | (state_write_buffer == `WRITE && valid && !op && write_buffer_offset[3:2]==offset[3:2]);                 
assign hit_write_conflict_exp = ((state_main == `LOOKUP) && valid && !op && reg_op && reg_offset[3:2]==offset[3:2]) 
                          | (state_write_buffer == `WRITE && valid && !op && write_buffer_offset[3:2]==offset[3:2]);

assign next_state_main =state_main == `IDLE && (!valid || hit_write_conflict_exp) ? `IDLE:
                        state_main == `IDLE && valid && !hit_write_conflict_exp   ? `LOOKUP:
                        state_main == `LOOKUP && ((!valid && cache_hit) ||  hit_write_conflict_exp)  ? `IDLE:
                        state_main == `LOOKUP && cache_hit && valid && !hit_write_conflict_exp ? `LOOKUP:
                        state_main == `LOOKUP && !cache_hit ? `MISS:
                        state_main == `MISS && !wr_rdy ? `MISS:
                        state_main == `MISS && wr_rdy ? `REPLACE:
                        state_main == `REPLACE && !rd_rdy ? `REPLACE:
                        state_main == `REPLACE && rd_rdy ? `REFILL:
                        state_main == `REFILL && !ret_last && ret_valid ? `REFILL:
                        state_main == `REFILL && ret_last && ret_valid ? `IDLE : `IDLE;

always @(posedge clk_g)begin
    if(~resetn)begin
        state_write_buffer <= `IDLE;
    end
    else begin
        state_write_buffer <= state_write_buffer_next;
    end
end

assign hit_write = (state_main == `LOOKUP) && reg_op && cache_hit;

assign state_write_buffer_next =state_write_buffer == `IDLE_W && !hit_write ? `IDLE_W:
                                state_write_buffer == `IDLE_W && hit_write ? `WRITE:
                                state_write_buffer == `WRITE && hit_write ? `WRITE:   
                                state_write_buffer == `WRITE && !hit_write ? `IDLE_W : `IDLE_W; 

//cache input
    //tagv
assign tagv_addr  = ({8{state_main == `IDLE}}  & index)//read
                   |({8{state_main ==`LOOKUP}} & reg_index)//read
                   |({8{state_main ==`MISS}} & reg_index)//read
                   |({8{state_main == `REPLACE}} & reg_index)//read
                   |({8{state_main == `REFILL}}  & reg_index);
assign tagv0_wen =  (state_main == `REFILL)  & (replace_way_r == 0);
assign tagv1_wen =  (state_main == `REFILL)  & (replace_way_r == 1);
assign tagv_wdata = ({21{state_main == `REFILL}}  & {reg_tag,1'b1}); 

    //d
assign d_waddr = ({8{state_write_buffer == `WRITE}}  & write_buffer_index)
               |({8{state_main == `REFILL}}  & reg_index);
assign d_raddr = {8{state_main == `REPLACE}} & reg_index;
assign d0_wen = ((state_write_buffer == `WRITE && write_buffer_way == 0) & (|write_buffer_wstrb))
              |((state_main == `REFILL ) & replace_way_r == 0);
assign d1_wen = ((state_write_buffer == `WRITE && write_buffer_way == 1) & (|write_buffer_wstrb))
              |((state_main == `REFILL ) & replace_way_r == 1);
assign d_wdata = ((state_write_buffer == `WRITE) & 1'b1)
               |((state_main == `REFILL) &  reg_op); // write d=1 ; otherwise d=0

    //ban
assign bank_addr = ({8{state_main == `IDLE}} & index)//read
                  |({8{state_main == `LOOKUP}} & reg_index)//read
                  |({8{state_main == `MISS}} & reg_index)//read
                  |({8{state_write_buffer == `WRITE}} &  write_buffer_index)
                  |({8{state_write_buffer == `REPLACE}} & reg_index)//read
                  |({8{state_main == `REFILL}} & reg_index);

assign bank0_wstrb =({4{state_write_buffer == `WRITE && write_buffer_way == 0}} & write_buffer_wstrb)
                   |({4{state_main == `REFILL && replace_way_r == 0}} & 4'b1111);

assign bank1_wstrb =({4{state_write_buffer == `WRITE && write_buffer_way == 1}} & write_buffer_wstrb)
                   |({4{state_main == `REFILL && replace_way_r == 1}} & 4'b1111);
assign CS =({4{state_write_buffer == `WRITE }} & write_buffer_offset[3:2])
           |({4{state_main == `REFILL }} & axi_num); //decided by axinum
           
assign w0b0_we = bank0_wstrb & {4{CS == 0}};
assign w0b1_we = bank0_wstrb & {4{CS == 1}};
assign w0b2_we = bank0_wstrb & {4{CS == 2}};
assign w0b3_we = bank0_wstrb & {4{CS == 3}};
assign w1b0_we = bank1_wstrb & {4{CS == 0}};
assign w1b1_we = bank1_wstrb & {4{CS == 1}};
assign w1b2_we = bank1_wstrb & {4{CS == 2}};
assign w1b3_we = bank1_wstrb & {4{CS == 3}};
                           
assign bank_wdata = ({32{state_write_buffer == `WRITE}} & write_buffer_wdata)
                   |({32{state_main == `REFILL}} & ret_data1);
assign ret_data1 = (axi_num == reg_offset[3:2] && reg_op ) ? reg_wdata : ret_data;


assign addr_ok = (state_main == `IDLE) 
               | (state_main == `LOOKUP && ((cache_hit && op == 1) || (cache_hit && op == 0 && !hit_write_conflict_exp)));
assign data_ok = (state_main == `LOOKUP && cache_hit)
                |(state_main == `LOOKUP && reg_op == 1)
                |(state_main ==`REFILL && ret_valid ==1 && axi_num == reg_offset[3:2] && !reg_op);
assign rdata = cache_hit ? load_res : ret_data;

assign rd_req = (state_main == `REPLACE);

always @ (posedge clk_g)begin
    if(~resetn)
        wr_req_r <= 0; 
    else if(state_main == `MISS && wr_rdy)
        wr_req_r <= 1;
    else if(wr_rdy == 1)
        wr_req_r <= 0;
end

//read from axi : do in REFILL
assign rd_addr = {reg_tag,reg_index,reg_offset};
assign rd_type = 3'b100;//word or row
//write to axi: do in REPLACE
assign wr_req = (|wr_wstrb) & wr_req_r;
assign wr_type = 3'b100;
assign wr_addr = replace_way_r ? {way1_tag,reg_index,4'b0000} : {way0_tag,reg_index,4'b0000};
assign wr_wstrb = {4{((v_d0 && !replace_way_r) || (v_d1 && replace_way_r)) && state_main == `REPLACE}} & 4'b1111;
assign wr_data = replace_data ;

endmodule