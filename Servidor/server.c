#include <stdio.h>
#include <stdlib.h>
#include "commands.h"
#include "server-functions.h"
int main(int argc, char** argv) {
    checkArgs();
    readCommands();
    return (EXIT_SUCCESS);
}

