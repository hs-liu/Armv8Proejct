movz x1, #0x1, lsl #0
movz x2, #0x1, lsl #0
subs x31, x1, x2, lsl #0
b.ne 0x14
movz x3, #0x3, lsl #0
movz x4, #0x4, lsl #0
and x0, x0, x0
