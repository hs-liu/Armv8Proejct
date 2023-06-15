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

int main(int argc, char **argv) {
    mail_queue *responses = malloc(sizeof(mail_queue));
    mail_queue *requests = malloc(sizeof(mail_queue));
}