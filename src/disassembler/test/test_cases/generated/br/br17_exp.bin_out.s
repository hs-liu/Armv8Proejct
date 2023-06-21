movz x10, #0xe520, lsl #0
movk x10, #0xb, lsl #16
movz x27, #0x0, lsl #0
movk x27, #0x8a00, lsl #16
str x27, [x10], #0
br x10
and x0, x0, x0
