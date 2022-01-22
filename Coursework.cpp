#include "Maze.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
TCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

/* Окно для ввода кол-ва вершин					*/
HWND hEdit;
/* Окно прогресса в главном окне (ProgressBar)	*/
HWND hProgress;
/* Строка состояния в главном окне				*/
HWND hMainStatusBar;
/* Строка состояния в окне ручного ввода		*/
HWND hManualStatusBar;
/* CheckBox для выбора режима работы			*/
HWND hCheckBox;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_COURSEWORK, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_COURSEWORK));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_COURSEWORK));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, (LPCWSTR)szTitle, WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
      200, 200, 355, 210, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

  // SetDialogBkColor();

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: DlgProcManual(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в диалоговом окне с ручным последовательным вводом вершин графа.
//
INT_PTR DlgProcManual(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc;
	/* Для пользовательских координат вершины */
	int xPos, yPos;
	/* Для случая совпадающих вершин */
	bool isIdentPoint = NULL;

	/* Относительно размеров окна инициализируем размеры частей statusbar'а */
	RECT rc;
	GetWindowRect(hWnd, &rc);
	int width = rc.right - rc.left;
	int wParts[3] = { width / 3, width - width / 3, width };

	switch (message)
	{
	case WM_INITDIALOG:
		
		hManualStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, 0, hWnd, 0);
		SendMessage(hManualStatusBar, SB_SETPARTS, 3, (LPARAM)&wParts);

		SendMessage(hManualStatusBar, SB_SETTEXT, 0, (LPARAM)"Please, click in any area on the window...");
		break;
	case WM_LBUTTONDOWN:
		/* Узнаём координаты */
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);

		/* Проверка для случая совпадающих вершин */
		if (Points)
			for (int i(0); i <= TotalPoints; i++)
				if (Points[i].x == xPos && Points[i].y == yPos)
					isIdentPoint = 1;
		if (!isIdentPoint) {
			/* Перераспределение памяти массива вершин */
			Points = (Vertex*)realloc(Points, (TotalPoints + 1) * sizeof(Vertex));
			/* Добавление очередной вершины */
			Points[TotalPoints] = Vertex(xPos, yPos, TotalPoints);
			TotalPoints++;
		}
		
		/* Общение с пользователем и вывод времени работы алгоритмов */
		if (TotalPoints < 3)
			SendMessage(hManualStatusBar, WM_SETTEXT, 0, LPARAM("Please, again..."));
		else {
			CreateManualMaze();
			char buf_mb[64];
			sprintf(buf_mb, "Numbers of vertex: %d", TotalPoints);
			SendMessage(hManualStatusBar, SB_SETTEXT, 0, (LPARAM)buf_mb);
			sprintf(buf_mb, "Time for Naive algorithm triangulation: %f(s)", TimeTriangle);
			SendMessage(hManualStatusBar, SB_SETTEXT, 1, (LPARAM)buf_mb);
			sprintf(buf_mb, "Time for algorithm searching of MST: %f(s)", TimeEmod);
			SendMessage(hManualStatusBar, SB_SETTEXT, 2, (LPARAM)buf_mb);
		}
		/* сообщение WM_PAINT */
		InvalidateRect(hWnd, NULL, 1); 
		break;

	case WM_PAINT:
		// перерисовка изображения
		// контекст графического устройства
		hdc = BeginPaint(hWnd, &ps);

		FillRect(ps.hdc, &ps.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));

		ListEdges::DrawEdges(hWnd, edges_triangle);
		ListEdges::DrawEdges(hWnd, edges_emst);
		DrawVertexes(hWnd, Points, TotalPoints);

		EndPaint(hWnd, &ps);
		return 1;
	case WM_CLOSE:
		delete[] Points;
		Points = 0;
		TotalPoints = 0;
		if (edges_triangle) {
			edges_triangle->~ListEdges();
			edges_triangle = 0;
			edges_emst->~ListEdges();
			edges_emst = 0;
		}
		SendMessage(hProgress, PBM_SETPOS, 0, 0);
		SendMessage(hMainStatusBar, WM_SETTEXT, 0, LPARAM("For automatic, select the number of graph vertices in [3;200000]"));
		EndDialog(hWnd, 0);
		return 1;
	}
	return 0;
}

//
//  ФУНКЦИЯ: DlgProcAutomatic(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Прорисовка в диалоговом окне уже сгенерированного лабиринта.
//
INT_PTR DlgProcAutomatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	PAINTSTRUCT ps;
	HDC hdc;

	ShowWindow(hWnd, SW_MAXIMIZE);

	switch (message) {
	case WM_PAINT:
		// перерисовка изображения
		// контекст графического устройства
		hdc = BeginPaint(hWnd, &ps);

		/* Заполняем прямоугольник окна черным цветом */
		FillRect(ps.hdc, &ps.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));
		/* Прорисовка всех эл-тов лабиринта */
		ListEdges::DrawEdges(hWnd, edges_triangle);
		ListEdges::DrawEdges(hWnd, edges_emst);
		DrawVertexes(hWnd, Points, TotalPoints);

		EndPaint(hWnd, &ps);
		return 1;
	case WM_CLOSE:
		/* Убираемся за собой */
		delete[] Points;
		Points = 0;
		TotalPoints = 0;
		edges_triangle->~ListEdges();
		edges_triangle = 0;
		edges_emst->~ListEdges();
		edges_emst = 0;
		/* Закрываем диалог */
		EndDialog(hWnd, 0);
		SendMessage(hProgress, PBM_SETPOS, 0, 0);
		SendMessage(hMainStatusBar, WM_SETTEXT, 0, LPARAM("For automatic, select the number of graph vertices in [3;200000]"));
		return 1;
	}
	return 0;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		HWND hBtnManual = CreateWindow(
			"BUTTON",  // Predefined class; Unicode assumed 
			"Manual generation",      // Button text 
			WS_VISIBLE | WS_CHILD | BS_NOTIFY | BS_PUSHBUTTON,  // Styles 
			180,         // x position 
			55,         // y position 
			150,        // Button width
			30,        // Button height
			hWnd,     // Parent window
			(HMENU)IDB_MANUAL,
			hInst,
			NULL);      // Pointer not needed.

		HWND hBtnAutomatic = CreateWindow(
			"BUTTON",  // Predefined class; Unicode assumed 
			"Automatic generation",      // Button text 
			WS_VISIBLE | WS_CHILD | BS_NOTIFY | BS_PUSHBUTTON,
			10,         // x position 
			55,         // y position 
			150,        // Button width
			30,        // Button height
			hWnd,     // Parent window
			(HMENU)IDB_AUTOMATIC,
			hInst,
			NULL);      // Pointer not needed.

		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "Edit", "test",
			WS_CHILD | WS_VISIBLE | ES_NUMBER, 180, 10, 150,
			35, hWnd, NULL, NULL, NULL);


		HWND hUpDown = CreateUpDownControl(
			WS_CHILD | WS_BORDER | WS_VISIBLE |
			UDS_WRAP | UDS_ARROWKEYS | UDS_ALIGNRIGHT | UDS_NOTHOUSANDS |
			UDS_SETBUDDYINT, 0, 0, 0, 0, hWnd, IDC_COURSEWORK, hInst,
			hEdit, 10000, 3, 3);

		static HWND hWndLabel = CreateWindow("static", "ST_U",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			10, 10, 150, 35,
			hWnd, 0,
			hInst, NULL);
		SetWindowText(hWndLabel, "Number of graph vertices(for automatic):");


		hProgress = CreateWindowEx(0, PROGRESS_CLASS,
			(LPSTR)NULL, WS_VISIBLE | WS_CHILD | PBS_SMOOTH,
			10, 95, 320, 30, hWnd, 0, hInst, NULL);

		hMainStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE,
			"For automatic, select the number of graph vertices in [3;200000]",
			hWnd, 0);

		hCheckBox = CreateWindow("button", "With ProgressBar",
			WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
			10, 130, 185, 15,
			hWnd, (HMENU)IDCB_PROGRESS, hInst, NULL);
		/* По умолчанию работаем с прогресс-баром */
		CheckDlgButton(hWnd, IDCB_PROGRESS, BST_CHECKED);
	}
	return 0;
	case WM_COMMAND:
		// Разобрать выбор в меню:
		switch (LOWORD(wParam))
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
		break;
		case IDCB_PROGRESS:
		{
			LRESULT res = SendMessage(hCheckBox, BM_GETCHECK, FALSE, FALSE);
			if (res == BST_CHECKED)
				hProgress = CreateWindowEx(0, PROGRESS_CLASS,
				(LPSTR)NULL, WS_VISIBLE | WS_CHILD | PBS_SMOOTH,
					10, 95, 320, 30, hWnd, 0, hInst, NULL);
			if (res == BST_UNCHECKED)
				DestroyWindow(hProgress);
		}
		break;
		case IDB_AUTOMATIC:
		{
			if (HIWORD(wParam) == BN_CLICKED) {
				WCHAR num_str[10];
				GetWindowTextW(hEdit, num_str, 10);
				TotalPoints = _wtoi(num_str);

				if (TotalPoints < 3 || TotalPoints > 200000) {
					int MB_Error = MessageBox(0, "Invalid input!",
						"Exception",
						MB_ICONERROR | MB_OK | MB_SETFOREGROUND | MB_TASKMODAL
					);
					if (MB_Error == IDOK)
						break;
				}
				CreateRandomPoints();

				CreateRandomMaze();

				char buf_mb[200];
				sprintf(buf_mb, "Generating completed for %d vertices.\n\
							Time for algorithm Triangulation(with pre-sorting): %f(s)\n\
							Time for algorithm searching of MST: %f(s)", TotalPoints, TimeTriangle, TimeEmod);

				int MB_Result = MessageBox(0, buf_mb,
					"Do you want vizualize labirint?",
					MB_ICONINFORMATION | MB_YESNO | MB_SETFOREGROUND | MB_TASKMODAL
				);

				if (MB_Result == IDYES)
					DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_AUTOMATIC), hWnd, DlgProcAutomatic, lParam);

				if (MB_Result == IDNO) {
					SendMessage(hProgress, PBM_SETPOS, 0, 0);
					SendMessage(hMainStatusBar, WM_SETTEXT, 0, LPARAM("For automatic, select the number of graph vertices in [3;200000]"));
					delete[] Points;
					Points = 0;
					TotalPoints = 0;
					edges_triangle->~ListEdges();
					edges_triangle = 0;
					edges_emst->~ListEdges();
					edges_emst = 0;
				}
			}
		}
		break;
		case IDB_MANUAL:
			if (HIWORD(wParam) == BN_CLICKED)
				DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_MANUAL), hWnd, DlgProcManual, lParam);
			break;
		}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
