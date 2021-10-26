`include "mycpu.h"

module if_stage(
    input                          clk            ,
    input                          reset          ,
    //allwoin
    input                          ds_allowin     ,
    //brbus
    input  [`BR_BUS_WD       -1:0] br_bus         ,
    output reg                     fs_valid       ,
    output                         fs_allowin     ,
    output                         pre_fs_ready_go,
    //to ds
    output                         fs_to_ds_valid ,
    output [`FS_TO_DS_BUS_WD -1:0] fs_to_ds_bus   ,
    input                          ws_reflush     , 
    input  [`WS_TO_FS_BUS_WD -1:0] ws_to_fs_bus   ,

    output reg    inst_sram_req    ,
    output        inst_sram_wr     ,/*write or read*/
    output [ 1:0] inst_sram_size   ,
    output [31:0] inst_sram_addr   ,
    output [ 3:0] inst_sram_wstrb  ,
    output [31:0] inst_sram_wdata  ,
    input         inst_sram_addr_ok,
    input         inst_sram_data_ok,
    input  [31:0] inst_sram_rdata  ,
    input         to_refetch       ,
    input         refetch          , 
    output [`FS_TLB_BUS_WD-1:0]fs_tlb_bus   
);

wire [31:0] epc;
wire        ws_eret;
wire [31:0] ws_pc;
wire        inst_found;
wire        inst_valid;
wire [19:0] inst_pfn;
wire        refill;
assign {refill,
		inst_found,
		inst_valid,
		inst_pfn,
		ws_pc, 
		ws_eret, 
		epc
		} = ws_to_fs_bus;

reg  [31:0] ex_hold;
reg         ex_hold_valid;
always @(posedge clk) begin
    if (reset) begin
        ex_hold_valid <= 1'b0;
    end
    else if (ws_reflush | refetch) begin
        ex_hold_valid <= 1'b1;
    end
    else if(pre_fs_ready_go & inst_cancel) begin
        ex_hold_valid <= 1'b0;
    end
end

always @(posedge clk) begin
    if (ws_eret) begin
        ex_hold <= epc;
    end
    else if(refill) begin
    	ex_hold <= 32'hbfc00200;
    end
    else if(ws_reflush) begin
        ex_hold <= 32'hbfc00380;
    end
    else if(refetch) begin
    	ex_hold <= seq_pc;
    end
end

wire         br_taken ;
wire [ 31:0] br_target;
wire        if_bd;
assign {if_bd,br_taken,br_target} = br_bus;                       

wire [31:0] fs_inst;
reg  [31:0] fs_pc;
assign fs_to_ds_bus = {if_refill_r, //71:71
					   if_bd     , //70:70
                       if_ex     , //69:69
                       if_excode , //68:64
                       fs_inst   , //63:32
                       fs_pc    }; //31:0


// pre-IF stage
wire        to_fs_valid;
wire [31:0] seq_pc;
wire [31:0] nextpc;
assign to_fs_valid  = ~reset & pre_fs_ready_go;
assign seq_pc       = fs_pc + 3'h4;
assign nextpc       = ws_eret                ? epc
					: refill                 ? 32'hbfc00200
                    : ws_reflush             ? 32'hbfc00380
                    : refetch                ? seq_pc
                    : ex_hold_valid          ? ex_hold
					: (br_taken & ~fs_valid) ? seq_pc
                    : (br_taken &  fs_valid) ? br_target 
                    : seq_pc; 

assign  pre_fs_ready_go = inst_sram_req & inst_sram_addr_ok;

// IF stage
wire fs_ready_go;
assign fs_ready_go    = if_bd ? pre_fs_ready_go : (inst_sram_data_ok || fs_hold_valid) && inst_cancel;
assign fs_allowin     = !fs_valid || fs_ready_go && ds_allowin;
assign fs_to_ds_valid =  fs_valid && fs_ready_go;
always @(posedge clk) begin
    if (reset) begin
        fs_valid <= 1'b0;
    end
    else if(~inst_cancel) 
        fs_valid <= 1'b0;
    else if (fs_allowin) begin
        fs_valid <= to_fs_valid;
    end

    if (reset) begin
        fs_pc <= 32'hbfbffffc;  //trick: to make nextpc be 0xbfc00000 during reset 
    end
    else if (to_fs_valid && fs_allowin) begin
        fs_pc <= nextpc;
    end
end

reg [31 : 0]fs_inst_hold;/*hold the inst get from if-stage*/
reg fs_hold_valid;
always @ (posedge clk)begin
    if(inst_sram_data_ok && ~(ds_allowin && fs_ready_go))
        fs_inst_hold <= inst_sram_rdata;
end

always @ (posedge clk)begin/*需要保存的*/
    if(reset)
        fs_hold_valid <= 1'b0;
    else if(ws_reflush | refetch) begin
        fs_hold_valid <= 1'b0;
    end
    else if(inst_sram_data_ok && ~(ds_allowin && fs_ready_go) && inst_cancel)
        fs_hold_valid <= 1;
    else if(ds_allowin && fs_ready_go)
        fs_hold_valid <= 0;
    else
        fs_hold_valid <= fs_hold_valid;
end

assign fs_inst         = (fs_hold_valid) ? fs_inst_hold : inst_sram_rdata;

/*new stage machine, cancel*/
reg  inst_cancel_r_1;
reg  inst_cancel_r_2;
wire inst_cancel;
assign inst_cancel = ~inst_cancel_r_1 & ~inst_cancel_r_2;
//assign inst_cancel = ~reset & ws_reflush & (to_fs_valid || (~fs_allowin && ~fs_ready_go)) & ~inst_sram_data_ok & ~inst_cancel;

always @(posedge clk)begin
    if(reset)
    	inst_cancel_r_1 <= 1'b0;
    else if((ws_reflush | refetch) && (to_fs_valid || (~fs_allowin && ~fs_ready_go)))
    	inst_cancel_r_1 <= 1'b1;
    else if(inst_sram_data_ok)
    	inst_cancel_r_1 <= 1'b0;
end

always @(posedge clk)begin
    if(reset)
        inst_cancel_r_2 <= 1'b0;
    else if((ws_reflush | refetch)&& (~to_fs_valid || fs_allowin || (~fs_allowin & fs_ready_go)))
        inst_cancel_r_2 <= 1'b1;
    else if(inst_cancel_r_2)
        inst_cancel_r_2 <= 1'b0;
end

// sram
assign inst_sram_size = 2;
assign inst_sram_wr = (|inst_sram_wstrb);
assign inst_sram_wstrb = 4'h0;
assign inst_sram_addr  = (if_refill | if_invalid) ? {nextpc[31:2], 2'b0}
					   : no_map                   ?  nextpc 
					   :                             pa;
assign inst_sram_wdata = 32'b0; 
always @(posedge clk) begin
    if(reset)
        inst_sram_req<=1'b1;
    else if(pre_fs_ready_go & inst_sram_req)
        inst_sram_req<=1'b0;
    else if(ds_allowin & (inst_sram_data_ok | fs_hold_valid))
        inst_sram_req<=1'b1;
end

wire        if_AdEL;
wire        if_ex;
wire [ 4:0] if_excode;
assign if_AdEL   = (fs_pc[1:0] != 2'b00);
assign if_excode = (if_refill_r | if_invalid_r) ? `Tlbl
				 :  if_AdEL                     ? `AdEL 
				 :                                `Int;
assign if_ex     = (if_excode != `Int) & fs_valid;


//TLB
wire no_map;
wire if_refill;
wire if_invalid;
reg  if_refill_r;
reg  if_invalid_r;
wire [31:0] pa;
assign no_map = (nextpc[31:28] == 4'h8 || nextpc[31:28] == 4'h9 || nextpc[31:28] == 4'ha || nextpc[31:28] == 4'hb);
assign if_refill  = ~inst_found & ~no_map & ~reset;
assign if_invalid = ~no_map & inst_found & ~inst_valid & ~reset;
assign fs_tlb_bus = {nextpc[31:12]};
assign pa = {inst_pfn, nextpc[11:0]};
always @(posedge clk) begin
	if (reset) begin
		if_invalid_r <= 1'b0;
		if_refill_r  <= 1'b0;
	end
	else if (to_fs_valid & fs_allowin) begin
		if_refill_r  <= if_refill;
		if_invalid_r <= if_invalid;
	end
end

endmodule
