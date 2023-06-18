.section .data
mailbox_base:   .equ 0x3F000000
mailbox_offset: .equ 0xB880
read_offset:    .equ 0x00
write_offset:   .equ 0x20
status_offset:  .equ 0x18
led_channel:    .equ 8
led_on_msg:     .equ 0x00010001
led_off_msg:    .equ 0x00010000
delay_count:    .equ 20000000

.section .text
.global _start

_start:
    // Open /dev/mem
    mov x0, 0
    mov x1, 0x01C00000
    mov x2, 0x10
    mov x7, 8
    svc 0

    // Map peripheral base to virtual memory
    lsl x1, x0, #12
    add x0, x1, mailbox_offset
    mov x1, 0x1000
    lsl x2, x0, #1
    orr x2, x2, #0x0000001C
    lsl x3, x2, #12
    mov x7, 0
    mov x8, 0x3
    mov x0, 0
    mov x2, 0x00000000
    mov x3, 0x00000000
    mov x4, 0x00000000
    mov x5, 0x00000000
    mov x6, 0x00000000
    mov x7, 0x00000000
    svc 0

    // Main loop
loop:
    // Turn on LED
    lsl x1, x0, #2
    ldr x2, =led_on_msg
    orr x2, x2, x1, lsl #4
    str x2, [x0, write_offset]

    // Delay
    mov x1, delay_count
delay:
    subs x1, x1, #1
    bne delay

    // Turn off LED
    lsl x1, x0, #2
    ldr x2, =led_off_msg
    orr x2, x2, x1, lsl #4
    str x2, [x0, write_offset]

    // Delay
    mov x1, delay_count
delay2:
    subs x1, x1, #1
    bne delay2

    b loop

    // Exit program
exit:
    mov x8, 0x18
    mov x0, 0
    mov x1, 0
    mov x2, 0
    mov x3, 0
    mov x4, 0
    mov x7, 0x10
    svc 0