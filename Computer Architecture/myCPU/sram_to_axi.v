`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2020/11/19 15:38:53
// Design Name: 
// Module Name: sram_to_axi
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module sram_to_axi(
	input         clk,
	input         resetn,

    // inst sram interface
    input         inst_sram_req,
    input         inst_sram_wr,
    input  [ 1:0] inst_sram_size,
    input  [ 3:0] inst_sram_wstrb,
    input  [31:0] inst_sram_addr,
    input  [31:0] inst_sram_wdata,
    output        inst_sram_addr_ok,
    output        inst_sram_data_ok,
    output [31:0] inst_sram_rdata,
    // data sram interface
    input         data_sram_req,
    input         data_sram_wr ,
    input  [ 1:0] data_sram_size,
    input  [ 3:0] data_sram_wstrb,
    input  [31:0] data_sram_addr,
    input  [31:0] data_sram_wdata,
    output        data_sram_addr_ok,
    output        data_sram_data_ok,
    output [31:0] data_sram_rdata,

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
    output [ 1:0]  awlock,   
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
    output        bready
 );


//read request channel
assign arlen   = 8'b0;
assign arburst = 2'b1;
assign arlock  = 2'b0;
assign arcache = 4'b0;
assign arprot  = 3'b0;

reg    read_req ;
reg    read_rps;
reg    read_data;
wire   addr_same;
wire   RAW;
reg    wfinish_n;
reg    rfinish_n;
assign addr_same = data_sram_addr & inst_sram_addr;
assign RAW       = addr_same & write_req & read_data;
//assign wfinish_n = bready & ~bvalid;
//assign rfinish_n = rready & ~rvalid;
//assign read_data = data_sram_req & ~data_sram_wr & ~rfinish_n;
//assign read_req  = (read_data | (inst_sram_req & ~inst_sram_wr)) & ~(RAW & wfinish_n);
assign arid      = read_data ? 4'b1 : 4'b0;

always @(posedge clk) begin
	if (~resetn) begin
		read_data <= 1'b0;
	end
	else if(arready) begin
		read_data <= 1'b0;
	end
	else if ((~read_data & rfinish_n)) begin
		read_data <= 1'b0;
	end
	else if (~rfinish_n & data_sram_req & ~data_sram_wr & ~read_req) begin
		read_data <= 1'b1;
	end
end

always @(posedge clk) begin
	if (~resetn) begin
		read_req <= 1'b0;
	end
	else if(read_req & arready) begin
		read_req <= 1'b0;
	end
	else if(RAW & wfinish_n) begin
		read_req <= 1'b0;
	end
	else if ((read_data | (inst_sram_req & ~inst_sram_wr)) & ~arready) begin
		read_req <= 1'b1;
	end
end

always @(posedge clk) begin
	if (~resetn) begin
		read_rps <= 1'b0;
	end
	else if (arready) begin
		read_rps <= 1'b1;
	end
	else if(rvalid) begin
		read_rps <= 1'b0;
	end
end

always @(posedge clk) begin
	if (~resetn) begin
		wfinish_n <= 1'b0;
	end
	else if (write_req) begin
		wfinish_n <= 1'b1;
	end
	else if (bvalid & bready) begin
		wfinish_n <= 1'b0;
	end
end

always @(posedge clk) begin
	if (~resetn) begin
		rfinish_n <= 1'b0;
	end
	else if (rready & rvalid) begin
		rfinish_n <= 1'b0;
	end
	else if (read_req | read_rps) begin
		rfinish_n <= 1'b1;
	end
end

reg [31:0] araddr_r;
reg [ 2:0] arsize_r;
reg        arvalid_r;
assign araddr  = araddr_r;
assign arsize  = arsize_r;
assign arvalid = arvalid_r;

always @(posedge clk) begin
	if (~resetn) begin
		araddr_r <= 32'b0;
	end
	else if(arready) begin
		araddr_r <= 32'b0;
	end
	else if (read_req) begin
		araddr_r <= read_data ? data_sram_addr : inst_sram_addr;
	end
end

always @(posedge clk) begin
	if (~resetn) begin
		arsize_r <= 3'b0;
	end
	else if(arready) begin
		arsize_r <= 3'b0;
	end
	else if (read_req) begin
		arsize_r <= {1'b0, (read_data ? data_sram_size : inst_sram_size)};
	end
end

always @(posedge clk) begin
	if (~resetn) begin
		arvalid_r <= 1'b0;
	end
	else if(arready) begin
		arvalid_r <= 1'b0;
	end
	else if (read_req) begin
		arvalid_r <= 1'b1;
	end	
end

//read repsonse channel
reg rready_r;
assign rready = rready_r;

always @(posedge clk) begin
	if (~resetn) begin
		rready_r <= 1'b0;		
	end
	else if(read_rps) begin
		rready_r <= 1'b1;
	end	
	else if (rvalid) begin
		rready_r <= 1'b0;
	end
end

//read_data_buffer? is necessary? YES!!!
reg  [31:0] rdata_r;
wire [31:0] rdata_buf;
reg         rdata_r_valid;
assign rdata_buf = rdata_r_valid ? rdata_r : rdata;

always @(posedge clk) begin
	if (~resetn) begin
		rdata_r <= 32'b0;
	end
	else if (rvalid) begin
		rdata_r <= rdata;
	end
	else if(read_req) begin
		rdata_r <= 32'b0;
	end
end

always @(posedge clk) begin
	if (~resetn) begin
		rdata_r_valid <= 1'b0;
	end
	else if (rvalid & ~rready) begin
		rdata_r_valid <= 1'b1;
	end
	else if(read_req) begin
		rdata_r_valid <= 1'b0;
	end
end

//write request channel
assign awid    = 4'b1;
assign awlen   = 8'b0;
assign awburst = 2'b1;
assign awlock  = 2'b0;
assign awcache = 4'b0;
assign awprot  = 3'b0;

wire   write_req;
assign write_req = data_sram_req & data_sram_wr;

reg [31:0] awaddr_r;
reg [ 2:0] awsize_r;
reg        awvalid_r;
assign awaddr  = awaddr_r;
assign awsize  = awsize_r;
assign awvalid = awvalid_r;

always @(posedge clk) begin
	if (~resetn) begin
		awaddr_r <= 32'b0;
	end
	else if(awready_buf) begin
		awaddr_r <= 32'b0;
	end
	else if (write_req) begin
		awaddr_r <= data_sram_addr;
	end
end

always @(posedge clk) begin
	if (~resetn) begin
		awsize_r <= 3'b0;
	end
	else if(awready_buf) begin
		awsize_r <= 3'b0;
	end
	else if (write_req) begin
		awsize_r <= {1'b0, data_sram_size};
	end
end

always @(posedge clk) begin
	if (~resetn) begin
		awvalid_r <= 1'b0;
	end
	else if(awready_buf) begin
		awvalid_r <= 1'b0;
	end
	else if (write_req) begin
		awvalid_r <= 1'b1;
	end
end

reg  awready_r;
wire awready_buf;
assign awready_buf = awready_r | awready;

always @(posedge clk) begin
	if (~resetn) begin
		awready_r <= 1'b0;
	end
	else if(awready_buf & wready_buf) begin
		awready_r <= 1'b0;
	end
	else if (awready) begin
		awready_r <= 1'b1;
	end
end

//write data channel
assign wid   = 4'b1;
assign wlast = 1'b1;

reg [31:0] wdata_r;
reg [ 3:0] wstrb_r;
reg        wvalid_r;
assign wdata  = wdata_r;
assign wstrb  = wstrb_r;
assign wvalid = wvalid_r;

always @(posedge clk) begin
	if (~resetn) begin
		wdata_r <= 32'b0;
	end
	else if(wready_buf) begin
		wdata_r <= 32'b0;
	end
	else if (write_req) begin
		wdata_r <= data_sram_wdata;
	end
end

always @(posedge clk) begin
	if (~resetn) begin
		wstrb_r <= 4'b0;
	end
	else if(wready_buf) begin
		wstrb_r <= 4'b0;
	end
	else if (write_req) begin
		wstrb_r <= data_sram_wstrb;
	end
end

always @(posedge clk) begin
	if (~resetn) begin
		wvalid_r <= 1'b0;
	end
	else if(wready_buf) begin
		wvalid_r <= 1'b0;
	end
	else if (write_req) begin
		wvalid_r <= 1'b1;
	end
end

reg  wready_r;
wire wready_buf;
assign wready_buf = wready_r | wready;

always @(posedge clk) begin
	if (~resetn) begin
		wready_r <= 1'b0;
	end
	else if(wready_buf & awready_buf) begin
		wready_r <= 1'b0;
	end
	else if (wready) begin
		wready_r <= 1'b1;
	end
end

//write response channel
reg bready_r;
assign bready = bready_r ;

always @(posedge clk) begin
	if (~resetn) begin
		bready_r <= 1'b0;
	end
	else if (bvalid & wlast) begin
		bready_r <= 1'b0;
	end
	else if(write_req) begin
		bready_r <= 1'b1;
	end
end

//sram slave output
//inst
assign inst_sram_addr_ok = read_data ? 1'b0 : arready;
assign inst_sram_data_ok = rvalid & rready & (rid == 4'b0);
/*reg inst_sram_addr_ok_r;
always @(posedge clk) begin
	if (~resetn) begin
		inst_sram_addr_ok_r <= 1'b0;
	end
	else if (read_data ? ) begin
		
	end
end*/

assign inst_sram_rdata   = rdata_buf;

//data
assign data_sram_addr_ok = write_req ? (awready_buf & wready_buf) 
                         : read_data ? arready
                         : 1'b0;
assign data_sram_data_ok = wfinish_n ? (bready & bvalid) : (rvalid & rready & (rid == 4'b1));
assign data_sram_rdata   = rdata_buf;

endmodule