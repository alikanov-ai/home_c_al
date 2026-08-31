/*
#include <stdio.h>
#include <stdlib.h>

typedef int datatype;

typedef struct tree
{
	datatype key;
	struct tree *left, *right;
	struct tree *parent; //ссылка на родителя
} tree;

tree* insert(tree *root, datatype key)
{
	tree* newNode = (tree*)malloc(sizeof(tree));
	newNode->key = key;
	newNode->left = NULL;
	newNode->right = NULL;
	newNode->parent = NULL;

	if (root == NULL) return newNode;

	tree* current = root;
	tree* parent = NULL;

	// Находим место для вставки и запоминаем родителя
	while (current != NULL)
	{
		parent = current;
		if (key < current->key) current = current->left;
		else current = current->right;
    }

	// Привязываем новый узел к родителю
	newNode->parent = parent;
	if (key < parent->key) parent->left = newNode;
	else parent->right = newNode;

    return root;
}
*/
tree * findBrother(tree *root, int key)
{
	if(root == NULL) return 0;
	if(root->key == key)
	{
		if(root->parent == NULL) return 0;
		if(root->parent->left == root) return root->parent->right;
		else return root->parent->left;
	}
	else if(root->key > key) return findBrother(root->left, key);
	else return findBrother(root->right, key);
}
/*
// Вспомогательная функция для освобождения памяти дерева
void freeTree(tree* root)
{
	if (root == NULL) return;
	freeTree(root->left);
	freeTree(root->right);
	free(root);
}

int main()
{
	tree* root = NULL;
	int val;

	// Читаем элементы дерева. Согласно примеру, 0 является признаком конца списка элементов дерева.
	while (scanf("%d", &val) == 1 && val != 0) root = insert(root, val);

	// Читаем искомый ключ (в примере это последнее число 3)
	int query_key;
	if (scanf("%d", &query_key) == 1)
	{
		tree* brother = findBrother(root, query_key);
		printf("%d\n", brother->key);
	}

	// Освобождаем выделенную память
	freeTree(root);

    return 0;
}
*/
