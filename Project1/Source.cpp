#include <Windows.h> // заголовочный файл, содержащий функции API
#include <math.h>
#include <time.h>
#include <string>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#define Left 0
#define Right 1
#define Up 2
#define Down 3
#define NE 4
#define SE 5
#define SW 6
#define NW 7
#define R 0x52
#define E 0x45
#define A 0x41
#define P 0x50
#define TimerID 1
const int MainWindowHeight = 760;
const int MainWindowWidth = 700;
const HBRUSH WindowBackground = CreateSolidBrush(RGB(168, 50, 131));
const HBRUSH ObjectBackground = CreateSolidBrush(RGB(66, 239, 245));
const int PixelsStepSize = 5;
RECT ObjectRect = { 200,200,260,240 };
int mode = 0;
int globalRoute = SE;
int figure = R;
int RandomY, RandomX;
RECT ClientRect;
HBITMAP Picture;
POINTS PrevPoint;
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void DrowDVD1(int route);
void DrowDVD2(int route);
void DrowNewPos(int key);
void drawBitmap(HDC hdc, int x, int y, HBITMAP hBitmap);
HBITMAP PngToBitmap(WCHAR* pngFilePath);
int TimerDVD;
bool ActivFlag = false;
int WINAPI WinMain(HINSTANCE hInstance, // дескриптор экземпляра приложения
	HINSTANCE hPrevInstance, // в Win32 не используется
	LPSTR lpCmdLine, // нужен для запуска окна в режиме командной строки
	int nCmdShow) // режим отображения окна
{
	srand(time(NULL));
	WNDCLASSEX WinClass;
	const char* ClassName = "MainClass";
	WinClass.cbSize = sizeof(WinClass);
	WinClass.lpfnWndProc = (WNDPROC)WndProc;
	WinClass.style = 0;
	WinClass.cbClsExtra = 0;
	WinClass.cbWndExtra = 0;
	WinClass.hInstance = hInstance;
	WinClass.hIcon = LoadIcon(NULL, IDI_ERROR);
	WinClass.hCursor = LoadCursor(NULL, IDC_SIZEALL);
	WinClass.hbrBackground = WindowBackground;
	WinClass.lpszMenuName = NULL;
	WinClass.lpszClassName = ClassName;
	WinClass.hIconSm = NULL;

	if (!RegisterClassEx(&WinClass)) {
		MessageBox(NULL,
			"Не получилось зарегистрировать класс!",
			"Ошибка", MB_OK);
		return NULL; 
	}

	const char* WindowName = "MainWindow";
	HWND MainWindow = CreateWindowEx(0, ClassName, WindowName, WS_SYSMENU, CW_USEDEFAULT, 0, MainWindowHeight, MainWindowWidth, NULL, NULL, hInstance, NULL);
	if (!MainWindow) {
		MessageBox(NULL,
			"Не получилось создать окно",
			"Ошибка", MB_OK);
		return NULL;
	}

	ShowWindow(MainWindow, nCmdShow);
	MSG RecMessage;
	while (GetMessage(&RecMessage, NULL, 0, 0)) {
		TranslateMessage(&RecMessage);
		DispatchMessage(&RecMessage);
	}
	return RecMessage.wParam;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HDC hDC;
	WCHAR test[] = L"shark.png";
	bool eli = true;
	int WheelDirec = 0;
	POINTS point;
	PAINTSTRUCT PaintStruct;
	switch (uMsg) {
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &PaintStruct);
		GetClientRect(hWnd, &ClientRect);
		FillRect(hDC, &ClientRect, WindowBackground);
		SelectObject(hDC, ObjectBackground);
		switch (figure) {
		case R:
			Rectangle(hDC, ObjectRect.left, ObjectRect.top, ObjectRect.right, ObjectRect.bottom);
			break;
		case E:
			Ellipse(hDC, ObjectRect.left, ObjectRect.top, ObjectRect.right, ObjectRect.bottom);
			break;
		case P:
			drawBitmap(hDC, ObjectRect.left, ObjectRect.top, Picture);
		}
		EndPaint(hWnd, &PaintStruct);
		break;
	case WM_CREATE:
		Picture= PngToBitmap(test);
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
			DrowNewPos(Left);
			break;
		case VK_RIGHT:
			DrowNewPos(Right);
			break;
		case VK_UP:
			DrowNewPos(Up);
			break;
		case VK_DOWN:
			DrowNewPos(Down);
			break;
		case R:
			figure = R;
			break;
		case E:
			figure = E;
			break;
		case P:
			figure = P;

			break;
		case A:
			if (ActivFlag) {
				ActivFlag = false;
				KillTimer(hWnd, TimerID);
			}
			else {
				globalRoute = rand() % 4 + 4;
				TimerDVD = SetTimer(hWnd, TimerID, 10, NULL);
				ActivFlag = true;
			}
			break;
		}
		InvalidateRect(hWnd, &ClientRect, false);
		break;
	case WM_LBUTTONDOWN:
	    point = MAKEPOINTS(lParam);
		PrevPoint.x = point.x;
		PrevPoint.y = point.y;
		eli = (((((PrevPoint.x - (ObjectRect.left + 30))*(PrevPoint.x - (ObjectRect.left + 30))) / (30*30)) + ((PrevPoint.y - (ObjectRect.top + 20))*(PrevPoint.y - (ObjectRect.top + 20))) / (20*20)) <= 1);
		/*if (eli) {
			MessageBox(hWnd, "asd", "dasd", MB_OK);
		}*/
		break;
	case WM_MOUSEMOVE:
	    point = MAKEPOINTS(lParam);
		eli = (PrevPoint.x >= ObjectRect.left) && (PrevPoint.y >= ObjectRect.top) && (PrevPoint.x <= ObjectRect.right) && (PrevPoint.y <= ObjectRect.bottom);
		if ((wParam==MK_LBUTTON)&&(eli)) {
				OffsetRect(&ObjectRect, point.x - PrevPoint.x, point.y - PrevPoint.y);
				if (((ObjectRect.left) <= (ClientRect.left))|| ((ObjectRect.right) >= (ClientRect.right))) {
					OffsetRect(&ObjectRect, -(point.x - PrevPoint.x), 0);
				}
				if (((ObjectRect.top) <= (ClientRect.top)) || ((ObjectRect.bottom) >= (ClientRect.bottom))) {
					OffsetRect(&ObjectRect, 0, -(point.y - PrevPoint.y));
				}
				PrevPoint.x = point.x;
				PrevPoint.y = point.y;
				InvalidateRect(hWnd, &ClientRect, false);
		}
		break;
	case WM_MOUSEWHEEL:
		WheelDirec = GET_WHEEL_DELTA_WPARAM(wParam);
		if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT) {
			if (WheelDirec < 0) {
				DrowNewPos(Right);
			}
			else {
				DrowNewPos(Left);
			}
		}
		else {
			if (WheelDirec > 0) {
				DrowNewPos(Up);
			}
			else {
				DrowNewPos(Down);
			}
		}
		InvalidateRect(hWnd, &ClientRect, false);
		break;
	case WM_TIMER:
		InvalidateRect(hWnd, &ClientRect, false);
		DrowDVD1(globalRoute);
		//DrowDVD2(globalRoute);
		break;
	case WM_DESTROY:
		KillTimer(hWnd, TimerID);
		PostQuitMessage(NULL);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return NULL;
}
void DrowDVD2(int route) {
	float angle = (rand() % 360 - 180) * 3.14 / 180;
	int x = PixelsStepSize * cos(angle);
	int y = PixelsStepSize * sin(angle);
	switch (route) {
	case NE:
		OffsetRect(&ObjectRect, PixelsStepSize + x, -PixelsStepSize + y);
	case SE:
		OffsetRect(&ObjectRect, PixelsStepSize + x, PixelsStepSize + y);
	case NW:
		OffsetRect(&ObjectRect, -PixelsStepSize + x, -PixelsStepSize + y);
	case SW:
		OffsetRect(&ObjectRect, -PixelsStepSize + x, PixelsStepSize + y);
	}
}
void DrowDVD1(int route){
	switch (route) {
	case NE:
		if (ClientRect.top >= ObjectRect.top) {
			globalRoute = SE;
			RandomX = rand() % PixelsStepSize - PixelsStepSize + 1;
			RandomY = rand() % PixelsStepSize - PixelsStepSize + 1;
			OffsetRect(&ObjectRect, PixelsStepSize+RandomX, PixelsStepSize+RandomY);
			break;
		}
		if (ClientRect.right <= ObjectRect.right) {
			globalRoute = NW;
			RandomX = rand() % PixelsStepSize - PixelsStepSize + 1;
			RandomY = rand() % PixelsStepSize - PixelsStepSize + 1;
			OffsetRect(&ObjectRect, -PixelsStepSize + RandomX, - PixelsStepSize+RandomY);
			break;
		}
		OffsetRect(&ObjectRect, PixelsStepSize + RandomX, -PixelsStepSize + RandomY);
		break;
	case SE:
		if (ClientRect.bottom <= ObjectRect.bottom) {
			globalRoute = NE;
			RandomX = rand() % PixelsStepSize - PixelsStepSize + 1;
			RandomY = rand() % PixelsStepSize - PixelsStepSize + 1;
			OffsetRect(&ObjectRect, PixelsStepSize + RandomX, -PixelsStepSize + RandomY);
			break;
		}
		if (ClientRect.right <= ObjectRect.right) {
			globalRoute = SW;
			RandomX = rand() % PixelsStepSize - PixelsStepSize + 1;
			RandomY = rand() % PixelsStepSize - PixelsStepSize + 1;
			OffsetRect(&ObjectRect, -PixelsStepSize + RandomX, PixelsStepSize + RandomY);
			break;
		}
		OffsetRect(&ObjectRect, PixelsStepSize + RandomX, PixelsStepSize + RandomY);
		break;
	case NW:
		if (ClientRect.top >= ObjectRect.top) {
			globalRoute = SW;
			RandomX = rand() % PixelsStepSize - PixelsStepSize + 1;
			RandomY = rand() % PixelsStepSize - PixelsStepSize + 1;
			OffsetRect(&ObjectRect, -PixelsStepSize + RandomX, PixelsStepSize + RandomY);
			break;
		}
		if (ClientRect.left >= ObjectRect.left) {
			globalRoute = NE;
			RandomX = rand() % PixelsStepSize - PixelsStepSize + 1;
			RandomY = rand() % PixelsStepSize - PixelsStepSize + 1;
			OffsetRect(&ObjectRect, -PixelsStepSize + RandomX, PixelsStepSize + RandomY);
			break;
		}
		OffsetRect(&ObjectRect, -PixelsStepSize + RandomX, -PixelsStepSize + RandomY);
		break;
	case SW:
		if (ClientRect.bottom <= ObjectRect.bottom) {
			globalRoute = NW;
			RandomX = rand() % PixelsStepSize - PixelsStepSize + 1;
			RandomY = rand() % PixelsStepSize - PixelsStepSize + 1;
			OffsetRect(&ObjectRect, -PixelsStepSize + RandomX, -PixelsStepSize + RandomY);
			break;
		}
		if (ClientRect.left >= ObjectRect.left) {
			globalRoute = SE;
			RandomX = rand() % PixelsStepSize - PixelsStepSize + 1;
			RandomY = rand() % PixelsStepSize - PixelsStepSize + 1;
			OffsetRect(&ObjectRect, PixelsStepSize + RandomX, PixelsStepSize + RandomY);
			break;
		}
		OffsetRect(&ObjectRect, -PixelsStepSize + RandomX, PixelsStepSize + RandomY);
		break;
	}
} 

int CheckPos(int pos1, int pos2) {
	if (pos2 < pos1) {
		return  -6 * PixelsStepSize;
	}
	else {
		return  PixelsStepSize;
	}
}
void DrowNewPos(int key) {
	int CheckPos1;
	int CheckPos2;
	int step;
	switch (key) {
	case Left:
	 	CheckPos1 = ClientRect.left;
		CheckPos2 = ObjectRect.left - PixelsStepSize;
		step = CheckPos(CheckPos1, CheckPos2);
		OffsetRect(&ObjectRect, -step, 0);
		break;
	case Right:
		CheckPos2 = ClientRect.right;
		CheckPos1 = ObjectRect.right + PixelsStepSize;
		step = CheckPos(CheckPos1, CheckPos2);
		OffsetRect(&ObjectRect, step, 0);
		break;
	case Up:
		CheckPos1 = ClientRect.top;
		CheckPos2 = ObjectRect.top - PixelsStepSize;
		step = CheckPos(CheckPos1, CheckPos2);
		OffsetRect(&ObjectRect, 0, -step);
		break;
	case Down:
		CheckPos2 = ClientRect.bottom;
		CheckPos1 = ObjectRect.bottom + PixelsStepSize;
		step = CheckPos(CheckPos1, CheckPos2);
		OffsetRect(&ObjectRect, 0, step);
		break;
	}
};

HBITMAP PngToBitmap(WCHAR* pngFilePath) {
	GdiplusStartupInput test;
	ULONG_PTR token;
	GdiplusStartup(&token, &test, NULL);
	Color Back = Color(Color::MakeARGB(0, 168, 50, 131));
	HBITMAP convertedBitmap = NULL;
	Bitmap* Bitmap = Bitmap::FromFile(pngFilePath, false);
	if (Bitmap) {
		Bitmap->GetHBITMAP(Back, &convertedBitmap);
		delete Bitmap;
	}
	GdiplusShutdown(token);
	return convertedBitmap;
}

void drawBitmap(HDC hdc, int x, int y, HBITMAP HBitmap) {
	HDC LocalDC;
	BITMAP LocalBitmap;
	HBITMAP LocalHBitmap;
	LocalDC = CreateCompatibleDC(hdc);
	LocalHBitmap = (HBITMAP)SelectObject(LocalDC, HBitmap);
	if (LocalHBitmap) {
		GetObject(HBitmap, sizeof(BITMAP), (LPSTR)&LocalBitmap);
		BitBlt(hdc, x, y, LocalBitmap.bmWidth, LocalBitmap.bmHeight, LocalDC, 0 ,0, SRCCOPY);
	}
	DeleteDC(LocalDC);
}