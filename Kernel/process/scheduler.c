// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/memoryManager.h"
#include "../include/scheduler.h"
#include "../include/syscallHandle.h"
#include "../include/videoDriver.h"
#include <interrupts.h>
Quantum quantumsLeft = 0;
List list;
Process *currentProcess = NULL;
char isYield = YIELD_NOT_DONE;

//YIELD es para hacer la syscall yield

void initScheduler()
{
    currentProcess = list.head->pcb;
}

void garbageCollect()
{
    if (list.head == NULL)
        return;

    Node *current = list.head;
    Node *previous = list.tail; // To keep track of the previous node in the circular list
    Node *temp;

    do
    {
        // Check if the current process is BLOCKED or DEAD
        if (current->pcb->state == BLOCKED || current->pcb->state == DEAD)
        {
            temp = current;

            if (current == list.head)
            {
                // Move the head if we're removing the first node
                list.head = list.head->next;
                list.tail->next = list.head; // Keep the list circular
            }
            else
            {
                previous->next = current->next; // Skip the current node
                if (current == list.tail)
                {
                    // Move the tail if we're removing the last node
                    list.tail = previous;
                    list.tail->next = list.head; // Update the circular link
                }
            }

            current = current->next;
            freeMemory(temp); // Free the memory for the removed node

            // If the list becomes empty after removing the node
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
    } while (current != list.head); // Continue until we circle back to the head
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
        list.head = list.head->next; // Move head to the next node
        list.tail->next = list.head; // Update tail to point to the new head (circular link)
        freeMemory(temp);
    }
    return pcb;
}

static counter = 0;

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


    /*
    vdPrint(" ", 0x00FF00);
    vdPrintInt(currentProcess->pid);
    vdPrint(" \n", 0x00FF00);
    */

    //vdPrintInt(currentProcess->pid);

    if(currentProcess->pid == 3){
        return 0x00F0D4B8;
    }
    
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