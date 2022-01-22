#ifndef _MAZE_H_
#define _MAZE_H_

#include "stdafx.h"

/* ����� ��� ���������� */
enum COLOR { 
	BLUE  =	RGB(0, 0, 255), 
	GREEN = RGB(0, 255, 0), 
	RED	  =	RGB(255, 0, 0) 
};

/* ���� ��������� � ������� ���� (ProgressBar) */
extern HWND hProgress;
/* ������ ��������� � ������� ����			   */
extern HWND hMainStatusBar;

extern double TimeTriangle, TimeEmod;

struct Vertex {
	/* ������������� */
	unsigned int id;
	/* ���������� �� ��������� */
	int x, y;
	/* ����������� �� ��������� */
	Vertex() : x(0), y(0) {};
	/* ����������� ��� ������ �����-������������ */
	Vertex(int x, int y) : x(x), y(y) {};
	/* ����������� � ��������������� ������ */
	Vertex(int x, int y, int id) : x(x), y(y), id(id) {};

	/* ���������� ��������� ��������� ��� ���� ������ */
	bool operator== (const Vertex&);

	/* ���������� ����� ����� ������� (��� �����) */
	double Distance(Vertex);
};

/* ��������� �� ������ ������ */
extern Vertex* Points;
/* ���������� ������ */
extern int TotalPoints;
/* ���������� ����� ��� SET_POS in PB */
extern int TotalEdges;

/* ������ ������� */
void DrawVertexes(HWND, Vertex*, int);
/* ���������� ������� ������ ���������� ������� */
void CreateRandomPoints();
/* ������� ���������� ������� ������ �� ���������� x */
void QuickSort(Vertex* arr, int left, int right);

struct ListEdges {
	/* �������� ������� ������ (�����) */
	struct Node {
		/* ���� �����*/
		COLOR c;
		/* ������� ����� */
		Vertex v1, v2;
		/* ��� ����� */
		double w;
		/* ��������� �� ��������� ������� */
		Node* link;
		/* ����������� �� ��������� */
		Node() :link(0) {}
		/* ����������� �������������� */
		Node(Vertex x, Vertex y, COLOR c) : v1(x), v2(y), link(0), c(c) {
			/* ����������� ���� ����� */
			w = x.Distance(y);
		}
		/* ���������� ����� */
		void DrawEdge(HDC hdc);
	};
	/* �������� ������� ������ */
	Node *head;

	/* ����������� �� ��������� */
	ListEdges() :head(0) {};

	/* ������������ ������ (����������) */
	~ListEdges() {
		while (head) {
			/* ��������� ��������� �� ������ ����, ������� ������ ����� �������� ��������� */
			Node* newHead = head->link;
			/* ����������� ������, ���������� ��� ���������� ��������� �������� */
			delete head;
			/* ��������� ����� �������� ������� */
			head = newHead;
		}
	}
	/* ���������� ����� */
	void addEdge(Vertex v1, Vertex v2, COLOR c);
	/* ���������� ������ ����� */
	static void DrawEdges(HWND hWnd, const ListEdges* edges);
	/*void addEdge(Vertex v1, Vertex v2)
	{
		Node* tmp = head;
		// ��������� ��������� ������ ��� ������ �����
		if (Node* e = new Node(v1, v2))
			// ��������� ������� ������
			if (tmp) {
				// ���� ��� ���������� ��-��
				while (tmp->link) {
					// ������� ���������� �����
					if ((tmp->v1 == v1 && tmp->v2 == v2) ||
						(tmp->v1 == v2 && tmp->v2 == v1)) {
						//length--;
						delete e;
						return;
					}
					// ������ ������
					tmp = tmp->link;
				}
				// ��������� �����
				tmp->link = e;
			}
			else {
				// ��������� ������
				head = e;
			}
		//length++;
	}*/
	/* ���������� �������� ������ ����� */
	static void MergeSort(ListEdges *edges);

};

/* �������� ��� �������� ���������������� �������� */
int dsu_get(int v, int* p);
void dsu_unite(int a, int b, int* p);
/* ���������� ��������� ������������ ��������� ������ */
ListEdges* CreateEMOD(ListEdges* edges, int N);

struct Triangle {
	Vertex v1, v2, v3;
	/* ���������� ��� �����. �����., ������� �� ����� ������������� */
	bool IsComplete;
	/* ����������� �� ��������� */
	Triangle() {}
	/* ����������� �������������� */
	Triangle(Vertex a, Vertex b, Vertex c) : v1(a), v2(b), v3(c), IsComplete(0) {}
	/* �������� ������� ������ */
	bool IsInside(Vertex v);

	bool IsBase(Vertex v);
};

struct BufferEdges {
	/* �������� ������� ������ (�����) */
	struct Node {
		/* ������� ����� */
		Vertex v1, v2;
		/* ��������� �� ��������� ������� */
		Node* link;
		/* ����������� �� ��������� */
		Node() :link(0) {}
		/* ����������� �������������� */
		Node(Vertex x, Vertex y) : v1(x), v2(y), link(0) {}
	};
	/* �������� ������� ������ */
	Node *head;

	/* ����������� �� ��������� */
	BufferEdges() :head(0) {};

	/* ������������ ������ (����������) */
	~BufferEdges() {
		while (head) {
			// ��������� ��������� �� ������ ����, ������� ������ ����� �������� ���������
			Node* newHead = head->link;
			// ����������� ������, ���������� ��� ���������� ��������� ��������
			delete head;
			// ��������� ����� �������� �������
			head = newHead;
		}
	}

	/* ���������� ����� � ������ */
	void addEdge(Vertex v1, Vertex v2);
};
/* ��������������� ������������ */
ListEdges* CreateIncTriangulate(Vertex* points, int N);
/* ������� ������������ */
ListEdges* CreateNativeTriangle(Vertex* points, int N);

/* ��������� �� ������ ����� ������������				   */
extern ListEdges* edges_triangle;
/* ��������� �� ������ ����� ������������ ��������� ������ */
extern ListEdges* edges_emst;

/* ��������� ���������� */
void CreateRandomMaze();
void CreateManualMaze();

#endif
