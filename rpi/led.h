
#define SELECT_BITS(value, offset, size) (((value) >> (offset)) & ((1ll << (size)) - 1))
#define CHECK_BITS(value, mask, target_value) (((value) & (mask)) == (target_value))
#define SIGN_EXT(value, in_size, out_size) ((value) << ((out_size) - (in_size)) >> ((out_size) - (in_size)))

#define LED_CHANNEL 8

typedef struct {
    uint8_t channel: 4; // : defines how many bits channel uses
    uint32_t data: 28;
} mailbox_message_t;

typedef struct {
    uint32_t reserved: 30;
    uint8_t E: 1;
    uint8_t F: 1;
} mail_status_t;

typedef struct {
    mailbox_message_t mails[10];
    uint16_t len;
} mail_queue;