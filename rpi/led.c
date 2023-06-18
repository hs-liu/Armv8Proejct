#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "led.h"

static int check_full(mail_queue_t *queue) {
    if (queue->len >= MAX_MESSAGES) {
        return 1;
    } else {
        return 0;
    }
}

static void switch_state(req_res_buffer_t *buffer) {
    // rewrite buffer
    buffer->tag->pin = 130;
    if (buffer->tag->state == LED_ON) {
        buffer->tag->state = LED_OFF;
    } else {
        buffer->tag->state = LED_ON;
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

void sleep(double s) {
    time_t cur = time(NULL); // NULL because we don't need to save the time in a specific address
    while ((difftime(time(NULL), cur)) < s);
}

int main (int argc, char **argv) {
    // allocate memory for the response and required queues
    mail_queue_t *responses;
    responses = malloc(sizeof(mail_queue_t));
    mail_queue_t *requests;
    requests = malloc(sizeof(mail_queue_t));

    // declare the power LED on buffer:
    req_res_buffer_t *buffer;
    buffer = malloc(sizeof(req_res_buffer_t)); //TODO: ask about 16-byte alignment
    buffer->size = 32; buffer->code = 0; buffer->tag = malloc(MAX_MESSAGES*sizeof(msg_tag_t));
    buffer->tag->id = 0x00038041; buffer->tag->size = 0; buffer->tag->code = 0x0; buffer->tag->pin = 130;
    buffer->tag->state = LED_ON;

    // declare message sent to write register:
    mailbox_message_t *msg;
    msg = malloc(sizeof(mailbox_message_t));
    msg->channel = LED_CHANNEL; msg->data = &buffer + START_OFFSET;

    while (1) {

        // send message to mailbox to turn on/off LED
        mailbox_send(requests, responses, *msg);
        mailbox_read(requests,responses);

        // wait for certain amount of time
        sleep(WAIT_SECONDS);

        // reset buffer tag
        switch_state(buffer);
    }
}