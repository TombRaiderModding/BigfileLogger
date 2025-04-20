.ps2
.create "output.bin", 0

; save the registers
addi    $sp, $sp, -8
sw      $s0, 0($sp)
sw      $a0, 4($sp)

; load the address of the format string
lui     $s0, 0x005f
ori     $s0, $s0, 0x9f58

; print the value
li      $v1, 117    ; print
move    $a0, $s0    ; format string
move    $a1, $a1    ; redundant but for readability
syscall
nop                 ; delay slot

; restore the registers
lw      $a0, 4($sp)
lw      $s0, 0($sp)
addi    $sp, $sp, 8

; original instructions at our jump
addiu   sp, sp, -0x110
sd      s0, 0x100(sp)

; jump back to the original code
j       0x314620
nop                 ; delay slot

.close