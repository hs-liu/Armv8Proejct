movz x28, #0xa681, lsl #0
movk x28, #0xb, lsl #16
movz w0, #0x0, lsl #0
movk w0, #0x8a00, lsl #16
str w0, [x28], #0
br x28
and x0, x0, x0
