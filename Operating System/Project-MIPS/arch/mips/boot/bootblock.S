.text
.global main

main:
	# 1) call BIOS print string "It's bootblock!"
	dla  $a0,msg
	ld   $t0,printstr
	jalr $t0  
	nop

	# 2) call BIOS read kernel in SD card
	# ld  $a0,kernel
	# daddi $a1,$zero,512
	# daddi $a2,$zero,512
	# ld  $t0,read_sd_card
	# jalr  $t0
	# nop

	# 2.5)big kernel and relocate
	ld    $a0,kernel1
	daddi $a1,$zero,512
	ld    $a2,kernel_num
	lw    $a2,0($a2)   # 16bit/32bit lh(16) lw(32)
	ld    $t0,read_sd_card
	ld    $ra,kernel_main1
	jr    $t0

	# ld    $ra,kernel_main1  # 利用分支延迟槽
	

	# 3) jump to kernel  要跳转到kernel里面去执行.c
	# ld $t0,kernel_main
	# jalr  $t0
	# nop




# 4) while(1) --> stop here
 L1: lui $t0,1
	lui $t1,1
	beq $t0,$t1,L1


.data

kernel_num: .dword 0xffffffffa08001fc
msg: .ascii "It's a bootloader...\n"

# 1. PMON read SD card function address
# read_sd_card();
read_sd_card: .dword 0xffffffff8f0d5e10

# 2. PMON print string function address
# printstr(char *string)
printstr: .dword 0xffffffff8f0d5534

# 3. PMON print char function address
# printch(char ch)
printch: .dword 0xffffffff8f0d5570

# 4. kernel address (move kernel to here ~)
kernel: .dword 0xffffffffa0800200

# 5. kernel main address (jmp here to start kernel main!)
kernel_main: .dword 0xffffffffa0800200

# 6. kernel address (move kernel to here ~)
kernel1: .dword 0xffffffffa0800000

# 7. kernel main address (jmp here to start kernel main!)
kernel_main1: .dword 0xffffffffa0800000
