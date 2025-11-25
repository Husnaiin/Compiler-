    .option nopic
    .data
    .globl global_x
global_x:
    .word 0
    .globl global_y
global_y:
    .word 0

    .text
    .globl add
add:
    addi sp, sp, -32
    sw ra, 28(sp)
    sw s0, 24(sp)
    addi s0, sp, 32
    sw a0, -16(s0)
    sw a1, -12(s0)
    lw t0, -16(s0)
    lw t1, -12(s0)
    add t2, t0, t1
    sw t2, -20(s0)
    lw a0, -20(s0)
    j add_ret
add_ret:
    lw ra, -4(s0)
    lw s0, -8(s0)
    addi sp, s0, -32
    ret

    .globl add
add:
    addi sp, sp, -32
    sw ra, 28(sp)
    sw s0, 24(sp)
    addi s0, sp, 32
    sw a0, -20(s0)
    sw a1, -16(s0)
    sw a2, -12(s0)
    li a0, 0
    j add_ret
add_ret:
    lw ra, -4(s0)
    lw s0, -8(s0)
    addi sp, s0, -32
    ret

    .globl main
main:
    addi sp, sp, -48
    sw ra, 44(sp)
    sw s0, 40(sp)
    addi s0, sp, 48
    lw t0, -28(s0)
    sw t0, -32(s0)
    lw t0, -24(s0)
    sw t0, -28(s0)
    li a0, 0
    li a1, 0
    call add
    sw a0, -16(s0)
    lw t0, -16(s0)
    sw t0, -12(s0)
    li a0, 0
    j main_ret
main_ret:
    lw ra, -4(s0)
    lw s0, -8(s0)
    addi sp, s0, -48
    ret

