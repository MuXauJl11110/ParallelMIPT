// Implementation of stack was taken from https://www.geeksforgeeks.org/stack-data-structure-introduction-program/
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 
typedef struct item {
    double a;
    double b;
    double fa;
    double fb;
    double sab;
} Item;

// A structure to represent a stack
struct Stack {
    int top;
    unsigned capacity;
    Item* array;
};
 
// Function to create a stack of given capacity. It initializes size of
// stack as 0
struct Stack* createStack(unsigned capacity)
{
    struct Stack* stack = (struct Stack*)malloc(sizeof(struct Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (Item*)malloc(stack->capacity * sizeof(Item));
    return stack;
}

// Function to delete the stack
void deleteStack(struct Stack* stack)
{
    free(stack->array);
    free(stack);
}
 
// Stack is full when top is equal to the last index
int isFull(struct Stack* stack)
{
    return stack->top == stack->capacity - 1;
}
 
// Stack is empty when top is equal to -1
int isEmpty(struct Stack* stack)
{
    return stack->top == -1;
}
 
// Function to add an item to stack.  It increases top by 1
void push(struct Stack* stack, Item item)
{
    if (isFull(stack))
        return;
    stack->array[++stack->top] = item;
}
 
// Function to remove an item from stack.  It decreases top by 1
// Returns INT_MIN if stack is empty
int pop(struct Stack* stack, Item *item)
{
    if (isEmpty(stack)) {
        return INT_MIN;
    }
    *item = stack->array[stack->top--];
    return 0;
}
 
// Function to return the top from stack without removing it
int peek(struct Stack* stack, Item *item)
{
    if (isEmpty(stack))
        return INT_MIN;
    *item = stack->array[stack->top];
    return 0;
}

// Function to print Item
void printItem(Item *item)
{
    printf("Item:\na: %lf\nb: %lf\nfa: %lf\nfb: %lf\nsab: %lf\n", 
        item->a, item->b, item->fa, item->fb, item->sab);
        double a;
}

double f(double x)
{
    return (sin(1 / x) / x) * (sin(1 / x) / x);
}

// Function calculating an integral
double IntLocalStack(double a, double b, double epsilon, struct Stack* stack)
{
    double I = 0; // Integral value
    double fa = f(a), fb = f(b);
    double fc = 0;
    double c = 0; // Midpoint
    double sab = 0, sac = 0, scb = 0, sabc = 0;
    sab = (fa + fb) * (b - a) / 2;

    Item *item;
    item = (Item*)malloc(sizeof(Item));
    while(1) {
        c = (a + b) / 2;
        fc = f(c);
        sac = (fa + fc) * (c - a) / 2;
        scb = (fc + fb) * (b - c) / 2;
        sabc = sac + scb;
        if(abs(sab - sabc) > epsilon * abs(sabc)) {
            item->a = a;
            item->b = c;
            item->fa = fa;
            item->fb = fc;
            item->sab = sac;
            push(stack, *item);
            a = c; 
            fa = fc;
            sab = scb;
        } else {
            I += sabc;
            if (isEmpty(stack)) {
                break;
            }
            pop(stack, item);
            a = item->a;
            b = item->b;
            fa = item->fa;
            fb = item->fb;
            sab = item->sab;
        }
    }
    return I;
}
 
// Driver program to test above functions
int main()
{
    struct Stack* stack = createStack(5000);
    
    printf("%lf\n", IntLocalStack(0.0001, 1.0, 0.0000000001, stack));

    deleteStack(stack);
    
    return 0;
}