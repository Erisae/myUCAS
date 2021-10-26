
#include "asm.h"

.equ    KERNEL, 0
.equ    USER,   312  # 定义好了offset
.equ    NESTED_COUNT, 624
.equ    ExcCode, 0x7c
.global first_entry
lockkey : .word 0

 
LEAF(asm_start)
    mtc0    zero, CP0_STATUS
    mtc0    zero, CP0_WATCHLO
    mtc0    zero, CP0_WATCHHI
    mfc0    t0, CP0_CONFIG
    and     t0, ~0x7
    ori     t0, 0x2
    mtc0    t0, CP0_CONFIG
    
    jr  ra
END(asm_start)

 .macro spin_lock # acquire spin lock
 L3:
    #  dla     k0,lockkey
    #  ll      k1,0x0(k0)
     ll      k1,lockkey
     bnez    k1,L3
     li      k1,1
     sc      k1,lockkey
     beqz    k1,L3
     sync
 .endm

 .macro spin_unlock # unlock spin lock
    #  dla     k0,lockkey 
    #  sw      zero,0x0(k0)
     sw        zero,lockkey
     sync
 .endm

 .macro switch_stack offset
    .set    noat
    mfc0    k1,CP0_PRID,1
    andi    k1,k1,0x1
    dsll    k1,3
    dla     k0,current_running # 加载current_running的PCB
    addu    k0,k0,k1
    ld      k0,0x0(k0)
    addiu   k0,k0,\offset
    ld      sp,OFFSET_REG29(k0)
    .set    at
.endm


.macro SAVE_CONTEXT offset # 把内核栈context放在最前面
    .set    noat
    mfc0    k1,CP0_PRID,1
    andi    k1,k1,0x1
    dsll    k1,3
    dla     k0,current_running # 加载current_running的PCB
    addu    k0,k0,k1
    ld      k0,0x0(k0)
    addiu   k0,k0,\offset
    sd      AT,OFFSET_REG1(k0)

    sd      v0,OFFSET_REG2(k0)
    sd      v1,OFFSET_REG3(k0)

    sd      a0,OFFSET_REG4(k0)
    sd      a1,OFFSET_REG5(k0)
    sd      a2,OFFSET_REG6(k0)
    sd      a3,OFFSET_REG7(k0)

    sd      t0,OFFSET_REG8(k0)
    sd      t1,OFFSET_REG9(k0)
    sd      t2,OFFSET_REG10(k0)
    sd      t3,OFFSET_REG11(k0)
    sd      t4,OFFSET_REG12(k0)
    sd      t5,OFFSET_REG13(k0)
    sd      t6,OFFSET_REG14(k0)
    sd      t7,OFFSET_REG15(k0)
    sd      t8,OFFSET_REG24(k0)
    sd      t9,OFFSET_REG25(k0)

    sd      s0,OFFSET_REG16(k0)
    sd      s1,OFFSET_REG17(k0)
    sd      s2,OFFSET_REG18(k0)
    sd      s3,OFFSET_REG19(k0)
    sd      s4,OFFSET_REG20(k0)
    sd      s5,OFFSET_REG21(k0)
    sd      s6,OFFSET_REG22(k0)
    sd      s7,OFFSET_REG23(k0)
    sd      s8,OFFSET_REG30(k0)
    # k0,k1一般不会用
    sd      gp,OFFSET_REG28(k0)
    sd      sp,OFFSET_REG29(k0)
    sd      fp,OFFSET_REG30(k0)
    sd      ra,OFFSET_REG31(k0)  

    mfc0    k1,CP0_STATUS
    sw      k1,OFFSET_STATUS(k0)
    mfc0    k1,CP0_CAUSE
    sw      k1,OFFSET_CAUSE(k0)
    mfc0    k1,CP0_WATCHHI
    sd      k1,OFFSET_HI(k0)
    mfc0    k1,CP0_WATCHLO
    sd      k1,OFFSET_LO(k0)
    mfc0    k1,CP0_BADVADDR
    sd      k1,OFFSET_BADVADDR(k0)
    mfc0    k1,CP0_EPC
    sd      k1,OFFSET_EPC(k0)
    # mfc0    k1,CP0_PC
    # sd      k1,OFFSET_PC(k0)

    .set    at
.endm

.macro RESTORE_CONTEXT offset
    .set    noat
    mfc0    k1,CP0_PRID,1
    andi    k1,k1,0x1
    dsll    k1,3
    dla     k0,current_running # 加载current_running的PCB
    addu    k0,k0,k1
    ld      k0,0x0(k0)
    addiu   k0,k0,\offset
    ld      AT,OFFSET_REG1(k0)

    ld      v0,OFFSET_REG2(k0)
    ld      v1,OFFSET_REG3(k0)

    ld      a0,OFFSET_REG4(k0)
    ld      a1,OFFSET_REG5(k0)
    ld      a2,OFFSET_REG6(k0)
    ld      a3,OFFSET_REG7(k0)

    ld      t0,OFFSET_REG8(k0)
    ld      t1,OFFSET_REG9(k0)
    ld      t2,OFFSET_REG10(k0)
    ld      t3,OFFSET_REG11(k0)
    ld      t4,OFFSET_REG12(k0)
    ld      t5,OFFSET_REG13(k0)
    ld      t6,OFFSET_REG14(k0)
    ld      t7,OFFSET_REG15(k0)
    ld      t8,OFFSET_REG24(k0)
    ld      t9,OFFSET_REG25(k0)

    ld      s0,OFFSET_REG16(k0)
    ld      s1,OFFSET_REG17(k0)
    ld      s2,OFFSET_REG18(k0)
    ld      s3,OFFSET_REG19(k0)
    ld      s4,OFFSET_REG20(k0)
    ld      s5,OFFSET_REG21(k0)
    ld      s6,OFFSET_REG22(k0)
    ld      s7,OFFSET_REG23(k0)
    ld      s8,OFFSET_REG30(k0)
    # k0,k1一般不会用
    ld      gp,OFFSET_REG28(k0)
    ld      sp,OFFSET_REG29(k0)
    ld      fp,OFFSET_REG30(k0)
    ld      ra,OFFSET_REG31(k0)

    lw      k1,OFFSET_STATUS(k0)
    mtc0    k1,CP0_STATUS
    lw      k1,OFFSET_CAUSE(k0)
    mtc0    k1,CP0_CAUSE
    ld      k1,OFFSET_HI(k0)
    mtc0    k1,CP0_WATCHHI
    ld      k1,OFFSET_LO(k0)
    mtc0    k1,CP0_WATCHLO
    ld      k1,OFFSET_BADVADDR(k0)
    mtc0    k1,CP0_BADVADDR
    ld      k1,OFFSET_EPC(k0)
    mtc0    k1,CP0_EPC
    # ld      k1,OFFSET_PC(k0)
    # mtc0    k1,CP0_PC

   .set    at
.endm

# LEAF(check_kernel)
#     判断是否在内核态
#     dla    k0,current_running
#     ld     k0,0x0(k0)
#     ld     k0,NESTED_COUNT(k0) # kernel_state
#     addiu  k1,zero,1
#     beq    k0,k1,L1
#     li     ra,0x123456
# L1:
#     add    k0,zero,ra
#     jalr   k0
# END(check_kernel)

# LEAF(check_user)
#     判断是否在内核态
#     dla   k0,current_running
#     ld    k0,0x0(k0)
#     ld    k0,NESTED_COUNT(k0) # kernel_state
#     addiu k1,zero,0
#     beq   k0,k1,L2
#     li    ra,0x123456
# L2:
#     add   k0,zero,ra
#     jalr  k0
# END(check_user)

.macro set_user
    .set    noat
    mfc0  k1,CP0_PRID,1
    andi  k1,k1,0x1
    dsll  k1,3
    dla   k0,current_running
    addu  k0,k0,k1
    ld    k0,0x0(k0)
    li    k1,0
    sd    k1,NESTED_COUNT(k0) # kernel_state

   .set    at
.endm

.macro set_kernel
    .set    noat
    mfc0  k1,CP0_PRID,1
    andi  k1,k1,0x1
    dsll  k1,3
    dla   k0,current_running
    addu  k0,k0,k1
    ld    k0,0x0(k0)
    li    k1,1
    sd    k1,NESTED_COUNT(k0) # kernel_state

   .set    at
.endm

# function do_scheduler
NESTED(do_scheduler, 0, ra) # 在这里写
    SAVE_CONTEXT(KERNEL)
    jal   scheduler
    RESTORE_CONTEXT(KERNEL)
    jr    ra
END(do_scheduler)

LEAF(reset_timer) 
    
END(reset_timer)

LEAF(set_cp0_status)
    mtc0 a0,CP0_STATUS
    jr   ra
END(set_cp0_status)

LEAF(set_cp0_cause)
   
END(set_cp0_cause)

LEAF(get_cp0_status)
    mfc0 v0,CP0_STATUS
    jr   ra
END(get_cp0_status)

LEAF(get_cp0_cause)
    mfc0 v0,CP0_CAUSE
    jr   ra
END(get_cp0_cause)

LEAF(set_cp0_count)
    mtc0 a0,CP0_COUNT
    jr   ra
END(set_cp0_count)

LEAF(set_cp0_epc)
    mtc0 a0,CP0_EPC
    jr   ra
END(set_cp0_epc)

LEAF(get_cp0_count)
	mfc0 v0,CP0_COUNT
    jr   ra
END(get_cp0_count)

LEAF(get_cp0_compare)
    mfc0 v0,CP0_COUNT
    jr   ra
END(get_cp0_compare)

LEAF(set_cp0_compare)
    mtc0 a0,CP0_COMPARE
    jr   ra
END(set_cp0_compare)

LEAF(get_cp0_prid)
    mfc0   v0,CP0_PRID,1
    andi   v0,v0,0x1
    jr   ra
END(get_cp0_prid)
 
.global exception_handler_begin
.global exception_handler_end
.global TLBexception_handler_begin
.global TLBexception_handler_end

.macro open_i
    .set noat
    mfc0  k0,CP0_STATUS
    ori   k0,k0,0x1
    mtc0  k0,CP0_STATUS
    .set at
.endm
.macro close_i
    .set noat
    mfc0   k0,CP0_STATUS
    dsrl   k0,k0,1
    dsll   k0,k0,1
    mtc0   k0,CP0_STATUS
    .set at
.endm

.macro reset_count
    .set noat
    mtc0  zero,CP0_COUNT
    .set at
.endm

NESTED(TLBexception_handler_entry, 0, sp)
TLBexception_handler_begin:
    j   0xffffffff80000180
TLBexception_handler_end:
END(TLBexception_handler_entry)

NESTED(exception_handler_entry, 0, sp)   
exception_handler_begin:
    #close_i              #关全局中断aaaaaa
    set_kernel
    SAVE_CONTEXT(USER)
    switch_stack(KERNEL)
    # spin_lock
    mfc0 k0,CP0_CAUSE
    andi k0,k0,ExcCode   #只有excode
    dsll k0,k0,1         #左移一位，相当于index*8
    dla  k1,exception_handler
    dadd k0,k0,k1
    ld   k0,0(k0)
    jr   k0              #有返回地址
exception_handler_end:
END(exception_handler_entry)

NESTED(handle_int, 0, sp)
    mfc0  a0,CP0_STATUS
    mfc0  a1,CP0_CAUSE
    jal   interrupt_helper
first_entry:
    set_user
    # spin_unlock
    RESTORE_CONTEXT(USER)
    mtc0  zero,CP0_COUNT
    li k0, 150000
    mtc0 k0, CP0_COMPARE
    eret 
END(handle_int)

NESTED(handle_syscall, 0, sp)
    addiu a3,a2,0
    addiu a2,a1,0
    addiu a1,a0,0  
    addiu a0,v0,0
    jal system_call_helper #又变成了四个参数
    nop
    set_user
    # spin_unlock
    RESTORE_CONTEXT(USER)
    eret 
END(handle_syscall)

NESTED(handle_tlb, 0, sp)
   jal  do_TLB_Refill
   # spin_unlock
   RESTORE_CONTEXT(USER)
   eret
END(handle_tlb)

NESTED(handle_other, 0, sp)
    mfc0  a0,CP0_STATUS
    mfc0  a1,CP0_CAUSE
    jal   interrupt_helper
    set_user
    # spin_unlock
    RESTORE_CONTEXT(USER)
    eret 
END(handle_other)

NESTED(exception_handler_exit, 0, sp)

END(exception_handler_exit)

LEAF(set_cp0_entryhi)
    mtc0 a0,CP0_ENTRYHI
    jr   ra
END(set_cp0_entryhi)

LEAF(get_cp0_epc)
    mfc0 v0, CP0_EPC
    jr   ra
END(get_cp0_epc)

LEAF(get_cp0_index)
    mfc0 v0, CP0_INDEX
    jr   ra 
END(get_cp0_index)

LEAF(set_cp0_index)
    mtc0 a0,CP0_INDEX
    jr   ra
END(set_cp0_index)

LEAF(get_cp0_badvaddr)
    mfc0  v0,CP0_BADVADDR
    jr    ra
END(get_cp0_badvaddr)

LEAF(get_cp0_entrylo0)
    mfc0  v0,CP0_ENTRYLO0
    jr    ra
END(get_cp0_entrylo0)

LEAF(set_cp0_entrylo0)
    mtc0 a0,CP0_ENTRYLO0
    jr   ra
END(set_cp0_entrylo0)

LEAF(get_cp0_entrylo1)
    mfc0  v0,CP0_ENTRYLO1
    jr    ra
END(get_cp0_entrylo1)

LEAF(get_cp0_entryhi)
    mfc0  v0,CP0_ENTRYHI
    jr    ra
END(get_cp0_entryhi)

LEAF(set_cp0_entrylo1)
    mtc0 a0,CP0_ENTRYLO1
    jr   ra
END(set_cp0_entrylo1)

LEAF(set_cp0_pagemask)
    mtc0 a0,CP0_PAGEMASK
    jr   ra
END(set_cp0_pagemask)

LEAF(tlbwr_operation)
   
END(tlbwr_operation)

LEAF(tlbwi_operation)
    
END(tlbwi_operation)

LEAF(tlbp_operation)
    
END(tlbp_operation)

LEAF(get_cp0_context)
    mfc0 v0,CP0_CONTEXT
    jr   ra 
END(get_cp0_context)

LEAF(get_cp0_config)
    
END(get_cp0_config)

LEAF(get_cpu_id)
    
END(get_cpu_id)


