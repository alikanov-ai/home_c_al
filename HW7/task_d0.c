#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct list
{
	char word[20];
	struct list *next;
};

// Добавление нового элемента в конец списка
void add_to_list(struct list **head, const char *word)
{
	struct list *new_node = calloc(1, sizeof(struct list));
	strncpy(new_node->word, word, 19);
	new_node->word[19] = '\0'; 
	new_node->next = NULL;

	if (*head == NULL) *head = new_node;
	else
	{
		struct list *current = *head;
		while (current->next != NULL) current = current->next;
		current->next = new_node;
	}
}

// Обмен содержимым (словами) между двумя узлами списка
void swap_elements(struct list *a, struct list *b)
{
	char temp[20];
	strcpy(temp, a->word);
	strcpy(a->word, b->word);
	strcpy(b->word, temp);
}

// Печать элементов списка через пробел
void print_list(struct list *head)
{
	struct list *current = head;
	while (current != NULL)
	{
		printf("%s", current->word);
		if (current->next != NULL) printf(" ");
		current = current->next;
	}
	printf("\n");
}

// Полное удаление списка и освобождение памяти
void delete_list(struct list **head)
{
	struct list *current = *head;
	struct list *next_node;

	while (current != NULL)
	{
		next_node = current->next;
		free(current);
		current = next_node;
	}
	*head = NULL;
}

// Сортировка списка
void sort_list(struct list *head)
{
	if (head == NULL || head->next == NULL) return;
	int swapped;
	struct list *ptr;
	struct list *lptr = NULL; // Указатель на последний отсортированный элемент

    do
	{
		swapped = 0;
		ptr = head;

		while (ptr->next != lptr)
        {
			// Если текущее слово больше следующего по алфавиту
			if (strcmp(ptr->word, ptr->next->word) > 0)
			{
				swap_elements(ptr, ptr->next);
				swapped = 1;
			}
			ptr = ptr->next;
		}
		lptr = ptr; // Сужаем область проверки, так как конец уже отсортирован
	} while (swapped);
}

int main()
{
	struct list *head = NULL;
	char buffer[256];

	// Считываем слова по одному
	while (scanf("%255s", buffer) == 1)
	{
		size_t len = strlen(buffer);
		int has_dot = 0;
		// Проверяем, является ли последний символ точкой
		if (len > 0 && buffer[len - 1] == '.') {
			buffer[len - 1] = '\0'; // Удаляем точку
			len--;
			has_dot = 1;
		}

		if (len > 0) add_to_list(&head, buffer);
		if (has_dot) break;
	}

	// Упорядочиваем список по алфавиту
	sort_list(head);

	// Печатаем результат
	print_list(head);

	// Удаляем список из памяти
	delete_list(&head);

	return 0;
}
