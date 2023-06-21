movz x18, #0x6f66, lsl #0
movk x18, #0xc, lsl #16
movz w20, #0x0, lsl #0
movk w20, #0x8a00, lsl #16
str w20, [x18], #0
br x18
and x0, x0, x0
