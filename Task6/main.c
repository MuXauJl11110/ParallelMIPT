#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

//операции доступа к данным стека
#define STACK_IS_NOT_FREE (sp > 0)

double f(double x) {
    //return (sin(1 / x) / x) * (sin(1 / x) / x);
    return sqrt(4 - x * x);
}

int sp = 0; //указатель вершины стека

typedef struct stack {
    double a;
    double b;
    double fa;
    double fb;
    double sab;
} Stack; //массив структур, в которых хранятся отложенные значения. 1000 может быть заменена на необходимое число

Stack stk[1000];

void PUT_INTO_STACK(double a, double b, double fa, double fb, double sab) {
    stk[sp].a = a;
    stk[sp].b = b;
    stk[sp].fa = fa;
    stk[sp].fb = fb;
    stk[sp].sab = sab;
    ++sp;
}
void GET_FROM_STACK(double *a, double *b, double *fa, double *fb, double *sab) {
    --sp;
    *a = stk[sp].a;
    *b = stk[sp].b;
    *fa = stk[sp].fa;
    *fb = stk[sp].fb;
    *sab = stk[sp].sab;
}

 //основная функция 
double IntLocalStack(double a, double b, double epsilon) {
    double I = 0;     //значение интеграла
    double fa = f(a); //значение функции f(x) в точке a
    double fb = f(b); //значение функции f(x) в точке b
    double fc = 0;
    double c = 0;        //середина отрезка интегрирования
    double sab = 0;      //
    double sac = 0;      //
    double scb = 0;      //
    double sabc = 0;     //переменные для подсчёта текущих сумм
    sab = (fa + fb) * (b - a) / 2;
    while(1) {
        c = (a + b) / 2;
        fc = f(c);
        sac = (fa + fc) * (c - a) / 2;
        scb = (fc + fb) * (b - c) / 2;
        sabc = sac + scb;
        if(abs(sab - sabc) > epsilon * abs(sabc)) {//epsilon - заданная точность
            PUT_INTO_STACK(a, c, fa, fc, sac);
            a = c; 
            fa = fc;
            sab = scb;
        }
        else {
            I += sabc;
            if(STACK_IS_NOT_FREE){
                break;
            }
            GET_FROM_STACK(&a, &b, &fa, &fb, &sab);
        }
    }
    return I;
}

int main() {
    //printf("%lf\n", IntLocalStack(1e-4, 1.0, 0.1));
    printf("%lf\n", IntLocalStack(0.1, 2.0, 0.001));
    return 0;
}