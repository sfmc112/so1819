#include <stdio.h>
#include <stdlib.h>
#include "commands.h"
#include "server-functions.h"

EditorData eData;
ServerData sData;


int main(int argc, char** argv) {
    
    checkArgs(argc, argv, &sData);
    
    getEnvironmentVariables(&eData);
    
    defineMaxUsers(&sData);
    initializeMEDITLines(&eData);
    
    readCommands();
    
    return (EXIT_SUCCESS);
}

