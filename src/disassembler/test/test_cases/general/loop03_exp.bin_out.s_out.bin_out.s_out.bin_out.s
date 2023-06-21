movz x2, #0xa, lsl #0
sub x2, x2, #0x1, lsl #0
subs x31, x2, #0x0, lsl #0
b.ne 0x4
and x0, x0, x0
