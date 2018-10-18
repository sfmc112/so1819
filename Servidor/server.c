#include <stdio.h>
#include <stdlib.h>
#include "commands.h"
#include "server-functions.h"

EditorData eData;
ServerData sData;

void initializeStructs();

int main(int argc, char** argv) {
    checkArgs(argc, argv, &sData);
    
    //getEnvironmentVariables();
    
    initializeStructs();
    
    readCommands();
    
    return (EXIT_SUCCESS);
}

void initializeStructs(){
    
}

