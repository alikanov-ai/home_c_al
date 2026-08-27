#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK 1000
#define MAX_STRING 10000

int stack[MAX_STACK];// массив для хранения промежуточных результатов вычислений
int top = -1;// хранит индекс верхнего элемента stack (-1 - стек пуст)

/* Добавление в stack */
void push(int value)
{
	stack[++top] = value;
}

/* Извлечение из stack */
int pop()
{
	return stack[top--];
}

/* Проверка, является ли символ оператором */
int isOperator(const char *character)
{
	return strcmp(character, "+") == 0 ||
	strcmp(character, "-") == 0 ||
	strcmp(character, "×") == 0 ||
	strcmp(character, "*") == 0 ||
	strcmp(character, "/") == 0;
}

int main()
{
	char input[MAX_STRING];
	fgets(input, sizeof(input), stdin);
	char *character = strtok(input, " \t\n.");

	while (character != NULL)
	{
		if (isOperator(character))
		{
			int b = pop();
			int a = pop();
			int result;
			if (strcmp(character, "+") == 0) result = a + b;
			else if (strcmp(character, "-") == 0) result = a - b;
			else if (strcmp(character, "*") == 0 || strcmp(character, "×") == 0) result = a * b;
			else if (strcmp(character, "/") == 0) result = a / b;
			push(result);
		}
		else push(atoi(character));
		character = strtok(NULL, " \t\n.");
	}
    
	printf("%d\n", pop());
	return 0;
}
