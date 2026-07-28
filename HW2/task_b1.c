//#include <stdio.h>
//#include <stdint.h>

/*
typedef struct list
{
	uint64_t address;
	size_t size;
	char comment[64];
	struct list *next;
} list;
*/

size_t totalMemoryUsage(list *head)
{
	size_t sumSize = 0;
	list *current = head;
	while (current != NULL)
	{
		sumSize += current->size;
		current = current->next;
	}
	return sumSize;
}

/*
int main(void)
{
	return 0;
}
*/
