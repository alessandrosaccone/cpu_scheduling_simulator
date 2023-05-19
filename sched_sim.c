#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "fake_os.h"

FakeOS os;

typedef struct { //only for order
  int quantum;
} SchedArgs;

FakePCB* getFakePCBWithLowestBurstPrediction(ListHead* head) {
    if (head->first == NULL) {
        return NULL;
    }

    ListItem* current = head->first;
    FakePCB* minPCB = (FakePCB*)current;

    while (current != NULL) {
        FakePCB* pcb = (FakePCB*)current;
        if (pcb->burst_prediction < minPCB->burst_prediction) {
            minPCB = pcb;
        }
        current = current->next;
    }
    return minPCB;
}

void schedSJF(FakeOS* os, void* args_){
  SchedArgs* args=(SchedArgs*)args_;

  // look for the first process in ready
  // if none, return

  if (! os->ready.first)
    return;

  printList(&os->ready);
  while (!List_empty(&os->ready) && List_size(&os->running) < os->num_core) {
      
      
      FakePCB* pcb = getFakePCBWithLowestBurstPrediction(&os->ready);
      FakePCB* proof=(FakePCB*) List_detach(&os->ready, (ListItem*)pcb);
      if (!proof) exit(-1);
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
    }
};

void schedRR(FakeOS* os, void* args_){
  SchedArgs* args=(SchedArgs*)args_;

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
  SchedArgs srr_args;
  //default quantum for preemption
  
  if (argc<=5) {
    printf("You must write <decay_coefficient> <core_number> <scheduling_policy> <quantum> <process_files...>\n");
    exit(-1);
  }

  assert( atof(argv[1])!=0 && atof(argv[1])<=1 && "You have to digit a number between 0 and 1");
  os.decay_coefficient=atof(argv[1]);
  printf("DECAY COEFFICIENT: %f\n", os.decay_coefficient);

  assert( (atoi(argv[2])!=0 && *argv[2]!='0') && "You have to digit the number of cores");
  int num_core=atoi(argv[2]);
  printf("CORE NUMBER: \t%d\n", num_core);
  os.num_core=num_core;

  if (strcmp(argv[3], "RR")==0) {
    os.schedule_args=&srr_args;
    os.schedule_fn=schedRR; 
    printf("SCHEDULING POLICY CHOSEN: RR\n");
  }
  else {
    os.schedule_args=&srr_args;
    os.schedule_fn=schedSJF;  //default SJF
    printf("SCHEDULING POLICY CHOSEN: SJF\n");
  }

  assert(atoi(argv[4])!=0 && *argv[4]!='0' && "You must insert an integer value of quantum");
  srr_args.quantum=atoi(argv[4]); 
  printf("QUANTUM: \t%d\n\n", atoi(argv[4]));
  
  for (int i=5; i<argc; ++i){
    FakeProcess new_process;
    int num_events=FakeProcess_load(&new_process, argv[i]);
    printf("loading [%s], pid: %d, events:%d\n",
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
  FakeOS_destroy(&os);
}
