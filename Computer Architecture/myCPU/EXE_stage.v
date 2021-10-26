`include "mycpu.h"

module exe_stage(
    input                           clk           ,
    input                           reset         ,
    //allowin
    input                           ms_allowin    ,
    output                          es_allowin    ,
    //from ds
    input                           ds_to_es_valid,
    input  [`DS_TO_ES_BUS_WD  -1:0] ds_to_es_bus  ,
    //to ms
    output                          es_to_ms_valid,
    output [`ES_TO_MS_BUS_WD  -1:0] es_to_ms_bus  ,
    output                          es_load_op    ,
    output [`ES_FORWARDING_BUS_WD-1:0] es_forwarding ,
    // data sram interface
    output        data_sram_req    ,
	output        data_sram_wr     ,
	output [ 1:0] data_sram_size   ,
    output [ 3:0] data_sram_wstrb  ,
    output [31:0] data_sram_addr   ,
    output [31:0] data_sram_wdata  ,
	input         data_sram_addr_ok,
    //exception reflush
    input         ws_reflush,
    input         ms_reflush,
    input         ms_w_entryhi,
    input         refetch,
    output        es_ref_inst,
    input         to_refetch ,
    output [`ES_TLB_BUS_WD-1:0] es_tlb_bus,
    input  [`WS_TO_ES_BUS_WD-1:0] ws_to_es_bus
);

reg         es_valid      ;
wire        es_ready_go   ;

reg  [31:0] lo            ;
reg  [31:0] hi            ;

reg  [`DS_TO_ES_BUS_WD -1:0] ds_to_es_bus_r;
wire        inst_mult     ;
wire        inst_multu    ;
wire        inst_div      ;
wire        inst_divu     ;
wire        inst_mtlo     ;
wire        inst_mflo     ;
wire        inst_mthi     ;
wire        inst_mfhi     ;
wire        inst_mfc0	  ;
wire        inst_mtc0     ;
wire        inst_eret     ;
wire        inst_syscall  ;
wire        inst_tlbr     ;
wire        inst_tlbp     ;
wire        inst_tlbwi    ;
wire [11:0] es_alu_op     ;
wire        es_src1_is_sa ;  
wire        es_src1_is_pc ;
wire        es_src2_is_imm; 
wire        es_src2_is_imm_0; 
wire        es_src2_is_8  ;
wire [ 3:0] es_gr_we      ;
wire        es_mem_we     ;
wire [ 4:0] es_dest       ;
wire [15:0] es_imm        ;
wire [31:0] es_rs_value   ;
wire [31:0] es_rt_value   ;
wire [31:0] es_pc         ;
wire [ 1:0] mem_width     ;
wire [ 1:0] mem_align     ;
wire        mem_sign      ;
wire [ 1:0] mem_sel       ;
wire [ 3:0] store_wen     ;
wire        sb_op         ;
wire        sh_op         ;
wire        sw_op         ;
wire        swr_op        ;
wire        swl_op        ;
wire        lb_op;
wire        lbu_op;
wire        lh_op;
wire        lhu_op;
wire        lw_op;
wire        lwr_op;
wire        lwl_op;
wire [32:0] mdsrc1        ;
wire [32:0] mdsrc2        ;
wire [65:0] res_mul       ;

wire        load_AdEL     ;
wire        store_AdES    ;
wire        overflow      ;
wire        if_ex         ;
wire        id_ex         ;
wire        exe_ex        ;
wire [ 4:0] if_excode     ;
wire [ 4:0] id_excode     ;
wire [ 4:0] exe_excode    ;
wire        exe_ex_wen    ;

wire [31:0] divisor_tdata   ;
wire        divisor_tready  ;
reg         divisor_tvalid  ;
reg         divisor_success ;
wire [31:0] dividend_tdata  ;
wire        dividend_tready ;
reg         dividend_tvalid ;
reg         dividend_success;        
wire [63:0] dout_tdata      ;
wire        dout_tvalid     ;

wire [31:0] divisor_tdata_u   ;
wire        divisor_tready_u  ;
reg         divisor_tvalid_u  ;
reg         divisor_success_u ;
wire [31:0] dividend_tdata_u  ;
wire        dividend_tready_u ;
reg         dividend_tvalid_u ;
reg         dividend_success_u;
wire [63:0] dout_tdata_u      ;
wire        dout_tvalid_u     ;
wire [ 4:0] es_rd             ;
wire        ov_act            ; 
wire        exe_bd            ;
wire        if_refill         ;

assign {if_refill      ,  //178:178
		inst_tlbwi     ,  //177:177
		inst_tlbp      ,  //176:176 
		inst_tlbr      ,  //175:175
		exe_bd         ,  //174:174
		id_ex          ,  //173:173
		id_excode      ,  //172:168
		if_ex          ,  //167:167
		if_excode      ,  //166:162
		ov_act         ,  //161:161
		es_rd          ,  //160,156
		inst_mfc0      ,  //155:155
        inst_mtc0      ,  //154:154
        inst_eret      ,  //153:153
		mem_sign       ,  //152:152
        mem_align      ,  //151:150
        mem_width      ,  //149:148
        es_src2_is_imm_0, //147:147
		inst_mult      ,  //146:146
		inst_multu     ,  //145:145
        inst_div       ,  //144:144
		inst_divu      ,  //143:143
        inst_mtlo      ,  //142:142
		inst_mthi      ,  //141:141
		inst_mflo      ,  //140:140
		inst_mfhi      ,  //139:139
		es_alu_op      ,  //138:127
        es_load_op     ,  //126:126
        es_src1_is_sa  ,  //125:125
        es_src1_is_pc  ,  //124:124
        es_src2_is_imm ,  //123:123
        es_src2_is_8   ,  //122:122
        es_gr_we       ,  //121:118
        es_mem_we      ,  //117:117
        es_dest        ,  //116:112
        es_imm         ,  //111:96
        es_rs_value    ,  //95 :64
        es_rt_value    ,  //63 :32
        es_pc             //31 :0
       } = ds_to_es_bus_r;

wire [31:0] es_alu_src1   ;
wire [31:0] es_alu_src2   ;
wire [31:0] es_alu_result ;
wire [31:0] es_final_result;

wire        es_res_from_mem;

assign es_final_result = inst_mfhi ? hi
                       : inst_mflo ? lo
                       : es_alu_result;

assign es_res_from_mem = es_load_op;
assign es_to_ms_bus = {exe_refill     ,  //146:146
					   if_refill      ,  //145:145
					   inst_tlbwi     ,  //144:144
					   inst_tlbp      ,  //143:143 
		               inst_tlbr      ,  //142:142
					   es_mem_we      ,  //141:141
					   data_sram_req  ,  //140:140
					   exe_bd         ,  //139:139
					   exe_ex         ,  //138:138
					   exe_excode     ,  //137:133
					   id_ex          ,  //132:132
					   id_excode      ,  //131:127
					   if_ex          ,  //126:126
					   if_excode      ,  //125:121
					   es_rd          ,  //120:116
					   es_rt_value    ,  //115:84
					   inst_mfc0      ,  //83:83
        			   inst_mtc0      ,  //82:82
        			   inst_eret      ,  //81:81
					   mem_sel        ,  //80:79
					   mem_sign       ,  //78:78
					   mem_width      ,  //77:76
					   mem_align      ,  //75:74
					   es_res_from_mem,  //73:73
                       es_gr_we       ,  //72:69
                       es_dest        ,  //68:64
                       es_final_result,  //63:32
                       es_pc             //31:0
                      };
assign es_forwarding = {inst_mfc0    , //42:42
						es_valid     , //41:41
                        es_gr_we     , //40:37
                        es_dest      , //36:32
                        es_final_result  //31:0
                       };
assign es_ready_go    = (inst_tlbp & ms_w_entryhi & es_valid) ? 1'b0
					  :	((es_mem_we || es_load_op) && exe_ex) ? 1'b1
					  : ((es_mem_we || es_load_op)) ? (data_sram_req && data_sram_addr_ok)
					  : ((inst_div && dout_tvalid == 1'b0) || (inst_divu && dout_tvalid_u == 1'b0)) ? 1'b0
					  : 1'b1;

assign es_allowin     = !es_valid || es_ready_go && ms_allowin;
assign es_to_ms_valid =  es_valid && es_ready_go;
always @(posedge clk) begin
    if (reset) begin
        es_valid <= 1'b0;
    end
    else if(ws_reflush | refetch) begin
        es_valid <= 1'b0;
    end
    else if (es_allowin) begin
        es_valid <= ds_to_es_valid;
    end

    if (ds_to_es_valid && es_allowin) begin
        ds_to_es_bus_r <= ds_to_es_bus;
    end
end

assign es_alu_src1 = es_src1_is_sa  ? {27'b0, es_imm[10:6]} : 
                     es_src1_is_pc  ? es_pc[31:0] :
                                      es_rs_value;
assign es_alu_src2 = es_src2_is_imm   ? {{16{es_imm[15]}}, es_imm[15:0]} : 
					 es_src2_is_imm_0 ? {{16{1'b0}}, es_imm[15:0]}       ://3
                     es_src2_is_8     ? 32'd8 :
                                      es_rt_value;

alu u_alu(
    .alu_op     (es_alu_op    ),
    .alu_src1   (es_alu_src1  ),
    .alu_src2   (es_alu_src2  ),
    .alu_result (es_alu_result)
    );

// multiply
assign mdsrc1 = (inst_mult)? ({es_alu_src1[31], es_alu_src1})
              : 			 ({1'b0           , es_alu_src1});


assign mdsrc2 = (inst_mult) ? ({es_alu_src2[31], es_alu_src2})
              : 			  ({1'b0           , es_alu_src2});
              
assign res_mul = $signed(mdsrc1) * $signed(mdsrc2);


//divide
assign divisor_tdata    = es_alu_src2;
assign dividend_tdata   = es_alu_src1;
assign divisor_tdata_u  = es_alu_src2;
assign dividend_tdata_u = es_alu_src1;

always@(posedge clk) begin
	if(reset) begin
		divisor_tvalid <= 1'b0;
	end
	else begin
		if(inst_div && divisor_success == 1'b0 && divisor_tvalid == 1'b0) begin
			divisor_tvalid <= 1'b1;
		end
		else if(divisor_tvalid == 1'b1 && divisor_tready == 1'b1) begin
			divisor_tvalid <= 1'b0;
		end
		else begin
				divisor_tvalid <= divisor_tvalid;
		end
	end
end
  

always@(posedge clk) begin
	if(reset) begin
		dividend_tvalid <= 1'b0;
	end
	else begin
		if(inst_div && dividend_success == 1'b0 && dividend_tvalid == 1'b0) begin
			dividend_tvalid <= 1'b1;
		end
		else if(dividend_tvalid == 1'b1 && dividend_tready == 1'b1) begin
			dividend_tvalid <= 1'b0;
		end
		else begin
			dividend_tvalid <= dividend_tvalid;
		end
	end
end


always@(posedge clk) begin
	if(reset) begin
		divisor_success <= 1'b0;
	end
	else begin
		if(divisor_tvalid == 1'b1 && divisor_tready == 1'b1) begin
			divisor_success <= 1'b1;
		end
		else if(dout_tvalid == 1'b1) begin
			divisor_success <= 1'b0;
		end
		else begin
			divisor_success <= divisor_success;
		end
	end
end

always@(posedge clk) begin
	if(reset) begin
		dividend_success <= 1'b0;
	end
	else begin
		if(dividend_tvalid == 1'b1 && dividend_tready == 1'b1) begin
			dividend_success <= 1'b1;
		end
		else if(dout_tvalid == 1'b1) begin
			dividend_success <= 1'b0;
		end
		else begin
			dividend_success <= dividend_success;
		end
	end
end

//divu
always@(posedge clk) begin
	if(reset) begin
		divisor_tvalid_u <= 1'b0;
	end
	else begin
		if(inst_divu && divisor_success_u == 1'b0 && divisor_tvalid_u == 1'b0) begin
			divisor_tvalid_u <= 1'b1;
		end
		else if(divisor_tvalid_u == 1'b1 && divisor_tready_u == 1'b1) begin
			divisor_tvalid_u <= 1'b0;
		end
		else begin
			divisor_tvalid_u <= divisor_tvalid_u;
		end
	end
end
  

always@(posedge clk) begin
	if(reset) begin
		dividend_tvalid_u <= 1'b0;
	end
	else begin
		if(inst_divu && dividend_success_u == 1'b0 && dividend_tvalid_u == 1'b0) begin
			dividend_tvalid_u <= 1'b1;
		end
		else if(dividend_tvalid_u == 1'b1 && dividend_tready_u == 1'b1) begin
			dividend_tvalid_u <= 1'b0;
		end
		else begin
			dividend_tvalid_u <= dividend_tvalid_u;
		end
	end
end


always@(posedge clk) begin
	if(reset) begin
		divisor_success_u <= 1'b0;
	end
	else begin
		if(divisor_tvalid_u == 1'b1 && divisor_tready_u == 1'b1) begin
			divisor_success_u <= 1'b1;
		end
		else if(dout_tvalid_u == 1'b1) begin
			divisor_success_u <= 1'b0;
		end
		else begin
			divisor_success_u <= divisor_success_u;
		end
	end
end

always@(posedge clk) begin
	if(reset) begin
		dividend_success_u <= 1'b0;
	end
	else begin
		if(dividend_tvalid_u == 1'b1 && dividend_tready_u == 1'b1) begin
			dividend_success_u <= 1'b1;
		end
		else if(dout_tvalid_u == 1'b1) begin
			dividend_success_u <= 1'b0;
		end
		else begin
			dividend_success_u <= dividend_success_u;
		end
	end
end


// update lo and hi
always @(posedge clk) begin
	if(reset) begin
		lo <= 32'b0;
		hi <= 32'b0;
	end
	else begin
		if(~exe_ex_wen) begin
			lo <= lo;
			hi <= hi;
		end
		else if((inst_mult || inst_multu) && es_valid) begin
			lo <= res_mul[31: 0];
			hi <= res_mul[63:32];
		end
		else if(inst_div && dout_tvalid && es_valid) begin
			lo <= dout_tdata[63:32];
			hi <= dout_tdata[31: 0];
		end
		else if(inst_divu && dout_tvalid_u && es_valid) begin
			lo <= dout_tdata_u[63:32];
			hi <= dout_tdata_u[31: 0];
		end
		else if(inst_mthi && es_valid) begin
			hi <= es_rs_value;
		end
		else if(inst_mtlo && es_valid) begin
			lo <= es_rs_value;
		end
	end
end  

div_s div_s(
	.s_axis_divisor_tdata  (divisor_tdata  ),
	.s_axis_divisor_tready (divisor_tready ),
	.s_axis_divisor_tvalid (divisor_tvalid ),
	.s_axis_dividend_tdata (dividend_tdata ),
	.s_axis_dividend_tready(dividend_tready),
	.s_axis_dividend_tvalid(dividend_tvalid),
	.m_axis_dout_tdata     (dout_tdata     ),
	.m_axis_dout_tvalid    (dout_tvalid    ),
	.aclk                  (clk            )
	);

div_u div_u(
	.s_axis_divisor_tdata  (divisor_tdata_u  ),
	.s_axis_divisor_tready (divisor_tready_u ),
	.s_axis_divisor_tvalid (divisor_tvalid_u ),
	.s_axis_dividend_tdata (dividend_tdata_u ),
	.s_axis_dividend_tready(dividend_tready_u),
	.s_axis_dividend_tvalid(dividend_tvalid_u),
	.m_axis_dout_tdata     (dout_tdata_u     ),
	.m_axis_dout_tvalid    (dout_tvalid_u    ),
	.aclk                  (clk              )
	);


assign sb_op           = es_mem_we & (mem_width ==2'b01);
assign sh_op           = es_mem_we & (mem_width ==2'b10);
assign sw_op           = es_mem_we & (mem_width ==2'b11) & (mem_align == 2'b01);
assign swl_op          = es_mem_we & (mem_width ==2'b11) & (mem_align == 2'b10);
assign swr_op          = es_mem_we & (mem_width ==2'b11) & (mem_align == 2'b11);
assign lb_op           = es_res_from_mem & (mem_width == 2'b01) &  mem_sign;
assign lbu_op          = es_res_from_mem & (mem_width == 2'b01) & ~mem_sign;
assign lh_op           = es_res_from_mem & (mem_width == 2'b10) &  mem_sign;
assign lhu_op          = es_res_from_mem & (mem_width == 2'b10) & ~mem_sign;
assign lw_op           = es_res_from_mem & (mem_width == 2'b11) & (mem_align == 2'b01);
assign lwl_op          = es_res_from_mem & (mem_width == 2'b11) & (mem_align == 2'b10);
assign lwr_op          = es_res_from_mem & (mem_width == 2'b11) & (mem_align == 2'b11);

assign mem_sel         = es_alu_result[1:0];

assign store_wen       = ({4{sb_op  & mem_sel    == 2'b00}} & 4'b0001) 
                       | ({4{sb_op  & mem_sel    == 2'b01}} & 4'b0010) 
				       | ({4{sb_op  & mem_sel    == 2'b10}} & 4'b0100) 
				       | ({4{sb_op  & mem_sel    == 2'b11}} & 4'b1000) 
				       | ({4{sh_op  & mem_sel[1] == 1'b0 }} & 4'b0011) 
				       | ({4{sh_op  & mem_sel[1] == 1'b1 }} & 4'b1100) 
                       | ({4{sw_op                       }} & 4'b1111) 
                       | ({4{swl_op & mem_sel    == 2'b00}} & 4'b0001) 
                       | ({4{swl_op & mem_sel    == 2'b01}} & 4'b0011) 
                       | ({4{swl_op & mem_sel    == 2'b10}} & 4'b0111) 
                       | ({4{swl_op & mem_sel    == 2'b11}} & 4'b1111) 
                       | ({4{swr_op & mem_sel    == 2'b00}} & 4'b1111) 
                       | ({4{swr_op & mem_sel    == 2'b01}} & 4'b1110) 
                       | ({4{swr_op & mem_sel    == 2'b10}} & 4'b1100) 
                       | ({4{swr_op & mem_sel    == 2'b11}} & 4'b1000);		 

assign data_sram_wstrb = (es_mem_we && es_valid && exe_ex_wen) ? store_wen : 4'h0;
assign addr            = (swl_op || lwl_op)? {es_alu_result[31:2], 2'b00} : es_alu_result;
assign data_sram_wdata = ({32{sb_op                    }} & {4{es_rt_value[ 7:0]     }})
                       | ({32{sh_op                    }} & {2{es_rt_value[15:0]     }})
                       | ({32{sw_op                    }} & es_rt_value                )
                       | ({32{swl_op & mem_sel == 2'b00}} & {24'b0, es_rt_value[31:24]}) 
                       | ({32{swl_op & mem_sel == 2'b01}} & {16'b0, es_rt_value[31:16]})
                       | ({32{swl_op & mem_sel == 2'b10}} & {8'b0 , es_rt_value[31: 8]})
                       | ({32{swl_op & mem_sel == 2'b11}} & es_rt_value                ) 
                       | ({32{swr_op & mem_sel == 2'b00}} & es_rt_value                )
                       | ({32{swr_op & mem_sel == 2'b01}} & {es_rt_value[23:0], 8'b0  }) 
                       | ({32{swr_op & mem_sel == 2'b10}} & {es_rt_value[15:0], 16'b0 }) 
                       | ({32{swr_op & mem_sel == 2'b11}} & {es_rt_value[ 7:0], 24'b0 });

assign load_AdEL  = es_load_op && ((mem_width == 2'b11 && mem_align == 2'b01 && mem_sel != 2'b00) | (mem_width == 2'b10 && mem_sel[0] != 1'b0));
assign store_AdES = (sw_op && mem_sel != 2'b00) | (sh_op && mem_sel[0] != 1'b0);
assign overflow   = ((es_alu_op[0] && ((es_alu_src1[31] & es_alu_src2[31] & ~es_alu_result[31]) || (~es_alu_src1[31] & ~es_alu_src2[31] & es_alu_result[31]))) | (es_alu_op[1] && ((es_alu_src1[31] & ~es_alu_src2[31] & ~es_alu_result[31]) || (~es_alu_src1[31] & es_alu_src2[31] & es_alu_result[31])))) && ov_act;

assign exe_excode = overflow                                       ? `Ov
				  : load_AdEL                                      ? `AdEL
				  : exe_modify                                     ? `Mod
				  : ((exe_refill | exe_invalid) & es_mem_we      ) ? `Tlbs
				  : ((exe_refill | exe_invalid) & es_res_from_mem) ? `Tlbl
				  : store_AdES ? `AdES
				  : `Int;

assign exe_ex = (overflow | load_AdEL | store_AdES | exe_refill | exe_invalid | exe_modify) & es_valid;		

assign exe_ex_wen = ~((if_ex | id_ex | exe_ex) | ws_reflush | ms_reflush | refetch | to_refetch);		  

assign data_sram_size = ({2{(sw_op || lw_op) || ((lwl_op || swl_op) && (mem_sel==2 || mem_sel==3)) || ((swr_op || lwr_op) && (mem_sel==1 || mem_sel == 0))}}  & 2'b10)
					   |({2{(sh_op || lhu_op || lh_op) || ((swl_op || lwl_op) && mem_sel == 1) || ((swr_op || lwr_op) && mem_sel == 2)}} & 2'b1);
assign data_sram_wr   = (|data_sram_wstrb) & ~to_refetch;

reg data_sram_req_r;
always @ (posedge clk)begin
	if(reset)
		data_sram_req_r <= 0;
	else if(data_sram_req_r & data_sram_addr_ok)
		data_sram_req_r <= 0;
	else if(ms_allowin && (es_load_op || es_mem_we) && es_valid && ~exe_invalid && ~exe_modify && ~exe_refill)
		data_sram_req_r <= 1;
end
assign data_sram_req = data_sram_req_r & es_valid & exe_ex_wen;

assign es_ref_inst = es_valid & (inst_tlbwi | inst_tlbr);

//TLB
wire data_valid;
wire data_found;
wire data_modify;
wire [19:0] data_pfn; 
wire exe_refill;
wire exe_invalid;
wire exe_modify;
wire no_map;
wire [31:0] addr;
assign no_map = (addr[31:28] == 4'h8 || addr[31:28] == 4'h9 || addr[31:28] == 4'ha || addr[31:28] == 4'hb);
assign exe_refill  = ~no_map & ~data_found & es_valid & (es_mem_we | es_res_from_mem);
assign exe_invalid = ~no_map & data_found & ~data_valid & es_valid & (es_mem_we | es_res_from_mem);
assign exe_modify  = ~no_map & data_valid & ~data_modify & es_mem_we & es_valid & (es_mem_we | es_res_from_mem);
assign es_tlb_bus  = {inst_tlbp & es_valid, addr[31:12]};
assign {data_found,
		data_valid,
		data_modify,
		data_pfn
		} = ws_to_es_bus;
assign data_sram_addr = no_map ? addr : {data_pfn, addr[11:0]};

endmodule
