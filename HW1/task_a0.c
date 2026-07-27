#include <stdio.h>

int main(void)
{
	int n = 0;
	if (scanf("%d", &n) != 1 || n <= 0) return 1;
	int max_val;  // Текущий максимум
	int count;    // Счетчик повторений максимума
	int current;  // Переменная для считывания текущего числа
	
    if (scanf("%d", &current) == 1)
    {
		max_val = current;
		count = 1;
		for (int i = 1; i < n; i++)
		{
			if (scanf("%d", &current) == 1)
			{
                if (current > max_val)
				{
					max_val = current;
					count = 1;
                }
				else if (current == max_val) count++;
            }
        }
    }
	else return 1; 
 
    printf("%d\n", count);
    return 0;
}
