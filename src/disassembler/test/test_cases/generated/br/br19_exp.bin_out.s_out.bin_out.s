movz x17, #0x217f, lsl #0
movk x17, #0x8, lsl #16
movz x29, #0x0, lsl #0
movk x29, #0x8a00, lsl #16
str x29, [x17], #0
br x17
and x0, x0, x0
