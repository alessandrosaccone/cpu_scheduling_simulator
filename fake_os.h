#include "fake_process.h"
#include "linked_list.h"
#define DECAY_COEFFICENT 0.9
#pragma once


typedef struct {
  ListItem list;
  int pid;
  ListHead events;
  int arrival_time;
  float burst_prediction;
  int last_burst;
  float last_prediction;
} FakePCB;

struct FakeOS;
typedef void (*ScheduleFn)(struct FakeOS* os, void* args);

typedef struct FakeOS{
  ListHead running; //FakePCB* running;
  ListHead ready;
  ListHead waiting;
  int timer;
  ScheduleFn schedule_fn;
  void* schedule_args;

  ListHead processes;
} FakeOS;

void FakeOS_init(FakeOS* os);
void FakeOS_simStep(FakeOS* os);
void FakeOS_destroy(FakeOS* os);
