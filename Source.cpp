#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>

TCHAR szClassName[] = TEXT("Window");

UINT_PTR CALLBACK fnPrintHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_INITDIALOG)
	{
		SetDlgItemInt(hWnd, 0x480, 1, 0); // 開始ページを指定
		SetDlgItemInt(hWnd, 0x481, 5, 0); // 終了ページを指定
		CheckRadioButton(hWnd, 0x420, 0x422, 0x420); // ラジオボタンの選択を「すべて」に戻す
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hButton;
	switch (msg)
	{
	case WM_CREATE:
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("印刷..."), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		break;
	case WM_SIZE:
		MoveWindow(hButton, 10, 10, 256, 32, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			PRINTDLG pd = { sizeof(PRINTDLG) };
			pd.hwndOwner = hWnd;
			pd.hDevMode = NULL;
			pd.hDevNames = NULL;
			pd.Flags = PD_RETURNDC | PD_NOSELECTION | PD_USEDEVMODECOPIES | PD_ENABLEPRINTHOOK;
			pd.nFromPage = 1;
			pd.nToPage = 10;
			pd.nMinPage = 1;
			pd.nMaxPage = 10;
			pd.lpfnPrintHook = fnPrintHook;
			if (PrintDlg(&pd) == TRUE)
			{
				DOCINFO docinfo = { 0 };
				StartDoc(pd.hDC, &docinfo);
				for (int nPage = pd.nFromPage; nPage <= pd.nToPage; ++nPage)
				{
					StartPage(pd.hDC);
					TCHAR szText[1024];
					wsprintf(szText, TEXT("%d / %d"), nPage, pd.nMaxPage);
					TextOut(pd.hDC, 100, 100, szText, lstrlen(szText));
					EndPage(pd.hDC);
				}
				EndDoc(pd.hDC);
				DeleteDC(pd.hDC);
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("印刷サンプル"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
