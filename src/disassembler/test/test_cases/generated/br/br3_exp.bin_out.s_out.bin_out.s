movz x3, #0x223d, lsl #0
movk x3, #0x2, lsl #16
movz w16, #0x0, lsl #0
movk w16, #0x8a00, lsl #16
str w16, [x3], #0
br x3
and x0, x0, x0
