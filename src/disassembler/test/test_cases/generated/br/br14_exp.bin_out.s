movz x11, #0xadd6, lsl #0
movk x11, #0xe, lsl #16
movz x21, #0x0, lsl #0
movk x21, #0x8a00, lsl #16
str x21, [x11], #0
br x11
and x0, x0, x0
