stwu    sp,-0x80 (sp)       # save the stack
mflr    r11
stmw    r3, 0x8 (sp)

lis     r3, 0x8036          # load the address of "Failed to open %s\n" into r3
ori     r3, r3, 0xd670

lis     r12, 0x8034         # load the address of printf into r12
ori     r12, r12, 0x7214
mtlr    r12                 # move r12 into the link register
blrl                        # call printf

lmw     r3, 0x8 (sp)        # restore the stack
mtlr    r11
addi    sp, sp, 0x80

stwu    sp, -0x0110 (sp)    # original code of the location of our hook