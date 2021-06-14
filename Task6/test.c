// Implementation of stack was taken from https://www.geeksforgeeks.org/stack-data-structure-introduction-program/
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
 
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
double IntLocalStack(double A, double B, double epsilon, struct Stack* stack, int local_stack_size)
{
    double I = 0; // Integral value
    int tick = 0;
    #pragma omp parallel reduction(+: I, tick)
    {
        int size = omp_get_num_threads();
        int rank = omp_get_thread_num();
        double h = (B - A) / size;
        double a = A + h * rank;
        double b = a + h;
        double fa = f(a), fb = f(b);
        double fc = 0;
        double c = 0; // Midpoint
        double sab = 0, sac = 0, scb = 0, sabc = 0;
        sab = (fa + fb) * (b - a) / 2;
        int is_empty = 0;
        struct Stack* local_push_stack = createStack(local_stack_size);
        //struct Stack* local_pop_stack = createStack(local_stack_size);
        
        Item *item;
        item = (Item*)malloc(sizeof(Item));
        while(1) {
            c = (a + b) / 2;
            fc = f(c);
            sac = (fa + fc) * (c - a) / 2;
            scb = (fc + fb) * (b - c) / 2;
            sabc = sac + scb;
            if(fabs(sab - sabc) > epsilon * fabs(sabc)) {
                item->a = a;
                item->b = c;
                item->fa = fa;
                item->fb = fc;
                item->sab = sac;
                push(local_push_stack, *item);
                a = c; 
                fa = fc;
                sab = scb;
            } else {
                // Checking if I not Nan
                if (sabc != sabc) {
                    //printf("Nan was caught at %d!\n", rank);
                    sabc = 0;
                }
                I += sabc;
                // Local stack is empty
                if (isEmpty(local_push_stack)) {
                    #pragma omp critical
                    is_empty = isEmpty(stack);
                    if (is_empty) {
                        break;
                    } else {
                        #pragma omp critical
                        {
                            while ((isFull(local_push_stack) != 1) && (pop(stack, item) != INT_MIN)) {
                                push(local_push_stack, *item);
                            }
                        }
                    }
                }
                pop(local_push_stack, item);
                a = item->a;
                b = item->b;
                fa = item->fa;
                fb = item->fb;
                sab = item->sab;
            }
            tick++;
            // If local stack is full
            if (isFull(local_push_stack)) {
                #pragma omp critical
                {
                    for (int i = 0; i < local_stack_size; i++)
                        push(stack, local_push_stack->array[i]);
                }
                local_push_stack->top = -1;
                //printf("Pushed from %d\n", rank);
            }
        }
        deleteStack(local_push_stack);
        //deleteStack(local_pop_stack);
        //printf("%d %d\n", rank, tick);
        free(item);
    }
    //printf("%d\n", tick);
    return I;
}
 
// Driver program to test above functions
int main(int argc, char **argv)
{
    if (argc < 2) {
		printf("Too few arguments!\n");
		return 1;
	}
	char *eptr;
    double eps = strtod(argv[1], &eptr);
    struct Stack* stack = createStack(5000);

    double start, end;
    
    start = omp_get_wtime();
    double result = IntLocalStack(0.0001, 1.0, eps, stack, 8);
    end = omp_get_wtime();
    //printf("%lf\n", result);
    printf("%lf\n", end - start);

    deleteStack(stack);
    
    return 0;
}