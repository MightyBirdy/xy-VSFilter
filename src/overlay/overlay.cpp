// overlay.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "overlay.h"
#include "overlay_func.h"


#define MAX_LOADSTRING 100

FILE *mylog = NULL;
const char logFile[] = "\\Desktop\\xy.log";

ISubRenderFrame *frame = NULL;
std::vector<tsubdata> subs;

RECT clipRect;

// Global Variables:
HINSTANCE hInst;								// current instance
HWND hWnd;
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void trigger_paint()
{
	//RedrawWindow(hWnd, NULL, NULL, RDW_INTERNALPAINT);
	InvalidateRect(hWnd, NULL, TRUE);
}

void clear_screen()
{
	subs.clear();
	frame = NULL();
	trigger_paint();
}

DWORD WINAPI init_overlay(LPVOID lpParam)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	MSG msg;
	HACCEL hAccelTable;

#ifdef DEBUG
	char* docdir = getenv("USERPROFILE");
	char logFileName[256];
	sprintf(&logFileName[0], "%s%s", docdir, logFile);
	mylog = fopen(logFileName, "a");
#endif


	// Initialize global strings
	wcscpy(szTitle, _T("overlay"));
	wcscpy(szWindowClass, _T("OVERLAY"));
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance))
	{
		return FALSE;
	}

	//This is probably failing... oh well
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OVERLAY));

	// Main message loop:
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon = NULL;// LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OVERLAY));
	wcex.hCursor = NULL;// LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = NULL;// MAKEINTRESOURCE(IDC_OVERLAY);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm = NULL;// LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      0, 0, 100, 100, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }   

	ShowWindow(hWnd, SW_SHOWNA);
	UpdateWindow(hWnd);
  
	SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) ^ (WS_CAPTION | WS_OVERLAPPEDWINDOW));

	int x = GetSystemMetrics(SM_CXSCREEN);
	int y = GetSystemMetrics(SM_CYSCREEN);

	// Set WS_EX_LAYERED on this window 
	SetWindowLong(hWnd, GWL_EXSTYLE,
		GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_COMPOSITED | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST);
	// Make this window alpha
	SetLayeredWindowAttributes(hWnd, RGB(0,0,0), 255*70/100, LWA_COLORKEY);

	BOOL ret = SetWindowPos(hWnd,
		HWND_TOPMOST,
		0,
		0,
		x,
		y,
		SWP_SHOWWINDOW);
	SetFocus(hWnd);
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
   

   SetCapture(hWnd);
   ShowCursor(false);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc = NULL;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		hdc = BeginPaint(hWnd, &ps);
		if (!hdc)
		{
#ifdef DEBUG
			int err = GetLastError();
			fprintf(mylog, "Begin Paint returned null... %d\n", err);
			fflush(mylog);
#endif
			break;
		}

		for (int idx = 0; idx < subs.size(); idx++)
		{

			POINT position = subs[idx].position;
			SIZE size = subs[idx].size;
			LPCVOID pixels = subs[idx].pixels;
			int pitch = subs[idx].pitch;

			if (pixels)
			{
				HBITMAP       hOldBitmap;
				BITMAP        bm;

				const int offset = (pitch - size.cx * 4) / -4;

				BITMAPINFO bitmapinfo = { 0 };
				bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bitmapinfo.bmiHeader.biWidth = size.cx - offset;
				bitmapinfo.bmiHeader.biHeight = size.cy*-1;
				bitmapinfo.bmiHeader.biPlanes = 1;
				bitmapinfo.bmiHeader.biCompression = BI_RGB;
				bitmapinfo.bmiHeader.biBitCount = 32;

				unsigned char* vImageBuff = NULL;
				HDC hdcMem = CreateCompatibleDC(hdc);
				if (!hdcMem) {
#ifdef DEBUG
					int err = GetLastError();
					fprintf(mylog, "CreateCompatibleDC returned null... %d\n", err);
					fflush(mylog);
#endif
				}
				else
				{
					HBITMAP bitmap = CreateDIBSection(hdcMem, &bitmapinfo, DIB_RGB_COLORS, (void **)&vImageBuff, NULL, 0);

					if (vImageBuff) {
						memcpy(vImageBuff, (unsigned char *)(pixels), size.cy*(size.cx - offset) * 4);
						GetObject(bitmap, sizeof(BITMAP), &bm);
						hOldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);
						RealizePalette(hdc);

						int x = GetDeviceCaps(hdc, HORZRES);
						int y = GetDeviceCaps(hdc, VERTRES);

						double xscale = (double)x / clipRect.right;
						double yscale = (double)y / clipRect.bottom;

						int xpos = position.x;
						int ypos = position.y;
						
						double outAspect = (double)x / y;
						double inAspect = (double)clipRect.right / clipRect.bottom;
						
						double extraXScale = 1.0;
						double extraYScale = 1.0;

						//eg: 4:3 -> 16:9
						if (inAspect < outAspect)
						{
							double xp = (double)clipRect.right / clipRect.bottom * y;
							xpos *= xp / clipRect.right;
							xpos += (x - xp) / 2;

							ypos *= yscale;
							extraXScale = outAspect / inAspect;
						}
						//eg: 16:9 -> 4:3
						else if (outAspect > inAspect)
						{
							double yp = (double)x / (clipRect.right / clipRect.bottom);
							ypos *= yp / clipRect.bottom;
							ypos += (y - yp) / 2;
							xpos *= xscale;
							extraYScale = inAspect / outAspect;
						}
						//aspect ratios match so scaling is straightforward
						else
						{
							xpos *= xscale;
							ypos *= yscale;
						}
						

						StretchBlt(hdc,
							xpos, ypos, //out position
							(bm.bmWidth + offset)*xscale / (extraXScale), bm.bmHeight*yscale / (extraYScale),  //out size
							hdcMem, 
							0, 0, //in position
							bm.bmWidth + offset, bm.bmHeight, //in size
							SRCCOPY);

						SelectObject(hdcMem, hOldBitmap);
						DeleteObject(bitmap);
						DeleteObject(hOldBitmap);
						DeleteDC(hdcMem);
					}
#ifdef DEBUG
					else
					{
						int err = GetLastError();
						fprintf(mylog, "null vImageBuff... %d\n", err);
						fflush(mylog);
					}
#endif
				}				

			}
		}

		subs.clear();

		if (frame)
		{
			//frame->Release();
			frame = NULL;
		}

		DeleteDC(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_MOUSEMOVE:
		//fprintf(stdout, "Mouse move [%d][%d]\n", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		SetForegroundWindow(hWnd);
		break;

	case WM_LBUTTONDOWN:
		printf("Mouse click\n");
		SetForegroundWindow(hWnd);
		break;

	case WM_NCACTIVATE:
		return false;

	case WM_ACTIVATEAPP:
	{
		BOOL wActive = (bool)wParam;

		if (wActive == false)
			return 0;
		else
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	
	}

	return (INT_PTR)FALSE;
}
