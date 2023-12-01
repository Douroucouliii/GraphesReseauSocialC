#pragma once

typedef struct {
  int *data;
  unsigned size;
  unsigned capacity;
} Pile;

Pile *create();
void destroy(Pile *);
void push(Pile *, int);
int size(Pile *);
int pop(Pile *);
int top(Pile *);