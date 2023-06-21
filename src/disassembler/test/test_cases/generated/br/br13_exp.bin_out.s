movz x23, #0x4931, lsl #0
movk x23, #0xd, lsl #16
movz x17, #0x0, lsl #0
movk x17, #0x8a00, lsl #16
str x17, [x23], #0
br x23
and x0, x0, x0
