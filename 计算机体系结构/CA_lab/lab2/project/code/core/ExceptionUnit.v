`timescale 1ns / 1ps

module ExceptionUnit(
    input clk, rst,
    input csr_rw_in,
    input[1:0] csr_wsc_mode_in,
    input csr_w_imm_mux,
    input[11:0] csr_rw_addr_in,
    input[31:0] csr_w_data_reg,
    input[4:0] csr_w_data_imm,
    output[31:0] csr_r_data_out,

    input interrupt,
    input illegal_inst,
    input l_access_fault,
    input s_access_fault,
    input ecall_m,

    input mret,

    input[31:0] epc_cur,
    input[31:0] epc_next,
    output[31:0] PC_redirect,
    output redirect_mux,

    output reg_FD_flush, reg_DE_flush, reg_EM_flush, reg_MW_flush, 
    output RegWrite_cancel
);

    reg[11:0] csr_raddr, csr_waddr;
    reg[31:0] csr_wdata;
    reg csr_w;
    reg[1:0] csr_wsc;

    wire[31:0] mstatus;

    CSRRegs csr(.clk(clk),.rst(rst),.csr_w(csr_w),.raddr(csr_raddr),.waddr(csr_waddr),
        .wdata(csr_wdata),.rdata(csr_r_data_out),.mstatus(mstatus),.csr_wsc_mode(csr_wsc));

    //According to the diagram, design the Exception Unit
    
    parameter STATE_IDLE = 2'b00;
    parameter STATE_MEPC = 2'b01;
    parameter STATE_MCAUSE = 2'b10;
    
    reg[1:0] cur_state = STATE_IDLE;
    reg[1:0] next_state = STATE_IDLE;

    wire exception = illegal_inst | l_access_fault | s_access_fault | ecall_m;
    wire trap_in = mstatus[3] & (interrupt | exception);
    wire mepc;
    reg reg_FD_flush_ = 0, reg_DE_flush_ = 0, reg_EM_flush_ = 0, reg_MW_flush_ = 0;
    reg RegWrite_cancel_ = 0,mepc_ = 0;

    reg [31:0] epc;
    reg [31:0] cause;

    assign reg_FD_flush = reg_FD_flush_;
    assign reg_DE_flush = reg_DE_flush_;
    assign reg_EM_flush = reg_EM_flush_;
    assign reg_MW_flush = reg_MW_flush_;
    assign RegWrite_cancel = RegWrite_cancel_;
    assign mepc = mepc_;
    assign redirect_mux = mret | mepc;
    assign PC_redirect = (mret | mepc)? csr_r_data_out:0;
    
    always@(posedge clk or posedge rst) begin
    if(rst) begin
        //cur_state <= STATE_IDLE;   
        /*reg_FD_flush_ <= 0;
        reg_DE_flush_ <= 0;
        reg_EM_flush_ <= 0;
        reg_MW_flush_ <= 0;
        RegWrite_cancel_ <= 0;
        mepc_ <= 0;*/
        epc <= 32'd0; // Initial value
        cause <= 32'd0; // Initial value
    end
    else begin
        cur_state <= next_state;
        if(cur_state == STATE_IDLE) begin
            // record epc and cause 
            // if exception
            if(exception) begin
                epc <= epc_cur;
                if(illegal_inst) begin
                    cause <= 32'd2;
                end
                else if(l_access_fault) begin
                    cause <= 32'd5;
                end
                else if(s_access_fault) begin
                    cause <= 32'd7;
                end
                else if(ecall_m) begin
                    cause <= 32'd11;
                end
                else begin
                    cause <= 32'd0;
                end
            end
            // if interrupt
            else begin
                epc <= epc_next;
                cause <= 32'h8000000B;
            end
        end
    end
 end
    
    always@* begin
        mepc_ = 0;  // default value
        case(cur_state)
            STATE_IDLE:begin
                mepc_ = 0;
                if(trap_in)begin
                    // write mstatus
                    csr_w = 1;
                    csr_wsc = 2'b01;
                    csr_waddr = 12'h300;
                    csr_wdata = {mstatus[31:8],mstatus[3],mstatus[6:4],1'b0,mstatus[2:0]};
                    csr_raddr = 12'h0;

                    next_state = STATE_MEPC;

                    // flush all the pipeline registers
                    reg_FD_flush_ = 1;
                    reg_DE_flush_ = 1;
                    reg_EM_flush_ = 1;
                    reg_MW_flush_ = 1;

                    // record epc and cause 
                    // if exception
                    if(exception)begin
                        /*epc = epc_cur;
                        if(illegal_inst)begin
                            cause = 32'd2;
                        end
                        else if(l_access_fault)begin
                            cause = 32'd5;
                        end
                        else if(s_access_fault)begin
                            cause = 32'd7;
                        end
                        else if(ecall_m)begin
                            cause = 32'd11;
                        end
                        else begin
                            cause = 32'd0;
                        end*/
                        // if exception (not interrupt), cancel regwrite
                        RegWrite_cancel_ = 1;
                    end
                    // if interrupt
                    else begin
                        /*epc = epc_next;
                        cause = 32'h8000000B;*/
                        
                        RegWrite_cancel_ = 0;
                    end
                end
                    else if(mret)begin
                        // write mstatus & read mtvec
                        csr_w = 1;
                        csr_wsc = 2'b01;
                        csr_waddr = 12'h300;
                        csr_wdata = {mstatus[31:8],1'b1,mstatus[6:4],mstatus[7],mstatus[2:0]};
                        csr_raddr = 12'h341;

                        next_state = STATE_IDLE;

                        // set redirect_mux
                        // redirect_mux_ <= 1;
                        // set PC_redirect
                        // PC_redirect_ <= csr_r_data_out;

                        // flush EM,DE,FD
                        reg_EM_flush_ = 1;
                        reg_DE_flush_ = 1;
                        reg_FD_flush_ = 1;
                        reg_MW_flush_ = 0;
                        
                        RegWrite_cancel_ = 0;
                    end
                    else if(csr_rw_in)begin
                        csr_w = 1;
                        csr_wsc = csr_wsc_mode_in;
                        csr_wdata = csr_w_imm_mux ? csr_w_data_imm : csr_w_data_reg;
                        csr_raddr = csr_rw_addr_in;
                        csr_waddr = csr_rw_addr_in;

                        next_state = STATE_IDLE;

                        reg_EM_flush_ = 0;
                        reg_DE_flush_ = 0;
                        reg_FD_flush_ = 0;
                        reg_MW_flush_ = 0;
                        RegWrite_cancel_ = 0;
                    end
                    else begin
                        csr_w = 0;
                        csr_wsc = 0;
                        csr_wdata = 0;
                        csr_raddr = 0;
                        csr_waddr = 0;
                        RegWrite_cancel_ = 0;
                        reg_EM_flush_ = 0;
                        reg_DE_flush_ = 0;
                        reg_FD_flush_ = 0;
                        reg_MW_flush_ = 0;
                        next_state = STATE_IDLE;
                    end
                end
            STATE_MEPC:begin
                mepc_ = 1;

                // write mepc & read mtvec
                csr_w = 1;
                csr_wsc = 2'b01;
                csr_waddr = 12'h341;
                csr_wdata = epc;
                csr_raddr = 12'h305;

                next_state = STATE_MCAUSE;

                // set redirect_mux
                // redirect_mux_ <= 1;
                // redirect pc
                // PC_redirect_ <= csr_r_data_out;

                reg_EM_flush_ = 0;
                reg_DE_flush_ = 0;
                reg_FD_flush_ = 1;
                reg_MW_flush_ = 0;
                
                RegWrite_cancel_ = 0;
            end
            STATE_MCAUSE:begin
                mepc_ = 0;
                // write mcause
                csr_w = 1;
                csr_wsc = 2'b01;
                csr_waddr = 12'h342;
                csr_wdata = cause;
                csr_raddr = 12'h0;

                next_state = STATE_IDLE;

                reg_EM_flush_ = 0;
                reg_DE_flush_ = 0;
                reg_FD_flush_ = 0;
                reg_MW_flush_ = 0;
                
                RegWrite_cancel_ = 0;
            end
            default: begin
                csr_w = 0;
                csr_wsc = 0;
                csr_wdata = 0;
                csr_raddr = 0;
                csr_waddr = 0;
                RegWrite_cancel_ = 0;
                reg_EM_flush_ = 0;
                reg_DE_flush_ = 0;
                reg_FD_flush_ = 0;
                reg_MW_flush_ = 0;
                next_state = STATE_IDLE;
           end
        endcase
                end
endmodule