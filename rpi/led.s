// R0: Read Register Address
// R1: Write Register Address
// R2: Write status Register Address
// R3, R4: Temporary registers
// R5: Address of led_buffer
// R6: Address of template_led_buffer
// R7: Read status register address

start:
    // Move Read Register Address into x0
    movz x0, #0x3f00, lsl #16
    movk x0, #0xb880
    // Move Write Register Address into x1
    add x1, x0, #0x20
    // Move Write Status Register Address into x2
    add x2, x0, #0x38
    // Move Read Status Register Address into x7
    add x7, x0, #0x18
    // Move led_buffer address into x5
    movz x5, #0x8, lsl #16
    add x5, x5, #0xa0 // TODO Address of buffer relative to base address 
    // Move led_buffer address into x6
    add x6, x5, #0x20

toggle_led_status:
    ldr w3, [x6, #0x18]
    movz w4, #1
    eor w3, w3, w4
    str w3, [x6, #0x18]

set_led_buffer:
    // Copy template buffer to led buffer
    ldr x3, [x6]
    str x3, [x5]
    ldr x3, [x6, #8]
    str x3, [x5, #8]
    ldr x3, [x6, #0x10]
    str x3, [x5, #0x10]
    ldr x3, [x6, #0x18]
    str x3, [x5, #0x18]

write:
    // Get status register
    ldr w3, [x2]
    // Test F flag of status register
    movz w4, #1
    tst w3, w4, lsl #31
    // Loop until F flag is clear
    b.ne write
write_buffer_to_register:
    // Move message (Address of buffer (led_buffer) | Property Tags channel (8)) into x3
    movz w3, #0x8
    orr w3, w3, w5
    // Write message to write register
    str w3, [x1]

read:
    // Get status register
    ldr w3, [x2]
    // Test E flag of status register
    movz w4, #1
    tst w3, w4, lsl #30
    // Loop until E flag is clear
    b.ne read
read_message_from_register:
    // Load from the read register
    ldr w3, [x0]

wait:
    movz x3, #0x0
    movz x4, #0x80, lsl #16
wait_loop:
    add x3, x3, #1
    cmp x3, x4
    b.lt wait_loop

    b toggle_led_status

nop
nop
led_buffer:
    .int 0
    .int 0
    .int 0
    .int 0
    .int 0
    .int 0
    .int 0
    .int 0

template_led_buffer:
    .int 32
    .int 0
    .int 0x00038041
    .int 8
    .int 0x0
    .int 130
    .int 0x0
    .int 0x0
