#pragma once

struct commands_t {
    /* Struct of read command */
    bool addToExistFold;
    bool replExistFold;
    bool replExistFile;
};

extern struct commands_t commands;

void init(void);
void help(void);
