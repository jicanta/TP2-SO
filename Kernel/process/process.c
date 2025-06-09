// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <process.h>
#include <memoryManager.h>
#include <defs.h>
#include <stdint.h>
#include <lib.h>
#include <interrupts.h>
#include <scheduler.h>
#include <videoDriver.h>
#include <fileDescriptors.h>

Process processes[MAX_PROCESSES];
PID current;
 
void blockAllExceptShell(){

    for(int i = 0; i < 10; i++)
    {
        if (processes[i].pid != INITPID && processes[i].pid != 2 )
        {
            processes[i].state = BLOCKED;
        }
    }

    processes[2].state = READY; // Shell process

}

Process*  getTerminalForegroundProcess(){
    for(int i = 0;i < MAX_PROCESSES; i++)
    {
        if (processes[i].foreground && processes[i].state != DEAD && processes[i].pid != INITPID && processes[i].pid != 2 && processes[i].pid != 0)
        {
            return & processes[i];
        }
    }
    return NULL;
}

int isValidPID(PID pid)
{
    return pid > 0 && pid <= MAX_PID && processes[pid - 1].state != DEAD;
}

void unblockWaitingProcesses(PID pid, int returnValue)
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].waitingPID == pid && processes[i].state == BLOCKED)
        {
            processes[i].waitingPID = NONPID;
            processes[i].childReturnValue = returnValue;
            unblockProcess(processes[i].pid);
        }
    }
}

void waitProcess(PID pidToWait, int *wstatus)
{
    Process *currentProcess = getCurrentProcess();
    if (!isValidPID(pidToWait) || currentProcess->pid == pidToWait || processes[pidToWait - 1].state == DEAD)
    {
        return;
    }
    currentProcess->waitingPID = pidToWait;
    blockProcess(currentProcess->pid);
    if (wstatus != NULL)
    {
        *wstatus = currentProcess->childReturnValue;
    }
}

int processLoader(int argc, char *argv[], entryPoint entry)
{
    int returnValue = entry(argc, argv);
    PID processPid = getpid();
    unblockWaitingProcesses(processPid, returnValue);
    kill(processPid);
    return returnValue;
}

PID initProcesses(void)
{
    current = 1;
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        processes[i].pid = i + 1;
        processes[i].state = DEAD;
        processes[i].argv = NULL;
        processes[i].argc = 0;
        processes[i].waitingPID = NONPID;
    }
    return 0;
}

int checkPriority(Priority priority)
{
    return priority >= MIN_PRIORITY && priority <= MAX_PRIORITY;
}

int checkName(const char *name)
{
    return name != NULL && strlen(name) <= MAX_NAME_LENGTH;
}

int getFreeProcess()
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].state == DEAD)
        {
            return i;
        }
    }
    return -1;  
}

PID createProcess(creationParameters *params)
{

    if (params == NULL || !checkPriority(params->priority) || params->argc < 0 || params->entryPoint == NULL || !checkName(params->name) || current > MAX_PID)
    {
        return -1;
    }
    void *stackLimit = allocMemory(STACK_SIZE);
    char **args;
    if (stackLimit == NULL || (params->argc != 0 && (args = allocMemory(params->argc * sizeof(char *))) == NULL))
    {
        freeMemory(stackLimit);
        freeMemory(args);
        return -1;
    }

    // Copy args

    for (int i = 0; i < params->argc; i++)
    {
        int len = strlen(params->argv[i]);
        if ((args[i] = allocMemory(len + 1)) == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                freeMemory(args[j]);
            }
            freeMemory(args);
            freeMemory(stackLimit);
            return -1;
        }

        memcpy(args[i], params->argv[i], len + 1);
    }

    Process *currentProcess;
    int allocatedProcess = getFreeProcess();
    if (allocatedProcess == -1)
    {
        freeMemory(stackLimit);
        for (int i = 0; i < params->argc; i++)
        {
            freeMemory(args[i]);
        }
        freeMemory(args);
        return -1;
    }

    // Set current process Information
    memcpy(processes[allocatedProcess].name, params->name, strlen(params->name) + 1);
    processes[allocatedProcess].parentpid = (currentProcess = getCurrentProcess()) == NULL ? 0 : currentProcess->pid;
    processes[allocatedProcess].waitingPID = NONPID;
    processes[allocatedProcess].argc = params->argc;
    processes[allocatedProcess].argv = args;
    processes[allocatedProcess].priority = params->priority;
    processes[allocatedProcess].entryPoint = params->entryPoint;
    processes[allocatedProcess].foreground = params->foreground;
    processes[allocatedProcess].state = READY;
    processes[allocatedProcess].stackBase = stackLimit + STACK_SIZE;
    processes[allocatedProcess].stackEnd = setupStack(params->argc, args, params->entryPoint, processes[allocatedProcess].stackBase, (entryPoint)processLoader);
    memcpy(processes[allocatedProcess].fds, params->fds, sizeof(int) * 2);

    schedule(&(processes[allocatedProcess]));
    return processes[allocatedProcess].pid;
    // TODO: Handle entryPoint return value
}

int getProcessesCount()
{
    int count = 0;
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].state != DEAD)
        {
            count++;
        }
    }
    return count;
}

PID getpid(void)
{
    return getCurrentProcess()->pid;
}

PID getppid(void)
{
    return getCurrentProcess()->parentpid;
}

Process *getProcess(PID pid)
{
    if (processes[pid - 1].state != DEAD)
    {
        return &processes[pid - 1];
    }
    return NULL;
}

Process *getProcessesInformation()
{
    int count = getProcessesCount(), ansIndex = 0;
    Process *ans = allocMemory((count + 1) * sizeof(Process));
    ans[count].pid = NONPID;

    for (int i = 0; i < MAX_PROCESSES && ansIndex != count; i++)
    {
        if (processes[i].state != DEAD)
        {
            memcpy(&(ans[ansIndex++]), &(processes[i]), sizeof(Process));
        }
    }
    return ans;
}

void freeProcessesInformation(Process *processesInfo)
{
    freeMemory(processesInfo);
}

int kill(PID pid)
{
    

    if (pid <= INITPID || pid > MAX_PID)
        return -1;
    Process *pcb = &processes[pid - 1];
    if (pcb->state == DEAD)
    {
        return -1;
    }

    freeMemory(((void *)pcb->stackBase - STACK_SIZE));
    if (pcb->argc > 0)
    {
        for (int i = 0; i < pcb->argc; i++)
        {
            freeMemory(pcb->argv[i]);
        }
        freeMemory(pcb->argv);
    }
    pcb->state = DEAD;
    pcb->argv = NULL;
    pcb->argc = 0;
    closeFD(pcb->fds[0]);
    closeFD(pcb->fds[1]);
    garbageCollect();

    unblockWaitingProcesses(pid, 0);

    if (getCurrentProcess()->pid == pid)
    {
        forceSwitchContent();
    }
    return 0;
}

int killAllChildren(PID pid)
{
    if (pid <= INITPID || pid > MAX_PID)
        return -1;

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].parentpid == pid && processes[i].state != DEAD)
        {
            kill(processes[i].pid);
        }
    }
    return 0;
}

int changeProccessPriority(PID pid, Priority priority)
{
    if (!isValidPID(pid) || !checkPriority(priority))
    {
        return -1;
    }
    processes[pid - 1].priority = priority;
    return 0;
}

int getFileDescriptors(int *fds){

    Process *currentProcess = getCurrentProcess();
    if (currentProcess == NULL)
    {
        return -1; // No current process
    }
    fds[0] = currentProcess->fds[0];
    fds[1] = currentProcess->fds[1];
    return 0;
}

