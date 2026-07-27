#include <stdio.h>
#include <stdint.h>

int main(void)
{
	uint32_t n;
	unsigned int k, max = 0;
	if (scanf("%u %u", &n, &k) != 2 || k < 1 || k > 31) return 1;
	for (int i = 0; i <= 32 - k; i++)
	{
		unsigned int val = (n >> i) & ((1 << k) - 1);
		if (val > max) max = val;
	}
	printf("%u\n", max);
	return 0;
}
