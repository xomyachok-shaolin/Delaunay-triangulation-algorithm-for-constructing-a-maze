#include "Maze.h"

// ���������� ����������:
/* ��������� �� ������ ������							   */
Vertex* Points;
/* ���������� ������									   */
int TotalPoints;
/* ���������� ����� ��� SET_POS in PB					   */
int TotalEdges;
/* ��������� �� ������ ����� ������������				   */
ListEdges* edges_triangle;
/* ��������� �� ������ ����� ������������ ��������� ������ */
ListEdges* edges_emst;

double TimeTriangle, TimeEmod;

/* ���������� ��������� ��������� ��� ���� ������	*/
/* ������� operator==								*/
/* ����������:										*/
/*	���������� ��������� ��������� ��� ���� ������  */
/* ������� ������:									*/
/*	v - ����� �������  								*/
/* ������������ ��������   						    */
/*  TRUE - ���������� ���� ������, FALSE - ��������	*/
bool Vertex::operator==(const Vertex &v) 
{
	return (this->x == v.x && this->y == v.y);
}

/* ������� Distance									*/
/* ����������:										*/
/*  ����������� ���� �����							*/
/* ������� ������:									*/
/*	v - ������ �������								*/
/* ������������ ��������:							*/
/*  ���������� ����� ����� �������					*/
double Vertex::Distance(Vertex v) 
{
	return sqrt(((v.x - this->x) * (v.x - this->x) + (v.y - this->y) * (v.y - this->y)));
}

/* ������� DrawVertexes								*/
/* ����������:										*/
/*  ���������� ������								*/
/* ������� ������:									*/
/*	hWnd - ���������� ����							*/
/*	points - ��������� �� ������ ������				*/
/*	N - ������ �������								*/
/* ������������ ��������:							*/
/*  ���������� ������ � �������� ����				*/
void DrawVertexes(HWND hWnd, Vertex* points, int N)
{
	/* ���������� */
	if (!points) return;

	/* �������� ����� �������� ����� */
	HBRUSH brush = CreateSolidBrush(RED);
	/* ����������� �������� */
	HDC hDC;
	/* �������� ������������ ���������� */
	hDC = GetDC(hWnd);
	/* �������� ����� � ��������� ��������� ���������� */
	SelectObject(hDC, brush);
	/* ������ ��� ������� */
	for (int i(0); i < N; i++) {
		if (i == 0 || i == N - 1)
			Ellipse(hDC, points[i].x - 5, points[i].y - 5, points[i].x + 5, points[i].y + 5);
		else
			Ellipse(hDC, points[i].x - 2, points[i].y - 2, points[i].x + 2, points[i].y + 2);
	}
	/* ������������ ������ */
	DeleteObject(brush);
}

/* ������� createRandomPoints													 */
/* ����������:																	 */
/*  ��������� ��������� ������													 */
/* ������� ������:																 */
/*	�����������																	 */
/* ������������ ��������:														 */
/*  ������ ������, ����������� ���������� �������, ������������ �������� ����	 */
void CreateRandomPoints()
{
	int xPos, yPos;

	/* ���������� �������� ���� */
	RECT rect;
	GetClientRect(GetDesktopWindow(), &rect);
	LONG width = rect.right;
	LONG height = rect.bottom - 20;
	/* ��������� ������ ��� ������� ������ */
	Points = new Vertex[TotalPoints];
	/* ���������� ���������� ���������� */
	for (int i = 0; i < TotalPoints; i++) {

		xPos = rand() % width;
		yPos = rand() % height;

		Points[i] = Vertex(xPos, yPos, i);
	}
}

/* ������� QuickSort											  */
/* ����������:													  */
/*  ������� ����������(������������) ������� ������				  */
/* ������� ������:												  */
/*	points - ��������� �� ������ ������							  */
/*	N - ������ �������											  */
/* ������������ ��������:										  */
/*  ��������������� ������ ������ �� ���������� x(�� �����������) */
void QuickSort(Vertex* arr, int left, int right) 
{
	int i = left, j = right;
	Vertex tmp;
	/* ������� ��-� */
	int pivot = arr[(left + right) / 2].x;

	SendMessage(hProgress, PBM_SETPOS, i, TotalPoints);

	/* partition */
	while (i <= j) {
		while (arr[i].x < pivot)
			i++;
		while (arr[j].x > pivot)
			j--;
		if (i <= j) {
			tmp = arr[i];
			arr[i++] = arr[j];
			arr[j--] = tmp;
		}
	}
	/* recursion */
	if (left < j)
		QuickSort(arr, left, j);
	if (i < right)
		QuickSort(arr, i, right);
}

/* ������� DrawEdge					*/
/* ����������:						*/
/*	���������� �����				*/
/* ������� ������:					*/
/*	hdc - �������� ����������		*/
/* ������������ ��������:			*/
/*  ���������� �����				*/
void ListEdges::Node::DrawEdge(HDC hdc) 
{
	/* ������ */
	MoveToEx(hdc, v1.x, v1.y, NULL);
	/* ������ */
	LineTo(hdc, v2.x, v2.y);
}

/* ������� addEdge						*/
/* ����������:							*/
/*	��������� ����� � ������ ������		*/
/* ������� ������:						*/
/*	v1, v2 - ������� �����				*/
/*  c - ���� �����						*/
/* ������������ ��������:				*/
/*  �����������							*/
void ListEdges::addEdge(Vertex v1, Vertex v2, COLOR c)
{
	Node* tmp = head;
	/* ��������� ��������� ������ ��� ������ ����� */
	if (Node* e = new Node(v1, v2, c))
		/* ��������� ������� ������ */
		if (tmp) {
			/* ���������� ������ */
			e->link = head;
			/* ������� ����� */
			head = e;
		}
		else {
			/* ��������� ������ */
			head = e;
		}
}

/* ������� DrawEdges							*/
/* ����������:									*/
/*	������ ��� ����� �� ������					*/
/* ������� ������:								*/
/*	hWnd - ���������� ����						*/
/*	edges - ��������� �� ������ �����			*/
/* ������������ ��������:						*/
/*  ���������� ���� ������ ������				*/
void ListEdges::DrawEdges(HWND hWnd, const ListEdges* edges) 
{
	if (!edges) return;
	// �������� ���������
	HPEN pen_triangl = CreatePen(1, 2, BLUE);
	HPEN pen_mst = CreatePen(1, 2, GREEN);
	// ����������� ��������
	HDC hDC;
	/* �������� ������������ ���������� */
	hDC = GetDC(hWnd);
	/* ��������� ��������� ��� ����������� �� ������ */
	ListEdges::Node *temp = edges->head;
	/* ���������� ���� ����� */
	while (temp) {
		/* ����������� ���� ����� ��� �����. ��������� */
		if (temp->c == BLUE)
			SelectObject(hDC, pen_triangl);
		if (temp->c == GREEN)
			SelectObject(hDC, pen_mst);

		temp->DrawEdge(hDC);

		temp = temp->link;
	}
	/* ������������ ������ */
	DeleteObject(pen_triangl);
	DeleteObject(pen_mst);
}

/* ������� MergeSort								*/
/* ����������:										*/
/*	���������� �������� ������ ����� �� �� ����		*/
/* ������� ������:									*/
/*	hWnd - ���������� ����							*/
/*	edges - ��������� �� ������ �����				*/
/* ������������ ��������:							*/
/*  ��������������� ������ ����� �� �� ����			*/
void ListEdges::MergeSort(ListEdges *edges)
{
	int listSize = 1, numMerges, leftSize, rightSize;
	Node *tail, *left, *right, *temp;

	// �� ������ ������� �� ���������� ������ ������� K � ������ ������� 2K
	do {
		SendMessage(hProgress, PBM_SETPOS, listSize, TotalEdges);

		// left - ��������� �� ������ ������ 
		// tail - ������ ������, � �������� �� ������� �������� � �����, ����� �������� � ���� ��������
		numMerges = 0, left = edges->head; tail = 0;
		// ���� �� �������� ����� ������
		while (left) {
			numMerges++, right = left, leftSize = 0, rightSize = listSize;
			// ��������� ������ �� ��� �����, � ����������� �� ����, ��� �������� ������
			while (right && leftSize < listSize)
				leftSize++, right = right->link;
			// ��������� �� �������, ��������� ��-��
			while (leftSize > 0 || (rightSize > 0 && right)) {
				// ���� ���� �� ������� ����, �� ������ ������� ������ ��-� ������
				// ����� �����, ������� �������
				if (!leftSize) {
					temp = right; right = right->link; rightSize--;
					// ������ �����, ������� ������
				}
				else if (!rightSize || !right) {
					temp = left; left = left->link; leftSize--;
					// ��������� ����� ������ ��������� ������� �� ������� � ����� �������
				}
				else if (left->w < right->w) {
					temp = left; left = left->link; leftSize--;
				}
				else {
					temp = right; right = right->link; rightSize--;
				}
				// ��������� ��������� ��� ������������ ���������� � ������ � ���������� ���������
				tail ? tail = tail->link = temp : tail = edges->head = temp;
			}
			// ������ ������, ������� �� ������ ��� �������������
			left = right;
		}
		// ���������� ������, �������� ������� ������ ���������� 
		tail->link = 0, listSize <<= 1;
		// ���� ���������� ���� ���� �������, ������ ������ ������������
	} while (numMerges != 1);
}

/* ������� dsu_get															 */
/* ����������:																 */
/*	����������� �� ������� �� ������� v, ���� �� ����� �� �����(����������) */
/* ������� ������:															 */
/*	v - ������� ��������� (�������) 										 */
/*	p - ��������� �� ���������												 */
/* ������������ ��������:													 */
/*  ���������� ������ ���������												 */
int dsu_get(int v, int* p) 
{
	return (v == p[v]) ? v : (p[v] = dsu_get(p[v], p));
}

/* ������� dsu_unite												 */
/* ����������:														 */
/*	������� ������� �������� � ���������� �� � ������ �������������	 */
/* ������� ������:													 */
/*	a,b - �������� ��������											 */
/*	p - ��������� �� ���������										 */
/* ������������ ��������:											 */
/*  ������������ ���������											 */
void dsu_unite(int a, int b, int* p) 
{
	a = dsu_get(a, p);
	b = dsu_get(b, p);
	if (a != b)
		p[a] = b;
}

/* ������� CreateEMOD									*/
/* ����������:											*/
/*	�������� �������� ������������ ��������� ������		*/
/* ������� ������:										*/
/*	edges - ��������� �� ������ �����					*/
/*	N - ������ ������� ������							*/
/* ������������ ��������:								*/
/*  ������ ����� ������������ ��������� ������			*/
ListEdges* CreateEMOD(ListEdges* edges, int N) 
{
	/* ��������� �������������� ������ */
	if (!edges->head) return 0;

	/* ��������������� ���������� �������� */
	SendMessage(hMainStatusBar, WM_SETTEXT, 0, LPARAM("Merge sort for the list of triangulated edges"));
	ListEdges::MergeSort(edges);

	/* ��������� ��� ������ ������� ���� ��������� */
	SendMessage(hMainStatusBar, WM_SETTEXT, 0, LPARAM("Forming for each vertex of the graph its own set"));
	int *set_vertex = new int[N];
	for (int i(0); i < N; i++) {
		SendMessage(hProgress, PBM_SETPOS, i, N);
		set_vertex[i] = i;
	}

	SendMessage(hMainStatusBar, WM_SETTEXT, 0, LPARAM("Creating the minimum spanning tree"));
	/* �������� ������ ��� ������ ����� ������ */
	ListEdges* tree = new ListEdges();
	// ��������� ��������� ��� ����� ��������� �����
	ListEdges::Node* tmp = edges->head;
	// ���������� ��� ���� 
	int i = 0;
	while (tmp) {

		SendMessage(hProgress, PBM_SETPOS, i++, TotalEdges);

		int a = tmp->v1.id, b = tmp->v2.id;
		/* ����������, ����������� �� ��� ����� ������ �������� */
		if (dsu_get(a, set_vertex) != dsu_get(b, set_vertex)) {
			/* �������� ������ ��� */
			tree->addEdge(tmp->v1, tmp->v2, GREEN);
			/* ����������� ���� �������� */
			dsu_unite(a, b, set_vertex);
		}
		tmp = tmp->link;
	}

	delete[] set_vertex;

	return tree;
}

/* ������� IsInside																		 */
/* ����������:																			 */
/*	��������� ������� ������ ��� ������ ����� � �������� ������������					 */
/* ������� ������:																		 */
/*	v - ������ �����(�������)  															 */
/* ������������ ��������:																 */
/*  TRUE - � ������ ��������� ������ ����� � ��������� ���������� ��������� ������������ */
/*	FALSE - ���������� ��������															 */
bool Triangle::IsInside(Vertex v)
{
	// ������������� ��������� ������ ������������
	double x1 = v1.x, x2 = v2.x, x3 = v3.x,
		y1 = v1.y, y2 = v2.y, y3 = v3.y;

	double a = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);

	// ����� ��� 3 ������� ����� �� ����� ������
	if (a == 0) return TRUE;

	// ���������� �������������
	double b = ((y1 - y2) * (x3 * x3 + y3 * y3)) +
		((y2 - y3) * (x1 * x1 + y1 * y1)) +
		((y3 - y1) * (x2 * x2 + y2 * y2));
	double c = ((x1 - x2) * (x3 * x3 + y3 * y3)) +
		((x2 - x3) * (x1 * x1 + y1 * y1)) +
		((x3 - x1) * (x2 * x2 + y2 * y2));
	// ���������� ������ ����������
	double x_circle = b / (2 * a),
		y_circle = -c / (2 * a);

	// ���������� �������� ������� ����������
	double rad_2 = (x1 - x_circle) * (x1 - x_circle) + (y1 - y_circle) * (y1 - y_circle);

	/* �������� ��� ������ ����������� ����� �� ����� ����������� ��� ������� �����,    */
	/*	��������� ���������� ����� ������� ������ �� ����� �� �����. ����� ����� �����. */
	if (x_circle + sqrt(rad_2) < v.x)
		IsComplete = TRUE;

	// ��������� ���������� �� ����� �� ������ ���. � ��. ������� ���.
	if ((v.x - x_circle) * (v.x - x_circle) +
		(v.y - y_circle) * (v.y - y_circle) < rad_2)
		return TRUE;
	else
		return FALSE;
}

/* ������� IsBase														*/
/* ����������:															*/
/*	��������� �������� �� ������ ����� ���������� � ������ ������������	*/
/* ������� ������:														*/
/*	v - ������ �����(�������)  											*/
/* ������������ ��������:												*/
/*  TRUE - �������� ����� �� ����� �����������, FALSE - ��������		*/
bool Triangle::IsBase(Vertex v) 
{
	return (v == v1 || v == v2 || v == v3) ? TRUE : FALSE;
}

/* ������� addEdge											   */
/* ����������:												   */
/*	��������� ����� � ����� ������, ��������� ��������� �����, */
/*	� ������ ������������ ������� ��� �����					   */
/* ������� ������:											   */
/*	v1, v2 - ������� �����									   */
/* ������������ ��������:									   */
/*  ������ ����� ����������� ��������������					   */
void BufferEdges::addEdge(Vertex v1, Vertex v2) 
{
	Node* tmp = head;
	/* ��������� ��������� ������ ��� ������ ����� */
	if (Node* e = new Node(v1, v2))
		/* �������� �� ������� ������ */
		if (tmp) {
			/* ������ ��� ������������� ����� � ������ ������ */
			if ((tmp->v1 == v1 && tmp->v2 == v2) ||
				(tmp->v1 == v2 && tmp->v2 == v1)) {
				head = head->link;
				delete e;
				return;
			}
			while (tmp->link) {
				/* ������ ��� ������������� ����� ��������� ��������� ������ */
				if ((tmp->link->v1 == v1 && tmp->link->v2 == v2) ||
					(tmp->link->v1 == v2 && tmp->link->v2 == v1)) {
					tmp->link = tmp->link->link;
					delete e;
					return;
				}
				tmp = tmp->link;
			}
			/* ��������� ����� */
			tmp->link = e;
		} else {
			/* ��������� ������ */
			head = e;
		}
}


/* ������� createIncTriangulate						*/
/* ����������:										*/
/*	�������� ��������������� ������������			*/
/* ������� ������:									*/
/*	points - ��������� �� ������ ������				*/
/*	N - ������ ������� ������						*/
/* ������������ ��������:							*/
/*  ������ ����������������� �����					*/
ListEdges* CreateIncTriangulate(Vertex* points, int N) 
{
	/* �������� ������ ��� ������������� */
	Triangle* triangles = new Triangle[N * 2 + 1];
	int n_triangles = 0;
	/* ������ ������� ���� ��� �����-������������ */
	RECT rect;
	GetWindowRect(GetDesktopWindow(), &rect);
	LONG width = rect.right;
	LONG height = rect.bottom;
	/* ��������� ������� �����-������������ */
	Vertex s_v1(width * (-10), height * 10),
		s_v2(width * 10, height * 10),
		s_v3(width / 2, height * (-10));
	/* ��������� �����-����������� � ������ ������������ */
	triangles[0] = Triangle(s_v1, s_v2, s_v3);
	n_triangles++;
	/* ��������� ����� ����� */
	BufferEdges* buf_edges;
	/* ���� �� ������ ������� ����� */
	for (int i(0); i < N; i++) {
		/* �������� ������ ��� ������ ����� ������������ */
		buf_edges = new BufferEdges();
		/* ������������� ��� ������������ � ������� ������ */
		for (int j(0); j < n_triangles; j++) {
			/* ���������� ������������, ������� ����� �� ����������� */
			if (triangles[j].IsComplete)
				continue;
			/* ������� �����������(����� �� ��������������),		*/
			/* � �������� ������ ��������� ���. �������� ����� ���� */
			if (triangles[j].IsInside(points[i]))
			{
				/* �������� ��� ����� � ����� */
				/* ������ ��� ������ ��������� �����, �������� ������ ���� ���������� �������.*/
				buf_edges->addEdge(triangles[j].v1, triangles[j].v2);
				buf_edges->addEdge(triangles[j].v2, triangles[j].v3);
				buf_edges->addEdge(triangles[j].v3, triangles[j].v1);
				/* ������� ������ ����������� */
				triangles[j--] = triangles[(n_triangles--) - 1];
			}
		}
		/* ��������� ������������, ������������ ����� ������ � ������ �������. �������. */
		BufferEdges::Node* itEdges = buf_edges->head;
		while (itEdges) {
			triangles[n_triangles] = Triangle(itEdges->v1, itEdges->v2, points[i]);
			n_triangles++;

			itEdges = itEdges->link;
		}
		/* ������� ������ ������ */
		buf_edges->~BufferEdges();

		SendMessage(hProgress, PBM_SETPOS, i, N);
	}

	/* ������� ������ ������������� �� ������������� � �����-��������� */
	SendMessage(hMainStatusBar, WM_SETTEXT, 0,
		LPARAM("Deleting additional triangles and creating the list of edges"));
	ListEdges *edges = new ListEdges();
	TotalEdges = 0;
	for (int i(0); i < n_triangles; i++) {
		SendMessage(hProgress, PBM_SETPOS, i, n_triangles);
		if (triangles[i].IsBase(s_v1) || triangles[i].IsBase(s_v2) || triangles[i].IsBase(s_v3)) {
			triangles[i--] = triangles[(n_triangles--) - 1];
		} else {
			/* ��������� ������ ����� ������������ */
			edges->addEdge(triangles[i].v1, triangles[i].v2, BLUE);
			edges->addEdge(triangles[i].v2, triangles[i].v3, BLUE);
			edges->addEdge(triangles[i].v3, triangles[i].v1, BLUE);

			TotalEdges++;
		}
	}

	delete[] triangles;

	return edges;
}

/* ������� CreateNativeTriangle						*/
/* ����������:										*/
/*	�������� ������� ������������					*/
/* ������� ������:									*/
/*	points - ��������� �� ������ ������				*/
/*	N - ������ ������� ������						*/
/* ������������ ��������:							*/
/*  ������ ����������������� �����					*/
ListEdges* CreateNativeTriangle(Vertex* points, int N) 
{
	ListEdges *edges = new ListEdges();

	// ������������� ��� ������������
	for (int i = 0; i < N; i++) {
		for (int j = i + 1; j < N; j++) {
			for (int k = j + 1; k < N; k++) {
				// �� ��������� ������ ��������� ���. ������������ ��� ������ ������
				bool isTriangle = false;
				// ������������� ��������� ������� ������������ ��������� ���������� ���������� �����������
				for (int a = 0; a < N; a++) {
					// ���������� ���������� �������
					if (a == i || a == j || a == k) continue;
					// ������� ��������� � ����������
					Triangle tr;
					tr.v1 = points[i]; tr.v2 = points[j]; tr.v3 = points[k];
					if (isTriangle = tr.IsInside(points[a]))
						break;
				}
				if (!isTriangle) {
					edges->addEdge(points[i], points[j], BLUE);
					edges->addEdge(points[i], points[k], BLUE);
					edges->addEdge(points[j], points[k], BLUE);
				}
			}
		}
	}
	return edges;
}

/* ������� CreateRandomMaze									   */
/* ����������:												   */
/*	������� ��������� ��������, ��������������� �������������  */
/* ������� ������:											   */
/*  �����������							 					   */
/* ������������ ��������:								       */
/*  �����������							 					   */
void CreateRandomMaze()
{
	// �������� �������� ���
	SendMessage(hProgress, PBM_SETPOS, 0, 0);
	// ������������� ����� �������� �������� ���� ��� ����������
	// ����� � �������� ������� N, �� ��������� ������� ����. NLOGN
	SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, TotalPoints));

	SendMessage(hMainStatusBar, WM_SETTEXT, 0, LPARAM("Quick sort of vertex array"));

	clock_t start_t = clock();
	// ��������� ������� ����� (�� ���������� x)
	QuickSort(Points, 0, TotalPoints - 1);

	SendMessage(hMainStatusBar, WM_SETTEXT, 0, LPARAM("Triangulation"));

	// ���������� ������������ ������
	edges_triangle = CreateIncTriangulate(Points, TotalPoints);
	clock_t end_t = clock();
	TimeTriangle = (double)(end_t - start_t) / CLOCKS_PER_SEC;

	start_t = clock();
	// ���������� ����������� �������� ������
	edges_emst = CreateEMOD(edges_triangle, TotalPoints);
	end_t = clock();
	TimeEmod = (double)(end_t - start_t) / CLOCKS_PER_SEC; // ������� �����
}

/* ������� CreateManualMaze																	 */
/* ����������:																				 */
/*	������� �������� � ��������� ������� ������������� � ����, ������� �������������		 */
/* ������� ������:																			 */
/*  �����������							 													 */
/* ������������ ��������:																	 */
/*  �����������							 													 */
void CreateManualMaze()
{
	clock_t start_t = clock();
	/* ���������� ������� ���������� ������������ ������ */
	edges_triangle = CreateNativeTriangle(Points, TotalPoints);
	clock_t end_t = clock();
	TimeTriangle = (double)(end_t - start_t) / CLOCKS_PER_SEC;

	start_t = clock();
	/* ���������� ����������� �������� ������ */
	edges_emst = CreateEMOD(edges_triangle, TotalPoints);
	end_t = clock();
	TimeEmod = (double)(end_t - start_t) / CLOCKS_PER_SEC;
}