ldr x2, #24
add x1, x2, #0x2, lsl #0
subs x31, x3, #0x2, lsl #0
b.ne 0x14
add x1, x2, #0x5, lsl #0
and x0, x0, x0
.int 0x20200020
.int 0x0
.int 0x0
