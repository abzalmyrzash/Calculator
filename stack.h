#pragma once
#include <stdlib.h>
#include <stdio.h>

struct Stack_node {
	int data;
	struct Stack_node* down;
};

struct Stack_node* _Stack_node_new(int data, struct Stack_node* down) {
	struct Stack_node* node = malloc(sizeof(struct Stack_node));
	node->data = data;
	node->down = down;
}

typedef struct {
	struct Stack_node* top;
	int size;
} Stack;

Stack* Stack_new() {
	Stack *stack = malloc(sizeof(Stack));
	stack->top = NULL;
	stack->size = 0;
	return stack;
}

void Stack_add(Stack* stack, int data) {
	struct Stack_node* prev_top = stack->top;
	stack->top = _Stack_node_new(data, prev_top);
	stack->size++;
}

int Stack_pop(Stack* stack) {
	struct Stack_node* prev_top = stack->top;
	stack->top = prev_top->down;
	int data = prev_top->data;
	free(prev_top);
	stack->size--;
	return data;
}

int Stack_isEmpty(Stack* stack) {
	return stack->top == NULL;
}
