#include <windows.h>
#include <windowsx.h>
#include <string>


const int windowWidth = 1000;
const int windowHeight = 600;

const int numberRows = 7;
const int numberColumns = 7;

int GlobalYOffset = 0;
int tableHeight;
float cellHeight[numberRows];
float cellWidth;
SIZE wndSize;
int activeRow = 0, activeColumn = 0;

std::wstring text[numberRows][numberColumns];
std::wstring outText[numberRows][numberColumns];

HBRUSH brushBackground;
HBRUSH brushHallow;
HPEN penBlack;
HPEN penRed;

LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);

BOOL Main_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void Main_OnPaint(HWND hwnd);
void Main_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
void Main_OnChar(HWND hwnd, TCHAR ch, int cRepeat);
void Main_OnMouseWheel(HWND hwnd, int xPos, int yPos, int zDelta, UINT fwKeys);
void Main_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
void Main_OnSize(HWND hwnd, UINT state, int cx, int cy);
void Main_OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo);
void Main_OnDestroy(HWND hwnd);

std::wstring SplitString(HDC hdc, std::wstring data, int width);
void ScrollOffsetCorrection();
void calculateTable(HWND hwnd);
bool calculateRow(HWND hwnd, int rowIndex);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd;
	MSG lpMsg;
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = (LPCWSTR)L"SP2";

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, (LPCWSTR)L"ERROR REGISTRATION WINDOW CLASS", (LPCWSTR)L"Œ¯Ë·Í‡", MB_OK);
		return 0;
	}

	hWnd = CreateWindow((LPCWSTR)L"SP2", L"WinAPI", WS_OVERLAPPEDWINDOW, 50, 50, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&lpMsg, NULL, 0, 0))
	{
		TranslateMessage(&lpMsg);
		DispatchMessage(&lpMsg);
	}
	return (lpMsg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	switch (messg)
	{
		HANDLE_MSG(hWnd, WM_CREATE, Main_OnCreate);
		HANDLE_MSG(hWnd, WM_SIZE, Main_OnSize);
		HANDLE_MSG(hWnd, WM_PAINT, Main_OnPaint);
		HANDLE_MSG(hWnd, WM_GETMINMAXINFO, Main_OnGetMinMaxInfo);
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, Main_OnLButtonDown);
		HANDLE_MSG(hWnd, WM_KEYDOWN, Main_OnKey);
		HANDLE_MSG(hWnd, WM_CHAR, Main_OnChar);
		HANDLE_MSG(hWnd, WM_MOUSEWHEEL, Main_OnMouseWheel);
		HANDLE_MSG(hWnd, WM_DESTROY, Main_OnDestroy);

	default:
		return (DefWindowProc(hWnd, messg, wParam, lParam));
	}
	return (0);
}


BOOL Main_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	LOGBRUSH br;
	br.lbStyle = BS_SOLID;
	br.lbColor = 0xFFFFFF;
	brushBackground = CreateBrushIndirect(&br);

	brushHallow = GetStockBrush(HOLLOW_BRUSH);
	penBlack = GetStockPen(BLACK_PEN);
	penRed = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));

	return true;
}

void Main_OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	HDC hdcBuf = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, wndSize.cx, wndSize.cy);

	SelectObject(hdcBuf, hBitmap);
	RECT rect = { 0, 0, wndSize.cx, wndSize.cy };

	FillRect(hdcBuf, &rect, brushBackground);

	SelectBrush(hdcBuf, brushHallow);
	SelectPen(hdcBuf, penBlack);

	SetTextColor(hdcBuf, 0x000000);
	SetBkMode(hdcBuf, TRANSPARENT);

	float cellOffset = 0;
	for (int i = 0; i < numberRows; i++)
	{
		for (int j = 0; j < numberColumns; j++)
		{
			RECT rect = { j * cellWidth, GlobalYOffset + cellOffset, (j + 1) * cellWidth, GlobalYOffset + cellOffset + cellHeight[i] };

			DrawText(hdcBuf, (LPWSTR)outText[i][j].c_str(), -1, &rect, DT_LEFT | DT_TOP);

			if (i == activeRow && j == activeColumn) {
				SelectPen(hdcBuf, penRed);
				Rectangle(hdcBuf, rect.left, rect.top, rect.right, rect.bottom);
				SelectPen(hdcBuf, penBlack);
			}
			else
			{
				Rectangle(hdcBuf, rect.left, rect.top, rect.right, rect.bottom);
				SelectPen(hdcBuf, penBlack);
			}
		}
		cellOffset += cellHeight[i];
	}

	BitBlt(hdc, 0, 0, wndSize.cx, wndSize.cy, hdcBuf, 0, 0, SRCCOPY);

	DeleteObject(hBitmap);
	DeleteDC(hdcBuf);

	EndPaint(hwnd, &ps);
}

void Main_OnMouseWheel(HWND hwnd, int xPos, int yPos, int zDelta, UINT fwKeys)
{
	GlobalYOffset += (zDelta / WHEEL_DELTA) * 0.1 * wndSize.cy;
	ScrollOffsetCorrection();
	RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
}

void Main_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	activeColumn = x / cellWidth;
	activeRow = -1;

	float temp = y - GlobalYOffset;
	while (temp > 0) {
		activeRow++;
		temp -= cellHeight[activeRow];
	}

	RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
}

void Main_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	if (state != SIZE_MINIMIZED)
	{
		int oldWndHeight = wndSize.cy;
		wndSize.cx = cx;
		wndSize.cy = cy;
		cellWidth = (float)wndSize.cx / numberColumns;

		float oldTableHeight = tableHeight;
		calculateTable(hwnd);
		if (oldTableHeight != 0) {
			ScrollOffsetCorrection();
		}
	}
}

void Main_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags) {
	switch (vk)
	{
	case VK_ESCAPE:
		activeColumn = -1;
		activeRow = -1;
		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
		break;
	default:
		break;
	}
}

void Main_OnChar(HWND hwnd, TCHAR ch, int cRepeat) {
	if (activeColumn != -1 && activeRow != -1)
	{
		if (ch == VK_BACK) {
			if (text[activeRow][activeColumn].length() > 0) {
				text[activeRow][activeColumn].erase(text[activeRow][activeColumn].length() - 1, cRepeat);
			}
		}
		else {
			text[activeRow][activeColumn].insert(text[activeRow][activeColumn].length(), cRepeat, ch);
		}

		if (calculateRow(hwnd, activeRow)) {
			tableHeight = 0;
			for (int i = 0; i < numberRows; i++)
			{
				tableHeight += cellHeight[i];
			}
		}

		ScrollOffsetCorrection();
		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
	}
}

void Main_OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo)
{
	lpMinMaxInfo->ptMinTrackSize.x = 800;
	lpMinMaxInfo->ptMinTrackSize.y = 800;
}

void Main_OnDestroy(HWND hwnd)
{
	DeleteObject(brushBackground);
	DeleteObject(brushHallow);
	DeleteObject(penBlack);
	DeleteObject(penRed);

	PostQuitMessage(0);
}


std::wstring SplitString(HDC hdc, std::wstring str, int width)
{
	std::wstring result, temp;
	SIZE textSize;

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
}

bool calculateRow(HWND hwnd, int rowIndex)
{
	bool result = false;
	HDC hdc = GetDC(hwnd);
	cellHeight[rowIndex] = (float)wndSize.cy / numberRows;
	for (int i = 0; i < numberColumns; i++) {
		outText[rowIndex][i] = SplitString(hdc, text[rowIndex][i], cellWidth);

		RECT rect = { 0, 0, 0, 0 };
		DrawText(hdc, outText[rowIndex][i].c_str(), -1, &rect, DT_CALCRECT);
		if (rect.bottom - rect.top > cellHeight[rowIndex])
		{
			cellHeight[rowIndex] = rect.bottom - rect.top;
			result = true;
		}
	}

	ReleaseDC(hwnd, hdc);
	return result;
}

void calculateTable(HWND hwnd) {
	tableHeight = 0;
	for (int i = 0; i < numberRows; i++) {
		calculateRow(hwnd, i);
		tableHeight += cellHeight[i];
	}
}

void ScrollOffsetCorrection()
{
	if (GlobalYOffset > 0) {
		GlobalYOffset = 0;
	}
	if (GlobalYOffset < -(tableHeight - wndSize.cy)) {
		GlobalYOffset = -(tableHeight - wndSize.cy);
	}
}