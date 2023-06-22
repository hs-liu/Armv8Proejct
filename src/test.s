    movz x3, #0x1000
    and x0, x0, x0
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
    .int 0x1
    .int 0x0
