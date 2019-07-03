#ifndef STACK_H_
#define STACK_H_

struct Stack
{
	int top; 
	unsigned capacity; 
	int* number_Array;
};

struct Stack* createStack(unsigned capacity);
int pop(struct Stack *stack);
void push(struct Stack *stack, int number);
int isFull(struct Stack* stack);
int isEmpty(struct Stack* stack);

#endif /* STACK_H_ */
