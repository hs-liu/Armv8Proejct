movz w0, #0xef12, lsl #16
movk x0, #0x1234, lsl #32
movz x0, #0xabcd, lsl #0
and x0, x0, x0
