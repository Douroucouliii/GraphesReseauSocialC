#include "pile.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define BLOC 128

void push(Pile *p, int e) {
  assert(p);
  if (p->size >= p->capacity) {
    p->capacity += BLOC;
    p->data = (int *)realloc(p->data, p->capacity);
    if (!p->data) {
      perror("realloc()\n");
      exit(EXIT_FAILURE);
    }
  }
  p->data[p->size++] = e;
}

int pop(Pile *p) {
  assert(p && p->size);
  return p->data[--p->size];
}

int top(Pile *p) {
  assert(p && p->size);
  return p->data[p->size - 1];
}

Pile *create() {
  Pile *ret = (Pile *)malloc(sizeof(Pile));
  if (!ret) {
    perror("malloc()\n");
    exit(EXIT_FAILURE);
  }
  *ret = (Pile){NULL, 0, 0};
  return ret;
}

void destroy(Pile *p) {
  if (!p) return;
  if (p->data) free(p->data);
  free(p);
}

int size(Pile *p) {
  assert(p);
  return p->size;
}