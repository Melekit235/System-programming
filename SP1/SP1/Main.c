#include <windows.h>
#include <windowsx.h>
#include <tchar.h>


RECT rcClient;

int x, y;
int a = 1;
int speedX = 0;
int speedY = 0;

int idTimer = -1;
HBITMAP hBitmap = NULL;
HINSTANCE hI;
POINT bmpSize;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL Main_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
void Main_OnPaint(HWND hWnd);
void Main_OnWheel(HWND hWnd, int xPos, int yPos, int zDelta, UINT fwKeys);
void Main_OnSize(HWND hWnd, UINT state, int cx, int cy);
void Main_OnTimer(HWND hWnd, UINT id);
void Main_OnDestroy(HWND hWnd);

void DrawSprite(HDC hdc, HBITMAP* hSourceBitmap, HBITMAP* hMaskBitmap);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)

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
	wcex.lpszClassName = _T("SP1Class");
	wcex.hIconSm = wcex.hIcon;

	RegisterClassEx(&wcex);

	hWnd = CreateWindow(_T("SP1Class"), _T("Hello"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);


	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT fn)
{
	switch (message)
	{
		HANDLE_MSG(hWnd, WM_PAINT, Main_OnPaint);
		HANDLE_MSG(hWnd, WM_CREATE, Main_OnCreate);
		HANDLE_MSG(hWnd, WM_SIZE, Main_OnSize);
		HANDLE_MSG(hWnd, WM_MOUSEWHEEL, Main_OnWheel);
		HANDLE_MSG(hWnd, WM_TIMER, Main_OnTimer);		
		HANDLE_MSG(hWnd, WM_DESTROY, Main_OnDestroy);
	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = 300;
		lpMMI->ptMinTrackSize.y = 300;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

BOOL Main_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	hBitmap = LoadImage(hI, L"bmp.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	BITMAP bmp;
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bmp);
	bmpSize.x = bmp.bmWidth;
	bmpSize.y = bmp.bmHeight;

	SetTimer(hWnd, idTimer = 1, 10, NULL);

	return 1;
}

void Main_OnPaint(HWND hWnd) 
{
	HDC hdc, hCmpDC;
	HBITMAP hBmp;
	PAINTSTRUCT ps;

	hdc = BeginPaint(hWnd, &ps);

	hCmpDC = CreateCompatibleDC(hdc);
	hBmp = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom );
	SelectObject(hCmpDC, hBmp);

	LOGBRUSH br;
	br.lbStyle = BS_SOLID;
	br.lbColor = 0x00ffff
		;
	HBRUSH brush;
	brush = CreateBrushIndirect(&br);
	FillRect(hCmpDC,&rcClient, brush);
	DeleteObject(brush);

	DrawSprite(hCmpDC, &hBitmap, &hBitmap);

	//SetStretchBltMode(hdc, COLORONCOLOR);
	BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, hCmpDC, 0, 0, SRCCOPY);

	DeleteDC(hCmpDC);
	DeleteObject(hBmp);

	EndPaint(hWnd, &ps);
}

void Main_OnTimer(HWND hWnd, UINT id)
{
	if (GetKeyState(VK_LEFT) < 0)
	{
		speedX -= a;
	} 
	else if (GetKeyState(VK_RIGHT) < 0)
	{
		speedX += a;
	}
	else
	{
		if (speedX != 0) 
		{
			if (speedX > 0)
			{
				speedX -= a;
			}
			else
			{
				speedX += a;
			}
		}
	}

	if (GetKeyState(VK_UP) < 0)
	{
		speedY -= a;
	}
	else if (GetKeyState(VK_DOWN) < 0)
	{
		speedY += a;
	}
	else
	{
		if (speedY != 0)
		{
			if (speedY > 0)
			{
				speedY -= a;
			}
			else
			{
				speedY += a;
			}
		}
	}


	x += speedX;
	y += speedY;
	if (x < rcClient.left)
	{
		speedX = speedX * -1 / 2;
		x = rcClient.left;
	}
	if (x + bmpSize.x > rcClient.right)
	{
		speedX = speedX * -1 / 2;
		x = rcClient.right - bmpSize.x;
	}
	if (y < rcClient.top)
	{
		speedY = speedY * -1 / 2;
		y = rcClient.top;
	}
	if (y + bmpSize.y > rcClient.bottom)
	{
		speedY = speedY * -1 / 2;
		y = rcClient.bottom - bmpSize.y;
	}

	InvalidateRect(hWnd, NULL, FALSE);
}

void Main_OnWheel(HWND hWnd, int xPos, int yPos, int zDelta, UINT fwKeys)
{
	if (GetKeyState(VK_LSHIFT) & 0x8000)
	{
		if (zDelta > 0)
		{
			speedX += 4 * a;
		}
		else if (zDelta < 0)
		{
			speedX -= 4 * a;
		}
	}
	else
	{
		if (zDelta > 0)
		{
			speedY -= 4 * a;
		}
		else if (zDelta < 0)
		{
			speedY += 4 * a;
		}
	}
}

void Main_OnSize(HWND hWnd, UINT state, int cx, int cy)
{
	if (idTimer == -1)
	{
		SetTimer(hWnd, idTimer = 1, 10, NULL);
	}

	GetClientRect(hWnd, &rcClient);
	if(state == SIZE_MINIMIZED)
	{
		KillTimer(hWnd, 1);
		idTimer = -1;
	}
}

void Main_OnDestroy(HWND hWnd)
{
	DeleteObject(hBitmap);
	KillTimer(hWnd, 1);
	PostQuitMessage(0);
}

void DrawSprite(HDC hdc, HBITMAP* hSourceBitmap, HBITMAP* hMaskBitmap) {
	HDC hdcMask = CreateCompatibleDC(hdc);
	HDC hdcSource = CreateCompatibleDC(hdc);
	HDC hdcInvertedMask = CreateCompatibleDC(hdc);
	HBITMAP hBlankBitmap = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom);

	SelectObject(hdcMask, *hMaskBitmap);
	SelectObject(hdcSource, *hSourceBitmap);
	SelectObject(hdcInvertedMask, hBlankBitmap);

	BitBlt(hdcInvertedMask, 0, 0, rcClient.right, rcClient.bottom, hdcMask, 0, 0, NOTSRCCOPY);
	BitBlt(hdcSource, 0, 0, rcClient.right, rcClient.bottom, hdcInvertedMask, 0, 0, SRCAND);
	BitBlt(hdc, x, y, x+bmpSize.x, x + bmpSize.y, hdcMask, 0, 0, SRCAND);
	BitBlt(hdc, x, y, x + bmpSize.x, x + bmpSize.y, hdcSource, 0, 0, SRCPAINT);

	DeleteDC(hdcMask);
	DeleteDC(hdcSource);
	DeleteDC(hdcInvertedMask);
	DeleteObject(hMaskBitmap);
}