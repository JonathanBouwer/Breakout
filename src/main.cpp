#include <Windows.h>
#include <stdint.h>

const int WindowWidth = 1280;
const int WindowHeight = 720;
const int GameWidth = 1152;
const int GameHeight = 648;

LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
	switch (Message) {
		case (WM_DESTROY): {
			PostQuitMessage(0);
		} break;
		case WM_PAINT: {
			PAINTSTRUCT paintStruct{};
			paintStruct.hdc = GetDC(Window);
			RECT clientRect;
			GetClientRect(Window, &clientRect);
			paintStruct.rcPaint = clientRect;
			BeginPaint(Window, &paintStruct);
			EndPaint(Window, &paintStruct);
		} break;
		default: return DefWindowProc(Window, Message, wParam, lParam);
	}
	return result;
}

void RenderGame(HWND Window, RECT *ClientRect, VOID *Bitmap, BITMAPINFO *BitmapInfo) {
	HDC hdc = GetDC(Window);
	int ret = StretchDIBits(hdc,
		ClientRect->left, ClientRect->top, ClientRect->right, ClientRect->bottom,
		ClientRect->left, ClientRect->top, ClientRect->right, ClientRect->bottom,
		Bitmap, BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(Window, hdc);
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowType) {
	WNDCLASS WindowClass{};
	WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = Instance;
	WindowClass.lpszClassName = "BreakoutGameClass";
	RegisterClass(&WindowClass);
	DWORD WindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
	HWND Window =
		CreateWindow("BreakoutGameClass", "Breakout", WindowStyle, CW_USEDEFAULT,
			CW_USEDEFAULT, WindowWidth, WindowHeight, NULL, NULL, Instance, NULL);
	
	RECT ClientRect;
	GetClientRect(Window, &ClientRect);
	BITMAPINFO BitmapInfo{};
	BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BitmapInfo.bmiHeader.biWidth = ClientRect.right;
	BitmapInfo.bmiHeader.biHeight = ClientRect.bottom;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;

	int32_t BitmapSize = sizeof(DWORD) * ClientRect.right * ClientRect.bottom;
	VOID *Bitmap = VirtualAlloc(NULL, BitmapSize, MEM_COMMIT, PAGE_READWRITE);

	int keystate = 0;
	while (true) {
		MSG Message;
		int count = 0;
		while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)>0) {
			switch (Message.message) {
				case WM_KEYDOWN: {
					switch (Message.wParam) {
						case VK_LEFT: {
							keystate |= 1 << 3;
						} break;
						case VK_RIGHT: {
							keystate |= 1 << 2;
						} break;
						case VK_UP: {
							keystate |= 1 << 1;
						} break;
						case VK_DOWN: {
							keystate |= 1;
						} break;
					}
				} break;
				case WM_KEYUP: {
					switch (Message.wParam) {
						case VK_LEFT: {
							keystate &= ~(1 << 3);
						} break;
						case VK_RIGHT: {
							keystate &= ~(1 << 2);
						} break;
						case VK_UP: {
							keystate &= ~(1 << 1);
						} break;
						case VK_DOWN: {
							keystate &= ~(1);
						} break;
					}
				} break;
				default: {
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
			}
			++count;
		}
		for(int i=0; i<ClientRect.bottom; i++) {
			for (int j = 0; j<ClientRect.right; j++) {
				int32_t *Pixel = (int32_t *) (Bitmap);
				Pixel = Pixel + i*ClientRect.right + j;
				int32_t r = (keystate & 1 << 3) * 0xff;
				int32_t g = (keystate & 1 << 2) * 0xff;
				int32_t b = (keystate & 1 << 0) * 0xff;
				*Pixel = (r<<16) + (g<<8) + (b<<0);
			}
		}
		RenderGame(Window, &ClientRect, Bitmap, &BitmapInfo);
		if (Message.message == WM_QUIT) {
			return (int) Message.wParam;
		}
	}
	return 0;
}
