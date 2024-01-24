.section .text.entry

.global trap_s
.extern handler_s
.equ reg_size, 0x8
.align 2

trap_s:
	# TODO: save the caller saved registers and sepc
	# push寄存器到栈上
    addi sp, sp, -136            # 为所有寄存器预留空间
    sw ra, 128(sp)               # 保存 ra
    sw a0, 120(sp)               # 保存 a0
    sw a1, 112(sp)               # 保存 a1
    sw a2, 104(sp)               # 保存 a2
	sw a3, 96(sp)               # 保存 a3
	sw a4, 88(sp)               # 保存 a4
	sw a5, 80(sp)               # 保存 a5
	sw a6, 72(sp)               # 保存 a6
	sw a7, 64(sp)               # 保存 a7
	sw t0, 56(sp)               # 保存 t0
	sw t1, 48(sp)               # 保存 t1
	sw t2, 40(sp)               # 保存 t2
	sw t3, 32(sp)               # 保存 t3
	sw t4, 24(sp)                # 保存 t4
	sw t5, 16(sp)                # 保存 t5
	sw t6, 8(sp)                # 保存 t6

	# 保存sepc
    csrr a0, sepc               # 将sepc读入a0
    sw a0, 0(sp)               # 保存原sepc
	

	# TODO: call handler_s(scause)
    csrr a0, scause             # 将scause读入a0
    call handler_s              # 调用 handler_s

	# TODO: load sepc and caller saved registers
	# 恢复sepc
    lw a0, 0(sp)
    csrw sepc, a0

    # 从栈上pop寄存器，恢复它们的值
    lw ra, 128(sp)
	lw a0, 120(sp)
	lw a1, 112(sp)
	lw a2, 104(sp)
	lw a3, 96(sp)
	lw a4, 88(sp)
	lw a5, 80(sp)
	lw a6, 72(sp)
	lw a7, 64(sp)
	lw t0, 56(sp)
	lw t1, 48(sp)
	lw t2, 40(sp)
	lw t3, 32(sp)
	lw t4, 24(sp)
	lw t5, 16(sp)
	lw t6, 8(sp)
	# 恢复栈指针
	addi sp, sp, 136             
	
	sret