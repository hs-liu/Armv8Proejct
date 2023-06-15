#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "led.h"

static int check_full(mail_queue *queue) {
    if (queue->len >= MAX_MESSAGES) {
        return 1;
    } else {
        return 0;
    }
}

void mailbox_send(mail_queue *requests, mail_queue *responses, mailbox_message_t msg) {
    mail_status_t status;

    do {
        status = *MAILBOX_STATUS;
    } while (status.F)

    if (!check_full(responses)) {
        *MAILBOX_WRITE = msg;
        requests->len++;
    }
    // ignore this request if response queue is full
}

int main(int argc, char **argv) {
    mail_queue *responses = malloc(sizeof(mail_queue));
    mail_queue *requests = malloc(sizeof(mail_queue));
}