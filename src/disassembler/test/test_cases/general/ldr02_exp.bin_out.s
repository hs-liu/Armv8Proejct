movz x0, #0x2, lsl #0
ldr x2, [x0]
movz x0, #0x300, lsl #16
subs x31, x2, x0, lsl #0
and x0, x0, x0
