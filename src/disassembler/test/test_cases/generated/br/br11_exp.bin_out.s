movz x30, #0x1b99, lsl #0
movk x30, #0x3, lsl #16
movz w6, #0x0, lsl #0
movk w6, #0x8a00, lsl #16
str w6, [x30], #0
br x30
and x0, x0, x0
