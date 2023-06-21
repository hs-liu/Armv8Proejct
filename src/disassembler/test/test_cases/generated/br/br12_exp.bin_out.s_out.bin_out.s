movz x6, #0xf5dd, lsl #0
movk x6, #0x7, lsl #16
movz w16, #0x0, lsl #0
movk w16, #0x8a00, lsl #16
str w16, [x6], #0
br x6
and x0, x0, x0
