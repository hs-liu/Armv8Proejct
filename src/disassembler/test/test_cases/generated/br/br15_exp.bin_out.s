movz x5, #0xc5ce, lsl #0
movk x5, #0x5, lsl #16
movz w25, #0x0, lsl #0
movk w25, #0x8a00, lsl #16
str w25, [x5], #0
br x5
and x0, x0, x0
