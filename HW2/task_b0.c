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

uint64_t findMaxBlock(list *head)
{
	if (head == NULL) return 0;
	uint64_t maxAddress = head->address;
	size_t maxSize = head->size;
	list *current = head->next;
	while (current != NULL)
	{
		if (current->size > maxSize)
		{
			maxAddress = current->address;
			maxSize = current->size;
		}
		current = current->next;
	}
	return maxAddress;
}

/*
int main(void)
{
	return 0;
}
*/
