movz x2, #0x6653, lsl #0
movk x2, #0xc, lsl #16
movz x21, #0x0, lsl #0
movk x21, #0x8a00, lsl #16
str x21, [x2], #0
br x2
and x0, x0, x0
