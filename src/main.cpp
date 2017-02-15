#include <iostream> 
#include <Windows.h>

const int WindowWidth = 1280;
const int WindowHeight = 720;

LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
	switch (Message) {
		case (WM_DESTROY): {
			PostQuitMessage(0);
		} break;
		default: return DefWindowProc(Window, Message, wParam, lParam);
	}
	return result;
}


int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowType) {
	WNDCLASS WindowClass{};
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = Instance;
	WindowClass.lpszClassName = "BreakoutGameClass";
	RegisterClass(&WindowClass);
	HWND Window =
		CreateWindow("BreakoutGameClass", "Breakout", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT,
			CW_USEDEFAULT, WindowWidth, WindowHeight, NULL, NULL, Instance, NULL);

	while (true) {
		MSG Message;
		while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)) {
			switch (Message.message) {
				case (WM_KEYDOWN): {
					auto dx = 0;
					auto dy = 0;
					switch (Message.wParam) {
						case (VK_LEFT): {
							dx -= 1;
						} break;
						case (VK_RIGHT): {
							dx += 1;
						} break;
						case (VK_UP): {
							dy -= 1;
						} break;
						case (VK_DOWN): {
							dy += 1;
						} break;
					}
					RECT WindowRect;
					GetWindowRect(Window, &WindowRect);
					SetWindowPos(Window, HWND_TOP, WindowRect.left + dx, WindowRect.top + dy,
						0, 0, SWP_NOZORDER | SWP_NOSIZE);
				} break;
				default: {
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
			}
		}
		if (Message.message == WM_QUIT) {
			break;
		}
	}
	return 0;
}
