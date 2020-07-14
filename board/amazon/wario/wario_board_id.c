// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2020 Brian Moyer <bdm310@gmail.com>
 */

#include <stddef.h>
#include <common.h>
#include "wario_board_id.h"

int board_info_valid (char *info, int len)
{
    for (int i = 0; i < len; i++) {
        if ((info[i] < '0') &&
            (info[i] > '9') &&
            (info[i] < 'A') &&
            (info[i] > 'Z'))
            return 0;
    }

    return 1;
}

const struct board_type *get_board_type(char *pcbsn)
{
    int i;

    if (!board_info_valid(pcbsn, BOARD_PCBA_LEN)) {
        printf("board inf invalid: %s\n", pcbsn);
        return NULL;
    }

    for (i = 0; i < NUM_KNOWN_BOARDS; i++) {
        if (strncmp((const char *) pcbsn, boards[i].id, strlen(boards[i].id)) == 0) {
            return &(boards[i]);
        }
    }

    return NULL;
}