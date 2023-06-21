movz x5, #0x2d27, lsl #0
movk x5, #0xe, lsl #16
movz x17, #0x0, lsl #0
movk x17, #0x8a00, lsl #16
str x17, [x5], #0
br x5
and x0, x0, x0
