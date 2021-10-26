`include "mycpu.h"

module wb_stage(
    input                           clk           ,
    input                           reset         ,
    //allowin
    output                          ws_allowin    ,
    //from ms
    input                           ms_to_ws_valid,
    input  [`MS_TO_WS_BUS_WD  -1:0] ms_to_ws_bus  ,
    //to rf: for write back
    output [`WS_TO_RF_BUS_WD  -1:0] ws_to_rf_bus  ,
    output [`WS_FORWARDING_BUS_WD -1:0] ws_forwarding ,
    //trace debug interface
    output [31:0] debug_wb_pc     ,
    output [ 3:0] debug_wb_rf_wen ,
    output [ 4:0] debug_wb_rf_wnum,
    output [31:0] debug_wb_rf_wdata,
    //exception reflush
    output                          ws_reflush,
    output [`WS_TO_FS_BUS_WD  -1:0] ws_to_fs_bus,
    output                          refetch,
    input                           to_refetch,
    output                          ws_ref_inst,
    input  [`FS_TLB_BUS_WD        -1:0] fs_tlb_bus,
    input  [`ES_TLB_BUS_WD        -1:0] es_tlb_bus,
    output [`WS_TO_ES_BUS_WD  -1:0] ws_to_es_bus
);

reg         ws_valid;
wire        ws_ready_go;
wire        inst_mfc0;
wire        inst_mtc0;
wire        inst_eret;
wire        inst_syscall;
wire [31:0] rt_value;

wire        if_ex;
wire        id_ex;
wire        exe_ex;
wire        mem_ex;
wire [ 4:0] if_excode;
wire [ 4:0] id_excode;
wire [ 4:0] exe_excode;
wire [ 4:0] mem_excode;
wire [31:0] mem_addr;
wire        if_refill;
wire        exe_refill;

reg [`MS_TO_WS_BUS_WD -1:0] ms_to_ws_bus_r;
wire [ 3:0] ws_gr_we;
wire [ 4:0] ws_dest;
wire [31:0] ws_final_result;
wire [31:0] ws_pc;
wire [ 4:0] ws_rd;
wire        ws_bd;
wire        wb_bd;
assign {exe_refill     ,  //174:174
        if_refill      ,  //173:173
        inst_tlbwi     ,  //172:172
        inst_tlbp      ,  //171:171 
        inst_tlbr      ,  //170:170
        ws_bd          ,  //169:169
		mem_addr       ,  //168:137
		mem_ex         ,  //136:136
		mem_excode     ,  //135:131		   
		exe_ex         ,  //130:130
		exe_excode     ,  //129:125
		id_ex          ,  //124:124
	    id_excode      ,  //123:119
		if_ex          ,  //118:118
		if_excode      ,  //117:113
		ws_rd          ,  //112:108
        rt_value       ,  //107:76
		inst_mfc0      ,  //75:75
        inst_mtc0      ,  //74:75
        inst_eret      ,  //73:73
        ws_gr_we       ,  //72:69
        ws_dest        ,  //68:64
        ws_final_result,  //63:32
        ws_pc             //31:0
       } = ms_to_ws_bus_r;

wire [ 3:0] rf_we;
wire [ 4:0] rf_waddr;
wire [31:0] rf_wdata;
assign ws_to_rf_bus = {rf_we   ,  //40:37
                       rf_waddr,  //36:32
                       rf_wdata   //31:0
                      };
wire [31:0] epc;
wire [31:0] status;
wire [31:0] cause;
wire [ 3:0] index_index;
wire        index_p;
wire [ 7:0] entryhi_asid;
wire [18:0] entryhi_vpn2;
wire [19:0] entrylo0_pfn0;
wire [ 2:0] entrylo0_c0;
wire        entrylo0_d0;
wire        entrylo0_v0;
wire        entrylo0_g0;
wire [19:0] entrylo1_pfn1;
wire [ 2:0] entrylo1_c1;
wire        entrylo1_d1;
wire        entrylo1_v1;
wire        entrylo1_g1;
wire [31:0] index;
wire [31:0] entryhi;
wire [31:0] entrylo0;
wire [31:0] entrylo1;
wire        refill;

assign refill = (ws_valid & wb_ex & ((wb_excode == `Tlbl & if_refill) | ((wb_excode == `Tlbl | wb_excode == `Tlbs) & exe_refill)));

assign ws_to_fs_bus = {refill,
                       s0_found,
                       s0_v,
                       s0_pfn,
                       ws_pc,
                       inst_eret & ws_valid,
                       epc                  
                      };

assign ws_to_es_bus = {s1_found,
                       s1_v,
                       s1_d,
                       s1_pfn
                      } ;                      

assign ws_forwarding = {inst_mfc0   ,    //106:106
						cause       ,    //105:74
                        status      ,    //73:42
                        ws_valid    ,    //41:41
                        ws_to_rf_bus     //40:0
                       };
//CP0 parameters
wire [31:0]   c0_wdata;
wire [31:0]   c0_rdata;
wire [ 4:0]   c0_waddr;
wire [ 4:0]   c0_raddr;
wire           mtc0_we;
wire        eret_flush;
wire             wb_ex;
//wire [ 5:0] ext_int_in;
wire [ 4:0] wb_excode;
wire [31:0] wb_pc;
wire [31:0] wb_badvaddr;
wire        tlb_ex;

assign ws_ref_inst = ws_valid & (inst_tlbwi | inst_tlbr);
assign refetch     = ws_valid & to_refetch;
assign ws_ready_go = 1'b1;
assign ws_allowin  = !ws_valid || ws_ready_go;
always @(posedge clk) begin
    if (reset) begin
        ws_valid <= 1'b0;
    end
    else if(ws_reflush | refetch) begin
        ws_valid <= 1'b0;
    end
    else if (ws_allowin) begin
        ws_valid <= ms_to_ws_valid;
    end

    if (ms_to_ws_valid && ws_allowin) begin
        ms_to_ws_bus_r <= ms_to_ws_bus;
    end
end

assign rf_we    = ws_gr_we & {4{ws_valid}} & {4{~ws_reflush}} & {4{~to_refetch}};
assign rf_waddr = ws_dest;
assign rf_wdata = inst_mfc0 ? c0_rdata : ws_final_result;

// debug info generate
assign debug_wb_pc       = ws_pc;
assign debug_wb_rf_wen   = rf_we;
assign debug_wb_rf_wnum  = ws_dest;
assign debug_wb_rf_wdata = rf_wdata;

assign c0_wdata   = rt_value;
assign c0_waddr   = ({5{inst_mtc0}} & ws_dest);              
assign c0_raddr   = ({5{inst_mfc0}} & ws_rd);
assign mtc0_we    = inst_mtc0 && ws_valid && !wb_ex;
assign wb_ex      = (if_ex | id_ex | exe_ex | mem_ex) & ws_valid;
assign ws_reflush = (if_ex | id_ex | exe_ex | mem_ex | inst_eret) & ws_valid;
assign wb_excode  = (id_ex  & id_excode  == `Int ) ? `Int
				  : (if_ex  & if_excode  == `AdEL) ? `AdEL
                  : (if_ex  & if_excode  == `Tlbl) ? `Tlbl
				  : (id_ex                       ) ? id_excode
				  : (exe_ex                      ) ? exe_excode
				  :									 `Int;
assign wb_pc      = ws_pc;
assign wb_bd      = ws_bd;
assign wb_badvaddr= (if_ex  & (if_excode  == `AdEL || if_excode == `Tlbl)) ? wb_pc : mem_addr;
assign tlb_ex     = wb_excode == `Tlbl || wb_excode == `Tlbs || wb_excode == `Mod;
assign eret_flush = inst_eret;

assign tlbr_we  = inst_tlbr && ws_valid && !wb_ex;
assign tlbp_we  = es_tlbp && !wb_ex;
assign index    = ((inst_tlbwi | inst_tlbr) && ws_valid) ? {index_p, {27{1'b0}}, index_index} : {~s1_found, {27{1'b0}}, s1_index};
assign entryhi  = tlb_ex ? {wb_badvaddr[31:13], {5{1'b0}}, r_asid} : {r_vpn2, {5{1'b0}}, r_asid};
assign entrylo0 = {{6{1'b0}}, r_pfn0, r_c0, r_d0, r_v0, r_g};
assign entrylo1 = {{6{1'b0}}, r_pfn1, r_c1, r_d1, r_v1, r_g};

cp0 u_cp0(
    .clk          (          clk),
    .reset        (        reset),
    .c0_wdata     (     c0_wdata),
    .c0_rdata     (     c0_rdata),
    .epc          (          epc),
    .status       (       status),
    .cause        (        cause),
    .c0_waddr     (     c0_waddr),
    .c0_raddr     (     c0_raddr),
    .mtc0_we      (      mtc0_we),
    .eret_flush   (   eret_flush),
    .wb_ex        (        wb_ex),
    .wb_bd        (        wb_bd),
    .wb_badvaddr  (  wb_badvaddr),
    .wb_excode    (    wb_excode),
    .wb_pc        (        wb_pc),
    .tlbr_we      (      tlbr_we),
    .tlbp_we      (      tlbp_we),
    .index        (        index),
    .entryhi      (      entryhi),
    .entrylo0     (     entrylo0),
    .entrylo1     (     entrylo1),
    .tlb_ex       (       tlb_ex),
    .index_index  (  index_index),
    .index_p      (      index_p),
    .entryhi_vpn2 ( entryhi_vpn2),
    .entryhi_asid ( entryhi_asid),
    .entrylo0_pfn0(entrylo0_pfn0),
    .entrylo0_c0  (  entrylo0_c0),
    .entrylo0_d0  (  entrylo0_d0),
    .entrylo0_v0  (  entrylo0_v0),
    .entrylo0_g0  (  entrylo0_g0),
    .entrylo1_pfn1(entrylo1_pfn1),
    .entrylo1_c1  (  entrylo1_c1),
    .entrylo1_d1  (  entrylo1_d1),
    .entrylo1_v1  (  entrylo1_v1),
    .entrylo1_g1  (  entrylo1_g1)
);


// TLB
//search0
wire [18:0] s0_vpn2;
wire        s0_odd_page;
wire [ 7:0] s0_asid;
wire        s0_found;
wire [ 3:0] s0_index;
wire [19:0] s0_pfn;
wire [ 2:0] s0_c;
wire        s0_d;
wire        s0_v;
//search1
wire [18:0] s1_vpn2;
wire        s1_odd_page;
wire [ 7:0] s1_asid;
wire        s1_found;
wire [ 3:0] s1_index;
wire [19:0] s1_pfn;
wire [ 2:0] s1_c;
wire        s1_d;
wire        s1_v;
//write
wire        we;
wire [ 3:0] w_index;
wire [18:0] w_vpn2;
wire [ 7:0] w_asid;
wire        w_g;
wire [19:0] w_pfn0;
wire [ 2:0] w_c0;
wire        w_d0;
wire        w_v0;
wire [19:0] w_pfn1;
wire [ 2:0] w_c1;
wire        w_d1;
wire        w_v1;
//read
wire [ 3:0] r_index;
wire [18:0] r_vpn2;
wire [ 7:0] r_asid;
wire        r_g;
wire [19:0] r_pfn0;
wire [ 2:0] r_c0;
wire        r_d0;
wire        r_v0;
wire [19:0] r_pfn1;
wire [ 2:0] r_c1;
wire        r_d1;
wire        r_v1;

//search0
wire       if_refill_bd;
wire       if_refill_if;
wire [31:0]if_refill_pc;
wire       if_refill_valid;

assign {//if_refill_valid,
        //if_refill_no_map,
        //if_refill_bd,
        //if_refill_pc,
        s0_vpn2,
        s0_odd_page
        } = fs_tlb_bus;
assign s0_asid = entryhi_asid;

//search1
wire [18:0] s1_vpn2_tmp;
wire        s1_odd_page_tmp;
wire        es_tlbp;
assign {es_tlbp,
        s1_vpn2_tmp,
        s1_odd_page_tmp
        } = es_tlb_bus;
assign s1_asid = entryhi_asid;
assign s1_vpn2 = es_tlbp ? entryhi_vpn2 : s1_vpn2_tmp;
assign s1_odd_page = es_tlbp ? 1'b0 : s1_odd_page_tmp;

//read
assign r_index = index_index;

//write
assign we      = inst_tlbwi & ws_valid & ~wb_ex;
assign w_index = index_index;
assign w_vpn2  = entryhi_vpn2;
assign w_asid  = entryhi_asid;
assign w_g     = entrylo1_g1 & entrylo0_g0;
assign w_pfn0  = entrylo0_pfn0;
assign w_c0    = entrylo0_c0;
assign w_d0    = entrylo0_d0;
assign w_v0    = entrylo0_v0;
assign w_pfn1  = entrylo1_pfn1;
assign w_c1    = entrylo1_c1;
assign w_d1    = entrylo1_d1;
assign w_v1    = entrylo1_v1;

tlb u_tlb(
    .clk        (clk        ),
    .s0_vpn2    (s0_vpn2    ),
    .s0_odd_page(s0_odd_page),
    .s0_asid    (s0_asid    ),
    .s0_found   (s0_found   ),
    .s0_index   (s0_index   ),
    .s0_pfn     (s0_pfn     ),
    .s0_c       (s0_c       ),
    .s0_d       (s0_d       ),
    .s0_v       (s0_v       ),

    .s1_vpn2    (s1_vpn2    ),
    .s1_odd_page(s1_odd_page),
    .s1_asid    (s1_asid    ),
    .s1_found   (s1_found   ),
    .s1_index   (s1_index   ),
    .s1_pfn     (s1_pfn     ),
    .s1_c       (s1_c       ),
    .s1_d       (s1_d       ),
    .s1_v       (s1_v       ),

    .we         (we         ),
    .w_index    (w_index    ),
    .w_vpn2     (w_vpn2     ),
    .w_asid     (w_asid     ),
    .w_g        (w_g        ),
    .w_pfn0     (w_pfn0     ),
    .w_c0       (w_c0       ),
    .w_d0       (w_d0       ),
    .w_v0       (w_v0       ),
    .w_pfn1     (w_pfn1     ),
    .w_c1       (w_c1       ),
    .w_d1       (w_d1       ),
    .w_v1       (w_v1       ),

    .r_index    (r_index    ),
    .r_vpn2     (r_vpn2     ),
    .r_asid     (r_asid     ),
    .r_g        (r_g        ),
    .r_pfn0     (r_pfn0     ),
    .r_c0       (r_c0       ),
    .r_d0       (r_d0       ),
    .r_v0       (r_v0       ),
    .r_pfn1     (r_pfn1     ),
    .r_c1       (r_c1       ),
    .r_d1       (r_d1       ),
    .r_v1       (r_v1       )
);
endmodule
