/*#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <string>


RECT rcClient;


HINSTANCE hI;
const int tableColumn = 7;
const int tableRow = 10;
int tableWidth;
int tableHeight;

std::wstring tableText[tableRow][tableColumn];
std::wstring tableOutText[tableRow][tableColumn];

int deltaHeight[tableRow];
int deltaWidth;


HBRUSH brushBackground;
HBRUSH brushWhite;
RECT tab;
int numberRow, numberColumn;
BOOL keyPre = FALSE;
LONG activeCellRow = 0, activeCellColumn = 0;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL Main_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
void Main_OnPaint(HWND hWnd);
void Main_OnWheel(HWND hWnd, int xPos, int yPos, int zDelta, UINT fwKeys);
void Main_OnSize(HWND hWnd, UINT state, int cx, int cy);
void Main_OnDestroy(HWND hWnd);
void Main_OnChar(HWND hWnd, TCHAR ch, int cRepeat);
void Main_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);


bool AdjustRow(HWND hwnd, int rowIndex);
std::wstring SplitString(HDC hdc, std::wstring str, LONG width);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR     lpCmdLine, _In_ int       nCmdShow)

{
	WNDCLASSEX wcex; HWND hWnd; MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("SP2Class");
	wcex.hIconSm = wcex.hIcon;

	RegisterClassEx(&wcex);

	hWnd = CreateWindow(_T("SP2Class"), _T("Table"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);


	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(_In_ HWND   hWnd, _In_ UINT   message, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hWnd, WM_PAINT, Main_OnPaint);
		HANDLE_MSG(hWnd, WM_CREATE, Main_OnCreate);
		HANDLE_MSG(hWnd, WM_SIZE, Main_OnSize);
		HANDLE_MSG(hWnd, WM_MOUSEWHEEL, Main_OnWheel);
		HANDLE_MSG(hWnd, WM_DESTROY, Main_OnDestroy);
		HANDLE_MSG(hWnd, WM_CHAR, Main_OnChar);
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, Main_OnLButtonDown);


	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = 500;
		lpMMI->ptMinTrackSize.y = 500;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void Main_OnChar(HWND hWnd, TCHAR ch, int cRepeat)
{
	if (activeCellColumn != -1 && activeCellRow != -1)
	{
		if (ch == VK_BACK) {
			if (tableText[activeCellRow][activeCellColumn].length() > 0) {
				tableText[activeCellRow][activeCellColumn].erase(tableText[activeCellRow][activeCellColumn].length() - 1, cRepeat);

			}
		}
		else {
			tableText[activeCellRow][activeCellColumn].insert(tableText[activeCellRow][activeCellColumn].length(), cRepeat, ch);
		}
		if (AdjustRow(hWnd, activeCellRow)) {
			 = 0;
			for (int i = 0; i < tableRow; i++)
			{
				 += deltaHeight[i];
			}
		}
		InvalidateRect(hWnd, NULL, FALSE);
	}
}

void Main_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	activeCellColumn = x / deltaWidth;
	activeCellRow = -1;

	float temp = y;
	while (temp > 0) {
		activeCellRow++;
		temp -= deltaHeight[activeCellRow];
	}

	RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
}

BOOL Main_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	GetWindowRect(hWnd, &rcClient);

	LOGBRUSH br;
	br.lbStyle = BS_SOLID;
	br.lbColor = 0xCFCE9C;
	brushBackground = CreateBrushIndirect(&br);

	br.lbStyle = BS_SOLID;
	br.lbColor = 0xffffff;
	brushWhite = CreateBrushIndirect(&br);

	return 1;
}

/*
BOOL Line(HDC hdc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hdc, x1, y1, NULL);
	return LineTo(hdc, x2, y2);
}

void DrawTable(HDC hdc)
{
	for (int i = 0; i < tableColumn; i++)
	{
		Line(hdc, i * tableWidth, 0, i * tableWidth, rcClient.bottom);
	}

	int d = 0;
	for (int i = 0; i < tableRow; i++)
	{

		if(deltaHeight[i]<tableHeight)
		{
			Line(hdc, 0, i * tableHeight + d, rcClient.right, i * tableHeight+ d);
		}
		else
		{
			Line(hdc, 0, i * tableHeight + d, rcClient.right, i * tableHeight + d);
			d= deltaHeight[i];
		}
	}
}

void Main_OnPaint(HWND hWnd)
{
	HDC hdc, hCmpDC;
	HBITMAP hBmp;
	PAINTSTRUCT ps;

	hdc = BeginPaint(hWnd, &ps);

	hCmpDC = CreateCompatibleDC(hdc);
	hBmp = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom);
	SelectObject(hCmpDC, hBmp);


	FillRect(hCmpDC, &rcClient, brushBackground);
	//
	//FillRect(hCmpDC, &tab, brushWhite);
	for (size_t i = 0; i < tableRow; i++)
	{
		AdjustRow(hWnd, i);
	}

	DrawTable(hCmpDC);
	for (int i = 0; i < tableRow; i++)
	{
		for (int j = 0; j < tableColumn; j++)
		{
			if (!tableText[j][i].empty()) {
				LPCWSTR sw = tableOutText[j][i].c_str();
				SetBkMode(hCmpDC, TRANSPARENT);
				RECT FrectText = {i*tableWidth,j*tableHeight,(i+1) * tableWidth,(j+1) * tableHeight };
				RECT rectText = {0,0,0,0};
				DrawText(hCmpDC, sw, -1, &FrectText, DT_CALCRECT);
				SetBkMode(hCmpDC, TRANSPARENT);
				/*if (FrectText.bottom - FrectText.top > rectText.bottom - rectText.top)
				{
					rectText.bottom = FrectText.bottom;
					rectText.top = FrectText.top;

				}
				DrawText(hCmpDC, sw, -1, &FrectText, DT_LEFT | DT_TOP);
			}

		}
	}


	//Процедура отрисовки текста

	BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, hCmpDC, 0, 0, SRCCOPY);

	DeleteDC(hCmpDC);
	DeleteObject(hBmp);

	EndPaint(hWnd, &ps);
}

void Main_OnWheel(HWND hWnd, int xPos, int yPos, int zDelta, UINT fwKeys)
{

}

void Main_OnSize(HWND hWnd, UINT state, int cx, int cy)
{
	GetClientRect(hWnd, &rcClient);
	tableWidth = rcClient.right / tableColumn;
	tableHeight = rcClient.bottom / tableRow;
	if (state == SIZE_MINIMIZED)
	{

	}
	InvalidateRect(hWnd, NULL, FALSE);
}

void Main_OnDestroy(HWND hWnd)
{
	DeleteObject(brushWhite);
	DeleteObject(brushBackground);

	PostQuitMessage(0);
}

bool AdjustRow(HWND hwnd, int rowIndex)
{
	bool result = false;
	HDC hdc = GetDC(hwnd);
	deltaHeight[rowIndex] = rcClient.left / tableRow;
	for (int i = 0; i < tableColumn; i++) {
		tableOutText[rowIndex][i] = SplitString(hdc, tableText[rowIndex][i], tableWidth);

		RECT rect = { 0, 0, 0, 0 };
		DrawText(hdc, tableOutText[rowIndex][i].c_str(), -1, &rect, DT_CALCRECT);
		if (rect.bottom - rect.top > deltaHeight[rowIndex])
		{
			deltaHeight[rowIndex] = rect.bottom - rect.top;
			result = true;
		}
	}

	ReleaseDC(hwnd, hdc);
	return result;
}


std::wstring SplitString(HDC hdc, std::wstring str, LONG width)
{
	std::wstring result, temp;
	SIZE textSize;
	//SelectFont(hdc, hFont);
	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == '\n')
		{
			temp += str[i];
			result += temp;
			temp = L"";
		}
		else
		{
			temp += str[i];
			GetTextExtentPoint32(hdc, temp.c_str(), temp.length(), &textSize);
			if (textSize.cx > width)
			{
				temp[temp.length() - 1] = '\n';
				result += temp;
				temp = L"";
				temp += str[i];
			}
		}
	}
	result += temp;

	return result;
}*/