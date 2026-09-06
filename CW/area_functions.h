#ifndef AREA_FUNCTIONS_H
#define AREA_FUNCTIONS_H

#include <stdio.h>
#include <math.h>

extern int root_iterations; // число итераций метода root

/*
root — находит корень уравнения f(x) = g(x) на отрезке [a, b] методом деления отрезка пополам.
@param f    — указатель на первую функцию
@param g    — указатель на вторую функцию
@param a    — левая граница отрезка поиска
@param b    — правая граница отрезка поиска
@param eps1 — требуемая точность
@return приближённое значение корня
 */
double root(double (*f)(double), double (*g)(double), double a, double b, double eps1);

/*
integral — вычисляет определённый интеграл функции f на отрезке [a, b] по составной формуле Симпсона с автоматическим выбором числа разбиений.
Алгоритм: начинаем с n=2, удваиваем n до тех пор, пока разница между двумя последовательными приближениями не станет меньше eps2.
@param f — указатель на подынтегральную функцию
@param a — левая граница интегрирования
@param b — правая граница интегрирования
@param eps2 — требуемая точность
@return приближённое значение интеграла
 */
double integral(double (*f)(double), double a, double b, double eps2);

/* Исходные функции */
double f1(double x);
double f2(double x);
double f3(double x);

/* f1(x) − f3(x) — подынтегральная функция на первом участке [xA, xB] */
double area_func1(double x);

/* f1(x) − f2(x) — подынтегральная функция на втором участке [xB, xC] */
double area_func2(double x);

/* Функция вывода справки */
void print_help(void);

/* Вспомогательные функции для тестирования */
double test_sq(double x);
double test_const4(double x);
double test_const5(double x);
double test_2x(double x);

/* Функция тестирования */
void test_functions(void);

#endif
