// ProgTaskBar.cpp :
//Adding an icon system tray Win32 C++
//This file part of a program to transfer data between  modules running in DOS and Windows from RS232
//Module under windows starts,initialized com port and perform set operations(such as printing a receipt or report).
//This file creates an icon in the system tray. Programata no need for dialog.Starts and an icon appears in the system line with menu.
//We need the following two functions : minimization and recovery.

#include "stdafx.h"
#include "ProgTaskBar.h"
#include <Commdlg.h>
#include <Winspool.h>
#include <Shellapi.h>
#include <conio.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

DCB m_dcb;
HANDLE hComm;
//menu
NOTIFYICONDATA g_notifyIconData;
UINT WM_TASKBARCREATED = 0;
HWND g_hwnd;
HMENU g_menu;
HDC hdc, hdcMem;
int cxsize = 0, cxpage = 0;
int cysize = 0, cypage = 0;
int bxWidth, bxHeight, flag = 0;
#define stringcopy wcscpy_s
#define ID_TRAY_APP_ICON                5000
#define ID_TRAY_EXIT_CONTEXT_MENU_ITEM  3000
#define ID_TRAY_PRINT_CONTEXT_MENU_ITEM  3002
#define WM_TRAYICON ( WM_USER + 1 )

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

//Add the icon the system tray and hide the window
void Minimize()
{
	// add the icon to the system tray
	Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);

	//and hide the main window
	ShowWindow(g_hwnd, SW_HIDE);
}

//Remove the icon and show the window
void Restore()
{
	// Remove the icon from the system tray
	Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);

	// ..and show the window
	ShowWindow(g_hwnd, SW_SHOW);
}
//Init icon
void InitNotifyIconData()
{
	memset(&g_notifyIconData, 0, sizeof(NOTIFYICONDATA));
	g_notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	g_notifyIconData.hWnd = g_hwnd;
	g_notifyIconData.uID = ID_TRAY_APP_ICON;
	// Set up flags.
	g_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;     

	g_notifyIconData.uCallbackMessage = WM_TRAYICON; //this message must be handled in hwnd's window procedure. more info below.

	// Load the icon.  Be sure to include an icon "arrow_refresh.ico"
	g_notifyIconData.hIcon = (HICON)LoadImage(NULL, TEXT("arrow_refresh.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	// set the tooltip text.  
	stringcopy(g_notifyIconData.szTip, TEXT("Connect"));
}



int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
	
	TCHAR className[] = TEXT("tray icon");
	WM_TASKBARCREATED = RegisterWindowMessageA("CreatedTaskBar");
	// Initialize global strings
	hInst = hInstance; // Store instance handle in our global variable
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PROGTASKBAR, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROGTASKBAR));
	
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	if (!IsWindowVisible(g_hwnd))
		Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
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

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROGTASKBAR));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PROGTASKBAR);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   else
	   g_hwnd = hWnd;
   InitNotifyIconData();
   Minimize();//add the icon
   UpdateWindow(hWnd);

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
	HDC hdc;
	if (message == WM_TASKBARCREATED && !IsWindowVisible(g_hwnd))
	{
		Minimize();
		return 0;
	}
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
		case IDM_PRINT://create raport.txt file and print

			PRINTDLG pd;
			
			memset(&pd, 0, sizeof(PRINTDLG));
			pd.lStructSize = sizeof(PRINTDLG);
			pd.hwndOwner = hWnd;
			pd.Flags = PD_RETURNDC;
			pd.hDevMode = NULL;//hInst.hDevMode;
			pd.hDevNames = NULL;//psd.hDevNames;

			if (PrintDlg(&pd))
			{
				WORD     wCopies;
				
				RECT     rect;
				DOCINFO  docInfo;
				FILE *file;
				errno_t err;
								
				LPDEVMODE lpDevMode = (LPDEVMODE)GlobalLock(pd.hDevMode);
				LPCWSTR lpszString = (LPCWSTR)L"This is a test";

				memset(&docInfo	, 0, sizeof(DOCINFO));
				GetClientRect(hWnd, &rect);
				docInfo.cbSize = sizeof(DOCINFO);
				
				docInfo.lpszDocName = (LPCWSTR)L"report.txt";
				
				err = fopen_s(&file, "report.txt", "w+");

				if (StartDoc(pd.hDC, &docInfo) > 0)
				{
					for (wCopies = 0; wCopies < pd.nCopies; wCopies++)
					{
						StartPage(pd.hDC);
						TextOut(pd.hDC, 100, 100, LPCWSTR(lpszString), lstrlen(lpszString));
						EndPage(pd.hDC);
						fclose(file);
					}
					EndDoc(pd.hDC);
				}
				GlobalUnlock(pd.hDevMode);
				DeleteDC(pd.hDC);
			}
			else
			if (CommDlgExtendedError() == 0x1008)
				MessageBox(hWnd, (LPCWSTR)L"No default printer selected.", (LPCWSTR)L"Print", MB_OK | MB_ICONSTOP);

			break;//IDM_PRINT
		
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		//create menu - if push left button
	case WM_CREATE:// create the menu once.
		HBITMAP hbmpPie1, hbmpPie2;
		hbmpPie1 = (HBITMAP)LoadImage(NULL, TEXT("gnome_session_logout.bmp"), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE);
		hbmpPie2 = (HBITMAP)LoadImage(NULL, TEXT("green_print.bmp"), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
		g_menu = CreatePopupMenu();
		AppendMenu(g_menu, MF_STRING | MF_BYPOSITION | MF_POPUP, ID_TRAY_PRINT_CONTEXT_MENU_ITEM, TEXT("  Print Report"));
		SetMenuItemBitmaps(g_menu, ID_TRAY_PRINT_CONTEXT_MENU_ITEM, MF_BYCOMMAND, hbmpPie2, hbmpPie2);
		AppendMenu(g_menu, MF_SEPARATOR, NULL, NULL);
		InsertMenu(g_menu, -1, MF_STRING | MF_BYPOSITION | MF_POPUP, ID_TRAY_EXIT_CONTEXT_MENU_ITEM, _T("  Exit"));
		SetMenuItemBitmaps(g_menu, ID_TRAY_EXIT_CONTEXT_MENU_ITEM, MF_BITMAP | MF_BYCOMMAND, hbmpPie1, hbmpPie1);
		break;

	case WM_SYSCOMMAND:
		switch (wParam & 0xfff0)
		{
			case SC_MINIMIZE:
			case SC_CLOSE:  // redundant to WM_CLOSE, it appears
				Minimize();
				return 0;
			break;
		}

	case WM_TRAYICON:
	{
		if (lParam == WM_RBUTTONDOWN) // I'm using WM_RBUTTONDOWN here because
		{
			printf("Mmm.  Let's get contextual.  I'm showing you my context menu.\n");
			POINT curPoint;
			GetCursorPos(&curPoint);
			SetForegroundWindow(hWnd);
			printf("calling track\n");
			UINT clicked = TrackPopupMenu(

				g_menu,
				TPM_RETURNCMD | TPM_NONOTIFY, // don't send me WM_COMMAND messages about this window, instead return the identifier of the clicked menu item
				curPoint.x,
				curPoint.y,
				0,
				hWnd,
				NULL
				);

			if (clicked == ID_TRAY_EXIT_CONTEXT_MENU_ITEM)
			{
				// quit the application.
				printf("I have posted the quit message, biatch\n");
				PostQuitMessage(0);
			}
			if (clicked == ID_TRAY_PRINT_CONTEXT_MENU_ITEM)
			{
				
				DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORTBOX), hWnd, About);
				SendMessage(g_hwnd, WM_COMMAND, IDM_PRINT, lParam);
				return 0;
			}
		}
	}
	break;

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
