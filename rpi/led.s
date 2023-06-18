ldr w0, #0x3f00b8A0 // address of mailbox write reg
ldr w1, #0x3F00B8B8 // addr of mailbox status reg

ldr w2, #0x00000020 // buffer start
orr w2, wzr, wzr, lsl #0x04
movz w3, #0x00038041 // temp value holder reg
orr w2, wzr, w3, lsl #0x08
movz w3, #0x00000008
orr w2, wzr, w3, lsl #0x0c
orr w2, wzr, wzr, lsl #0x010
movz w3, #0x00000082
orr w2, wzr, w3, lsl #0x014
movz w3, #0x00000001
orr w2, wzr, w3, lsl #0x18 //led_status: on
orr w2, wzr, wzr, lsl #0x1c

ldr w4, #0x8 //message: channel
ldr x3, [x4]
add w3, w3, #0x80000, lsl #0
orr w4, wzr, w3, lsl #4

check_write_empty:
    ldr w5, #0x1000
    orr w5, wzr, w1, lsr #31 // get F flag
    cmp w5, wzr
    b.ne check_write_empty // can write msg if zero

on_loop:
    ldr x0, [x4] // load msg into write_reg
    movz x3, #0x0002, lsl #16

wait1:
    sub x3, x3, #1
    cmp x3, #0
    b.ne wait1
    
off_loop:
    orr w2, wzr, wzr, lsl #0x18 // set led_off; TODO: reset buffer values
    movz x3, #0x0002, lsl #16

wait2:
    sub x3, x3, #1
    cmp x3, #0
    b.ne wait2
    b on_loop

//TODO: reset buffer values;
//read register
//check_queue_size
