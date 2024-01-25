`timescale 1ns / 1ps

module core_sim;
    reg clk, rst;
    wire [31:0]data;
    wire [31:0] out;
    RV32core core(
        .debug_en(1'b0),
        .debug_step(1'b0),
        .debug_addr(7'b1),
        .debug_data(out),
        .clk(clk),
        .rst(rst),
        .interrupter(1'b0)
    );

    initial begin
        clk = 0;
        rst = 1;
        #2 rst = 0;
    end
    always #1 clk = ~clk;
    assign data = out;

endmodule