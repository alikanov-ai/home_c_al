#include <stdio.h>
#include <stdint.h>

int main(void)
{
	uint32_t n;
	unsigned int k;
	if (scanf("%u %u", &n, &k) != 2 || k < 1 || k > 31) return 1;
	printf("%u\n", n & ((1 << k) - 1));
	return 0;
}
