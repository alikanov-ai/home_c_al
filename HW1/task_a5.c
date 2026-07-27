#include <stdio.h>
#include <stdint.h>

int main(void)
{
	uint32_t n;
	if (scanf("%u", &n) != 1) return 1;
	unsigned int count = 0;	
	while (n > 0)
	{
		n &= (n - 1); // самый правый единичный бит превращаем в ноль
		count++;
	}
	printf("%u\n", count);
	return 0;
}
