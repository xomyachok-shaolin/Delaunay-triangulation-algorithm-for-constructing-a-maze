#ifndef _MAZE_H_
#define _MAZE_H_

#include "stdafx.h"

/* Цвета для прорисовки */
enum COLOR { 
	BLUE  =	RGB(0, 0, 255), 
	GREEN = RGB(0, 255, 0), 
	RED	  =	RGB(255, 0, 0) 
};

/* Окно прогресса в главном окне (ProgressBar) */
extern HWND hProgress;
/* Строка состояния в главном окне			   */
extern HWND hMainStatusBar;

extern double TimeTriangle, TimeEmod;

struct Vertex {
	/* Идентификатор */
	unsigned int id;
	/* Координаты на плоскости */
	int x, y;
	/* Конструктор по умолчанию */
	Vertex() : x(0), y(0) {};
	/* Конструктор для вершин супер-треугольника */
	Vertex(int x, int y) : x(x), y(y) {};
	/* Конструктор с идентификатором вершин */
	Vertex(int x, int y, int id) : x(x), y(y), id(id) {};

	/* Перегрузка оператора сравнения для двух вершин */
	bool operator== (const Vertex&);

	/* Расстояние между двумя точками (вес графа) */
	double Distance(Vertex);
};

/* Указатель на массив вершин */
extern Vertex* Points;
/* Количество вершин */
extern int TotalPoints;
/* Количество ребер для SET_POS in PB */
extern int TotalEdges;

/* Рисует вершины */
void DrawVertexes(HWND, Vertex*, int);
/* Заполнение массива вершин случайными точками */
void CreateRandomPoints();
/* Быстрая сортировка массива вершин по координате x */
void QuickSort(Vertex* arr, int left, int right);

struct ListEdges {
	/* Основной элемент списка (ребро) */
	struct Node {
		/* Цвет ребра*/
		COLOR c;
		/* Вершины ребра */
		Vertex v1, v2;
		/* Вес ребра */
		double w;
		/* Указатель на следующий элемент */
		Node* link;
		/* Конструктор по умолчанию */
		Node() :link(0) {}
		/* Конструктор преобразования */
		Node(Vertex x, Vertex y, COLOR c) : v1(x), v2(y), link(0), c(c) {
			/* Определение веса ребра */
			w = x.Distance(y);
		}
		/* Прорисовка ребра */
		void DrawEdge(HDC hdc);
	};
	/* Головной элемент списка */
	Node *head;

	/* Конструктор по умолчанию */
	ListEdges() :head(0) {};

	/* Освобождение памяти (деструктор) */
	~ListEdges() {
		while (head) {
			/* Сохраняем указатель на второй узел, который станет новым головным элементом */
			Node* newHead = head->link;
			/* Освобождаем память, выделенную для удаляемого головного элемента */
			delete head;
			/* Назначаем новый головной элемент */
			head = newHead;
		}
	}
	/* Добавление ребра */
	void addEdge(Vertex v1, Vertex v2, COLOR c);
	/* Прорисовка списка ребер */
	static void DrawEdges(HWND hWnd, const ListEdges* edges);
	/*void addEdge(Vertex v1, Vertex v2)
	{
		Node* tmp = head;
		// Проверяем выделение памяти для нового ребра
		if (Node* e = new Node(v1, v2))
			// Проверяем наличие головы
			if (tmp) {
				// Пока нет следующего эл-та
				while (tmp->link) {
					// Условие идентичных ребер
					if ((tmp->v1 == v1 && tmp->v2 == v2) ||
						(tmp->v1 == v2 && tmp->v2 == v1)) {
						//length--;
						delete e;
						return;
					}
					// Шагаем дальше
					tmp = tmp->link;
				}
				// Вставляем ребро
				tmp->link = e;
			}
			else {
				// Вставляем голову
				head = e;
			}
		//length++;
	}*/
	/* Сортировка слиянием списка ребер */
	static void MergeSort(ListEdges *edges);

};

/* Операции над системой непересекающихся множеств */
int dsu_get(int v, int* p);
void dsu_unite(int a, int b, int* p);
/* Нахождение евклидово минимального остовного дерева */
ListEdges* CreateEMOD(ListEdges* edges, int N);

struct Triangle {
	Vertex v1, v2, v3;
	/* Переменная для идент. треуг., которые не нужно рассматривать */
	bool IsComplete;
	/* Конструктор по умолчанию */
	Triangle() {}
	/* Конструктор преобразования */
	Triangle(Vertex a, Vertex b, Vertex c) : v1(a), v2(b), v3(c), IsComplete(0) {}
	/* Проверка условия Делоне */
	bool IsInside(Vertex v);

	bool IsBase(Vertex v);
};

struct BufferEdges {
	/* Основной элемент списка (ребро) */
	struct Node {
		/* Вершины ребра */
		Vertex v1, v2;
		/* Указатель на следующий элемент */
		Node* link;
		/* Конструктор по умолчанию */
		Node() :link(0) {}
		/* Конструктор преобразования */
		Node(Vertex x, Vertex y) : v1(x), v2(y), link(0) {}
	};
	/* Головной элемент списка */
	Node *head;

	/* Конструктор по умолчанию */
	BufferEdges() :head(0) {};

	/* Освобождение памяти (деструктор) */
	~BufferEdges() {
		while (head) {
			// Сохраняем указатель на второй узел, который станет новым головным элементом
			Node* newHead = head->link;
			// Освобождаем память, выделенную для удаляемого головного элемента
			delete head;
			// Назначаем новый головной элемент
			head = newHead;
		}
	}

	/* Добавление ребра в список */
	void addEdge(Vertex v1, Vertex v2);
};
/* Инкрементальная триангуляция */
ListEdges* CreateIncTriangulate(Vertex* points, int N);
/* Наивная триангуляция */
ListEdges* CreateNativeTriangle(Vertex* points, int N);

/* Указатель на список ребер триангуляции				   */
extern ListEdges* edges_triangle;
/* Указатель на список ребер минимального остовного дерева */
extern ListEdges* edges_emst;

/* Генерация лабиринтов */
void CreateRandomMaze();
void CreateManualMaze();

#endif
