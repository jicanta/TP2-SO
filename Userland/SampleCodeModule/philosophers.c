#include "./include/philosophers.h"

phi_t philoTable[MAX_PHILOSOPHERS];
char philoStates[MAX_PHILOSOPHERS];
int forks[MAX_PHILOSOPHERS];
char update = 1;
char philoCount;

static void endPhilo(void){
    for (int i = 0; i < philoCount; i++){
        sysWait(philoTable[i].pid, NULL);
        semClose(forks[i]);
    }
}

static void handleKeyPress(void){
    unsigned key;

    if (key == 'q'){
        endPhilo();       
    }
}

static void printPhiloStates(void){
    while(1){
        if (update){
            for (int i = 0; i < philoCount; i++){
                (philoStates[i]? printf(".") : printf("E"));
                printf(" ");
            }
            printf("\n");
        }
        update = 0;
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

static void philosopher(uint64_t argc, char *argv[]){
    if (argc < 1){
        printf("Philosopher needs an index");
        return;
    }

    int index = atoi(argv[0]);

    if (index < 0 || index > MAX_PHILOSOPHERS){
        printf("Invalid index\n");
        return;
    }

    printf("Philosopher %d started\n", index);
    
    while(1){
        philoThink(index);
        philoEat(index);
    }

}

uint64_t startPhilo(uint64_t argc, char *argv[]){
    philoCount = 10;

    printf("Starting philosophers...\n");

    
    for (int i = 0; i < philoCount; i++){
        creationParameters params;
        char * philoIndex;
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