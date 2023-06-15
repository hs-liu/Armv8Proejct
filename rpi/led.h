
#define SELECT_BITS(value, offset, size) (((value) >> (offset)) & ((1ll << (size)) - 1))
#define CHECK_BITS(value, mask, target_value) (((value) & (mask)) == (target_value))
#define SIGN_EXT(value, in_size, out_size) ((value) << ((out_size) - (in_size)) >> ((out_size) - (in_size)))

#define LED_CHANNEL 8
#define BASE_ADDRESS 0x3f000000
#define READ_OFFSET 0x00
#define WRITE_OFFSET 0x20
#define STATUS_OFFSET 0x38
#define MAILBOX_OFFSET 0xb880

#define MAILBOX_ADDRESS BASE_ADDRESS+MAILBOX_OFFSET
#define MAILBOX_READ ((mailbox_message_t *)(BASE_ADDRESS+READ_OFFSET))
#define MAILBOX_WRITE ((mailbox_message_t *)(BASE_ADDRESS+WRITE_OFFSET))
#define MAILBOX_STATUS ((mailbox_message_t *)(BASE_ADDRESS+STATUS_OFFSET))



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

void mailbox_sent(mail_queue *requests, mailbox_message_t mag);