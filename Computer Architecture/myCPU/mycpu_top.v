module mycpu_top(
    input         int,//high active

    input         aclk,
    input         aresetn,//low active

    output [ 3:0] arid,    
    output [31:0] araddr,  
    output [ 7:0] arlen,   
    output [ 2:0] arsize,  
    output [ 1:0] arburst,  
    output [ 1:0] arlock,   
    output [ 3:0] arcache,  
    output [ 2:0] arprot,   
    output        arvalid,  
    input         arready,  
                
    input  [ 3:0] rid,       
    input  [31:0] rdata,   
    input  [ 1:0] rresp,    
    input         rlast,    
    input         rvalid,  
    output        rready,    
               
    output [ 3:0] awid,      
    output [31:0] awaddr,   
    output [ 7:0] awlen,    
    output [ 2:0] awsize,   
    output [ 1:0] awburst,  
    output [ 1:0] awlock,   
    output [ 3:0] awcache,  
    output [ 2:0] awprot,   
    output        awvalid,  
    input         awready,   

    output [ 3:0] wid,     
    output [31:0] wdata,     
    output [ 3:0] wstrb,    
    output        wlast,   
    output        wvalid,   
    input         wready,   
    
    input  [ 3:0] bid,       
    input  [ 1:0] bresp,     
    input         bvalid,  
    output        bready, 

    // trace debug interface
    output [31:0] debug_wb_pc,
    output [ 3:0] debug_wb_rf_wen,
    output [ 4:0] debug_wb_rf_wnum,
    output [31:0] debug_wb_rf_wdata
);

// inst sram interface
wire        inst_sram_req;
wire        inst_sram_wr;
wire [ 1:0] inst_sram_size;
wire [ 3:0] inst_sram_wstrb;
wire [31:0] inst_sram_addr;
wire [31:0] inst_sram_wdata;
wire        inst_sram_addr_ok;
wire        inst_sram_data_ok;
wire [31:0] inst_sram_rdata;
// data sram interface
wire        data_sram_req;
wire        data_sram_wr ;
wire [ 1:0] data_sram_size;
wire [ 3:0] data_sram_wstrb;
wire [31:0] data_sram_addr;
wire [31:0] data_sram_wdata;
wire        data_sram_addr_ok;
wire        data_sram_data_ok;
wire [31:0] data_sram_rdata;   

sram_to_axi sram_to_axi(
    .clk              (aclk             ),
    .resetn           (aresetn          ),

    .inst_sram_req    (inst_sram_req    ),
    .inst_sram_wr     (inst_sram_wr     ),
    .inst_sram_size   (inst_sram_size   ),
    .inst_sram_addr   (inst_sram_addr   ),
    .inst_sram_wstrb  (inst_sram_wstrb  ),
    .inst_sram_wdata  (inst_sram_wdata  ),
    .inst_sram_addr_ok(inst_sram_addr_ok),
    .inst_sram_data_ok(inst_sram_data_ok),
    .inst_sram_rdata  (inst_sram_rdata  ),

    .data_sram_req    (data_sram_req    ),
    .data_sram_wr     (data_sram_wr     ),
    .data_sram_size   (data_sram_size   ),
    .data_sram_wstrb  (data_sram_wstrb  ),
    .data_sram_addr   (data_sram_addr   ),
    .data_sram_wdata  (data_sram_wdata  ),
    .data_sram_addr_ok(data_sram_addr_ok),
    .data_sram_rdata  (data_sram_rdata  ),
    .data_sram_data_ok(data_sram_data_ok),

    .arid             (arid             ),
    .araddr           (araddr           ),
    .arlen            (arlen            ),
    .arsize           (arsize           ),
    .arburst          (arburst          ),
    .arlock           (arlock           ),
    .arcache          (arcache          ),
    .arprot           (arprot           ),
    .arvalid          (arvalid          ),
    .arready          (arready          ),
                      
    .rid              (rid              ),
    .rdata            (rdata            ),
    .rresp            (rresp            ),
    .rlast            (rlast            ),
    .rvalid           (rvalid           ),
    .rready           (rready           ),
               
    .awid             (awid             ),
    .awaddr           (awaddr           ),
    .awlen            (awlen            ),
    .awsize           (awsize           ),
    .awburst          (awburst          ),
    .awlock           (awlock           ),
    .awcache          (awcache          ),
    .awprot           (awprot           ),
    .awvalid          (awvalid          ),
    .awready          (awready          ),
    
    .wid              (wid              ),
    .wdata            (wdata            ),
    .wstrb            (wstrb            ),
    .wlast            (wlast            ),
    .wvalid           (wvalid           ),
    .wready           (wready           ),
    
    .bid              (bid              ),
    .bresp            (bresp            ),
    .bvalid           (bvalid           ),
    .bready           (bready           )
);

mytop mytop(
    .clk              (aclk             ),
    .resetn           (aresetn          ),  //low active

    .inst_sram_req    (inst_sram_req    ),
    .inst_sram_wr     (inst_sram_wr     ),
    .inst_sram_size   (inst_sram_size   ),
    .inst_sram_addr   (inst_sram_addr   ),
    .inst_sram_wstrb  (inst_sram_wstrb  ),
    .inst_sram_wdata  (inst_sram_wdata  ),
    .inst_sram_addr_ok(inst_sram_addr_ok),
    .inst_sram_data_ok(inst_sram_data_ok),
    .inst_sram_rdata  (inst_sram_rdata  ),

    .data_sram_req    (data_sram_req    ),
    .data_sram_wr     (data_sram_wr     ),
    .data_sram_size   (data_sram_size   ),
    .data_sram_wstrb  (data_sram_wstrb  ),
    .data_sram_addr   (data_sram_addr   ),
    .data_sram_wdata  (data_sram_wdata  ),
    .data_sram_addr_ok(data_sram_addr_ok),
    .data_sram_rdata  (data_sram_rdata  ),
    .data_sram_data_ok(data_sram_data_ok),

    //debug interface
    .debug_wb_pc      (debug_wb_pc      ),
    .debug_wb_rf_wen  (debug_wb_rf_wen  ),
    .debug_wb_rf_wnum (debug_wb_rf_wnum ),
    .debug_wb_rf_wdata(debug_wb_rf_wdata)
);    

endmodule

module mytop(
    input         clk,
    input         resetn,
    // inst sram interface
    output        inst_sram_req,
    output        inst_sram_wr,
    output [ 1:0] inst_sram_size,
    output [ 3:0] inst_sram_wstrb,
    output [31:0] inst_sram_addr,
    output [31:0] inst_sram_wdata,
    input         inst_sram_addr_ok,
    input         inst_sram_data_ok,
    input  [31:0] inst_sram_rdata,
    // data sram interface
    output        data_sram_req,
    output        data_sram_wr ,
    output [ 1:0] data_sram_size,
    output [ 3:0] data_sram_wstrb,
    output [31:0] data_sram_addr,
    output [31:0] data_sram_wdata,
    input         data_sram_addr_ok,
    input         data_sram_data_ok,
    input  [31:0] data_sram_rdata,

    // trace debug interface
    output [31:0] debug_wb_pc,
    output [ 3:0] debug_wb_rf_wen,
    output [ 4:0] debug_wb_rf_wnum,
    output [31:0] debug_wb_rf_wdata
);

reg                              reset;
always @(posedge clk) reset <= ~resetn;

wire                             ds_allowin;
wire                             es_allowin;
wire                             ms_allowin;
wire                             ws_allowin;
wire                             load_valid;
wire                             fs_to_ds_valid;
wire                             ds_to_es_valid;
wire                             es_to_ms_valid;
wire                             ms_to_ws_valid;
wire                             ws_reflush;
wire                             ms_reflush;
wire                             fs_valid;
wire                             fs_allowin;
wire                             pre_fs_ready_go;
wire                             ms_w_entryhi;
wire                             refetch;
wire                             to_refetch;
wire                             ds_ref_inst;
wire                             es_ref_inst;
wire                             ms_ref_inst;
wire                             ws_ref_inst;
wire [`FS_TO_DS_BUS_WD     -1:0] fs_to_ds_bus;
wire [`DS_TO_ES_BUS_WD     -1:0] ds_to_es_bus;
wire [`ES_TO_MS_BUS_WD     -1:0] es_to_ms_bus;
wire [`MS_TO_WS_BUS_WD     -1:0] ms_to_ws_bus;
wire [`WS_TO_RF_BUS_WD     -1:0] ws_to_rf_bus;
wire [`WS_TO_FS_BUS_WD     -1:0] ws_to_fs_bus;
wire [`WS_TO_ES_BUS_WD     -1:0] ws_to_es_bus;
wire [`FS_TLB_BUS_WD       -1:0] fs_tlb_bus;
wire [`ES_TLB_BUS_WD       -1:0] es_tlb_bus;
wire [`BR_BUS_WD           -1:0] br_bus;
wire [`ES_FORWARDING_BUS_WD-1:0] es_forwarding;
wire [`MS_FORWARDING_BUS_WD-1:0] ms_forwarding;
wire [`WS_FORWARDING_BUS_WD-1:0] ws_forwarding; 

assign to_refetch = ds_ref_inst | es_ref_inst | ms_ref_inst | ws_ref_inst;            

// IF stage
if_stage if_stage(
    .clk            (clk            ),
    .reset          (reset          ),
    //allowin
    .ds_allowin     (ds_allowin     ),
    //brbus
    .br_bus         (br_bus         ),
    .fs_valid       (fs_valid       ),
    .fs_allowin     (fs_allowin     ),
    .pre_fs_ready_go(pre_fs_ready_go),
    //outputs
    .fs_to_ds_valid (fs_to_ds_valid ),
    .fs_to_ds_bus   (fs_to_ds_bus   ),
    // inst sram interface
    .inst_sram_req  (inst_sram_req   ),
    .inst_sram_wr   (inst_sram_wr    ),
    .inst_sram_size (inst_sram_size  ),
    .inst_sram_addr (inst_sram_addr  ),
    .inst_sram_wstrb(inst_sram_wstrb ),
    .inst_sram_wdata(inst_sram_wdata ),
    .inst_sram_addr_ok(inst_sram_addr_ok),
    .inst_sram_data_ok(inst_sram_data_ok),
    .inst_sram_rdata(inst_sram_rdata),
    .ws_reflush     (ws_reflush     ),
    .ws_to_fs_bus   (ws_to_fs_bus   ),
    .refetch        (refetch        ),
    .to_refetch     (to_refetch     ),
    .fs_tlb_bus     (fs_tlb_bus     )
);
// ID stage
id_stage id_stage(
    .clk            (clk            ),
    .reset          (reset          ),
    //allowin
    .es_allowin     (es_allowin     ),
    .ds_allowin     (ds_allowin     ),
    //from fs
    .fs_to_ds_valid (fs_to_ds_valid ),
    .fs_to_ds_bus   (fs_to_ds_bus   ),
    //to es
    .ds_to_es_valid (ds_to_es_valid ),
    .ds_to_es_bus   (ds_to_es_bus   ),
    //to fs
    .br_bus_hold    (br_bus         ),
    .fs_valid       (fs_valid       ),
    .fs_allowin     (fs_allowin     ),
    .pre_fs_ready_go(pre_fs_ready_go),
    //to rf: for write back
    .load_valid     (load_valid     ),
    .ws_to_rf_bus   (ws_to_rf_bus   ),
    .es_forwarding  (es_forwarding  ),
    .ms_forwarding  (ms_forwarding  ),
    .ws_forwarding  (ws_forwarding  ),
    .ws_reflush     (ws_reflush     ),
    .refetch        (refetch        ),
    .ds_ref_inst    (ds_ref_inst    ),
    .to_refetch     (to_refetch     ) 
);
// EXE stage
exe_stage exe_stage(
    .clk            (clk            ),
    .reset          (reset          ),
    //allowin
    .ms_allowin     (ms_allowin     ),
    .es_allowin     (es_allowin     ),
    //from ds
    .ds_to_es_valid (ds_to_es_valid ),
    .ds_to_es_bus   (ds_to_es_bus   ),
    //to ms
    .es_to_ms_valid (es_to_ms_valid ),
    .es_to_ms_bus   (es_to_ms_bus   ),
    .es_load_op     (load_valid     ),
    .es_forwarding  (es_forwarding  ),
    // data sram interface
    .data_sram_req  (data_sram_req   ),
    .data_sram_wr   (data_sram_wr    ),
    .data_sram_size (data_sram_size  ),
    .data_sram_wstrb(data_sram_wstrb ),
    .data_sram_addr (data_sram_addr  ),
    .data_sram_wdata(data_sram_wdata ),
    .data_sram_addr_ok(data_sram_addr_ok),
    .ws_reflush     (ws_reflush      ),
    .ms_reflush     (ms_reflush      ),
    .ms_w_entryhi   (ms_w_entryhi    ),
    .refetch        (refetch         ),
    .es_ref_inst    (es_ref_inst     ),
    .to_refetch     (to_refetch      ),
    .es_tlb_bus     (es_tlb_bus      ),
    .ws_to_es_bus   (ws_to_es_bus    ) 
);
// MEM stage
mem_stage mem_stage(
    .clk            (clk            ),
    .reset          (reset          ),
    //allowin
    .ws_allowin     (ws_allowin     ),
    .ms_allowin     (ms_allowin     ),
    //from es
    .es_to_ms_valid (es_to_ms_valid ),
    .es_to_ms_bus   (es_to_ms_bus   ),
    //to ws
    .ms_to_ws_valid (ms_to_ws_valid ),
    .ms_to_ws_bus   (ms_to_ws_bus   ),
    .ms_forwarding  (ms_forwarding  ),
    //from data-sram
    .data_sram_rdata(data_sram_rdata),
    .data_sram_data_ok(data_sram_data_ok),
    .ws_reflush     (ws_reflush     ),
    .ms_reflush     (ms_reflush     ),
    .ms_w_entryhi   (ms_w_entryhi   ),
    .refetch        (refetch        ),
    .ms_ref_inst    (ms_ref_inst    ) ,
    .to_refetch     (to_refetch     )
);
// WB stage
wb_stage wb_stage(
    .clk            (clk            ),
    .reset          (reset          ),
    //allowin
    .ws_allowin     (ws_allowin     ),
    //from ms
    .ms_to_ws_valid (ms_to_ws_valid ),
    .ms_to_ws_bus   (ms_to_ws_bus   ),
    //to rf: for write back
    .ws_to_rf_bus   (ws_to_rf_bus   ),
    .ws_forwarding  (ws_forwarding  ),
    //trace debug interface 
    .debug_wb_pc      (debug_wb_pc      ),
    .debug_wb_rf_wen  (debug_wb_rf_wen  ),
    .debug_wb_rf_wnum (debug_wb_rf_wnum ),
    .debug_wb_rf_wdata(debug_wb_rf_wdata),
    .ws_reflush       (ws_reflush       ),
    .ws_to_fs_bus     (ws_to_fs_bus     ),
    .refetch          (refetch          ),
    .to_refetch       (to_refetch       ),
    .ws_ref_inst      (ws_ref_inst      ),
    .ws_to_es_bus     (ws_to_es_bus     ),
    .fs_tlb_bus       (fs_tlb_bus       ),
    .es_tlb_bus       (es_tlb_bus       ) 
);

endmodule

