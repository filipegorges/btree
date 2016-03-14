#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <assert.h>

#define T 3

typedef struct node_ {
	int key[2 * T - 1]; //ключи
	struct node_* child[2 * T];
	int n; //кол-во ключей
	int leaf;//лист если = 1 детей нет если = 0 - есть дети 
	struct node_* parent;
} node;

node* root = 0;

void tree_split(node* cur, int i) {
	node *z, *y;
	int j;

	z = (node*)malloc(sizeof(node));
	y = cur->child[i];
	z->leaf = y->leaf;//говорим что если элемент на котором мы стоим является листом то  разделенный тоже будет листом иначе будет разноуровн дерево
	z->n = T - 1;//делим т к макс 2t-1->t-1
	z->parent = cur;

	for (j = 0; j < T - 1; j++)
		z->key[j] = y->key[j + T];//помещаем значения ключей

	if (y->leaf == 0) {//есл у нашего исходного элемента были дети то они и останутся
		for (j = 0; j < T; j++) {
			z->child[j] = y->child[j + T];//разделяем детей
			z->child[j]->parent = z;
		}
	}

	y->n = T - 1;//обрезаем 

	for (j = cur->n; j >= i + 1; j--)
		cur->child[j + 1] = cur->child[j];
	cur->child[i + 1] = z;/

	for (j = cur->n - 1; j >= i; j--)
		cur->key[j + 1] = cur->key[j];
	cur->key[i] = y->key[T - 1];//кладем значения
	cur->n++;//кол-во увелич
}

void insert_nonfull(node* cur, int k) {
	int i;
	if (cur->leaf == 1) {//если лист
		i = cur->n - 1;//индекс последнего ключа в узле
		while (i >= 0 && cur->key[i] > k) {//бежим в обратную сторону(пока не дошли до конца и не привысили заданный элемент)
			cur->key[i + 1] = cur->key[i];//сдвиг
			i = i - 1;
		}
		cur->key[i + 1] = k;//нумерация теперь идет не с (0,1,2) а с появлением нового элемента стоит между(0,1,2,3)(был родитель под номером 0-> его ребенок будет под номером- 1)
		cur->n++;//говорим что кол-во ключей увелич на 1
	}
	else {
		i = cur->n - 1;
		while (i >= 0 && cur->key[i] > k)
			i = i - 1;
		i++;//пробежали входим в детей
		if (cur->child[i]->n == 2 * T - 1) {//если все занято деваться некуда мы идем разбивать
			tree_split(cur, i);
			if (k > cur->key[i])//идем дальше
				i++;
		}
		insert_nonfull(cur->child[i], k);
	}
}

int find_key_simple(node* cur, int k) {//нахождение элемента, без вывода
	int i;
	if (cur == 0)
		return 0;

	for (i = 0; i < cur->n; i++) {//пока в одном узле находимся
		if (cur->key[i] == k) //нашли-возвращаем 1
			return 1;
	}

	if (cur->leaf == 1) //если текущий элемент является листом то нет смысла идти дальше
		return 0;

	for (i = 0; i < cur->n; i++) {//если текущ элемент не является листом, бежим в одну сторону(k<)
		if (k < cur->key[i])
			return find_key_simple(cur->child[i], k);//ищем в ребенке передавая ключ
	}
	return find_key_simple(cur->child[cur->n], k);//k>
}

void insert(int k) {
	node *s, *r;

	// проверка: если ключ уже есть в дереве, то остановка
	if (find_key_simple(root, k))
		return;

	r = root;
	if (r->n == 2 * T - 1) {
		s = (node*)malloc(sizeof(node));
		root = s;
		s->leaf = 0;
		s->n = 0;
		s->child[0] = r;
		s->parent = 0;
		r->parent = s;
		tree_split(s, 0);
		insert_nonfull(s, k);
	}
	else {
		insert_nonfull(r, k);
	}
}

void print_level(node* cur, int cur_level, int level, int num) {
	int i;
	if (cur == 0)
		return;

	if (cur_level > level)
		return;

	if (cur_level < level && cur->leaf == 0) {
		for (i = 0; i < cur->n + 1; i++)
			print_level(cur->child[i], cur_level + 1, level, i);
		return;
	}

	if (cur_level == level) {
		printf("уровень %d.%d\n", cur_level, num);
		for (i = 0; i < cur->n; i++)
			printf("%d ", cur->key[i]);
		printf("\n");
	}
}

void print_tree() {
	int i;
	for (i = 0; i < 10; i++) {
		print_level(root, 0, i, 0);
	}
}

int find_key_rec(node* cur, int k, int cur_level) {//нахождение элемента
	int i;
	if (cur == 0) {
		printf("Не найдено\n");
		return 0;
	}

	for (i = 0; i < cur->n; i++) {//пока в одном узле находимся
		if (cur->key[i] == k) {//нашли-возвращаем 1
			printf("Найдено. Уровень %d, номер ключа %d\n", cur_level, i);
			return 1;
		}
	}

	if (cur->leaf == 1) {//если текущий элемент является листом то нет смысла идти дальше
		printf("Не найдено!\n");
		return 0;
	}

	for (i = 0; i < cur->n; i++) {//если текущ элемент не является листом, бежим в одну сторону(k<)
		if (k < cur->key[i])
			return find_key_rec(cur->child[i], k, cur_level + 1);//ищем в ребенке передавая ключ
	}
	return find_key_rec(cur->child[cur->n], k, cur_level + 1);//k>
}

// функция для исправления если осталось менее t-1 ключа после удаления
void fix_delete(node* cur) {
	node *parent, *left = 0, *right = 0;
	int parent_index, i;

	if (cur == root) {
		// если в корне осталось не менее 1 ключа, то всё нормально
		if (cur->n >= 1)
			return;

		// иначе корнем становится единстенный ребёнок корня, старый узел корня удаляется
		root = cur->child[0];
		free(cur);
		return;
	}

	// вычисляем родителя
	parent = cur->parent;

	// находим индекс данного узла как ребенка у родителя
	parent_index = -1;
	for (i = 0; i < parent->n + 1; i++) {
		if (parent->child[i] == cur) {
			parent_index = i;
			break;
		}
	}

	// не должно быть -1, иначе ошибка!
	assert(parent_index != -1);

	// вычисляем правого соседа
	if (parent_index + 1 < parent->n + 1)
		right = parent->child[parent_index + 1];
	// вычисляем левого соседа
	if (parent_index - 1 >= 0)
		left = parent->child[parent_index - 1];

	// смотрим на правого соседа, если у него есть "лишние" ключи (т.е. более T-1 ключей)
	if (right != 0 && right->n >= T) {
		// переносим в текущий узел ключ-разделитель соседей из родительского узла (в конец)
		cur->key[cur->n] = parent->key[parent_index];
		cur->n++;

		// у текущего появляется новый ребёнок - самый левый из правого соседа
		if (cur->leaf == 0) {
			cur->child[cur->n] = right->child[0];
			cur->child[cur->n]->parent = cur;
		}

		// вместо ключа-разделителя в родительский узел переносим самый левый ключ из правого соседа
		parent->key[parent_index] = right->key[0];

		// сдвигаем в правом узле, чтобы убрать левый ключ и ребёнка
		for (i = 0; i < right->n - 1; i++) {
			right->key[i] = right->key[i + 1];
			right->child[i] = right->child[i + 1];
		}
		right->child[right->n - 1] = right->child[right->n];
		right->n--;
	}
	// смотрим на левого соседа, если у него есть "лишние" ключи (т.е. более T-1 ключей)
	else if (left != 0 && left->n >= T) {
		// переносим в текущий узел ключ-разделитель соседей из родительского узла (в начало)
		// у текущего появляется новый ребёнок - самый правый из левого соседа
		cur->child[cur->n + 1] = cur->child[cur->n];
		for (int i = cur->n - 1; i >= 0; i--) {
			cur->key[i + 1] = cur->key[i];
			cur->child[i + 1] = cur->child[i];
		}
		cur->key[0] = parent->key[parent_index - 1];
		if (cur->leaf == 0) {
			cur->child[0] = left->child[left->n];
			cur->child[0]->parent = cur;
		}
		cur->n++;
		
		// вместо ключа-разделителя в родительский узел переносим самый правый ключ из левого соседа
		parent->key[parent_index - 1] = left->key[left->n - 1];

		// убираем в левом узле правый ключ и ребёнка
		left->n--;
	}
	// нет соседей с не менее T ключами
	else {
		// если есть, объединяем с правым соседом
		if (right != 0) {
			// добавляем в текущий узел ключ-разделитель из родительского узла (в конец)
			cur->key[cur->n] = parent->key[parent_index];
			cur->n++;

			// добавляем все ключи и детей из правого соседа
			for (i = 0; i < right->n; i++) {
				cur->key[cur->n] = right->key[i];
				if (cur->leaf == 0) {
					cur->child[cur->n] = right->child[i];
					cur->child[cur->n]->parent = cur;
				}
				cur->n++;
			}
			if (cur->leaf == 0) {
				cur->child[cur->n] = right->child[right->n];
				cur->child[cur->n]->parent = cur;
			}

			// удаляем правого соседа
			free(right);

			// удаляем ключ-разделитель в родителе, сдвигаем ключи и детей
			for (i = parent_index; i < parent->n - 1; i++) {
				parent->key[i] = parent->key[i + 1];
				parent->child[i + 1] = parent->child[i + 2];
			}
			parent->n--;
		}
		// иначе объединяем с левым
		else {
			// добавляем в левый узел ключ-разделитель из родительского узла (в конец)
			left->key[left->n] = parent->key[parent_index - 1];
			left->n++;

			// добавляем в левый все ключи и детей из текущего
			for (i = 0; i < cur->n; i++) {
				left->key[left->n] = cur->key[i];
				if (cur->leaf == 0) {
					left->child[left->n] = cur->child[i];
					left->child[left->n]->parent = left;
				}
				left->n++;
			}
			if (cur->leaf == 0) {
				left->child[left->n] = cur->child[cur->n];
				left->child[left->n]->parent = left;
			}

			// удаляем текущий узел
			free(cur);

			// удаляем ключ-разделитель в родителе, сдвигаем ключи и детей
			for (i = parent_index - 1; i < parent->n - 1; i++) {
				parent->key[i] = parent->key[i + 1];
				parent->child[i + 1] = parent->child[i + 2];
			}
			parent->n--;
		}

		// если после слияния в родителе осталось менее t-1 ключей, то рекурсивно повторяем
		if (parent->n < T - 1)
			fix_delete(parent);
	}
}

void delete_element(node* cur, int k) {
	int i, j;
	node* t;

	if (cur == 0)
		return;

	if (cur == root && cur->leaf == 1) {//корень-лист
		for (i = 0; i < cur->n; i++) {
			if (cur->key[i] == k) {
				for (j = i; j < cur->n - 1; j++)
					cur->key[j] = cur->key[j + 1];
				cur->n--;
				break;
			}
		}
		return;
	}

	// лист, но не корень
	if (cur->leaf == 1) {
		// ищем и удаляем
		for (i = 0; i < cur->n; i++) {
			if (cur->key[i] == k) {
				for (j = i; j < cur->n - 1; j++)
					cur->key[j] = cur->key[j + 1];
				cur->n--;
				break;
			}
		}

		// если осталось менее T-1 ключей, то исправляем
		if (cur->n < T - 1)
			fix_delete(cur);
	}
	// внутренний узел
	else {
		for (i = 0; i < cur->n; i++) {
			if (cur->key[i] == k) {
				// находим предшественника в левом поддереве
				t = cur->child[i];
				while (t->leaf == 0)
					t = t->child[t->n];
				// переносим вместо удаляемого ключа правый ключ предшественника
				cur->key[i] = t->key[t->n - 1];
				// удаляем правый ключ предшественника
				delete_element(t, t->key[t->n - 1]);
				return;
			}
		}

		// не нашли, переходим в соответствующего ребёнка
		for (i = 0; i < cur->n; i++) {
			if (k < cur->key[i]) {
				delete_element(cur->child[i], k);
				return;
			}
		}
		delete_element(cur->child[cur->n], k);
	}
}

void erase_tree(node* cur) {
	int i;
	
	if (cur == 0)
		return;

	if (cur->leaf == 0) {
		for (i = 0; i < cur->n + 1; i++)
			erase_tree(cur->child[i]);
	}

	free(cur);
}


int main() {
	int ans, x;
	int i;
	int mas[] = { 4, 17, 31, 1, 2, 3, 7, 9, 11, 13, 16, 19, 26, 27, 96, 97, 99 };

	setlocale(0, "rus");

	root = (node*)malloc(sizeof(node));
	root->n = 0;
	root->leaf = 1;
	root->parent = 0;

	//добавления значений для теста
	for (i = 0; i < sizeof(mas) / sizeof(int); i++)
		insert(mas[i]);

	while (1) {
		printf("[0] выход    [1] добавление   [2] поиск   [3] печать   [4] удалить \n");
		scanf("%d", &ans);

		if (ans == 0)
			break;

		if (ans == 1) {
			scanf("%d", &x);
			insert(x);
		}
		else if (ans == 2) {
			scanf("%d", &x);
			find_key_rec(root, x, 0);
		}
		else if (ans == 3) {
			print_tree();
		}
		else if (ans == 4) {
			scanf("%d", &x);
			delete_element(root, x);
		}
	}

	erase_tree(root);

	return 0;
}


















