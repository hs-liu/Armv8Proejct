movk x1, #0x1, lsl #0
movk x2, #0x1, lsl #0
ands x31, x1, x2, lsl #0
b.eq 0x14
movk x3, #0x3, lsl #0
movk x4, #0x4, lsl #0
and x0, x0, x0
