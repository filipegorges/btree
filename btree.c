#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <assert.h>

#define T 3

typedef struct node_ {
	int key[2 * T - 1]; //�����
	struct node_* child[2 * T];
	int n; //���-�� ������
	int leaf;//���� ���� = 1 ����� ��� ���� = 0 - ���� ���� 
	struct node_* parent;
} node;

node* root = 0;

void tree_split(node* cur, int i) {
	node *z, *y;
	int j;

	z = (node*)malloc(sizeof(node));//��� DEF
	y = cur->child[i];//ABCDE
	z->leaf = y->leaf;//������� ��� ���� ������� �� ������� �� ����� �������� ������ ��  ����������� ���� ����� ������ ����� ����� ���������� ������
	z->n = T - 1;//����� � � ���� 2t-1->t-1
	z->parent = cur;

	for (j = 0; j < T - 1; j++)
		z->key[j] = y->key[j + T];//�������� �������� ������

	if (y->leaf == 0) {//��� � ������ ��������� �������� ���� ���� �� ��� � ���������
		for (j = 0; j < T; j++) {
			z->child[j] = y->child[j + T];//��������� �����
			z->child[j]->parent = z;
		}
	}

	y->n = T - 1;//�������� 

	for (j = cur->n; j >= i + 1; j--)
		cur->child[j + 1] = cur->child[j];//������� � GM
	cur->child[i + 1] = z;//DEF

	for (j = cur->n - 1; j >= i; j--)//C
		cur->key[j + 1] = cur->key[j];//����������� � ������ C G M
	cur->key[i] = y->key[T - 1];//������ ��������
	cur->n++;//���-�� ������
}

void insert_nonfull(node* cur, int k) {
	int i;
	if (cur->leaf == 1) {//���� ����
		i = cur->n - 1;//������ ���������� ����� � ����
		while (i >= 0 && cur->key[i] > k) {//����� � �������� �������(���� �� ����� �� ����� � �� ��������� �������� �������)
			cur->key[i + 1] = cur->key[i];//�����
			i = i - 1;
		}
		cur->key[i + 1] = k;//��������� ������ ���� �� � (0,1,2) � � ���������� ������ �������� ����� �����(0,1,2,3)(��� �������� ��� ������� 0-> ��� ������� ����� ��� �������- 1)
		cur->n++;//������� ��� ���-�� ������ ������ �� 1
	}
	else {
		i = cur->n - 1;
		while (i >= 0 && cur->key[i] > k)
			i = i - 1;
		i++;//��������� ������ � �����
		if (cur->child[i]->n == 2 * T - 1) {//���� ��� ������ �������� ������ �� ���� ���������
			tree_split(cur, i);
			if (k > cur->key[i])//���� ������
				i++;
		}
		insert_nonfull(cur->child[i], k);
	}
}

int find_key_simple(node* cur, int k) {//���������� ��������, ��� ������
	int i;
	if (cur == 0)
		return 0;

	for (i = 0; i < cur->n; i++) {//���� � ����� ���� ���������
		if (cur->key[i] == k) //�����-���������� 1
			return 1;
	}

	if (cur->leaf == 1) //���� ������� ������� �������� ������ �� ��� ������ ���� ������
		return 0;

	for (i = 0; i < cur->n; i++) {//���� ����� ������� �� �������� ������, ����� � ���� �������(k<)
		if (k < cur->key[i])
			return find_key_simple(cur->child[i], k);//���� � ������� ��������� ����
	}
	return find_key_simple(cur->child[cur->n], k);//k>
}

void insert(int k) {
	node *s, *r;

	// ��������: ���� ���� ��� ���� � ������, �� ���������
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
		printf("������� %d.%d\n", cur_level, num);
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

int find_key_rec(node* cur, int k, int cur_level) {//���������� ��������
	int i;
	if (cur == 0) {
		printf("�� �������\n");
		return 0;
	}

	for (i = 0; i < cur->n; i++) {//���� � ����� ���� ���������
		if (cur->key[i] == k) {//�����-���������� 1
			printf("�������. ������� %d, ����� ����� %d\n", cur_level, i);
			return 1;
		}
	}

	if (cur->leaf == 1) {//���� ������� ������� �������� ������ �� ��� ������ ���� ������
		printf("�� �������!\n");
		return 0;
	}

	for (i = 0; i < cur->n; i++) {//���� ����� ������� �� �������� ������, ����� � ���� �������(k<)
		if (k < cur->key[i])
			return find_key_rec(cur->child[i], k, cur_level + 1);//���� � ������� ��������� ����
	}
	return find_key_rec(cur->child[cur->n], k, cur_level + 1);//k>
}

// ������� ��� ����������� ���� �������� ����� t-1 ����� ����� ��������
void fix_delete(node* cur) {
	node *parent, *left = 0, *right = 0;
	int parent_index, i;

	if (cur == root) {
		// ���� � ����� �������� �� ����� 1 �����, �� �� ���������
		if (cur->n >= 1)
			return;

		// ����� ������ ���������� ����������� ������ �����, ������ ���� ����� ���������
		root = cur->child[0];
		free(cur);
		return;
	}

	// ��������� ��������
	parent = cur->parent;

	// ������� ������ ������� ���� ��� ������� � ��������
	parent_index = -1;
	for (i = 0; i < parent->n + 1; i++) {
		if (parent->child[i] == cur) {
			parent_index = i;
			break;
		}
	}

	// �� ������ ���� -1, ����� ������!
	assert(parent_index != -1);

	// ��������� ������� ������
	if (parent_index + 1 < parent->n + 1)
		right = parent->child[parent_index + 1];
	// ��������� ������ ������
	if (parent_index - 1 >= 0)
		left = parent->child[parent_index - 1];

	// ������� �� ������� ������, ���� � ���� ���� "������" ����� (�.�. ����� T-1 ������)
	if (right != 0 && right->n >= T) {
		// ��������� � ������� ���� ����-����������� ������� �� ������������� ���� (� �����)
		cur->key[cur->n] = parent->key[parent_index];
		cur->n++;

		// � �������� ���������� ����� ������ - ����� ����� �� ������� ������
		if (cur->leaf == 0) {
			cur->child[cur->n] = right->child[0];
			cur->child[cur->n]->parent = cur;
		}

		// ������ �����-����������� � ������������ ���� ��������� ����� ����� ���� �� ������� ������
		parent->key[parent_index] = right->key[0];

		// �������� � ������ ����, ����� ������ ����� ���� � ������
		for (i = 0; i < right->n - 1; i++) {
			right->key[i] = right->key[i + 1];
			right->child[i] = right->child[i + 1];
		}
		right->child[right->n - 1] = right->child[right->n];
		right->n--;
	}
	// ������� �� ������ ������, ���� � ���� ���� "������" ����� (�.�. ����� T-1 ������)
	else if (left != 0 && left->n >= T) {
		// ��������� � ������� ���� ����-����������� ������� �� ������������� ���� (� ������)
		// � �������� ���������� ����� ������ - ����� ������ �� ������ ������
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
		
		// ������ �����-����������� � ������������ ���� ��������� ����� ������ ���� �� ������ ������
		parent->key[parent_index - 1] = left->key[left->n - 1];

		// ������� � ����� ���� ������ ���� � ������
		left->n--;
	}
	// ��� ������� � �� ����� T �������
	else {
		// ���� ����, ���������� � ������ �������
		if (right != 0) {
			// ��������� � ������� ���� ����-����������� �� ������������� ���� (� �����)
			cur->key[cur->n] = parent->key[parent_index];
			cur->n++;

			// ��������� ��� ����� � ����� �� ������� ������
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

			// ������� ������� ������
			free(right);

			// ������� ����-����������� � ��������, �������� ����� � �����
			for (i = parent_index; i < parent->n - 1; i++) {
				parent->key[i] = parent->key[i + 1];
				parent->child[i + 1] = parent->child[i + 2];
			}
			parent->n--;
		}
		// ����� ���������� � �����
		else {
			// ��������� � ����� ���� ����-����������� �� ������������� ���� (� �����)
			left->key[left->n] = parent->key[parent_index - 1];
			left->n++;

			// ��������� � ����� ��� ����� � ����� �� ��������
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

			// ������� ������� ����
			free(cur);

			// ������� ����-����������� � ��������, �������� ����� � �����
			for (i = parent_index - 1; i < parent->n - 1; i++) {
				parent->key[i] = parent->key[i + 1];
				parent->child[i + 1] = parent->child[i + 2];
			}
			parent->n--;
		}

		// ���� ����� ������� � �������� �������� ����� t-1 ������, �� ���������� ���������
		if (parent->n < T - 1)
			fix_delete(parent);
	}
}

void delete_element(node* cur, int k) {
	int i, j;
	node* t;

	if (cur == 0)
		return;

	if (cur == root && cur->leaf == 1) {//������-����
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

	// ����, �� �� ������
	if (cur->leaf == 1) {
		// ���� � �������
		for (i = 0; i < cur->n; i++) {
			if (cur->key[i] == k) {
				for (j = i; j < cur->n - 1; j++)
					cur->key[j] = cur->key[j + 1];
				cur->n--;
				break;
			}
		}

		// ���� �������� ����� T-1 ������, �� ����������
		if (cur->n < T - 1)
			fix_delete(cur);
	}
	// ���������� ����
	else {
		for (i = 0; i < cur->n; i++) {
			if (cur->key[i] == k) {
				// ������� ��������������� � ����� ���������
				t = cur->child[i];
				while (t->leaf == 0)
					t = t->child[t->n];
				// ��������� ������ ���������� ����� ������ ���� ���������������
				cur->key[i] = t->key[t->n - 1];
				// ������� ������ ���� ���������������
				delete_element(t, t->key[t->n - 1]);
				return;
			}
		}

		// �� �����, ��������� � ���������������� ������
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

	//���������� �������� ��� �����
	for (i = 0; i < sizeof(mas) / sizeof(int); i++)
		insert(mas[i]);

	while (1) {
		printf("[0] �����    [1] ����������   [2] �����   [3] ������   [4] ������� \n");
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


















