#include <stdio.h>

int main(void)
{
	unsigned int N;
	if (scanf("%u", &N) != 1) return 0;
    int c = getchar();
	if (c == ' ' || c == '\n' || c == '\r' || c == '\t') c = getchar(); // пропускаем символ-разделитель
	while (c != EOF && c != '.')
	{
		if (c >= 'a' && c <= 'z') c = 'a' + (c - 'a' + N) % 26;
		else if (c >= 'A' && c <= 'Z') c = 'A' + (c - 'A' + N) % 26;  
		putchar(c);
		c = getchar();
	}
	putchar('.');
	return 0;
}
