
#define SELECT_BITS(value, offset, size) (((value) >> (offset)) & ((1ll << (size)) - 1))
#define CHECK_BITS(value, mask, target_value) (((value) & (mask)) == (target_value))
#define SIGN_EXT(value, in_size, out_size) ((value) << ((out_size) - (in_size)) >> ((out_size) - (in_size)))

#define LED_CHANNEL 8
#define NUM_OF_TAGS 1 //based on spec
#define MAX_MESSAGES 10
#define BASE_ADDRESS 0x3f000000
#define START_OFFSET 0x80000
#define READ_OFFSET 0x00
#define WRITE_OFFSET 0x20
#define STATUS_OFFSET 0x38
#define MAILBOX_OFFSET 0xb880

#define MAILBOX_ADDRESS BASE_ADDRESS+MAILBOX_OFFSET
#define MAILBOX_READ ((mailbox_message_t *)(MAILBOX_ADDRESS+READ_OFFSET))
#define MAILBOX_WRITE ((mailbox_message_t *)(MAILBOX_BASE_ADDRESS+WRITE_OFFSET))
#define MAILBOX_STATUS ((mail_status_t *)(MAILBOX_BASE_ADDRESS+STATUS_OFFSET))

#define LED_ON 0x00000001
#define LED_OFF 0x00000000

#define WAIT_SECONDS 3


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
    mailbox_message_t mails[MAX_MESSAGES]; // from spec: ignored/not used for responses
    uint16_t len;
} mail_queue_t;

typedef struct {
    uint32_t id;
    uint32_t size;
    uint32_t code;
    uint32_t pin;
    uint32_t state;
} msg_tag_t;

typedef struct {
    uint32_t size;
    uint32_t code;
    msg_tag_t *tag;
    uint32_t end;
} req_res_buffer_t;

extern void mailbox_send(mail_queue_t *requests, mail_queue_t *responses, mailbox_message_t msg);
extern mailbox_message_t mailbox_read(mail_queue_t *requests, mail_queue_t *responses);