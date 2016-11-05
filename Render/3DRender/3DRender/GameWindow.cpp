#include "GameWindow.h"

GameWindow::GameWindow(const string name_t,const string title_t,int width,int height):name(name_t),title(title_t),WIDTH(width),HEIGHT(height)
{
	//1.����������
	WNDCLASSEX wndClass = {};
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = name.c_str();

	//2.ע�ᴰ����
	assert(RegisterClassEx(&wndClass));

	//3.��������
	hwnd = CreateWindow(name.c_str(),title.c_str(),WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,WIDTH,HEIGHT,NULL,NULL,wndClass.hInstance,NULL);

	//4.������С���ƶ�����ʾ������
	RECT window_rect = {0,0,WIDTH,HEIGHT};
	AdjustWindowRectEx(&window_rect, GetWindowStyle(hwnd), GetMenu(hwnd) != NULL, GetWindowExStyle(hwnd));
	MoveWindow(hwnd,300,150,window_rect.right-window_rect.left, window_rect.bottom-window_rect.top,false);
	ShowWindow(hwnd,SW_NORMAL);
	UpdateWindow(hwnd);

	GameManager::Instance().game_start(hwnd,WIDTH,HEIGHT);
	message_dispatch();
}

void GameWindow::message_dispatch()
{
	//ʱ���ʼ��
	DWORD curTime = GetTickCount();
	DWORD preTime = GetTickCount();
	//2.��Ϣѭ��
	MSG msg = {0};
	while (msg.message!=WM_QUIT)
	{
		//��ȡ��Ϣ
		if (PeekMessage(&msg,0,NULL,NULL,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			curTime = GetTickCount();
			if (curTime-preTime>30)
			{
				preTime = curTime;
				GameManager::Instance().game_update();
			}
		}
	}
}

LRESULT CALLBACK GameWindow::WndProc(HWND hwnd,UINT message,WPARAM wparam,LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		GameManager::Instance().game_end();
		break;
	case WM_KEYDOWN:
		if (wparam==VK_ESCAPE)
		{
			DestroyWindow(hwnd);
			GameManager::Instance().game_end();
		}
		break;
	default:
		return DefWindowProc(hwnd,message,wparam,lParam);
	}
	return 0;
}



GameWindow::~GameWindow()
{
	//5.ע��������
	UnregisterClass(name.c_str(),GetModuleHandle(NULL));
}
