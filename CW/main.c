#include <stdio.h>
#include <string.h>
#include <math.h>
#include "area_functions.h"

// Точности вычислений
#define EPS1 1e-10
#define EPS2 1e-10

int main(int argc, char *argv[])
{
	int opt_a = 0, opt_i = 0, opt_t = 0; // флаги опций
	double xA, xB, xC; // абсциссы точек пересечения
	int iterA, iterB, iterC; // число итераций для каждой точки
	double area1, area2;
	
	// Парсинг аргументов командной строки
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-a") == 0) opt_a = 1;
		else if (strcmp(argv[i], "-i") == 0) opt_i = 1;
		else if (strcmp(argv[i], "-help") == 0)
		{
			print_help();
			return 0;
        }
		else if (strcmp(argv[i], "-t") == 0) opt_t = 1;
        else {
			fprintf(stderr, "Unknown option: %s\n", argv[i]);
			print_help();
			return 1;
        }
    }
    
    // Режим тестирования: проверяем root и integral (опция -t)
	if (opt_t)
	{
		test_functions();
		return 0;
	}

	xA = root(f3, f1, 0.5, 1.0, EPS1); // пересечение f1 и f3 (F1 = f3 − f1 = 0) на отрезке [0.5, 1.0]
	iterA = root_iterations;
	xB = root(f3, f2, 3.0, 3.5, EPS1); // пересечение f2 и f3 (F2 = f3 − f2 = 0) на отрезке [3.0, 3.5]
	iterB = root_iterations;
	xC = root(f2, f1, 3.5, 4.0, EPS1); // пересечение f1 и f2 (F3 = f2 − f1 = 0) на отрезке [3.5, 4.0]
	iterC = root_iterations;
    
    // Вывод абсцисс точек пересечения (опция -a)
    if (opt_a)
    {
		printf("The abscissas of the intersection points\n");
		printf("  xA (f1 intersects f3): %.10f\n", xA);
		printf("  xB (f2 intersects f3): %.10f\n", xB);
		printf("  xC (f1 intersects f2): %.10f\n\n", xC);
	}
	
    // Вывод числа итераций (опция -i)
    if (opt_i)
    {
		printf("Number of iterations\n");
		printf("  Search xA: %d iterations\n", iterA);
		printf("  Search xB: %d iterations\n", iterB);
		printf("  Search xC: %d iterations\n\n", iterC);
	}
	
    // Вычисление площади фигуры как суммы двух интегралов: S = ∫[xA,xB](f1−f3)dx + ∫[xB,xC](f1−f2)dx
    area1 = integral(area_func1, xA, xB, EPS2);
    area2 = integral(area_func2, xB, xC, EPS2);
    printf("The area of the figure bounded by the curves f1, f2, f3\n");
    printf("  Integral 1 [%.6f, %.6f] (f1 - f3): %.10f\n", xA, xB, area1);
    printf("  Integral 2 [%.6f, %.6f] (f1 - f2): %.10f\n", xB, xC, area2);
    printf("  Total area: %.10f\n", area1 + area2);
	return 0;
}
