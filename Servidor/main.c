#include <stdio.h>
#include <stdlib.h>
#include "commands.h"
#include "server-defaults.h"

EditorData eData;
ServerData sData;

void initializeStructs();

int main(int argc, char** argv) {
    checkArgs(argc, argv);
    
    getEnvironmentVariables();
    
    initializeStructs();
    
    
    readCommands();
    
    return (EXIT_SUCCESS);
}

void initializeStructs(){
    
}

