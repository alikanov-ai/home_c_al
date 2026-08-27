#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int *pos; // в массиве все позиции цифр хранятся сгруппировано по значению цифры
int start[11]; // начала диапазонов для каждой цифры (с какого индекса в массиве pos начинаются позиции цифры)
int cnt[10]; // в массиве хранятся количества повторений каждой цифры

int find_next(int d, int after)
{
	int lo = start[d], hi = start[d] + cnt[d];
	while (lo < hi)
	{
		int mid = (lo + hi) / 2;
		if (pos[mid] > after) hi = mid;
        else lo = mid + 1;
	}
	if (lo < start[d] + cnt[d]) return pos[lo];
	return -1;
}

int main(void)
{
	char *s = NULL;
	size_t capacity = 0;
	size_t len = 0;
	int c;

	while ((c = getchar()) != EOF && c != '\n' && c != '\r')
	{
		if (len + 1 >= capacity)
		{
			capacity = capacity == 0 ? 1024 : capacity * 2;
			s = realloc(s, capacity);
			if (!s)
			{
				fprintf(stderr, "Не удалось выделить память\n");
				return 1;
			}
		}
		s[len++] = c;
	}
    
	if (len == 0)
	{
		free(s);
		return 0;
	}

	s[len] = '\0';
	int n = (int)len;
    
	/* Считаем количества цифр */
	memset(cnt, 0, sizeof(cnt));
	for (int i = 0; i < n; i++) cnt[s[i] - '0']++;

	/* Вычисляем начала диапазонов для каждой цифры */
	start[0] = 0;
	for (int d = 1; d <= 10; d++) start[d] = start[d - 1] + cnt[d - 1];

	/* Выделяем память под массив позиций */
	pos = malloc(n * sizeof(int));
	if (!pos)
	{
		fprintf(stderr, "Не удалось выделить память\n");
		free(s);
		return 1;
	}

	/* Заполняем массив позиций */
	memset(cnt, 0, sizeof(cnt));
	for (int i = 0; i < n; i++)
	{
		int d = s[i] - '0';
		pos[start[d] + cnt[d]] = i;
		cnt[d]++;
	}
	
	/* Перебираем все трёхзначные числа */
	int count = 0;
	for (int num = 100; num <= 999; num++)
	{
		int d1 = num / 100;
		int d2 = (num / 10) % 10;
		int d3 = num % 10;
	
		int p1 = find_next(d1, -1);
		if (p1 == -1) continue;
	
		int p2 = find_next(d2, p1);
		if (p2 == -1) continue;
	
		int p3 = find_next(d3, p2);
		if (p3 == -1) continue;
	
		count++;
	}
	
	printf("%d\n", count);
	
	/* Освобождаем память */
	free(s);
	free(pos);
	
	return 0;
}
