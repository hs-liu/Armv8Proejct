ldr x1, #16
ldr x2, [x1]
ldr x3, [x1, #8]
and x0, x0, x0
.int 0x1f8000
.int 0x0
