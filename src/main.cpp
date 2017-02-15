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

	int keystate = 0;
	while (true) {
		MSG Message;
		while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)) {
			switch (Message.message) {
				case (WM_KEYDOWN): {
					switch (Message.wParam) {
						case (VK_LEFT): {
							keystate |= 1 << 3;
						} break;
						case (VK_RIGHT): {
							keystate |= 1 << 2;
						} break;
						case (VK_UP): {
							keystate |= 1 << 1;
						} break;
						case (VK_DOWN): {
							keystate |= 1;
						} break;
					}
				} break;
				case (WM_KEYUP): {
					switch (Message.wParam) {
						case (VK_LEFT): {
							keystate &= ~(1 << 3);
						} break;
						case (VK_RIGHT): {
							keystate &= ~(1 << 2);
						} break;
						case (VK_UP): {
							keystate &= ~(1 << 1);
						} break;
						case (VK_DOWN): {
							keystate &= ~(1);
						} break;
					}
				} break;
				default: {
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
			}
		}
		float dx = 0, dy = 0;
		if ((keystate & 1 << 3)) dx -= 1;
		if ((keystate & 1 << 2)) dx += 1;
		if ((keystate & 1 << 1)) dy -= 1;
		if ((keystate & 1 << 0)) dy += 1;
		RECT WindowRect;
		GetWindowRect(Window, &WindowRect);
		SetWindowPos(Window, HWND_TOP, WindowRect.left + dx, WindowRect.top + dy,
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
		if (Message.message == WM_QUIT) {
			break;
		}
	}
	return 0;
}
