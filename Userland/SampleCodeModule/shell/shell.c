#include <shell.h>

void commandExecute();

static TBuffer commandBuffer = {.size=0};

void push(char c){
    commandBuffer.items[commandBuffer.size++] = c;
}

void pop(){
    commandBuffer.size--;
}

void freeBuffer(){
    commandBuffer.size = 0;
}

void writeShell() {
    
    char c = getchar();

    if (c != 0){
        if (c == '\n'){
            commandExecute();
        }
        else if (c == BACKSPACE){
            if (commandBuffer.size > 0){
                pop();
                putchar(BACKSPACE);
            }
        }
        else if (c == '\t'){
            unsigned int len = commandBuffer.size;
            char * com = fillCommand(commandBuffer.items, len);

            for (int i = 0; (com != NULL) && (com[i] != '\0'); i++){
                if (i >= len){
                    push(com[i]);
                    putchar(com[i]);
                }
            }
        }
        else{
            push(c);
            putchar(c);
        }
        
    }

    return;
}

void printPrompt(){
    setTextColor(CYAN);
    printf("fidOS@User> ");
    setTextColor(WHITE);

    return;
}

void commandExecute() {
    
    // TODO: Ejecutar comando.

    char * comando;

    strncpy(comando, commandBuffer.items, commandBuffer.size);

    execute(comando);

    freeBuffer();

    printPrompt();

    return;
}

void printCursor(){
    
}

void shell(){
    
    printPrompt();

	while (1){
        writeShell();
    }
}