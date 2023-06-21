movz x13, #0xcf69, lsl #0
movk x13, #0xb, lsl #16
movz x26, #0x0, lsl #0
movk x26, #0x8a00, lsl #16
str x26, [x13], #0
br x13
and x0, x0, x0
