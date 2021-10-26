`include "mycpu.h"

module mem_stage(
    input                           clk              ,
    input                           reset            ,
    //allowin
    input                           ws_allowin       ,
    output                          ms_allowin       ,
    //from es
    input                           es_to_ms_valid   ,
    input  [`ES_TO_MS_BUS_WD  -1:0] es_to_ms_bus     ,
    //to ws
    output                          ms_to_ws_valid   ,
    output [`MS_TO_WS_BUS_WD  -1:0] ms_to_ws_bus     ,
    output [`MS_FORWARDING_BUS_WD-1:0] ms_forwarding ,
    //from data-sram
    input  [31                  :0] data_sram_rdata  ,
    input                           data_sram_data_ok,
    input                           ws_reflush       ,
    output                          ms_reflush       ,
    output                          ms_w_entryhi     ,
    input                           refetch          ,
    output                          ms_ref_inst      ,
    input                           to_refetch       

);

reg         ms_valid;
wire        ms_ready_go;

reg [`ES_TO_MS_BUS_WD -1:0] es_to_ms_bus_r;
wire        ms_res_from_mem;
wire [ 3:0] ms_gr_we;
wire [ 3:0] gr_we;
wire [ 4:0] ms_dest;
wire [31:0] ms_alu_result;
wire [31:0] ms_pc;
wire [ 1:0] mem_sel;
wire [ 1:0] mem_width;
wire [ 1:0] mem_align;
wire        mem_sign;
wire        lb_op;
wire        lbu_op;
wire        lh_op;
wire        lhu_op;
wire        lw_op;
wire        lwr_op;
wire        lwl_op;
wire [ 4:0] load_wen;
wire [ 7:0] lb_data;
wire [15:0] lh_data;

wire [31:0] mem_result;
wire [31:0] ms_final_result;
wire [31:0] ms_result;
wire [31:0] mem_addr;

wire        inst_mfc0;
wire        inst_eret;
wire        inst_mtc0;
wire        inst_syscall;
wire        inst_tlbp;
wire        inst_tlbr;
wire        inst_tlbwi;
wire [31:0] rt_value;
wire [ 4:0] ms_rd   ;

wire        if_ex;
wire        id_ex;
wire        exe_ex;
wire        mem_ex;
wire [ 4:0] if_excode;
wire [ 4:0] id_excode;
wire [ 4:0] exe_excode;
wire [ 4:0] mem_excode;
wire        mem_bd;
reg         mem_state;
wire        data_sram_data_req;
wire        if_refill;
wire        exe_refill;

assign {exe_refill     ,  //146:146
		if_refill      ,  //145:145
		inst_tlbwi     ,  //144:144
		inst_tlbp      ,  //143:143 
		inst_tlbr      ,  //142:142
		es_mem_we      ,   //141:141
        data_sram_data_req,//140:140
        mem_bd         ,  //139:139
		exe_ex         ,  //138:138
		exe_excode     ,  //137:133
		id_ex          ,  //132:132
		id_excode      ,  //131:127
		if_ex          ,  //126:126
		if_excode      ,  //125:121
		ms_rd          ,  //120:116
        rt_value       ,  //115:84
		inst_mfc0      ,  //84:83
        inst_mtc0      ,  //82:82
        inst_eret      ,  //81:81
        mem_sel        ,  //80:79
        mem_sign       ,  //78:78   
        mem_width      ,  //77:76
        mem_align      ,  //75:74
        ms_res_from_mem,  //73:73
        gr_we          ,  //72:69
        ms_dest        ,  //68:64
        ms_alu_result  ,  //63:32
        ms_pc             //31:0
       } = es_to_ms_bus_r;

assign ms_forwarding = {ms_res_from_mem  , //44:44
						data_sram_data_ok, //43:43
						inst_mfc0      , //42:42
                        ms_valid       , //41:41
                        ms_gr_we       , //40:37
                        ms_dest        , //36:32
                        ms_final_result  //31:0
                       };

assign ms_to_ws_bus = {exe_refill     ,  //174:174
					   if_refill      ,  //173:173
					   inst_tlbwi     ,  //172:172
					   inst_tlbp      ,  //171:171 
					   inst_tlbr      ,  //170:170
					   mem_bd         ,  //169:169
				       mem_addr       ,  //168:137
 					   mem_ex         ,  //136:136
					   mem_excode     ,  //135:131
					   exe_ex         ,  //130:130
					   exe_excode     ,  //129:125
					   id_ex          ,  //124:124
					   id_excode      ,  //123:119
					   if_ex          ,  //118:118
					   if_excode      ,  //117:113
					   ms_rd          ,  //112:108
                       rt_value       ,  //107:76
		               inst_mfc0      ,  //75:75
                       inst_mtc0      ,  //74:74
                       inst_eret      ,  //73:73
                       ms_gr_we       ,  //72:69
                       ms_dest        ,  //68:64
                       ms_final_result,  //63:32
                       ms_pc             //31:0
                      };

assign ms_ready_go    = ((ms_res_from_mem || es_mem_we) && ~ms_reflush) ? data_sram_data_ok : 1;
assign ms_allowin     = !ms_valid || ms_ready_go && ws_allowin;
assign ms_to_ws_valid = ms_valid && ms_ready_go;
always @(posedge clk) begin
    if (reset) begin
        ms_valid <= 1'b0;
    end
    else if(ws_reflush | refetch) begin
        ms_valid <= 1'b0;
    end
    else if (ms_allowin) begin
        ms_valid <= es_to_ms_valid;
    end

    if (es_to_ms_valid && ms_allowin) begin
        es_to_ms_bus_r  <= es_to_ms_bus;
    end
end

assign lb_op      = ms_res_from_mem & (mem_width == 2'b01) &  mem_sign;
assign lbu_op     = ms_res_from_mem & (mem_width == 2'b01) & ~mem_sign;
assign lh_op      = ms_res_from_mem & (mem_width == 2'b10) &  mem_sign;
assign lhu_op     = ms_res_from_mem & (mem_width == 2'b10) & ~mem_sign;
assign lw_op      = ms_res_from_mem & (mem_width == 2'b11) & (mem_align == 2'b01);
assign lwl_op     = ms_res_from_mem & (mem_width == 2'b11) & (mem_align == 2'b10);
assign lwr_op     = ms_res_from_mem & (mem_width == 2'b11) & (mem_align == 2'b11);

assign load_wen   = ({4{lb_op | lbu_op | lh_op | lhu_op | lw_op}} & 4'b1111) 
                  | ({4{lwl_op & mem_sel == 2'b00              }} & 4'b1000)
                  | ({4{lwl_op & mem_sel == 2'b01              }} & 4'b1100) 
                  | ({4{lwl_op & mem_sel == 2'b10              }} & 4'b1110)
                  | ({4{lwl_op & mem_sel == 2'b11              }} & 4'b1111) 
                  | ({4{lwr_op & mem_sel == 2'b00              }} & 4'b1111)
                  | ({4{lwr_op & mem_sel == 2'b01              }} & 4'b0111)
                  | ({4{lwr_op & mem_sel == 2'b10              }} & 4'b0011) 
                  | ({4{lwr_op & mem_sel == 2'b11              }} & 4'b0001);

assign ms_gr_we = ms_res_from_mem ? load_wen : gr_we;

assign lb_data = ({8{mem_sel == 2'b00}} & data_sram_rdata[ 7: 0]) 
               | ({8{mem_sel == 2'b01}} & data_sram_rdata[15: 8])
               | ({8{mem_sel == 2'b10}} & data_sram_rdata[23:16])
               | ({8{mem_sel == 2'b11}} & data_sram_rdata[31:24]);

assign lh_data = ({16{mem_sel == 2'b00}} & {data_sram_rdata[15: 0]})
               | ({16{mem_sel == 2'b10}} & {data_sram_rdata[31:16]});                  

assign mem_result = ({32{lb_op                    }} & {{24{lb_data[7] }}, lb_data   })
                  | ({32{lbu_op                   }} & {{24{1'b0       }}, lb_data   })
                  | ({32{lh_op                    }} & {{16{lh_data[15]}}, lh_data   }) 
                  | ({32{lhu_op                   }} & {{16{1'b0       }}, lh_data   }) 
                  | ({32{lw_op                    }} & data_sram_rdata                ) 
                  | ({32{lwl_op & mem_sel == 2'b00}} & {data_sram_rdata[ 7:0], 24'b0 }) 
                  | ({32{lwl_op & mem_sel == 2'b01}} & {data_sram_rdata[15:0], 16'b0 })
                  | ({32{lwl_op & mem_sel == 2'b10}} & {data_sram_rdata[23:0],  8'b0 }) 
                  | ({32{lwl_op & mem_sel == 2'b11}} & data_sram_rdata                )
                  | ({32{lwr_op & mem_sel == 2'b00}} & data_sram_rdata                ) 
                  | ({32{lwr_op & mem_sel == 2'b01}} & {24'b0, data_sram_rdata[31: 8]}) 
                  | ({32{lwr_op & mem_sel == 2'b10}} & {16'b0, data_sram_rdata[31:16]}) 
                  | ({32{lwr_op & mem_sel == 2'b11}} & {32'b0, data_sram_rdata[31:24]});

assign ms_final_result = ms_res_from_mem ? mem_result
                                         : ms_alu_result;
assign mem_addr   = ms_alu_result;

assign mem_excode = `Int;
assign mem_ex     = 0;
assign ms_reflush = (mem_ex | exe_ex | id_ex | if_ex | inst_eret | refetch) & ms_valid;

assign ms_w_entryhi = inst_mtc0 & ms_valid & (ms_dest == `CP0_ENTRYHI);

assign ms_ref_inst = ms_valid & (inst_tlbwi | inst_tlbr);
endmodule
