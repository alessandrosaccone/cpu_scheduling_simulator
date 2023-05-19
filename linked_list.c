#include "linked_list.h"
#include "fake_os.h"
#include <stdio.h>
#include <assert.h>

void printList(const ListHead* head) {
    if (head == 0 || head->first == 0) {
        printf("List is empty.\n");
        return;
    }

    const ListItem* current = head->first;

    while (current) {
      int print_pid = ((FakePCB*)(current))->pid;
      int print_pred = ((FakePCB*)(current))->burst_prediction;
        printf("%d ", print_pid);
        printf("Predizione: %d\t |||\t", print_pred);
        current = current->next;
    }

    printf("\n");
}

int List_empty(ListHead* head) {
  return head->first == 0;
}

int List_size(ListHead* head) {
  int size = 0;
  ListItem* item = head->first;
  
  while (item) {
    size++;
    item = item->next;
  }
  
  return size;
}

void List_init(ListHead* head) {
  head->first=0;
  head->last=0;
  head->size=0;
}

ListItem* List_find(ListHead* head, ListItem* item) {
  // linear scanning of list
  ListItem* aux=head->first;
  while(aux){
    if (aux==item)
      return item;
    aux=aux->next;
  }
  return 0;
}

ListItem* List_insert(ListHead* head, ListItem* prev, ListItem* item) {
  if (item->next || item->prev)
    return 0;
  
#ifdef _LIST_DEBUG_
  // we check that the element is not in the list
  ListItem* instance=List_find(head, item);
  assert(!instance);

  // we check that the previous is inthe list

  if (prev) {
    ListItem* prev_instance=List_find(head, prev);
    assert(prev_instance);
  }
  // we check that the previous is inthe list
#endif

  ListItem* next= prev ? prev->next : head->first;
  if (prev) {
    item->prev=prev;
    prev->next=item;
  }
  if (next) {
    item->next=next;
    next->prev=item;
  }
  if (!prev)
    head->first=item;
  if(!next)
    head->last=item;
  ++head->size;
  return item;
}

ListItem* List_detach(ListHead* head, ListItem* item) {

#ifdef _LIST_DEBUG_
  // we check that the element is in the list
  ListItem* instance=List_find(head, item);
  assert(instance);
#endif

  ListItem* prev=item->prev;
  ListItem* next=item->next;
  if (prev){
    prev->next=next;
  }
  if(next){
    next->prev=prev;
  }
  if (item==head->first)
    head->first=next;
  if (item==head->last)
    head->last=prev;
  head->size--;
  item->next=item->prev=0;
  return item;
}

ListItem* List_pushBack(ListHead* head, ListItem* item) {
  return List_insert(head, head->last, item);
};

ListItem* List_pushFront(ListHead* head, ListItem* item) {
  return List_insert(head, 0, item);
};

ListItem* List_popFront(ListHead* head) {
  return List_detach(head, head->first);
}

