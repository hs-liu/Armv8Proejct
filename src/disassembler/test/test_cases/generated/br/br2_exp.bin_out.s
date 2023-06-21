movz x17, #0xf64, lsl #0
movk x17, #0x9, lsl #16
movz x4, #0x0, lsl #0
movk x4, #0x8a00, lsl #16
str x4, [x17], #0
br x17
and x0, x0, x0
