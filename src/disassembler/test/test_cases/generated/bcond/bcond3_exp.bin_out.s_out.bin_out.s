add w11, w8, #0x1, lsl #0
add w8, w8, #0x2, lsl #0
subs w31, w8, w11, lsl #0
b.ne 0x14
add w4, w4, #0x3ba, lsl #0
add w4, w4, #0x4ce, lsl #12
and x0, x0, x0
