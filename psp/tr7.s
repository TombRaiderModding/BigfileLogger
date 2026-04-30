.psp
.create "output.bin", 0

; save registers
addi    $sp, $sp, -20
sw      $a0, 0($sp)
sw      $a1, 4($sp)
sw      $a2, 8($sp)
sw      $v0, 12($sp)
sw      $ra, 16($sp)

; load the address of the format string
lui     $t0, 0x08b4
ori     $t0, $t0, 0xf668

; load the address of sceKernelPrintf 
lui     $t1, 0x08b4
ori     $t1, $t1, 0xc470

; print the value
move    $a0, $t0    ; format string
move    $a1, $a1    ; value
jalr    $t1         ; sceKernelPrintf($a0, $a1)
nop                 ; delay slot

; restore registers
lw      $ra, 16($sp)
lw      $v0, 12($sp)
lw      $a2, 8($sp)
lw      $a1, 4($sp)
lw      $a0, 0($sp)
addi    $sp, $sp, 20

; original instructions at our hook
addiu   sp, sp, -0x120
sw      s0, 0x110(sp)

; jump back
j       0x88beda8
nop                 ; delay slot

.close