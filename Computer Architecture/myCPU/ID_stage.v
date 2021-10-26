`include "mycpu.h"

module id_stage(
    input                              clk            ,
    input                              reset          ,
    //allowin
    input                              es_allowin     ,
    output                             ds_allowin     ,
    //from fs
    input                              fs_to_ds_valid ,
    input  [`FS_TO_DS_BUS_WD     -1:0] fs_to_ds_bus   ,
    //to es
    output                             ds_to_es_valid ,
    output [`DS_TO_ES_BUS_WD     -1:0] ds_to_es_bus   ,
    //to fs
    output [`BR_BUS_WD           -1:0] br_bus_hold    ,
    input                              fs_valid       ,
    input                              fs_allowin     ,
    input                              pre_fs_ready_go,
    //to rf: for write back
    input                              load_valid     ,                    
    input  [`WS_TO_RF_BUS_WD     -1:0] ws_to_rf_bus   ,
    input  [`ES_FORWARDING_BUS_WD-1:0] es_forwarding  ,
    input  [`MS_FORWARDING_BUS_WD-1:0] ms_forwarding  ,
    input  [`WS_FORWARDING_BUS_WD-1:0] ws_forwarding  ,
    //exception reflush
    input                              ws_reflush     ,
    input                              refetch        ,
    output                             ds_ref_inst    ,
    input                              to_refetch       

);

reg         ds_valid   ;
wire        ds_ready_go;

wire [31                 :0] fs_pc;
reg  [`FS_TO_DS_BUS_WD -1:0] fs_to_ds_bus_r;
assign fs_pc = fs_to_ds_bus[31:0];

wire [31:0] ds_inst;
wire [31:0] ds_pc  ;

wire [ 3:0] rf_we   ;
wire [ 4:0] rf_waddr;
wire [31:0] rf_wdata;
assign {rf_we   ,  //40:37
        rf_waddr,  //36:32
        rf_wdata   //31:0
       } = ws_to_rf_bus;

wire        br_taken;
wire [31:0] br_target;
wire        is_br;
wire        br_stall;
wire        br_go;

wire [11:0] alu_op;
wire        load_op;
wire        src1_is_sa;
wire        src1_is_pc;
wire        src2_is_imm;
wire        src2_is_imm_0;
wire        src2_is_8;
wire        res_from_mem;
wire [ 3:0] gr_we;
wire        mem_we;
wire [ 4:0] dest;
wire [15:0] imm;
wire [31:0] rs_value;
wire [31:0] rt_value;

wire [ 5:0] op;
wire [ 4:0] rs;
wire [ 4:0] rt;
wire [ 4:0] rd;
wire [ 4:0] sa;
wire [ 5:0] func;
wire [25:0] jidx;
wire [63:0] op_d;
wire [31:0] rs_d;
wire [31:0] rt_d;
wire [31:0] rd_d;
wire [31:0] sa_d;
wire [63:0] func_d;

wire        inst_addu;
wire        inst_subu;
wire        inst_slt;
wire        inst_sltu;
wire        inst_and;
wire        inst_or;
wire        inst_xor;
wire        inst_nor;
wire        inst_sll;
wire        inst_srl;
wire        inst_sra;
wire        inst_addiu;
wire        inst_lui;
wire        inst_lw;
wire        inst_sw;
wire        inst_beq;
wire        inst_bne;
wire        inst_jal;
wire        inst_jr;
wire        inst_mult;
wire        inst_multu;
wire        inst_div;
wire        inst_divu;
wire        inst_mfhi;
wire        inst_mflo;
wire        inst_mthi;
wire        inst_mtlo;
wire        inst_add;
wire        inst_sub;
wire        inst_addi;
wire        inst_slti;
wire        inst_sltiu;
wire        inst_andi;
wire        inst_ori;
wire        inst_xori;
wire        inst_sllv;
wire        inst_srav;
wire        inst_srlv;
wire        inst_bgez;
wire        inst_bltz;
wire        inst_bgtz;
wire        inst_blez;
wire        inst_bgezal;
wire        inst_bltzal;
wire        inst_j;
wire        inst_jalr;
wire        inst_lb;
wire        inst_lbu;
wire        inst_lh;
wire        inst_lhu;
wire        inst_lwl;
wire        inst_lwr;
wire        inst_sb;
wire        inst_sh;
wire        inst_swr;
wire        inst_swl;
wire        inst_mfc0;
wire        inst_mtc0;
wire        inst_eret;
wire        inst_syscall;
wire        inst_break;
wire        inst_tlbp;
wire        inst_tlbr;
wire        inst_tlbwi;


wire        es_inst_mfc0;
wire        ms_inst_mfc0;
wire        ws_inst_mfc0;
wire [31:0] status;
wire [31:0] cause;

wire        dst_is_r31;  
wire        dst_is_rt;  

wire        store_op; 
wire        inst_reserve;
wire        ov_act;
wire        id_ex;
wire [ 4:0] id_excode;
wire        if_ex;
wire [ 4:0] if_excode;
wire        time_int;
wire        sof_int1;
wire        sof_int0;
wire        id_bd;

wire [ 4:0] rf_raddr1;
wire [31:0] rf_rdata1;
wire [ 4:0] rf_raddr2;
wire [31:0] rf_rdata2;

wire        rs_eq_rt;
wire        rs_le_zero;
wire        rs_gt_zero;


wire [ 1:0] mem_align;
wire [ 1:0] mem_width;
wire        mem_sign;

wire [ 1:0]  forward_valid1    ;
wire [ 1:0]  forward_valid2    ;
wire [ 1:0]  forward_valid_rs_0;
wire [ 1:0]  forward_valid_rs_1;
wire [ 1:0]  forward_valid_rs_2;
wire [ 1:0]  forward_valid_rs_3;
wire [ 1:0]  forward_valid_rt_0;
wire [ 1:0]  forward_valid_rt_1;
wire [ 1:0]  forward_valid_rt_2;
wire [ 1:0]  forward_valid_rt_3;
wire [ 7:0]  fw_wdata_rs_0     ;
wire [ 7:0]  fw_wdata_rs_1     ;
wire [ 7:0]  fw_wdata_rs_2     ;
wire [ 7:0]  fw_wdata_rs_3     ;
wire [ 7:0]  fw_wdata_rt_0     ;
wire [ 7:0]  fw_wdata_rt_1     ;
wire [ 7:0]  fw_wdata_rt_2     ;
wire [ 7:0]  fw_wdata_rt_3     ;
wire [ 3:0]  es_fw_we          ;
wire [ 3:0]  ms_fw_we          ;
wire [ 3:0]  ws_fw_we          ;
wire [ 4:0]  es_fw_dst         ;
wire [ 4:0]  ms_fw_dst         ;
wire [ 4:0]  ws_fw_dst         ;
wire         es_fw_valid       ;
wire         ms_fw_valid       ;
wire         ws_fw_valid       ;
wire [31:0]  es_fw_data        ;
wire [31:0]  ms_fw_data        ;
wire [31:0]  ws_fw_data        ;
wire         data_ok_fw        ;
wire         ms_load           ;
wire         if_refill         ;

assign {if_refill, //71:71
		id_bd    , //70:70
        if_ex    , //69:69
		if_excode, //68:64
		ds_inst  , //63:32
        ds_pc      //31:0
       } = fs_to_ds_bus_r;

assign {es_inst_mfc0, //42:42
        es_fw_valid , //41:41
        es_fw_we    , //40:37
        es_fw_dst   , //36:32
        es_fw_data    //31:0
       } = es_forwarding;

assign {ms_load     ,//44:44
		data_ok_fw  ,//43:43
		ms_inst_mfc0,//42:42
        ms_fw_valid, //41:41
        ms_fw_we   , //40:37
        ms_fw_dst  , //36:32
        ms_fw_data   //31:0
       } = ms_forwarding;

assign {ws_inst_mfc0,//106:106
		cause       ,//105:74
        status      ,//73:42
        ws_fw_valid ,//41:41
        ws_fw_we    ,//40:37
        ws_fw_dst   ,//36:32
        ws_fw_data   //31:0
       } = ws_forwarding;
wire [`BR_BUS_WD-1:0] br_bus;

assign br_bus       = {is_br,br_taken,br_target};
wire [ 4:0]      ds_rd;
assign      ds_rd = rd;
assign ds_to_es_bus = {if_refill   ,  //178:178
					   inst_tlbwi  ,  //177:177
					   inst_tlbp   ,  //176:176
					   inst_tlbr   ,  //175:175
					   id_bd       ,  //174:174
                       id_ex       ,  //173:173
					   id_excode   ,  //172:168
			           if_ex       ,  //167:167 
		               if_excode   ,  //166:162
					   ov_act      ,  //161:161
			           ds_rd       ,  //160:156
                       inst_mfc0   ,  //155:155
                       inst_mtc0   ,  //154:154
                       inst_eret   ,  //153:153
                       mem_sign    ,  //152:152
                       mem_align   ,  //151:150
                       mem_width   ,  //149:148
                       src2_is_imm_0, //147:147
					   inst_mult   ,  //146:146
			           inst_multu  ,  //145:145
                       inst_div    ,  //144:144
					   inst_divu   ,  //143:143
                       inst_mtlo   ,  //142:142
					   inst_mthi   ,  //141:141
					   inst_mflo   ,  //140:140
					   inst_mfhi   ,  //139:139
					   alu_op      ,  //138:127
                       load_op     ,  //126:126
                       src1_is_sa  ,  //125:125
                       src1_is_pc  ,  //124:124
                       src2_is_imm ,  //123:123
                       src2_is_8   ,  //122:122
                       gr_we       ,  //121:118
                       mem_we      ,  //117:117
                       dest        ,  //116:112
                       imm         ,  //111:96
                       rs_value    ,  //95 :64
                       rt_value    ,  //63 :32
                       ds_pc          //31 :0
                      };

assign ds_ready_go    = (~(ws_reflush | refetch) & (((load_valid || es_inst_mfc0) && ((forward_valid1 == 2'b00) || (forward_valid2 == 2'b00))) || (ms_inst_mfc0 && ((forward_valid1 == 2'b01) || (forward_valid2 == 2'b01))) || (ms_load && ~data_ok_fw && ((forward_valid1 == 2'b01) || (forward_valid2 == 2'b01))))) ? 1'b0 : 1'b1;
assign ds_allowin     = !ds_valid || ds_ready_go && es_allowin;
assign ds_to_es_valid = ds_valid && ds_ready_go;
always @(posedge clk) begin
    if (reset) begin
        ds_valid <= 1'b0;
    end
    else if(ws_reflush | refetch) begin
        ds_valid <= 1'b0;
    end
    else if (ds_allowin) begin
        ds_valid <= fs_to_ds_valid;
    end
end
always @(posedge clk) begin
    if (fs_to_ds_valid && ds_allowin) begin
        fs_to_ds_bus_r <= fs_to_ds_bus;
    end
end

assign op   = ds_inst[31:26];
assign rs   = ds_inst[25:21];
assign rt   = ds_inst[20:16];
assign rd   = ds_inst[15:11];
assign sa   = ds_inst[10: 6];
assign func = ds_inst[ 5: 0];
assign imm  = ds_inst[15: 0];
assign jidx = ds_inst[25: 0];

decoder_6_64 u_dec0(.in(op  ), .out(op_d  ));
decoder_6_64 u_dec1(.in(func), .out(func_d));
decoder_5_32 u_dec2(.in(rs  ), .out(rs_d  ));
decoder_5_32 u_dec3(.in(rt  ), .out(rt_d  ));
decoder_5_32 u_dec4(.in(rd  ), .out(rd_d  ));
decoder_5_32 u_dec5(.in(sa  ), .out(sa_d  ));

assign inst_addu   = op_d[6'h00] & func_d[6'h21] & sa_d[5'h00];
assign inst_subu   = op_d[6'h00] & func_d[6'h23] & sa_d[5'h00];
assign inst_slt    = op_d[6'h00] & func_d[6'h2a] & sa_d[5'h00];
assign inst_sltu   = op_d[6'h00] & func_d[6'h2b] & sa_d[5'h00];
assign inst_and    = op_d[6'h00] & func_d[6'h24] & sa_d[5'h00];
assign inst_or     = op_d[6'h00] & func_d[6'h25] & sa_d[5'h00];
assign inst_xor    = op_d[6'h00] & func_d[6'h26] & sa_d[5'h00];
assign inst_nor    = op_d[6'h00] & func_d[6'h27] & sa_d[5'h00];
assign inst_sll    = op_d[6'h00] & func_d[6'h00] & rs_d[5'h00];
assign inst_srl    = op_d[6'h00] & func_d[6'h02] & rs_d[5'h00];
assign inst_sra    = op_d[6'h00] & func_d[6'h03] & rs_d[5'h00];
assign inst_addiu  = op_d[6'h09];
assign inst_lui    = op_d[6'h0f] & rs_d[5'h00];
assign inst_lw     = op_d[6'h23];
assign inst_sw     = op_d[6'h2b];
assign inst_beq    = op_d[6'h04];
assign inst_bne    = op_d[6'h05];
assign inst_jal    = op_d[6'h03];
assign inst_jr     = op_d[6'h00] & func_d[6'h08] & rt_d[5'h00] & rd_d[5'h00] & sa_d[5'h00];
assign inst_mult   = op_d[6'h00] & func_d[6'h18] & rd_d[5'h00] & sa_d[5'h00];
assign inst_multu  = op_d[6'h00] & func_d[6'h19] & rd_d[5'h00] & sa_d[5'h00];
assign inst_div    = op_d[6'h00] & func_d[6'h1a] & rd_d[5'h00] & sa_d[5'h00];
assign inst_divu   = op_d[6'h00] & func_d[6'h1b] & rd_d[5'h00] & sa_d[5'h00];
assign inst_mfhi   = op_d[6'h00] & func_d[6'h10] & rt_d[5'h00] & rs_d[5'h00] & sa_d[5'h00];
assign inst_mthi   = op_d[6'h00] & func_d[6'h11] & rt_d[5'h00] & rd_d[5'h00] & sa_d[5'h00];
assign inst_mflo   = op_d[6'h00] & func_d[6'h12] & rt_d[5'h00] & rs_d[5'h00] & sa_d[5'h00];
assign inst_mtlo   = op_d[6'h00] & func_d[6'h13] & rt_d[5'h00] & rd_d[5'h00] & sa_d[5'h00];
assign inst_add    = op_d[6'h00] & func_d[6'h20] & sa_d[5'h00];
assign inst_sub    = op_d[6'h00] & func_d[6'h22] & sa_d[5'h00];
assign inst_sllv   = op_d[6'h00] & func_d[6'h04] & sa_d[5'h00];
assign inst_srlv   = op_d[6'h00] & func_d[6'h06] & sa_d[5'h00];
assign inst_srav   = op_d[6'h00] & func_d[6'h07] & sa_d[5'h00];
assign inst_slti   = op_d[6'h0a];
assign inst_sltiu  = op_d[6'h0b];
assign inst_andi   = op_d[6'h0c];
assign inst_ori    = op_d[6'h0d];
assign inst_xori   = op_d[6'h0e];
assign inst_addi   = op_d[6'h08];
assign inst_bgez   = op_d[6'h01] & rt_d[5'h01];
assign inst_bgtz   = op_d[6'h07] & rt_d[5'h00];
assign inst_blez   = op_d[6'h06] & rt_d[5'h00];
assign inst_bltz   = op_d[6'h01] & rt_d[5'h00];
assign inst_bgezal = op_d[6'h01] & rt_d[5'h11];
assign inst_bltzal = op_d[6'h01] & rt_d[5'h10];
assign inst_j      = op_d[6'h02];
assign inst_jalr   = op_d[6'h00] & func_d[6'h09] & rt_d[5'h00] & sa_d[5'h00];
assign inst_lb     = op_d[6'h20];
assign inst_lh     = op_d[6'h21];
assign inst_lwl    = op_d[6'h22];
assign inst_lbu    = op_d[6'h24];
assign inst_lhu    = op_d[6'h25];
assign inst_lwr    = op_d[6'h26];
assign inst_sb     = op_d[6'h28];
assign inst_sh     = op_d[6'h29];
assign inst_swl    = op_d[6'h2a];
assign inst_swr    = op_d[6'h2e];
assign inst_mfc0   = op_d[6'h10] & rs_d[5'h00];
assign inst_mtc0   = op_d[6'h10] & rs_d[5'h04];
assign inst_eret   = op_d[6'h10] & func_d[6'h18];
assign inst_syscall= op_d[6'h00] & func_d[6'h0c];
assign inst_break  = op_d[6'h00] & func_d[6'h0d];
assign inst_reserve= ~(inst_addu | inst_subu | inst_slt | inst_sltu | inst_and | inst_or | inst_xor | inst_nor | inst_sll | inst_srl | inst_sra | inst_addiu | inst_lui | inst_lw | inst_sw | inst_beq | inst_bne | inst_jal | inst_jr | inst_mult | inst_multu | inst_div | inst_divu | inst_mfhi | inst_mthi | inst_mflo | inst_mtlo | inst_add | inst_sub | inst_sllv | inst_srlv | inst_srav | inst_slti | inst_sltiu | inst_andi | inst_ori | inst_xori | inst_addi | inst_bgez | inst_bgtz | inst_blez | inst_bltz | inst_bgezal | inst_bltzal | inst_j | inst_jalr | inst_lb | inst_lh | inst_lwl | inst_lwr | inst_lbu | inst_lhu | inst_sb | inst_sh | inst_swl | inst_swr | inst_mfc0 | inst_mtc0 | inst_eret | inst_syscall | inst_break | inst_tlbwi | inst_tlbr | inst_tlbp);
assign inst_tlbr   = op_d[6'h10] & func_d[6'h01] & rs_d[5'h10] & rt_d[5'h00] & rd_d[5'h00] & sa_d[5'h00];
assign inst_tlbp   = op_d[6'h10] & func_d[6'h08] & rs_d[5'h10] & rt_d[5'h00] & rd_d[5'h00] & sa_d[5'h00];
assign inst_tlbwi   = op_d[6'h10] & func_d[6'h02] & rs_d[5'h10] & rt_d[5'h00] & rd_d[5'h00] & sa_d[5'h00];


assign alu_op[ 0] = inst_addu | inst_add | inst_addiu | inst_addi | inst_jal | inst_jalr | inst_bgezal | inst_bltzal | load_op | store_op;//1
assign alu_op[ 1] = inst_subu | inst_sub;
assign alu_op[ 2] = inst_slt  | inst_slti;
assign alu_op[ 3] = inst_sltu | inst_sltiu;
assign alu_op[ 4] = inst_and  | inst_andi;
assign alu_op[ 5] = inst_nor;
assign alu_op[ 6] = inst_or   | inst_ori;
assign alu_op[ 7] = inst_xor  | inst_xori;
assign alu_op[ 8] = inst_sll  | inst_sllv;
assign alu_op[ 9] = inst_srl  | inst_srlv;
assign alu_op[10] = inst_sra  | inst_srav;
assign alu_op[11] = inst_lui;

assign src1_is_sa   = inst_sll   | inst_srl  | inst_sra;
assign src1_is_pc   = inst_jal   | inst_jalr | inst_bgezal | inst_bltzal;
assign src2_is_imm  = inst_addiu | inst_addi | inst_lui | inst_slti | inst_sltiu | load_op | store_op;
assign src2_is_imm_0= inst_andi  | inst_ori  | inst_xori;
assign src2_is_8    = inst_jal   | inst_jalr | inst_bgezal | inst_bltzal;
assign res_from_mem = load_op;
assign load_op      = inst_lw    | inst_lb   | inst_lbu    | inst_lh | inst_lhu | inst_lwr | inst_lwl;
assign store_op     = inst_sw    | inst_sb   | inst_swl    | inst_sh | inst_swr;
assign dst_is_r31   = inst_jal   | inst_bgezal | inst_bltzal;
assign dst_is_rt    = inst_addiu | inst_addi | inst_lui | inst_slti | inst_sltiu | inst_andi | inst_ori | inst_xori | load_op | inst_mfc0;
assign gr_we        = (~store_op & ~inst_beq & ~inst_bne & ~inst_jr & ~inst_mult & ~inst_multu & ~inst_div & ~inst_divu & ~inst_mthi & ~inst_mtlo & ~inst_bgez & ~inst_bltz & ~inst_bgtz & ~inst_blez & ~inst_j & ~inst_mtc0 & ~inst_eret & ~inst_syscall) ? 4'hf : 4'h0;
assign mem_we       = store_op;
assign ov_act       = inst_add | inst_sub | inst_addi;
assign dest         = dst_is_r31 ? 5'd31 :
                      dst_is_rt  ? rt    : 
                                   rd;

assign rf_raddr1 = rs;
assign rf_raddr2 = rt;
regfile u_regfile(
    .clk    (clk      ),
    .raddr1 (rf_raddr1),
    .rdata1 (rf_rdata1),
    .raddr2 (rf_raddr2),
    .rdata2 (rf_rdata2),
    .we     (rf_we    ),
    .waddr  (rf_waddr ),
    .wdata  (rf_wdata )
    );

assign forward_valid_rs_0 = (es_fw_valid & es_fw_we[0] & (es_fw_dst == rf_raddr1)) ? 2'b00
                      	  : (ms_fw_valid & ms_fw_we[0] & (ms_fw_dst == rf_raddr1)) ? 2'b01
                          : (ws_fw_valid & ws_fw_we[0] & (ws_fw_dst == rf_raddr1)) ? 2'b10
                      	  :                                                          2'b11;

assign forward_valid_rs_1 = (es_fw_valid & es_fw_we[1] & (es_fw_dst == rf_raddr1)) ? 2'b00
                          : (ms_fw_valid & ms_fw_we[1] & (ms_fw_dst == rf_raddr1)) ? 2'b01
                          : (ws_fw_valid & ws_fw_we[1] & (ws_fw_dst == rf_raddr1)) ? 2'b10
                          :                                                          2'b11;


assign forward_valid_rs_2 = (es_fw_valid & es_fw_we[2] & (es_fw_dst == rf_raddr1)) ? 2'b00
                          : (ms_fw_valid & ms_fw_we[2] & (ms_fw_dst == rf_raddr1)) ? 2'b01
                          : (ws_fw_valid & ws_fw_we[2] & (ws_fw_dst == rf_raddr1)) ? 2'b10
                          :                                                          2'b11;

assign forward_valid_rs_3 = (es_fw_valid & es_fw_we[3] & (es_fw_dst == rf_raddr1)) ? 2'b00
                          : (ms_fw_valid & ms_fw_we[3] & (ms_fw_dst == rf_raddr1)) ? 2'b01
                          : (ws_fw_valid & ws_fw_we[3] & (ws_fw_dst == rf_raddr1)) ? 2'b10
                          :                                                          2'b11;

assign forward_valid_rt_0 = (es_fw_valid & es_fw_we[0] & (es_fw_dst == rf_raddr2)) ? 2'b00
                      	  : (ms_fw_valid & ms_fw_we[0] & (ms_fw_dst == rf_raddr2)) ? 2'b01
                          : (ws_fw_valid & ws_fw_we[0] & (ws_fw_dst == rf_raddr2)) ? 2'b10
                      	  :                                                          2'b11;

assign forward_valid_rt_1 = (es_fw_valid & es_fw_we[1] & (es_fw_dst == rf_raddr2)) ? 2'b00
                          : (ms_fw_valid & ms_fw_we[1] & (ms_fw_dst == rf_raddr2)) ? 2'b01
                          : (ws_fw_valid & ws_fw_we[1] & (ws_fw_dst == rf_raddr2)) ? 2'b10
                          :                                                          2'b11;


assign forward_valid_rt_2 = (es_fw_valid & es_fw_we[2] & (es_fw_dst == rf_raddr2)) ? 2'b00
                          : (ms_fw_valid & ms_fw_we[2] & (ms_fw_dst == rf_raddr2)) ? 2'b01
                          : (ws_fw_valid & ws_fw_we[2] & (ws_fw_dst == rf_raddr2)) ? 2'b10
                          :                                                          2'b11;

assign forward_valid_rt_3 = (es_fw_valid & es_fw_we[3] & (es_fw_dst == rf_raddr2)) ? 2'b00
                          : (ms_fw_valid & ms_fw_we[3] & (ms_fw_dst == rf_raddr2)) ? 2'b01
                          : (ws_fw_valid & ws_fw_we[3] & (ws_fw_dst == rf_raddr2)) ? 2'b10
                          :                                                          2'b11;

assign forward_valid1 = (forward_valid_rs_0 ==2'b00 | forward_valid_rs_1 ==2'b00 | forward_valid_rs_2 ==2'b00 | 	forward_valid_rs_3 ==2'b00) ? 2'b00
                      : (forward_valid_rs_0 ==2'b01 | forward_valid_rs_1 ==2'b01 | forward_valid_rs_2 ==2'b01 | forward_valid_rs_3 ==2'b01) ? 2'b01
                      : (forward_valid_rs_0 ==2'b10 | forward_valid_rs_1 ==2'b10 | forward_valid_rs_2 ==2'b10 | forward_valid_rs_3 ==2'b10) ? 2'b10
                      : 2'b11;

assign forward_valid2 = (forward_valid_rt_0 ==2'b00 | forward_valid_rt_1 ==2'b00 | forward_valid_rt_2 ==2'b00 | forward_valid_rt_3 ==2'b00) ? 2'b00
                      : (forward_valid_rt_0 ==2'b01 | forward_valid_rt_1 ==2'b01 | forward_valid_rt_2 ==2'b01 | forward_valid_rt_3 ==2'b01) ? 2'b01
                      : (forward_valid_rt_0 ==2'b10 | forward_valid_rt_1 ==2'b10 | forward_valid_rt_2 ==2'b10 | forward_valid_rt_3 ==2'b10) ? 2'b10
                      : 2'b11;

assign fw_wdata_rs_0 = (forward_valid_rs_0 == 2'b00) ? es_fw_data[ 7:0]
					 : (forward_valid_rs_0 == 2'b01) ? ms_fw_data[ 7:0]
					 : (forward_valid_rs_0 == 2'b10) ? ws_fw_data[ 7:0]
					 : 								                 rf_rdata1 [ 7:0];


assign fw_wdata_rs_1 = (forward_valid_rs_1 == 2'b00) ? es_fw_data[15:8]
					 : (forward_valid_rs_1 == 2'b01) ? ms_fw_data[15:8]
					 : (forward_valid_rs_1 == 2'b10) ? ws_fw_data[15:8]
					 : 								                 rf_rdata1 [15:8];


assign fw_wdata_rs_2 = (forward_valid_rs_2 == 2'b00) ? es_fw_data[23:16]
					 : (forward_valid_rs_2 == 2'b01) ? ms_fw_data[23:16]
					 : (forward_valid_rs_2 == 2'b10) ? ws_fw_data[23:16]
					 :								                 rf_rdata1 [23:16];


assign fw_wdata_rs_3 = (forward_valid_rs_3 == 2'b00) ? es_fw_data[31:24]
					 : (forward_valid_rs_3 == 2'b01) ? ms_fw_data[31:24]
					 : (forward_valid_rs_3 == 2'b10) ? ws_fw_data[31:24]
					 : 								                 rf_rdata1 [31:24];


assign fw_wdata_rt_0 = (forward_valid_rt_0 == 2'b00) ? es_fw_data[ 7:0]
					 : (forward_valid_rt_0 == 2'b01) ? ms_fw_data[ 7:0]
					 : (forward_valid_rt_0 == 2'b10) ? ws_fw_data[ 7:0]
					 : 								                 rf_rdata2 [ 7:0];


assign fw_wdata_rt_1 = (forward_valid_rt_1 == 2'b00) ? es_fw_data[15:8]
					 : (forward_valid_rt_1 == 2'b01) ? ms_fw_data[15:8]
					 : (forward_valid_rt_1 == 2'b10) ? ws_fw_data[15:8]
					 : 						              		   rf_rdata2 [15:8];


assign fw_wdata_rt_2 = (forward_valid_rt_2 == 2'b00) ? es_fw_data[23:16]
					 : (forward_valid_rt_2 == 2'b01) ? ms_fw_data[23:16]
					 : (forward_valid_rt_2 == 2'b10) ? ws_fw_data[23:16]
					 :								                 rf_rdata2 [23:16];


assign fw_wdata_rt_3 = (forward_valid_rt_3 == 2'b00) ? es_fw_data[31:24]
					 : (forward_valid_rt_3 == 2'b01) ? ms_fw_data[31:24]
					 : (forward_valid_rt_3 == 2'b10) ? ws_fw_data[31:24]
					 : 								                 rf_rdata2 [31:24];                
                                                          
assign rs_value = (rs == 5'b0) ? 32'b0
                : {fw_wdata_rs_3, fw_wdata_rs_2, fw_wdata_rs_1, fw_wdata_rs_0};

assign rt_value = (rt == 5'b0) ? 32'b0
                : {fw_wdata_rt_3, fw_wdata_rt_2, fw_wdata_rt_1, fw_wdata_rt_0};


assign rs_eq_rt   = (rs_value == rt_value);
assign rs_le_zero = ($signed(rs_value) < 0);//[rs]<0
assign rs_gt_zero = ($signed(rs_value) > 0);//[rs]>0
assign br_taken = (   inst_beq    &&  rs_eq_rt
                   || inst_bne    && !rs_eq_rt
                   || inst_bgez   && !rs_le_zero
                   || inst_bgtz   &&  rs_gt_zero
                   || inst_blez   && !rs_gt_zero
                   || inst_bltz   &&  rs_le_zero
                   || inst_bgezal && !rs_le_zero
                   || inst_bltzal &&  rs_le_zero 
                   || inst_jal
                   || inst_jr
                   || inst_jalr
                   || inst_j
                  )&& ds_valid;
assign br_target = (inst_beq || inst_bne || inst_bgez || inst_bltz || inst_bgtz || inst_blez || inst_bgezal || inst_bltzal) ? (fs_pc_br + {{14{imm[15]}}, imm[15:0], 2'b0}) :
                   (inst_jr || inst_jalr) ? rs_value :
                  /*inst_jal || inst_j*/              {fs_pc_br[31:28], jidx[25:0], 2'b0};

wire [31:0] fs_pc_br;
assign fs_pc_br = ds_pc+4;

assign is_br = (inst_beq || inst_bne || inst_bgez || inst_bgtz || inst_blez || inst_bltz || inst_bgezal || inst_bltzal || inst_jal || inst_jr || inst_jalr || inst_j) & ds_valid;

assign mem_width = ({2{inst_lb | inst_lbu | inst_sb}}                                  & 2'b01)
                 | ({2{inst_lh | inst_lhu | inst_sh}}                                  & 2'b10)
                 | ({2{inst_lw | inst_lwr | inst_sw | inst_lwl | inst_swr | inst_swl}} & 2'b11)
                 |                                                                       2'b00;

assign mem_sign  = (inst_lbu | inst_lhu) ? 1'b0 : 1'b1;
assign mem_align = ({2{inst_lw  | inst_sw }} & 2'b01)
                 | ({2{inst_lwl | inst_swl}} & 2'b10)
                 | ({2{inst_lwr | inst_swr}} & 2'b11)
                 |                             2'b00;

assign time_int = cause[15] & status[15] & ~status[1] & status[0];
assign sof_int1 = cause[9]  & status[9]  & ~status[1] & status[0];
assign sof_int0 = cause[8]  & status[8]  & ~status[1] & status[0];

assign id_excode = (time_int | sof_int1 | sof_int0) ? `Int
               	 : inst_reserve ? `Ri
                 : inst_syscall ? `Sys
                 : inst_break   ? `Bp
         	     : `Int;	

assign id_ex = (time_int | sof_int1 | sof_int0 | inst_reserve | inst_syscall | inst_break) & ds_valid;

reg [`BR_BUS_WD-1:0]br_bus_r;
reg                 br_hold_valid;
assign br_bus_hold = br_hold_valid ? br_bus_r : br_bus;

always @(posedge clk)begin
    if(reset) begin
        br_hold_valid <= 1'b0;
    end
    else if(ws_reflush | refetch) begin
        br_hold_valid <= 1'b0;
    end
    else if((br_bus_hold[33] && fs_valid && fs_allowin) && pre_fs_ready_go) begin
        br_hold_valid <= 1'b0;
    end
    else if((is_br && ds_to_es_valid && es_allowin)) begin
        br_hold_valid <= 1'b1;
    end
end

always @(posedge clk)begin
    if(is_br && ds_to_es_valid && es_allowin)  begin
        br_bus_r <= br_bus;
    end
end

assign ds_ref_inst = ds_valid & (inst_tlbwi | inst_tlbr);

endmodule
