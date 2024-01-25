`timescale 1ps/1ps

module HazardDetectionUnit(
    input clk,
    input Branch_ID, rs1use_ID, rs2use_ID,
    input[1:0] hazard_optype_ID,
    input[4:0] rd_EXE, rd_MEM, rs1_ID, rs2_ID, rs2_EXE,
    // 控制对应的模块是否可写
    output PC_EN_IF, reg_FD_EN, reg_FD_stall, reg_FD_flush,
        reg_DE_EN, reg_DE_flush, reg_EM_EN, reg_EM_flush, reg_MW_EN,
    output forward_ctrl_ls,
    output[1:0] forward_ctrl_A, forward_ctrl_B
);
            //according to the diagram, design the Hazard Detection Unit
    reg[1:0] hazard_optype_EXE, hazard_optype_MEM;
    always @(posedge clk) begin
        hazard_optype_MEM <= hazard_optype_EXE;
        hazard_optype_EXE <= hazard_optype_ID & {2{~reg_DE_flush}};
    end
    
    // forward from EXE/MEM to ID
    wire forwardA1 = (rd_EXE != 5'b0) & rs1use_ID & (rs1_ID == rd_EXE) & (hazard_optype_EXE == 2'd1);
    // forward from MEM/WB to ID
    wire forwardA2 = (rd_MEM != 5'b0) & rs1use_ID & (rs1_ID == rd_MEM) & (hazard_optype_MEM == 2'd1);
    // forward from MEM/WB to ID (load)
    wire forwardA3 = (rd_MEM != 5'b0) & rs1use_ID & (rs1_ID == rd_MEM) & (hazard_optype_MEM == 2'd2);
    // load+sth(not store),need stall
    wire stallA = (rd_EXE != 5'b0) & rs1use_ID & (rs1_ID == rd_EXE) & (hazard_optype_EXE == 2'd2) & (hazard_optype_ID != 2'd3);
    
    wire forwardB1 = (rd_EXE != 5'b0) & rs2use_ID & (rs2_ID == rd_EXE) & (hazard_optype_EXE == 2'd1);
    wire forwardB2 = (rd_MEM != 5'b0) & rs2use_ID & (rs2_ID == rd_MEM) & (hazard_optype_MEM == 2'd1);
    wire forwardB3 = (rd_MEM != 5'b0) & rs2use_ID & (rs2_ID == rd_MEM) & (hazard_optype_MEM == 2'd2);
    wire stallB = (rd_MEM != 5'b0) & rs2use_ID & (rs2_ID == rd_EXE) & (hazard_optype_EXE == 2'd2) & (hazard_optype_ID != 2'd3);
    
    assign forward_ctrl_A = {2{forwardA1}} & 2'b01 |
                            {2{forwardA2}} & 2'b10 |
                            {2{forwardA3}} & 2'b11;
    assign forward_ctrl_B = {2{forwardB1}} & 2'b01 |
                            {2{forwardB2}} & 2'b10 |
                            {2{forwardB3}} & 2'b11;
    assign forward_ctrl_ls = (hazard_optype_MEM == 2'd2) & (hazard_optype_EXE == 2'd3) & (rs2_EXE == rd_MEM);
    
    // 当需要stall时，hazard detection unit传给PC的信号置为0，意为将PC设置为不可写状态，
    // 这样PC就不能更新了，从而达到停止流水线的目的
    assign PC_EN_IF = ~(stallA | stallB);
    // 同时，给解码阶段的流水线寄存器的信号置为1，将它设置为可写状态，使其中的内容可以被flush。
    // 当流水线正常执行时，传给PC的信号值为1，传给寄存器的信号值为0，使PC可以被正常更新，而寄存器不能被flush。
    assign reg_FD_stall = stallA | stallB;
    assign reg_FD_flush = Branch_ID;
    assign reg_FD_EN = 1'b1;
    assign reg_DE_EN = 1'b1;
    assign reg_DE_flush = stallA | stallB;
    assign reg_EM_EN = 1'b1;
    assign reg_EM_flush = 1'b0;
    assign reg_MW_EN = 1'b1;
endmodule