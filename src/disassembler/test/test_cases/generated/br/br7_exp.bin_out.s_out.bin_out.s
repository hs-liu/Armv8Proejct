movz x0, #0xadfb, lsl #0
movk x0, #0x6, lsl #16
movz w29, #0x0, lsl #0
movk w29, #0x8a00, lsl #16
str w29, [x0], #0
br x0
and x0, x0, x0
