
#include "../include/memoryManager.h"
#include "../include/scheduler.h"
#include "../include/syscallHandle.h"
#include "../include/videoDriver.h"
#include <interrupts.h>
Quantum quantumsLeft = 0;
List list;
Process *currentProcess = NULL;
char isYield = YIELD_NOT_DONE;

void initScheduler()
{
    currentProcess = list.head->pcb;
}

void garbageCollect()
{
    if (list.head == NULL)
        return;

    Node *current = list.head;
    Node *previous = list.tail;
    Node *temp;

    do
    {
        if (current->pcb->state == BLOCKED || current->pcb->state == DEAD)
        {
            temp = current;

            if (current == list.head)
            {
                list.head = list.head->next;
                list.tail->next = list.head;
            }
            else
            {
                previous->next = current->next;
                if (current == list.tail)
                {
                    list.tail = previous;
                    list.tail->next = list.head;
                }
            }

            current = current->next;
            freeMemory(temp);

            if (list.head == NULL)
            {
                list.tail = NULL;
                break;
            }
        }
        else
        {
            previous = current;
            current = current->next;
        }
    } while (current != list.head);
}

void schedule(Process *pcb)
{
    Node *node = allocMemory(sizeof(Node));
    node->pcb = pcb;

    if (list.head == NULL)
    {
        list.head = node;
        list.tail = node;
        node->next = list.head;
    }
    else
    {
        node->next = list.head;
        list.tail->next = node;
        list.tail = node;
    }
}

Process *unschedule()
{
    if (list.head == NULL)
    {
        return NULL;
    }

    Process *pcb = list.head->pcb;

    if (list.head == list.tail)
    {
        freeMemory(list.head);
        list.head = NULL;
        list.tail = NULL;
    }
    else
    {
        Node *temp = list.head;
        list.head = list.head->next;
        list.tail->next = list.head;
        freeMemory(temp);
    }
    return pcb;
}

uint64_t *switchContent(uint64_t *rsp)
{
    if (currentProcess == NULL)
    {
        return rsp;
    }

    if (currentProcess->state == RUNNING)
    {
        if (quantumsLeft > 0 && getYield() != YIELD_DONE)
        {
            (quantumsLeft)--;
            return rsp;
        }
        currentProcess->stackEnd = rsp;
        schedule(currentProcess);
        currentProcess->state = READY;
    }
    if (currentProcess->state == BLOCKED)
    {
        quantumsLeft = 0;
        currentProcess->stackEnd = rsp;
    }

    do
    {

        currentProcess = unschedule();
        if (currentProcess == NULL)
        {
            return rsp;
        }
        quantumsLeft = currentProcess->priority - 1;
    } while (currentProcess->state == BLOCKED || currentProcess->state == DEAD);

    clearYield();
    currentProcess->state = RUNNING;
    return currentProcess->stackEnd;
}

Process *getCurrentProcess()
{
    return currentProcess;
}

int blockProcess(PID pid)
{
    Process *pcb;
    if ((pcb = getProcess(pid)) == NULL)
        return 1;
    pcb->state = BLOCKED;
    garbageCollect();

    if (pcb->pid == currentProcess->pid)
    {
        return yield();
    }
    return 0;
}

int unblockProcess(PID pid)
{
    if (getProcess(pid)->state != BLOCKED)
        return -1;
    Process *pcb = getProcess(pid);
    pcb->state = READY;
    schedule(pcb);
    return 0;
}

void setYield()
{
    isYield = YIELD_DONE;
}
void clearYield()
{
    isYield = YIELD_NOT_DONE;
}
char getYield()
{
    return isYield;
}