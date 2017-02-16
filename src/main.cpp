#include <Windows.h>
#include "main.h"

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

HWND CreateWindowHandle(HINSTANCE Instance, LPSTR Title) {
	WNDCLASS WindowClass{};
	WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = Instance;
	WindowClass.lpszClassName = "BreakoutGameClass";
	RegisterClass(&WindowClass);
	DWORD WindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
	HWND Window = CreateWindow("BreakoutGameClass", Title, WindowStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, windowSizes.WindowWidth, windowSizes.WindowHeight,
		NULL, NULL, Instance, NULL);
	return Window;
}

void RenderGame(HWND Window, RECT *ClientRect, VOID *Bitmap, BITMAPINFO *BitmapInfo) {
	HDC hdc = GetDC(Window);
	int ret = StretchDIBits(hdc,
		ClientRect->left, ClientRect->top, ClientRect->right, ClientRect->bottom,
		ClientRect->left, ClientRect->top, ClientRect->right, ClientRect->bottom,
		Bitmap, BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(Window, hdc);
}

void SetupInput() {
	inputState.setKeys[0] = VK_UP;
	inputState.setKeys[1] = VK_DOWN;
	inputState.setKeys[2] = VK_LEFT;
	inputState.setKeys[3] = VK_RIGHT;
	inputState.setKeys[4] = VK_SPACE;
	inputState.setKeys[5] = VK_ESCAPE;
	inputState.setKeys[6] = VK_LBUTTON;
	inputState.setKeys[7] = VK_RBUTTON;
	inputState.keystate = 0;
}

void HandleInput(uint32_t keyPressed, int keyPos) {
	for (int i = 0; i < inputState.numberOfKeys; i++) {
		if (inputState.setKeys[i] == keyPressed) {
			if(keyPos == WM_KEYDOWN) {
				inputState.keystate |= 1 << i;
			} else {
				inputState.keystate &= ~(1 << i);
			}
			break;
		}
	}
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowType) {
	HWND Window = CreateWindowHandle(Instance, "Breakout");
	RECT ClientRect;
	GetClientRect(Window, &ClientRect);
	assert(ClientRect.right > windowSizes.GameWidth);
	assert(ClientRect.bottom > windowSizes.GameHeight);
	windowSizes.leftOffset = (((int16_t) ClientRect.right) - windowSizes.GameWidth) / 2;
	windowSizes.topOffset = (((int16_t) ClientRect.bottom) - windowSizes.GameHeight) / 2;

	BITMAPINFO BitmapInfo{};
	BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BitmapInfo.bmiHeader.biWidth = ClientRect.right;
	BitmapInfo.bmiHeader.biHeight = ClientRect.bottom;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;

	int32_t BitmapSize = sizeof(DWORD) * ClientRect.right * ClientRect.bottom;
	VOID *Bitmap = VirtualAlloc(NULL, BitmapSize, MEM_COMMIT, PAGE_READWRITE);

	SetupInput();
	int32_t r = 0, g = 0, b = 0;
	while (true) {
		MSG Message;
		int count = 0;
		while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE) > 0) {
			switch (Message.message) {
				case WM_KEYDOWN:
				case WM_KEYUP: {
					HandleInput((uint32_t) Message.wParam, Message.message);
				} break;
				default: {
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
			}
			++count;
		}
		r = (inputState.keystate & 1 << 3) ? (r < 0xff ? r + 1 : r) : (r > 0 ? r - 1 : r);
		g = (inputState.keystate & 1 << 2) ? (g < 0xff ? g + 1 : g) : (g > 0 ? g - 1 : g);
		b = (inputState.keystate & 1 << 1) ? (b < 0xff ? b + 1 : b) : (b > 0 ? b - 1 : b);
		int ystart = windowSizes.topOffset, 
			yend = ystart + windowSizes.GameHeight,
			xstart = windowSizes.leftOffset, 
			xend = xstart + windowSizes.GameWidth;
		for (int i = ystart; i < yend; i++) {
			for (int j = xstart; j < xend; j++) {
				int32_t *Pixel = (int32_t *) (Bitmap);
				Pixel = Pixel + i*ClientRect.right + j;
				*Pixel = (r << 16) + (g << 8) + (b << 0);
			}
		}
		RenderGame(Window, &ClientRect, Bitmap, &BitmapInfo);
		if (Message.message == WM_QUIT) {
			return (int) Message.wParam;
		}
	}
	return 0;
}
