#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "led.h"

static int check_full(mail_queue_t *queue) {
    if (queue->len >= MAX_MESSAGES) {
        return 1;
    } else {
        return 0;
    }
}

mailbox_message_t mailbox_read(mail_queue_t *requests, mail_queue_t *responses) {
    mail_status_t status;
    mailbox_message_t resp;

    requests->len--;
    responses->len++;
    // TODO: remove from request queue

    do {
        status = *MAILBOX_STATUS;
    } while (status.E);

    resp = *MAILBOX_READ;
    responses->len--;
    responses->mails[responses->len] = resp;
    return resp;
}

void mailbox_send(mail_queue_t *requests, mail_queue_t *responses, mailbox_message_t msg) {
    mail_status_t status;

    do {
        status = *MAILBOX_STATUS;
    } while (status.F);

    if (!check_full(responses)) {
        // write message to write_reg
        *MAILBOX_WRITE = msg;
        requests->len++;
        // call queue function to move every elem forward OR move pointer for circular
    }
    // ignore this request if response queue is full
}

int main (int argc, char **argv) {
    mail_queue_t *responses = malloc(sizeof(mail_queue_t));
    mail_queue_t *requests = malloc(sizeof(mail_queue_t));
}