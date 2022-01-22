#include "Maze.h"

// Глобальные переменные:
/* Указатель на массив вершин							   */
Vertex* Points;
/* Количество вершин									   */
int TotalPoints;
/* Количество ребер для SET_POS in PB					   */
int TotalEdges;
/* Указатель на список ребер триангуляции				   */
ListEdges* edges_triangle;
/* Указатель на список ребер минимального остовного дерева */
ListEdges* edges_emst;

double TimeTriangle, TimeEmod;

/* Перегрузка оператора сравнения для двух вершин	*/
/* Функция operator==								*/
/* Назначение:										*/
/*	перегрузка оператора сравнения для двух вершин  */
/* Входные данные:									*/
/*	v - вторя вершина  								*/
/* Возвращаемое значение   						    */
/*  TRUE - совпадение двух вершин, FALSE - обратное	*/
bool Vertex::operator==(const Vertex &v) 
{
	return (this->x == v.x && this->y == v.y);
}

/* Функция Distance									*/
/* Назначение:										*/
/*  определение веса графа							*/
/* Входные данные:									*/
/*	v - вторая вершина								*/
/* Возвращаемое значение:							*/
/*  расстояние между двумя точками					*/
double Vertex::Distance(Vertex v) 
{
	return sqrt(((v.x - this->x) * (v.x - this->x) + (v.y - this->y) * (v.y - this->y)));
}

/* Функция DrawVertexes								*/
/* Назначение:										*/
/*  прорисовка вершин								*/
/* Входные данные:									*/
/*	hWnd - дескриптор окна							*/
/*	points - указатель на массив вершин				*/
/*	N - размер массива								*/
/* Возвращаемое значение:							*/
/*  прорисовка вершин в заданном окне				*/
void DrawVertexes(HWND hWnd, Vertex* points, int N)
{
	/* Исключение */
	if (!points) return;

	/* Создание кисти красного цвета */
	HBRUSH brush = CreateSolidBrush(RED);
	/* Совместимый контекст */
	HDC hDC;
	/* Контекст графического устройства */
	hDC = GetDC(hWnd);
	/* Выделяем кисть в указанном контексте устройства */
	SelectObject(hDC, brush);
	/* Рисуем все вершины */
	for (int i(0); i < N; i++) {
		if (i == 0 || i == N - 1)
			Ellipse(hDC, points[i].x - 5, points[i].y - 5, points[i].x + 5, points[i].y + 5);
		else
			Ellipse(hDC, points[i].x - 2, points[i].y - 2, points[i].x + 2, points[i].y + 2);
	}
	/* Освобождение памяти */
	DeleteObject(brush);
}

/* Функция createRandomPoints													 */
/* Назначение:																	 */
/*  генерация случайных вершин													 */
/* Входные данные:																 */
/*	остутствуют																	 */
/* Возвращаемое значение:														 */
/*  массив вершин, заполненный случайными точками, относительно размеров окна	 */
void CreateRandomPoints()
{
	int xPos, yPos;

	/* Вычисление размеров окна */
	RECT rect;
	GetClientRect(GetDesktopWindow(), &rect);
	LONG width = rect.right;
	LONG height = rect.bottom - 20;
	/* Выделение памяти для массива вершин */
	Points = new Vertex[TotalPoints];
	/* Заплонение случайными величинами */
	for (int i = 0; i < TotalPoints; i++) {

		xPos = rand() % width;
		yPos = rand() % height;

		Points[i] = Vertex(xPos, yPos, i);
	}
}

/* Функция QuickSort											  */
/* Назначение:													  */
/*  быстрая сортировка(рекурсиваная) массива вершин				  */
/* Входные данные:												  */
/*	points - указатель на массив вершин							  */
/*	N - размер массива											  */
/* Возвращаемое значение:										  */
/*  остортированный массив вершин по координате x(по возрастанию) */
void QuickSort(Vertex* arr, int left, int right) 
{
	int i = left, j = right;
	Vertex tmp;
	/* опорный эл-т */
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

/* Функция DrawEdge					*/
/* Назначение:						*/
/*	нарисовать ребро				*/
/* Входные данные:					*/
/*	hdc - контекст устройства		*/
/* Возвращаемое значение:			*/
/*  прорисовка ребра				*/
void ListEdges::Node::DrawEdge(HDC hdc) 
{
	/* Откуда */
	MoveToEx(hdc, v1.x, v1.y, NULL);
	/* Докуда */
	LineTo(hdc, v2.x, v2.y);
}

/* Функция addEdge						*/
/* Назначение:							*/
/*	добавляет ребро в начало списка		*/
/* Входные данные:						*/
/*	v1, v2 - вершины ребра				*/
/*  c - цвет ребра						*/
/* Возвращаемое значение:				*/
/*  отсутствует							*/
void ListEdges::addEdge(Vertex v1, Vertex v2, COLOR c)
{
	Node* tmp = head;
	/* Проверяем выделение памяти для нового ребра */
	if (Node* e = new Node(v1, v2, c))
		/* Проверяем наличие головы */
		if (tmp) {
			/* Перемещаем голову */
			e->link = head;
			/* Вставка ребра */
			head = e;
		}
		else {
			/* Вставляем голову */
			head = e;
		}
}

/* Функция DrawEdges							*/
/* Назначение:									*/
/*	рисует все ребра из списка					*/
/* Входные данные:								*/
/*	hWnd - дескриптор окна						*/
/*	edges - указатель на список ребер			*/
/* Возвращаемое значение:						*/
/*  прорисовка всех ребрер списка				*/
void ListEdges::DrawEdges(HWND hWnd, const ListEdges* edges) 
{
	if (!edges) return;
	// Создание карандаша
	HPEN pen_triangl = CreatePen(1, 2, BLUE);
	HPEN pen_mst = CreatePen(1, 2, GREEN);
	// Совместимый контекст
	HDC hDC;
	/* Контекст графического устройства */
	hDC = GetDC(hWnd);
	/* Временный указатель для прохождения по списку */
	ListEdges::Node *temp = edges->head;
	/* Прорисовка всех ребер */
	while (temp) {
		/* Определение вида ребра для соотв. карандаша */
		if (temp->c == BLUE)
			SelectObject(hDC, pen_triangl);
		if (temp->c == GREEN)
			SelectObject(hDC, pen_mst);

		temp->DrawEdge(hDC);

		temp = temp->link;
	}
	/* Освобождение памяти */
	DeleteObject(pen_triangl);
	DeleteObject(pen_mst);
}

/* Функция MergeSort								*/
/* Назначение:										*/
/*	сортировка слиянием списка ребер по их весу		*/
/* Входные данные:									*/
/*	hWnd - дескриптор окна							*/
/*	edges - указатель на список ребер				*/
/* Возвращаемое значение:							*/
/*  отсортированный список ребер по их весу			*/
void ListEdges::MergeSort(ListEdges *edges)
{
	int listSize = 1, numMerges, leftSize, rightSize;
	Node *tail, *left, *right, *temp;

	// На каждом проходе мы объединяем списки размера K в списки размера 2K
	do {
		SendMessage(hProgress, PBM_SETPOS, listSize, TotalEdges);

		// left - указатель на начало списка 
		// tail - пустой список, к которому мы добавим элементы в конец, когда закончим с ними работать
		numMerges = 0, left = edges->head; tail = 0;
		// пока не достигли конца списка
		while (left) {
			numMerges++, right = left, leftSize = 0, rightSize = listSize;
			// Разрезаем список на две части, в зависимости от того, что наступит раньше
			while (right && leftSize < listSize)
				leftSize++, right = right->link;
			// Пробегаем по спискам, объединяя эл-ты
			while (leftSize > 0 || (rightSize > 0 && right)) {
				// Если один из списков пуст, мы должны выбрать другой эл-т списка
				// Слева пусто, поворот направо
				if (!leftSize) {
					temp = right; right = right->link; rightSize--;
					// Справа пусто, поворот налево
				}
				else if (!rightSize || !right) {
					temp = left; left = left->link; leftSize--;
					// Сравнение весов первых элементов каждого из списков и выбор нижнего
				}
				else if (left->w < right->w) {
					temp = left; left = left->link; leftSize--;
				}
				else {
					temp = right; right = right->link; rightSize--;
				}
				// Обновляем указатели для отслеживания нахождения в списке и дальнейшей сортирвки
				tail ? tail = tail->link = temp : tail = edges->head = temp;
			}
			// Справа список, который мы только что отсортировали
			left = right;
		}
		// Завершение списка, удвоение размера списка сортировки 
		tail->link = 0, listSize <<= 1;
		// Если произойдет лишь одно слияние, значит список отсортирован
	} while (numMerges != 1);
}

/* Функция dsu_get															 */
/* Назначение:																 */
/*	поднимается по предкам от вершины v, пока не дойдёт до корня(рекурсивно) */
/* Входные данные:															 */
/*	v - элемент множества (вершина) 										 */
/*	p - указатель на множество												 */
/* Возвращаемое значение:													 */
/*  возвращает лидера множества												 */
int dsu_get(int v, int* p) 
{
	return (v == p[v]) ? v : (p[v] = dsu_get(p[v], p));
}

/* Функция dsu_unite												 */
/* Назначение:														 */
/*	находит лидеров множеств и объединяет их в случае необходимости	 */
/* Входные данные:													 */
/*	a,b - элементы множеств											 */
/*	p - указатель на множество										 */
/* Возвращаемое значение:											 */
/*  объединенные множества											 */
void dsu_unite(int a, int b, int* p) 
{
	a = dsu_get(a, p);
	b = dsu_get(b, p);
	if (a != b)
		p[a] = b;
}

/* Функция CreateEMOD									*/
/* Назначение:											*/
/*	алгоритм наждения минимального остовного дерева		*/
/* Входные данные:										*/
/*	edges - указатель на список ребер					*/
/*	N - размер массива вершин							*/
/* Возвращаемое значение:								*/
/*  список ребер минимального остовного дерева			*/
ListEdges* CreateEMOD(ListEdges* edges, int N) 
{
	/* Обработка нетривиального случая */
	if (!edges->head) return 0;

	/* Предварительная сортировка слиянием */
	SendMessage(hMainStatusBar, WM_SETTEXT, 0, LPARAM("Merge sort for the list of triangulated edges"));
	ListEdges::MergeSort(edges);

	/* Формируем для каждой вершины свое множество */
	SendMessage(hMainStatusBar, WM_SETTEXT, 0, LPARAM("Forming for each vertex of the graph its own set"));
	int *set_vertex = new int[N];
	for (int i(0); i < N; i++) {
		SendMessage(hProgress, PBM_SETPOS, i, N);
		set_vertex[i] = i;
	}

	SendMessage(hMainStatusBar, WM_SETTEXT, 0, LPARAM("Creating the minimum spanning tree"));
	/* Выделяем память для списка ребер дерева */
	ListEdges* tree = new ListEdges();
	// Временный указатель для ребер исходного графа
	ListEdges::Node* tmp = edges->head;
	// Перебираем все рёбра 
	int i = 0;
	while (tmp) {

		SendMessage(hProgress, PBM_SETPOS, i++, TotalEdges);

		int a = tmp->v1.id, b = tmp->v2.id;
		/* Определяем, принадлежат ли его концы разным деревьям */
		if (dsu_get(a, set_vertex) != dsu_get(b, set_vertex)) {
			/* Помещаем дерево лес */
			tree->addEdge(tmp->v1, tmp->v2, GREEN);
			/* Объединение двух деревьев */
			dsu_unite(a, b, set_vertex);
		}
		tmp = tmp->link;
	}

	delete[] set_vertex;

	return tree;
}

/* Функция IsInside																		 */
/* Назначение:																			 */
/*	проверить условие Делоне для данной точки в заданном треугольнике					 */
/* Входные данные:																		 */
/*	v - данная точка(вершина)  															 */
/* Возвращаемое значение:																 */
/*  TRUE - в случае вхождения данной точки в описанную окружность заданного треугольника */
/*	FALSE - аналогично обратное															 */
bool Triangle::IsInside(Vertex v)
{
	// Инициализация координат вершин треугольника
	double x1 = v1.x, x2 = v2.x, x3 = v3.x,
		y1 = v1.y, y2 = v2.y, y3 = v3.y;

	double a = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);

	// Когда все 3 вершины лежат на одной прямой
	if (a == 0) return TRUE;

	// Вычисление коэффициентов
	double b = ((y1 - y2) * (x3 * x3 + y3 * y3)) +
		((y2 - y3) * (x1 * x1 + y1 * y1)) +
		((y3 - y1) * (x2 * x2 + y2 * y2));
	double c = ((x1 - x2) * (x3 * x3 + y3 * y3)) +
		((x2 - x3) * (x1 * x1 + y1 * y1)) +
		((x3 - x1) * (x2 * x2 + y2 * y2));
	// Вычисление центра окружности
	double x_circle = b / (2 * a),
		y_circle = -c / (2 * a);

	// Вычисление квадрата радиуса окружности
	double rad_2 = (x1 - x_circle) * (x1 - x_circle) + (y1 - y_circle) * (y1 - y_circle);

	/* Отмечаем что данный треугольник более не будет учитываться для поздних точек,    */
	/*	поскольку дальнейшие точки никогда больше не будут на внутр. части этого треуг. */
	if (x_circle + sqrt(rad_2) < v.x)
		IsComplete = TRUE;

	// Сравнение расстояния от точки до центра окр. с кв. радиуса окр.
	if ((v.x - x_circle) * (v.x - x_circle) +
		(v.y - y_circle) * (v.y - y_circle) < rad_2)
		return TRUE;
	else
		return FALSE;
}

/* Функция IsBase														*/
/* Назначение:															*/
/*	проверить является ли данная точка основанием в данном треугольнике	*/
/* Входные данные:														*/
/*	v - данная точка(вершина)  											*/
/* Возвращаемое значение:												*/
/*  TRUE - является одной из точек трегольника, FALSE - обратное		*/
bool Triangle::IsBase(Vertex v) 
{
	return (v == v1 || v == v2 || v == v3) ? TRUE : FALSE;
}

/* Функция addEdge											   */
/* Назначение:												   */
/*	добавляет ребро в конец списка, сравнивая остальные ребра, */
/*	в случае дублирования удаляет оба ребра					   */
/* Входные данные:											   */
/*	v1, v2 - вершины ребра									   */
/* Возвращаемое значение:									   */
/*  список ребер вмещащющего многоугольника					   */
void BufferEdges::addEdge(Vertex v1, Vertex v2) 
{
	Node* tmp = head;
	/* Проверяем выделение памяти для нового ребра */
	if (Node* e = new Node(v1, v2))
		/* Проверка на наличие головы */
		if (tmp) {
			/* Случай для дублированных ребер в голове списка */
			if ((tmp->v1 == v1 && tmp->v2 == v2) ||
				(tmp->v1 == v2 && tmp->v2 == v1)) {
				head = head->link;
				delete e;
				return;
			}
			while (tmp->link) {
				/* Случай для дублированных ребер остальных элементов списка */
				if ((tmp->link->v1 == v1 && tmp->link->v2 == v2) ||
					(tmp->link->v1 == v2 && tmp->link->v2 == v1)) {
					tmp->link = tmp->link->link;
					delete e;
					return;
				}
				tmp = tmp->link;
			}
			/* Вставляем ребро */
			tmp->link = e;
		} else {
			/* Вставляем голову */
			head = e;
		}
}


/* Функция createIncTriangulate						*/
/* Назначение:										*/
/*	алгоритм инкрементальной триангуляции			*/
/* Входные данные:									*/
/*	points - указатель на массив вершин				*/
/*	N - размер массива вершин						*/
/* Возвращаемое значение:							*/
/*  список триангулированных ребер					*/
ListEdges* CreateIncTriangulate(Vertex* points, int N) 
{
	/* Выделяем память для треугольников */
	Triangle* triangles = new Triangle[N * 2 + 1];
	int n_triangles = 0;
	/* Узнаем размеры окна для супер-треугольника */
	RECT rect;
	GetWindowRect(GetDesktopWindow(), &rect);
	LONG width = rect.right;
	LONG height = rect.bottom;
	/* ОБъявляем вершины супер-треугольника */
	Vertex s_v1(width * (-10), height * 10),
		s_v2(width * 10, height * 10),
		s_v3(width / 2, height * (-10));
	/* Добавляем супер-треугольник в массив трегольников */
	triangles[0] = Triangle(s_v1, s_v2, s_v3);
	n_triangles++;
	/* Объявляем буфер ребер */
	BufferEdges* buf_edges;
	/* Цикл по каждой вершине графа */
	for (int i(0); i < N; i++) {
		/* Выделяем память для буфера ребер многоульника */
		buf_edges = new BufferEdges();
		/* Рассматриваем все треугольники в текущий момент */
		for (int j(0); j < n_triangles; j++) {
			/* Пропускаем треугольники, которые более не учитываются */
			if (triangles[j].IsComplete)
				continue;
			/* Находим треугольник(более не действительный),		*/
			/* у которого внутрь описанной окр. попадает новый узел */
			if (triangles[j].IsInside(points[i]))
			{
				/* Забираем его ребра в буфер */
				/* Удаляя все дважды указанные ребра, оставляя только края вмещающего многоуг.*/
				buf_edges->addEdge(triangles[j].v1, triangles[j].v2);
				buf_edges->addEdge(triangles[j].v2, triangles[j].v3);
				buf_edges->addEdge(triangles[j].v3, triangles[j].v1);
				/* Удаляем данный треугольник */
				triangles[j--] = triangles[(n_triangles--) - 1];
			}
		}
		/* Формируем треугольники, образованные между точкой и краями вмещающ. многоуг. */
		BufferEdges::Node* itEdges = buf_edges->head;
		while (itEdges) {
			triangles[n_triangles] = Triangle(itEdges->v1, itEdges->v2, points[i]);
			n_triangles++;

			itEdges = itEdges->link;
		}
		/* Очищаем память буфера */
		buf_edges->~BufferEdges();

		SendMessage(hProgress, PBM_SETPOS, i, N);
	}

	/* Очищаем массив треугольников от треугольников с супер-вершинами */
	SendMessage(hMainStatusBar, WM_SETTEXT, 0,
		LPARAM("Deleting additional triangles and creating the list of edges"));
	ListEdges *edges = new ListEdges();
	TotalEdges = 0;
	for (int i(0); i < n_triangles; i++) {
		SendMessage(hProgress, PBM_SETPOS, i, n_triangles);
		if (triangles[i].IsBase(s_v1) || triangles[i].IsBase(s_v2) || triangles[i].IsBase(s_v3)) {
			triangles[i--] = triangles[(n_triangles--) - 1];
		} else {
			/* Формируем список ребер триангуляции */
			edges->addEdge(triangles[i].v1, triangles[i].v2, BLUE);
			edges->addEdge(triangles[i].v2, triangles[i].v3, BLUE);
			edges->addEdge(triangles[i].v3, triangles[i].v1, BLUE);

			TotalEdges++;
		}
	}

	delete[] triangles;

	return edges;
}

/* Функция CreateNativeTriangle						*/
/* Назначение:										*/
/*	алгоритм наивной триангуляции					*/
/* Входные данные:									*/
/*	points - указатель на массив вершин				*/
/*	N - размер массива вершин						*/
/* Возвращаемое значение:							*/
/*  список триангулированных ребер					*/
ListEdges* CreateNativeTriangle(Vertex* points, int N) 
{
	ListEdges *edges = new ListEdges();

	// Рассматриваем все треугольники
	for (int i = 0; i < N; i++) {
		for (int j = i + 1; j < N; j++) {
			for (int k = j + 1; k < N; k++) {
				// По умолчанию внутри описанной окр. треугольника нет других вершин
				bool isTriangle = false;
				// Рассматриваем четвертые вершины относительно описанной окружности выбранного треуголника
				for (int a = 0; a < N; a++) {
					// Пропускаем идентичные вершины
					if (a == i || a == j || a == k) continue;
					// Условие вхождения в окружность
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

/* Функция CreateRandomMaze									   */
/* Назначение:												   */
/*	создает случайный лабиринт, инкрементальной триангуляцией  */
/* Входные данные:											   */
/*  отсутствуют							 					   */
/* Возвращаемое значение:								       */
/*  отсутствует							 					   */
void CreateRandomMaze()
{
	// Обнуляем прогресс бар
	SendMessage(hProgress, PBM_SETPOS, 0, 0);
	// Устанавливаем новый диапозон прогресс бара для сортировки
	// Берем в качестве границы N, тк сложность быстрой сорт. NLOGN
	SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, TotalPoints));

	SendMessage(hMainStatusBar, WM_SETTEXT, 0, LPARAM("Quick sort of vertex array"));

	clock_t start_t = clock();
	// Сортируем вершины графа (по координате x)
	QuickSort(Points, 0, TotalPoints - 1);

	SendMessage(hMainStatusBar, WM_SETTEXT, 0, LPARAM("Triangulation"));

	// Генерируем триангуляцию Делоне
	edges_triangle = CreateIncTriangulate(Points, TotalPoints);
	clock_t end_t = clock();
	TimeTriangle = (double)(end_t - start_t) / CLOCKS_PER_SEC;

	start_t = clock();
	// Генерируем минимальное оставное дерево
	edges_emst = CreateEMOD(edges_triangle, TotalPoints);
	end_t = clock();
	TimeEmod = (double)(end_t - start_t) / CLOCKS_PER_SEC; // искомое время
}

/* Функция CreateManualMaze																	 */
/* Назначение:																				 */
/*	создает лабиринт с заданными точками пользователем в окне, наивной триангуляцией		 */
/* Входные данные:																			 */
/*  отсутствуют							 													 */
/* Возвращаемое значение:																	 */
/*  отсутствует							 													 */
void CreateManualMaze()
{
	clock_t start_t = clock();
	/* Генерируем наивным алгоритмом триангуляцию Делоне */
	edges_triangle = CreateNativeTriangle(Points, TotalPoints);
	clock_t end_t = clock();
	TimeTriangle = (double)(end_t - start_t) / CLOCKS_PER_SEC;

	start_t = clock();
	/* Генерируем минимальное оставное дерево */
	edges_emst = CreateEMOD(edges_triangle, TotalPoints);
	end_t = clock();
	TimeEmod = (double)(end_t - start_t) / CLOCKS_PER_SEC;
}