#include "asm.h"

LEAF(invoke_syscall) 
    addiu v0,a0,0
    addiu a0,a1,0
    addiu a1,a2,0
    addiu a2,a3,0
    syscall  
    jr    ra
END(invoke_syscall)