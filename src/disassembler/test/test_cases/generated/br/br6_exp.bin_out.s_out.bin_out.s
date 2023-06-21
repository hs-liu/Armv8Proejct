movz x18, #0x921c, lsl #0
movk x18, #0x1, lsl #16
movz x8, #0x0, lsl #0
movk x8, #0x8a00, lsl #16
str x8, [x18], #0
br x18
and x0, x0, x0
