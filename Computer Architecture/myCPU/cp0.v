module cp0(
    input             clk,
    input           reset,

    input [31:0] c0_wdata    ,//wtc0写数据
    output[31:0] c0_rdata    ,//wfc0读数据
    output[31:0] epc         ,//恒读epc
    output[31:0] status      ,//恒读status
    output[31:0] cause       ,//恒读cause
    input [ 4:0] c0_waddr    ,//wtc0写寄存器号
    input [ 4:0] c0_raddr    ,//wfc0读寄存器号
    input        mtc0_we     ,//wtc0写使能信号
    input        eret_flush  ,
    input        wb_ex       ,//wb发生例外
    input        wb_bd       ,//wb指令是否位于延迟槽
    input [31:0] wb_badvaddr ,//例外错误地址
    input [4: 0] wb_excode   ,//写回级所报例外的编码  
    input [31:0] wb_pc       ,
    input        tlbp_we     ,
    input        tlbr_we     ,
    input [31:0] index       ,
    input [31:0] entryhi     ,
    input [31:0] entrylo0    ,
    input [31:0] entrylo1    ,
    input        tlb_ex      ,
    output[ 3:0] index_index ,
    output       index_p     ,
    output[18:0] entryhi_vpn2,
    output[ 7:0] entryhi_asid,
    output[19:0] entrylo0_pfn0,
    output[ 2:0] entrylo0_c0 ,
    output       entrylo0_v0 ,
    output       entrylo0_d0 ,
    output       entrylo0_g0 ,
    output[19:0] entrylo1_pfn1,
    output[ 2:0] entrylo1_c1 ,
    output       entrylo1_v1 ,
    output       entrylo1_d1 ,
    output       entrylo1_g1 
);

reg [31:0]cp0[31:0];//协处理器

//STATUS
reg       c0_status_bev;
reg [7:0] c0_status_im;
reg       c0_status_exl;
reg       c0_status_ie;

/*change the STATUS*/
always @(posedge clk)begin
    c0_status_bev <= 1'b1;
end

always @(posedge clk) begin
    if (mtc0_we && c0_waddr== `CP0_STATUS)begin
        c0_status_im <= c0_wdata[15:8];
    end
end

always @(posedge clk) begin
    if (reset)begin
        c0_status_exl <= 1'b0;
    end else if (wb_ex)begin  //SYSCALL
        c0_status_exl <= 1'b1;
    end else if (eret_flush)begin //ERET
        c0_status_exl <= 1'b0;
    end else if (mtc0_we && c0_waddr==`CP0_STATUS)begin
        c0_status_exl <= c0_wdata[1];
    end
end

always @(posedge clk) begin
    if (reset)begin
        c0_status_ie <= 1'b0;
    end else if (mtc0_we && c0_waddr==`CP0_STATUS)begin
        c0_status_ie <= c0_wdata[0];
    end
end


//CAUSE
reg           c0_cause_bd;
reg           c0_cause_ti;
reg [7:0]     c0_cause_ip;
reg [4:0] c0_cause_excode;


wire count_eq_compare;//Count和Compare寄存器相等
assign count_eq_compare = cp0[`CP0_COMPARE] == cp0[`CP0_COUNT];

always @(posedge clk) begin
    if (reset)begin
        c0_cause_bd <= 1'b0;
    end else if (wb_ex && !c0_status_exl)begin
        c0_cause_bd <= wb_bd;//wb_bd
    end
end

always @(posedge clk) begin
    if (reset)begin
        c0_cause_ti <= 1'b0;
    end else if (mtc0_we && c0_waddr==`CP0_COMPARE)begin
        c0_cause_ti <= 1'b0;
    end else if (count_eq_compare)begin//时间中断
        c0_cause_ti <= 1'b1;
    end
end

always @(posedge clk) begin
    if (reset)
        c0_cause_ip[7:2] <= 6'b0;
    else begin
        //c0_cause_ip[7] <= ext_int_in[5] | c0_cause_ti;
        //c0_cause_ip[6:2] <= ext_int_in[4:0];
        c0_cause_ip[7] <= c0_cause_ti;
    end
end

always @(posedge clk) begin
    if (reset)begin
        c0_cause_ip[1:0] <= 2'b0;
    end 
    else if (mtc0_we && c0_waddr==`CP0_CAUSE)begin
        c0_cause_ip[1:0] <= c0_wdata[9:8];
    end
end

always @(posedge clk) begin
    if (reset)begin
        c0_cause_excode <= 5'b0;
    end else if (wb_ex)begin
        c0_cause_excode <= wb_excode;
    end
end


//EPC
reg [31:0]  c0_epc;
always @(posedge clk) begin
    if(reset)begin
        c0_epc <= 0;
    end else if (wb_ex && !c0_status_exl)begin//没中断（时钟更新下个周期才有）
        c0_epc <= wb_bd ? wb_pc - 3'h4 : wb_pc;//这条指令是不是延迟槽指令
    end else if (mtc0_we && c0_waddr==`CP0_EPC)begin
        c0_epc <= c0_wdata;
    end
end

//COUNT
reg        tick; 
reg [31:0] c0_count; 
always @(posedge clk) begin 
	if (reset) 
		tick <= 1'b0; 
 	else       
 		tick <= ~tick; 
 
	if (mtc0_we && c0_waddr==`CP0_COUNT)    
		c0_count <= c0_wdata; 
	else if (tick)     
		c0_count <= c0_count + 1'b1; 
end

//COMPARE
reg [31:0] c0_compare;
always @(posedge clk) begin
	if(reset)
		c0_compare <= 32'b0;
	else if(mtc0_we && c0_waddr == `CP0_COMPARE)
		c0_compare <= c0_wdata;
end

//BADVADDR
reg [31:0] c0_badvaddr; 
always @(posedge clk) begin 
	if (wb_ex && (wb_excode == `AdEL || wb_excode == `AdES || wb_excode == `Tlbl || wb_excode == `Tlbs || wb_excode == `Mod))     
		c0_badvaddr <= wb_badvaddr;
end 

//INDEX
reg [ 3:0] c0_index_index;
reg        c0_index_p;
always @(posedge clk) begin
    if (reset) begin
        c0_index_index <= 4'b0;
        c0_index_p <= 1'b0;
    end
    else if(mtc0_we && c0_waddr == `CP0_INDEX) begin
        c0_index_index <= c0_wdata[3:0];
    end
    else if(tlbp_we) begin
        c0_index_index <= index[3:0];
        c0_index_p <= index[31];
    end
end

//ENTRYHI
reg [18:0] c0_entryhi_vpn2;
reg [ 7:0] c0_entryhi_asid;
always @(posedge clk) begin
    if (reset) begin
        c0_entryhi_asid <= 8'b0;
        c0_entryhi_vpn2 <= 19'b0;
    end
    else if (tlb_ex) begin
        c0_entryhi_vpn2 <= entryhi[31:13];
    end
    else if(mtc0_we && c0_waddr == `CP0_ENTRYHI) begin
        c0_entryhi_asid <= c0_wdata[7:0];
        c0_entryhi_vpn2 <= c0_wdata[31:13];
    end
    else if(tlbr_we) begin
        c0_entryhi_asid <= entryhi[7:0];
        c0_entryhi_vpn2 <= entryhi[31:13];
    end
end

//ENTRYLO0
reg [19:0] c0_entrylo0_pfn0;
reg [ 2:0] c0_entrylo0_c0;
reg        c0_entrylo0_d0;
reg        c0_entrylo0_v0;
reg        c0_entrylo0_g0;
always @(posedge clk) begin
    if (reset) begin
        c0_entrylo0_pfn0 <= 20'b0;
        c0_entrylo0_c0 <= 3'b0;
        c0_entrylo0_d0 <= 1'b0;
        c0_entrylo0_v0 <= 1'b0;
        c0_entrylo0_g0 <= 1'b0;
    end
    else if (mtc0_we && c0_waddr == `CP0_ENTRYLO0) begin
        c0_entrylo0_pfn0 <= c0_wdata[25:6];
        c0_entrylo0_c0 <= c0_wdata[5:3];
        c0_entrylo0_d0 <= c0_wdata[2];
        c0_entrylo0_v0 <= c0_wdata[1];
        c0_entrylo0_g0 <= c0_wdata[0];
    end
    else if(tlbr_we) begin
        c0_entrylo0_pfn0 <= entrylo0[25:6];
        c0_entrylo0_c0 <= entrylo0[5:3];
        c0_entrylo0_d0 <= entrylo0[2];
        c0_entrylo0_v0 <= entrylo0[1];
        c0_entrylo0_g0 <= entrylo0[0];
    end
end

//ENTRYLO1
reg [19:0] c0_entrylo1_pfn1;
reg [ 2:0] c0_entrylo1_c1;
reg        c0_entrylo1_d1;
reg        c0_entrylo1_v1;
reg        c0_entrylo1_g1;
always @(posedge clk) begin
    if (reset) begin
        c0_entrylo1_pfn1 <= 20'b0;
        c0_entrylo1_c1 <= 3'b0;
        c0_entrylo1_d1 <= 1'b0;
        c0_entrylo1_v1 <= 1'b0;
        c0_entrylo1_g1 <= 1'b0;
    end
    else if (mtc0_we && c0_waddr == `CP0_ENTRYLO1) begin
        c0_entrylo1_pfn1 <= c0_wdata[25:6];
        c0_entrylo1_c1 <= c0_wdata[5:3];
        c0_entrylo1_d1 <= c0_wdata[2];
        c0_entrylo1_v1 <= c0_wdata[1];
        c0_entrylo1_g1 <= c0_wdata[0];
    end
    else if(tlbr_we) begin
        c0_entrylo1_pfn1 <= entrylo1[25:6];
        c0_entrylo1_c1 <= entrylo1[5:3];
        c0_entrylo1_d1 <= entrylo1[2];
        c0_entrylo1_v1 <= entrylo1[1];
        c0_entrylo1_g1 <= entrylo1[0];
    end
end


//更新cp0寄存器堆
always @(*)begin
    cp0[  `CP0_STATUS] = {{9{1'b0}}, c0_status_bev, {6{1'b0}}, c0_status_im, {6{1'b0}}, c0_status_exl, c0_status_ie};
    cp0[   `CP0_CAUSE] = {c0_cause_bd, c0_cause_ti, {14{1'b0}}, c0_cause_ip, 1'b0, c0_cause_excode, {2{1'b0}}};
    cp0[     `CP0_EPC] = c0_epc;
    cp0[`CP0_BADVADDR] = c0_badvaddr;
    cp0[   `CP0_COUNT] = c0_count;
    cp0[ `CP0_COMPARE] = c0_compare;
    cp0[   `CP0_INDEX] = {c0_index_p, {27{1'b0}}, c0_index_index};
    cp0[`CP0_ENTRYLO0] = {{6{1'b0}}, c0_entrylo0_pfn0, c0_entrylo0_c0, c0_entrylo0_d0, c0_entrylo0_v0, c0_entrylo0_g0};
    cp0[`CP0_ENTRYLO1] = {{6{1'b0}}, c0_entrylo1_pfn1, c0_entrylo1_c1, c0_entrylo1_d1, c0_entrylo1_v1, c0_entrylo1_g1};
    cp0[ `CP0_ENTRYHI] = {c0_entryhi_vpn2, {5{1'b0}}, c0_entryhi_asid};
end

//读寄存器
assign c0_rdata      = cp0[c0_raddr];
assign epc           = cp0[`CP0_EPC];
assign status        = cp0[`CP0_STATUS];
assign cause         = cp0[`CP0_CAUSE];
assign index_index   = cp0[`CP0_INDEX][3:0];
assign index_p       = cp0[`CP0_INDEX][31];
assign entryhi_asid  = cp0[`CP0_ENTRYHI][7:0];
assign entryhi_vpn2  = cp0[`CP0_ENTRYHI][31:13];
assign entrylo0_pfn0 = cp0[`CP0_ENTRYLO0][25:6];
assign entrylo0_c0   = cp0[`CP0_ENTRYLO0][5:3];
assign entrylo0_d0   = cp0[`CP0_ENTRYLO0][2];
assign entrylo0_v0   = cp0[`CP0_ENTRYLO0][1];
assign entrylo0_g0   = cp0[`CP0_ENTRYLO0][0];
assign entrylo1_pfn1 = cp0[`CP0_ENTRYLO1][25:6];
assign entrylo1_c1   = cp0[`CP0_ENTRYLO1][5:3];
assign entrylo1_d1   = cp0[`CP0_ENTRYLO1][2];
assign entrylo1_v1   = cp0[`CP0_ENTRYLO1][1];
assign entrylo1_g1   = cp0[`CP0_ENTRYLO1][0];

endmodule