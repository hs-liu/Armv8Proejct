movz x0, #0x1, lsl #0
movz x1, #0x5, lsl #0
madd x2, x1, x0, x31
add x0, x2, #0x0, lsl #0
sub x1, x1, #0x1, lsl #0
subs x31, x1, #0x0, lsl #0
b.ne 0x8
movz x3, #0x100, lsl #0
str x2, [x3]
and x0, x0, x0
