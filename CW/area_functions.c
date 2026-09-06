#include <stdio.h>
#include <math.h>
#include "area_functions.h"

int root_iterations = 0; // число итераций метода root

/*
root — находит корень уравнения f(x) = g(x) на отрезке [a, b] методом деления отрезка пополам.
@param f    — указатель на первую функцию
@param g    — указатель на вторую функцию
@param a    — левая граница отрезка поиска
@param b    — правая граница отрезка поиска
@param eps1 — требуемая точность
@return приближённое значение корня
 */
double root(double (*f)(double), double (*g)(double), double a, double b, double eps1)
{
    double c, fa, fc;
    root_iterations = 0;
    while ((b - a) > eps1)
    {
        c  = (a + b) / 2.0;
        fa = f(a) - g(a);
        fc = f(c) - g(c);
        
		// Если в середине отрезка функция обратилась в ноль (меньше заданной точности) - корень найден точно, выходим сразу
		if (fabs(fc) < eps1)
		{
			root_iterations++;
			return c;
		}
        
        if (fa * fc < 0.0) b = c; // корень в левой половине
        else a = c; // корень в правой половине
        root_iterations++;
    }
    return (a + b) / 2.0;
}

/*
integral — вычисляет определённый интеграл функции f на отрезке [a, b] по составной формуле Симпсона с автоматическим выбором числа разбиений.
Алгоритм: начинаем с n=2, удваиваем n до тех пор, пока разница между двумя последовательными приближениями не станет меньше eps2.
@param f — указатель на подынтегральную функцию
@param a — левая граница интегрирования
@param b — правая граница интегрирования
@param eps2 — требуемая точность
@return приближённое значение интеграла
 */
double integral(double (*f)(double), double a, double b, double eps2)
{
	int n = 2, i;
	double h, S1, S2, x;

	// Начальное приближение с n = 2 разбиениями
	h = (b - a) / n;
	S1 = f(a) + f(b);
	for (i = 1; i < n; i++)
	{
		x = a + i * h;
		S1 += (i % 2 == 0) ? 2.0 * f(x) : 4.0 * f(x);
	}
	S1 *= h / 3.0;

	// Увеличиваем число разбиений вдвое до достижения нужной точности
	do
	{
		n *= 2;
		h = (b - a) / n;
		S2 = f(a) + f(b);
		for (i = 1; i < n; i++) {
			x = a + i * h;
			S2 += (i % 2 == 0) ? 2.0 * f(x) : 4.0 * f(x);
		}
		S2 *= h / 3.0;
		if (fabs(S2 - S1) < eps2) break;
		S1 = S2;
	} while (1);

    return S2;
}

// Исходные функции
double f1(double x) { return 0.6 * x + 3.0; } // f1 = 0.6·x + 3
double f2(double x) { return (x - 2.0) * (x - 2.0) * (x - 2.0) - 1.0; } // f2 = (x − 2)³ − 1
double f3(double x) { return 3.0 / x; } // f3 = 3 / x

// f1(x) − f3(x) — подынтегральная функция на первом участке [xA, xB]
double area_func1(double x) { return f1(x) - f3(x); }

// f1(x) − f2(x) — подынтегральная функция на втором участке [xB, xC]
double area_func2(double x) { return f1(x) - f2(x); }

// Вывод справки
void print_help(void)
{
	printf("The program is designed to calculate the area formed by the equations:\n"
	"1. f1 = 0.6x + 3\n"
	"2. f2 = (x - 2)^3 - 1\n"
	"3. f3 = 3 / x\n"
    "-help - is used to output help.\n"
    "-a - use it to print the abscissas of the points of intersection of the curves.\n"
    "-i - use to print the number of iterations required for the approximate solution of equations when searching for intersection points.\n"
    "-t - use for testing.\n");
}

// Вспомогательные функции для тестирования
double test_sq(double x) { return x * x; }
double test_const4(double x) { return 4.0; }
double test_const5(double x) { return 5.0; }
double test_2x(double x) { return 2.0 * x; }

// test_functions — тестирует корректность работы root и integral путём сравнения с известными аналитическими значениями.
void test_functions(void)
{
	double result, expected;
	int iter;

	printf("Testing the root function (bisection method)\n");
	// Тест 1: корень уравнения x² = 4 на [1, 3], ожидаемое x = 2
	result = root(test_sq, test_const4, 1.0, 3.0, 1e-12);
	printf("Test 1: x^2 = 4 on [1, 3]\n");
	printf("  Result: %.15f\n", result);
	printf("  Expected: 2.000000000000000\n");
	printf("  Iterations: %d\n", root_iterations);
	printf("  Error rate: %.2e\n", fabs(result - 2.0));
	// Тест 2: корень уравнения x² = 5 на [1, 3], ожидаемое x = 2,236067977499789
	result = root(test_sq, test_const5, 1.0, 3.0, 1e-12);
	printf("Test 2: x^2 = 5 on [1, 3]\n");
	printf("  Result: %.15f\n", result);
	printf("  Expected: 2,236067977499789\n");
	printf("  Iterations: %d\n", root_iterations);
	printf("  Error rate: %.2e\n\n", fabs(result - 2.236067977499789));

	printf("Testing the integral function (Simpson's formula)\n");
	/*
	Тест 3: ∫ 2x dx на [0, 3]
	1. Найдём первообразную функции f(x)=2x. Первообразная для 2x равна x² + C
	2. Применим формулу Ньютона — Лейбница для отрезка [0, 3]: F(3)–F(0)=3² - 0² = 9
	*/
	result = integral(test_2x, 0.0, 3.0, 1e-12);
	printf("Test 3: integral 2x dx on [0, 3]\n");
	printf("  Result: %.15f\n", result);
	printf("  Expected: 9.000000000000000\n");
	printf("  Error rate: %.2e\n\n", fabs(result - 9.0));
}
