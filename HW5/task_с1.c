#include <stdio.h>
#include <string.h>

#define MAXN 10010 // длинна строки + '\0' + запас небольшой

char s1[MAXN], s2[MAXN];
char t[MAXN * 2];
int  z[MAXN * 2];

void zFunction(char *s, int n, int *z)
{
	z[0] = n;
	for (int i = 1, l = 0, r = 0; i < n; i++)
	{
		if (i < r)
			z[i] = (r - i < z[i - l]) ? r - i : z[i - l];
		else
			z[i] = 0;
		while (i + z[i] < n && s[z[i]] == s[i+z[i]])
			z[i]++;
		if (i + z[i] > r)
		{
			l = i;
			r = i+z[i]-1;
		}
	}
}

int main(void)
{
	scanf("%s", s1);
	scanf("%s", s2);

	int len1 = strlen(s1);
	int len2 = strlen(s2);
	int t_len = len1 + 1 + len2;

	/* Задача 1. Длина наибольшего префикса первой строки, являющегося суффиксом второй. */
	memcpy(t, s1, len1);
	t[len1] = '\0';
	memcpy(t + len1 + 1, s2, len2);
	t[t_len] = '\0';
	zFunction(t, t_len, z);

	int result1 = 0;
	for (int i = len1 + 1; i < t_len; i++)
	{
		if (i + z[i] == t_len)
		{
			result1 = z[i];
			break;
		}
	}

    /* Задача 2. Длина наибольшего суффикса первой строки, являющегося префиксом второй. */
	memcpy(t, s2, len2);
	t[len2] = '\0';
	memcpy(t + len2 + 1, s1, len1);
	t[t_len] = '\0';
	zFunction(t, t_len, z);

	int result2 = 0;
	for (int i = len2 + 1; i < t_len; i++)
	{
		if (i + z[i] == t_len)
		{
			result2 = z[i];
			break;
		}
	}

	printf("%d %d\n", result1, result2);
	return 0;
}
