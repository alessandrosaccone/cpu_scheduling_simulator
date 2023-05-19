#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "fake_os.h"

FakeOS os;

typedef struct { //only for order
  int quantum;
} SchedSJFArgs;

void schedSJF(FakeOS* os, void* args_){
  SchedSJFArgs* args=(SchedSJFArgs*)args_;

  // look for the first process in ready
  // if none, return

  if (! os->ready.first)
    return;

  int i=0;
  while (!List_empty(&os->ready) && i < os->num_core) {
      
      FakePCB* pcb=(FakePCB*) List_popFront(&os->ready);
      pcb->arrival_time=os->timer;
      List_pushFront(&os->running, (ListItem*)pcb);
      
      //os->running.first=(ListItem*)pcb;
      
      assert(pcb->events.first);
      ProcessEvent* e = (ProcessEvent*)pcb->events.first;
      assert(e->type==CPU);

      // look at the first event
      // if duration>quantum
      // push front in the list of event a CPU event of duration quantum
      // alter the duration of the old event subtracting quantum
      if (e->duration>args->quantum) {
        ProcessEvent* qe=(ProcessEvent*)malloc(sizeof(ProcessEvent));
        qe->list.prev=qe->list.next=0;
        qe->type=CPU;
        qe->duration=args->quantum;
        e->duration-=args->quantum;
        List_pushFront(&pcb->events, (ListItem*)qe);
      }
      i++;
    }
};

int main(int argc, char** argv) {
  FakeOS_init(&os);
  SchedSJFArgs srr_args;
  srr_args.quantum=10; //it sets the quantum 
  os.schedule_args=&srr_args;
  os.schedule_fn=schedSJF; //here it calls the scheduler involved 
  
  assert( (atoi(argv[1])!=0 && *argv[1]!='0') && "You have to digit the number of cores");
  int num_core=atoi(argv[1]);
  printf("\nCORE NUMBER: \t%d\n\n", num_core);
  os.num_core=num_core;

  for (int i=2; i<argc; ++i){
    FakeProcess new_process;
    int num_events=FakeProcess_load(&new_process, argv[i]);
    printf("loading [%s], pid: %d, events:%d",
           argv[i], new_process.pid, num_events);
    if (num_events) {
      FakeProcess* new_process_ptr=(FakeProcess*)malloc(sizeof(FakeProcess));
      *new_process_ptr=new_process;
      List_pushBack(&os.processes, (ListItem*)new_process_ptr);
    }
  }
  printf("num processes in queue %d\n", os.processes.size);
  while(os.running.first
        || os.ready.first
        || os.waiting.first
        || os.processes.first){
    FakeOS_simStep(&os);
  }
}
