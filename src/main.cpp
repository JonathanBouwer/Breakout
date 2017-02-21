#include <Windows.h>
#include <string>
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

void DrawGameRect(VOID *Bitmap, int32_t left, int32_t bottom, int32_t right, int32_t top, int32_t color) {
	if (left < 0) left = 0;
	if (bottom < 0) bottom = 0;
	if (right > windowSizes.GameWidth) right = windowSizes.GameWidth;
	if (top > windowSizes.GameHeight) top = windowSizes.GameHeight;
	for (int32_t j = bottom; j < top; j++) {
		uint32_t screenY = j + windowSizes.topOffset;
		for (int32_t i = left; i < right; i++) {
			uint32_t screenX = i + windowSizes.leftOffset;
			int32_t *Pixel = (int32_t *) (Bitmap);
			Pixel = Pixel + screenY*windowSizes.ClientWidth + screenX;
			*Pixel = color;
		}
	}
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

void SetupGame() {
	gameState.currentState = PLAY;
	gameState.goalState = PLAY;

	gameState.player.width = 180;
	gameState.player.height = 20;
	gameState.player.position = Vector(windowSizes.ClientWidth / 2.0f, 20.0f);
	gameState.player.velocity = Vector(200.0f, 0.0f);

	gameState.ball.radius = 10;
	Vector yoffset = Vector(0.0f, gameState.player.height / 2.0f + gameState.ball.radius);
	gameState.ball.position = gameState.player.position + yoffset;
	gameState.ball.velocity = Vector(300.0f, 300.0f);
}

void HandleInput(uint32_t keyPressed, int keyPos) {
	for (int i = 0; i < inputState.numberOfKeys; i++) {
		if (inputState.setKeys[i] == keyPressed) {
			if (keyPos == WM_KEYDOWN) {
				inputState.keystate |= 1 << i;
			} else {
				inputState.keystate &= ~(1 << i);
			}
			break;
		}
	}
}

void UpdateGame(float dt) {
	if (inputState.keystate & (1 << 3)) gameState.player.position += dt*gameState.player.velocity;
	if (inputState.keystate & (1 << 2)) gameState.player.position -= dt*gameState.player.velocity;
}

void DrawGameToBitmap(VOID* Bitmap) {
	// Background
	DrawGameRect(Bitmap, 0, 0, windowSizes.GameWidth, windowSizes.GameHeight, 0x00333333);
	// Player
	int32_t playerLeft = (int32_t) gameState.player.position.x - gameState.player.width / 2;
	int32_t playerBottom = (int32_t) gameState.player.position.y - gameState.player.height / 2;
	int32_t playerRight = playerLeft + gameState.player.width;
	int32_t playerTop = playerBottom + gameState.player.height;
	DrawGameRect(Bitmap, playerLeft, playerBottom, playerRight, playerTop, 0x00FFFFFF);
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowType) {
	HWND Window = CreateWindowHandle(Instance, "Breakout");
	RECT ClientRect;
	GetClientRect(Window, &ClientRect);
	assert(ClientRect.right > windowSizes.GameWidth);
	assert(ClientRect.bottom > windowSizes.GameHeight);
	windowSizes.ClientWidth = (int16_t) ClientRect.right;
	windowSizes.ClientHeight = (int16_t) ClientRect.bottom;
	windowSizes.leftOffset = (windowSizes.ClientWidth - windowSizes.GameWidth) / 2;
	windowSizes.topOffset = (windowSizes.ClientHeight - windowSizes.GameHeight) / 2;

	BITMAPINFO BitmapInfo{};
	BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BitmapInfo.bmiHeader.biWidth = windowSizes.ClientWidth;
	BitmapInfo.bmiHeader.biHeight = windowSizes.ClientHeight;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;

	int32_t BitmapSize = sizeof(DWORD) * windowSizes.ClientWidth * windowSizes.ClientHeight;
	VOID *Bitmap = VirtualAlloc(NULL, BitmapSize, MEM_COMMIT, PAGE_READWRITE);

	SetupInput();
	SetupGame();
	LARGE_INTEGER prevUpdateTime, curUpdateTime, frameStartTime, frameCurTime, frequency;
	float elapsedSeconds;
	QueryPerformanceFrequency(&frequency); // per second frequency
	QueryPerformanceCounter(&prevUpdateTime);
	TIMECAPS tc;
	timeGetDevCaps(&tc, sizeof(TIMECAPS));
	timeBeginPeriod(tc.wPeriodMin);

	HDC hdc = GetDC(Window);
	int refreshRate = GetDeviceCaps(hdc, VREFRESH);
	ReleaseDC(Window, hdc);
	const float updateTimestep = 1.0f / refreshRate;
	float acumulator = 0.0f;
	while (true) {
		QueryPerformanceCounter(&frameStartTime);
		// Get Input
		MSG Message;
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
		}
		if (Message.message == WM_QUIT) {
			return (int) Message.wParam;
		}

		// Update Game Physics etc
		QueryPerformanceCounter(&curUpdateTime);
		elapsedSeconds = (curUpdateTime.QuadPart - prevUpdateTime.QuadPart) / (float) frequency.QuadPart;
		prevUpdateTime = curUpdateTime;
		std::string s = std::to_string(elapsedSeconds) + "seconds " + std::to_string(1 / elapsedSeconds) + "fps\n";
		OutputDebugString(s.c_str());

		acumulator += elapsedSeconds;
		while (acumulator >= updateTimestep) {
			UpdateGame(updateTimestep);
			acumulator -= updateTimestep;
		}

		// Render Game to Bitmap
		DrawGameToBitmap(Bitmap);

		// Sleep till frame flip
		QueryPerformanceCounter(&frameCurTime);
		float sleepTime = updateTimestep - ((frameCurTime.QuadPart - frameStartTime.QuadPart) / (float) frequency.QuadPart);
		if (sleepTime > 0) { 
			Sleep((DWORD) (sleepTime * 1000) - 1); 
		}

		// Flip Frame
		RenderGame(Window, &ClientRect, Bitmap, &BitmapInfo);
	}
	return 0;
}
