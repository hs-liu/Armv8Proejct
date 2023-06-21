movz x2, #0xf141, lsl #0
movk x2, #0x1, lsl #16
movz x14, #0x0, lsl #0
movk x14, #0x8a00, lsl #16
str x14, [x2], #0
br x2
and x0, x0, x0
