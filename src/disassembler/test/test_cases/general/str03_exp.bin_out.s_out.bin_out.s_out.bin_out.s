movz x2, #0x2, lsl #0
movz x0, #0x1, lsl #0
movz x1, #0x63, lsl #0
str x1, [x0, #288]
movz x0, #0x121, lsl #0
sub x2, x2, #0x1, lsl #0
subs x31, x2, #0x0, lsl #0
str x1, [x0, #1]!
b.ne 0x14
and x0, x0, x0
