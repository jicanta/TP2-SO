#include <process.h>
#include <memoryManager.h>
#include <defs.h>
#include <lib.h>

extern void *setupStack(void *entryPoint, void *stackBase, int argc, char *argv[]);

PCB processes[MAX_PROCESSES];
pid_t current;

static uint64_t strlen(char *str)
{
    uint64_t i = 0;
    while (str[i] != '\0')
    {
        i++;
    }
    return i;
}

pid_t initProcesses(void)
{
    current = 1;
    return createProcess("init", 0, NULL, DEFAULT_PRIORITY, NULL, 1);
}

int checkPriority(uint32_t priority)
{
    return priority >= MIN_PRIORITY && priority <= MAX_PRIORITY;
}

int checkName(const char *name)
{
    return name != NULL && strlen(name) <= MAX_NAME_LENGTH;
}

pid_t createProcess(const char *name, uint32_t argc, char *argv[], uint32_t priority, entryPoint entryPoint, int foreground)
{
    if (!checkPriority(priority) || argc < 0 || entryPoint == NULL || !checkName(name) || current > MAX_PID)
    {
        return -1;
    }
    void *stackLimit = allocMemory(STACK_SIZE);
    char **args;
    if (stackLimit == NULL || (argc != 0 && (args = allocMemory(argc * sizeof(char *))) == NULL))
    {
        freeMemory(stackLimit);
        freeMemory(args);
        return -1;
    }

    // Copy args
    for (int i = 0; i < argc; i++)
    {
        int len = strlen(argv[i]);
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
        memcpy(args[i], argv[i], len);
    }

    pid_t pid = current++;

    // Set current process Information
    processes[pid].pid = pid;
    processes[pid].parentPid = 0; /* TODO: GetcurrentProcessPID */
    processes[pid].argc = argc;
    processes[pid].argv = args;
    processes[pid].priority = priority;
    processes[pid].entryPoint = entryPoint;
    processes[pid].foreground = foreground;
    processes[pid].state = READY;
    processes[pid].stackBase = stackLimit + STACK_SIZE;
    processes[pid].stackEnd = stackLimit;

    setupStack(entryPoint, processes[pid].stackBase, argc, args);
    return pid;
    // TODO: Handle entryPoint return value
}