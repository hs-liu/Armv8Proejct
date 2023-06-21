ldr x1, [x0], #2
ldr x2, #0
ldr x0, #28
add x0, x0, #0x2, lsl #0
b 0x14
ldr x0, #32
b 0x28
.int 0x1
.int 0x2
b 0x24
and x0, x0, x0
