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

check_write_empty: //every send msg need to check F empty
    ldr w5, wzr
    orr w5, wzr, w1, lsr #31 // get F flag
    cmp w5, wzr
    b.ne check_write_empty // can write msg if zero

check_status:
    orr w5, wzr, w2, lsr #0x18//check_status
    cmp w5, wzr
    b.eq  set_on//if status = off, go set on
    orr w2, w2, w2, lsl #0x18//if status = on, set off <?>

send_msg_loop:
    ldr x0, [x4] // send msg into write_reg
    movz x3, #0x0002, lsl #16

wait1: //first wait
    sub x3, x3, #1
    cmp x3, #0
    b.ne wait1
    b check_write_empty
    
set_on:
    movz x3, #0x00000001
    orr w2, w2, x3, lsl #0x18
    b send_msg_loop

//TODO: reset buffer values;
reset_buffer:

//read register
//check_queue_sizegit
