#include "./include/philosophers.h"

phi_t philoTable[MAX_PHILOSOPHERS];
char philoStates[MAX_PHILOSOPHERS];
int forks[MAX_PHILOSOPHERS];
char update = 1;
int philoCount;
int finishPhilo = 0;

 

void endPhilo(void){

    printf("Ending philosophers...\n");

    finishPhilo = 1;

    for (int i = 0; i < philoCount; i++){
        sysWait(philoTable[i].pid, NULL);
        semDestroy(forks[i]);
    }
}

void addPhilosopher(void){
    if (philoCount >= MAX_PHILOSOPHERS){
        printf("Maximum number of philosophers reached.\n");
        return;
    }

    creationParameters params;
    char philoIndex[4];
    itoa(philoCount, philoIndex);
    char baseName[MAX_NAME_LENGTH] = "philo_";
    params.name = baseName;
    strcat(params.name, philoIndex);
    params.argc = 1;
    char* argv[] = {philoIndex, NULL};
    params.argv = argv;
    params.priority = 1;
    params.entryPoint = (entryPoint)philosopher;
    params.foreground = 0;
    params.fds[0] = STDIN;
    params.fds[1] = STDOUT;

    philoTable[philoCount].pid = createProcess(&params);   
    strcpy(philoTable[philoCount].name, params.name);
    philoStates[philoCount] = THINKING;
    forks[philoCount] = semOpen(params.name, 1);
    
    philoCount++;
}

void removePhilosopher(void){
    if (philoCount <= STARTING_PHILOSOPHERS){
        printf("Min philosophers reached.\n");
        return;
    }

    philoCount--;
    semDestroy(forks[philoCount]);
    philoTable[philoCount].pid = -1; // Mark as removed
    philoStates[philoCount] = THINKING; // Reset state
    printf("Removed philosopher %d\n", philoCount);
}

static void handleKeyPress(void){
    static char lastKey = 0;
    char c;
    
    sysReadAtCurrentPosition(STDIN, &c, 1);       
   
    if(c != lastKey){
        lastKey = c;
        if (c == 'q'){
            endPhilo();       
        }else if (c == 'a')
        {
            addPhilosopher();
        }
        else if (c == 'r')
        {
            removePhilosopher();
        }
    }
}



static void printPhiloStates(void){
    while(!finishPhilo){
        if (update){
            for (int i = 0; i < philoCount; i++){
                (philoStates[i]? printf(".") : printf("E"));
                printf(" ");
            }
            printf("\n");
        }
        update = 0;
        handleKeyPress();
    }
}


static void philoThink(int index){
    sysSleep(0, GetUint() % 10);
    return;
}

static void philoEat(int index){
    int leftFork = forks[index];
    int rightFork = forks[(index+1) % philoCount];

    if (index % 2 == 0){
        semWait(leftFork);
        semWait(rightFork);
    }
    else{
        semWait(rightFork);
        semWait(leftFork);
    }

    philoStates[index] = EATING;
    update = 1;
    sysSleep(0, GetUint() % 40); // TODO: Chequear

    semPost(rightFork);
    semPost(leftFork);

    philoStates[index] = THINKING;
    update = 1;
    return;
}



uint64_t startPhilo(uint64_t argc, char *argv[]){
    philoCount = STARTING_PHILOSOPHERS;

    printf("Running \n");

    for (int i = 0; i < philoCount; i++){
        creationParameters params;
        char * philoIndex = NULL;
        itoa(i, philoIndex);
        char baseName[MAX_NAME_LENGTH] = "philo_";
        params.name = baseName;
        strcat(params.name, philoIndex);
        params.argc = 1;
        char* argv[] = {philoIndex, NULL};
        params.argv = argv;
        params.priority = 1;
        params.entryPoint = (entryPoint)philosopher;
        params.foreground = 0;
        params.fds[0] = STDIN;
        params.fds[1] = STDOUT;
        philoTable[i].pid = createProcess(&params);   
        strcpy(philoTable[i].name, params.name);
        philoStates[i] = THINKING;
        forks[i] = semOpen(params.name, 1);
    }

    printf("Creations done!\n");

    printPhiloStates();
    
    return 0;
}

void philosopher(uint64_t argc, char *argv[]){
    if (argc < 1){
        printf("Philosopher needs an index");
        return;
    }

    int index = atoi(argv[0]);

    if (index < 0 || index > MAX_PHILOSOPHERS){
        printf("Invalid index\n");
        return;
    }
    
    while(!finishPhilo){
        philoThink(index);
        philoEat(index);
        
    }

}