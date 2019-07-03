/*      Author: Willard Laxton                  */
/*      Project: CS 4760 Operating Systems P1   */
/*      Date: 02-06-2019                        */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"


struct Stack* createStack(unsigned capacity) 
{ 
    struct Stack* stack = (struct Stack*) malloc(sizeof(struct Stack)); 
    stack->capacity = capacity; 
    stack->top = -1; 
    stack->number_Array = (int*) malloc(stack->capacity * sizeof(int)); 
    return stack; 
} 

int pop(struct Stack *stack)
{
	stack->top--;
	return stack->number_Array[stack->top + 1];
}

void push(struct Stack *stack, int number)
{
	stack->top++;
	stack->number_Array[stack->top] = number;
}

int isFull(struct Stack* stack) 
{   
	if(stack->top == stack->capacity -1)
   	{
		return 1;
	}   
	else
	{
      		return 0;
	}
}

int isEmpty(struct Stack* stack) 
{
	if(stack->top == -1)
        {
                return 1;
        }
        else
        {
                return 0;
        }

} 
