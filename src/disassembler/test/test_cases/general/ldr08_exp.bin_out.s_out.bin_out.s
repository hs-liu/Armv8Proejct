ldr x0, #16
ldr x2, #28
add x2, x2, #0x8, lsl #0
and x0, x0, x0
.int 0x20200020
.int 0x0
.int 0x0
.int 0x20200028
.int 0x0
