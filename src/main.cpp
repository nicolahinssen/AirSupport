#include <ardrone/ardrone.h>
#include "../res/resource.h"

// Global defines
#define GUI_USE_ARDRONE         
#define GUI_PADDING                    10
#define GUI_WHITEROW                   5
#define GUI_COLWIDTH                   210
#define GUI_ROWHEIGHT                  20
#define GUI_SPEEDSTEP                  0.1
#define GUI_CAMERA_WIDTH               640
#define GUI_CAMERA_HEIGHT              360

// Global variables
HINSTANCE hInstance_ = NULL;                               // Store the instance
HWND hWinmain_ = NULL;                                     // Handle to winmain-window
HWND hWndVersion_ = NULL;                                  // Text for version
HWND hWndBattery_ = NULL;                                  // Text for battery
HWND hWndRoll_ = NULL;                                     // Text for roll
HWND hWndPitch_ = NULL;                                    // Text for pitch
HWND hWndYaw_ = NULL;                                      // Text for yaw
HWND hWndAltitude_ = NULL;                                 // Text for altitude
ARDrone oArdrone_;                                         // AR.Drone class
HBRUSH hBackground_ = NULL;                                // Remember the background color
RECT oCompass_;                                            // Compass area
RECT oCameraBottom_;                                       // Camera bottom image
HBITMAP hImage_;                                           // Background image

														   // Create the window
BOOL WinmainCreate(HINSTANCE hInstance, int nCmdShow);
// Message processing
LRESULT CALLBACK WinmainProcessing(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam);
// Message WM_TIMER
LRESULT WinmainWMTIMER(HWND hWnd, WPARAM wParam, LPARAM lParam);
// Message WM_SIZE
LRESULT WinmainWMSIZE(HWND hWnd, WPARAM wParam, LPARAM lParam);
// Message WM_PAINT
LRESULT WinmainWMPAINT(HWND hWnd, WPARAM wParam, LPARAM lParam);
// Message WM_COMMAND
LRESULT WinmainWMCOMMAND(HWND hWnd, WPARAM wParam, LPARAM lParam);

// --------------------------------------------------------------------------
// The application main-function
// --------------------------------------------------------------------------
int APIENTRY wWinMain(
	_In_     HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_     LPWSTR    lpCmdLine,
	_In_     int       nCmdShow)
{
	// Avoid compiler warnings for not used parameters
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef GUI_USE_ARDRONE
	// Initialize the drone (!! cost a few seconds !!)
	if (!oArdrone_.open()) {
		return -1;
	}
	// Select bottom camera
	oArdrone_.setCamera(1);
#endif // GUI_USE_ARDRONE

	// Init mainwindow
	if (!WinmainCreate(hInstance, nCmdShow)) {
		return FALSE;
	}

	// Main message loop
	MSG oMessage;
	while (GetMessage(&oMessage, NULL, 0, 0)) {

		if (!TranslateAccelerator(oMessage.hwnd, NULL, &oMessage)) {

			TranslateMessage(&oMessage);
			DispatchMessage(&oMessage);
		}
	}

#ifdef GUI_USE_ARDRONE
	// See you
	oArdrone_.close();
#endif // GUI_USE_ARDRONE

	// Close the program
	return (int)oMessage.wParam;
}

// --------------------------------------------------------------------------
// Create the window
// --------------------------------------------------------------------------
BOOL WinmainCreate(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEXA oWinClass;
	DWORD nX, nY;
	CHAR sText[1000];
	int nMajor, nMinor, nRevision;

	// Store the instance
	hInstance_ = hInstance;

	// Define the new window class
	hBackground_ = CreateSolidBrush(RGB(200, 200, 200));
	oWinClass.cbSize = sizeof(WNDCLASSEXA);
	oWinClass.style = CS_HREDRAW | CS_VREDRAW;
	oWinClass.lpfnWndProc = WinmainProcessing;
	oWinClass.cbClsExtra = 0;
	oWinClass.cbWndExtra = 0;
	oWinClass.hInstance = hInstance;
	oWinClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ID_ICON));
	oWinClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	oWinClass.hbrBackground = hBackground_;
	oWinClass.lpszMenuName = NULL;
	oWinClass.lpszClassName = "winmain";
	oWinClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(ID_ICON));
	if (!RegisterClassExA(&oWinClass)) {
		return FALSE;
	}

	// Create the new (type) of window
	hWinmain_ = CreateWindowExA(0, "winmain", "AR.Drone 2.0 cockpit", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, GUI_PADDING * 6 + GUI_COLWIDTH * 4, GUI_PADDING + (GUI_ROWHEIGHT + GUI_WHITEROW) * 10 + GUI_CAMERA_HEIGHT + GUI_PADDING * 2,
		NULL, NULL, hInstance, NULL);
	if (hWinmain_ == NULL) {
		return FALSE;
	}

	// AR.Drone version
	nY = GUI_PADDING;
	oArdrone_.getVersion(&nMajor, &nMinor, &nRevision);
	sprintf(sText, "%d.%d.%d", nMajor, nMinor, nRevision);
	CreateWindowExA(0, WC_STATIC, "AR.Drone version", WS_CHILD | WS_VISIBLE, GUI_PADDING, nY, GUI_COLWIDTH, GUI_ROWHEIGHT,
		hWinmain_, NULL, hInstance_, NULL);
	hWndVersion_ = CreateWindowExA(0, WC_STATIC, sText, WS_CHILD | WS_VISIBLE, GUI_PADDING + GUI_COLWIDTH, nY, GUI_COLWIDTH, GUI_ROWHEIGHT,
		hWinmain_, NULL, hInstance_, NULL);

	// AR.Drone battery status
	nY += GUI_ROWHEIGHT + GUI_WHITEROW;
	CreateWindowExA(0, WC_STATIC, "Battery", WS_CHILD | WS_VISIBLE, GUI_PADDING, nY, GUI_COLWIDTH, GUI_ROWHEIGHT,
		hWinmain_, NULL, hInstance_, NULL);
	hWndBattery_ = CreateWindowExA(0, WC_STATIC, "", WS_CHILD | WS_VISIBLE, GUI_PADDING + GUI_COLWIDTH, nY, GUI_COLWIDTH, GUI_ROWHEIGHT,
		hWinmain_, NULL, hInstance_, NULL);

	// AR.Drone roll status
	nY += GUI_ROWHEIGHT + GUI_WHITEROW;
	CreateWindowExA(0, WC_STATIC, "Roll", WS_CHILD | WS_VISIBLE, GUI_PADDING, nY, GUI_COLWIDTH, GUI_ROWHEIGHT,
		hWinmain_, NULL, hInstance_, NULL);
	hWndRoll_ = CreateWindowExA(0, WC_STATIC, "", WS_CHILD | WS_VISIBLE, GUI_PADDING + GUI_COLWIDTH, nY, GUI_COLWIDTH, GUI_ROWHEIGHT,
		hWinmain_, NULL, hInstance_, NULL);

	// AR.Drone pitch status
	nY += GUI_ROWHEIGHT + GUI_WHITEROW;
	CreateWindowExA(0, WC_STATIC, "Pitch", WS_CHILD | WS_VISIBLE, GUI_PADDING, nY, GUI_COLWIDTH, GUI_ROWHEIGHT,
		hWinmain_, NULL, hInstance_, NULL);
	hWndPitch_ = CreateWindowExA(0, WC_STATIC, "", WS_CHILD | WS_VISIBLE, GUI_PADDING + GUI_COLWIDTH, nY, GUI_COLWIDTH, GUI_ROWHEIGHT,
		hWinmain_, NULL, hInstance_, NULL);

	// AR.Drone yaw status
	nY += GUI_ROWHEIGHT + GUI_WHITEROW;
	CreateWindowExA(0, WC_STATIC, "Yaw", WS_CHILD | WS_VISIBLE, GUI_PADDING, nY, GUI_COLWIDTH, GUI_ROWHEIGHT,
		hWinmain_, NULL, hInstance_, NULL);
	hWndYaw_ = CreateWindowExA(0, WC_STATIC, "", WS_CHILD | WS_VISIBLE, GUI_PADDING + GUI_COLWIDTH, nY, GUI_COLWIDTH, GUI_ROWHEIGHT,
		hWinmain_, NULL, hInstance_, NULL);

	// AR.Drone altitude status
	nY += GUI_ROWHEIGHT + GUI_WHITEROW;
	CreateWindowExA(0, WC_STATIC, "Altitude", WS_CHILD | WS_VISIBLE, GUI_PADDING, nY, GUI_COLWIDTH, GUI_ROWHEIGHT,
		hWinmain_, NULL, hInstance_, NULL);
	hWndAltitude_ = CreateWindowExA(0, WC_STATIC, "", WS_CHILD | WS_VISIBLE, GUI_PADDING + GUI_COLWIDTH, nY, GUI_COLWIDTH, GUI_ROWHEIGHT,
		hWinmain_, NULL, hInstance_, NULL);

	// Show the buttons
	nX = GUI_PADDING + GUI_COLWIDTH + GUI_PADDING + GUI_COLWIDTH;
	nY = GUI_PADDING;
	CreateWindowExA(0, WC_BUTTON, "Takeoff", WS_CHILD | WS_VISIBLE, nX, nY, GUI_COLWIDTH / 2, GUI_ROWHEIGHT,
		hWinmain_, (HMENU)ID_BTN_TAKEOFF, hInstance_, NULL);
	CreateWindowExA(0, WC_BUTTON, "Landing", WS_CHILD | WS_VISIBLE, nX + GUI_COLWIDTH / 2, nY, GUI_COLWIDTH / 2, GUI_ROWHEIGHT,
		hWinmain_, (HMENU)ID_BTN_LANDING, hInstance_, NULL);

	nY += GUI_ROWHEIGHT + GUI_WHITEROW;
	CreateWindowExA(0, WC_BUTTON, "Emergency", WS_CHILD | WS_VISIBLE, nX, nY, GUI_COLWIDTH / 2, GUI_ROWHEIGHT,
		hWinmain_, (HMENU)ID_BTN_EMERGENCY, hInstance_, NULL);
	CreateWindowExA(0, WC_BUTTON, "Flat trim", WS_CHILD | WS_VISIBLE, nX + GUI_COLWIDTH / 2, nY, GUI_COLWIDTH / 2, GUI_ROWHEIGHT,
		hWinmain_, (HMENU)ID_BTN_FLATTRIM, hInstance_, NULL);

	nY += GUI_ROWHEIGHT + GUI_WHITEROW;
	nY += GUI_ROWHEIGHT + GUI_WHITEROW;
	CreateWindowExA(0, WC_BUTTON, "^", WS_CHILD | WS_VISIBLE, nX + GUI_COLWIDTH / 8, nY, GUI_COLWIDTH / 4, GUI_ROWHEIGHT,
		hWinmain_, (HMENU)ID_BTN_UP1, hInstance_, NULL);
	CreateWindowExA(0, WC_BUTTON, "^", WS_CHILD | WS_VISIBLE, nX + GUI_COLWIDTH / 8 + GUI_COLWIDTH / 2, nY, GUI_COLWIDTH / 4, GUI_ROWHEIGHT,
		hWinmain_, (HMENU)ID_BTN_UP2, hInstance_, NULL);
	nY += GUI_ROWHEIGHT + GUI_WHITEROW;
	CreateWindowExA(0, WC_BUTTON, "<", WS_CHILD | WS_VISIBLE, nX, nY, GUI_COLWIDTH / 4, GUI_ROWHEIGHT,
		hWinmain_, (HMENU)ID_BTN_LEFT1, hInstance_, NULL);
	CreateWindowExA(0, WC_BUTTON, ">", WS_CHILD | WS_VISIBLE, nX + GUI_COLWIDTH / 4, nY, GUI_COLWIDTH / 4, GUI_ROWHEIGHT,
		hWinmain_, (HMENU)ID_BTN_RIGHT1, hInstance_, NULL);
	CreateWindowExA(0, WC_BUTTON, "<", WS_CHILD | WS_VISIBLE, nX + GUI_COLWIDTH / 2, nY, GUI_COLWIDTH / 4, GUI_ROWHEIGHT,
		hWinmain_, (HMENU)ID_BTN_LEFT2, hInstance_, NULL);
	CreateWindowExA(0, WC_BUTTON, ">", WS_CHILD | WS_VISIBLE, nX + GUI_COLWIDTH / 4 + GUI_COLWIDTH / 2, nY, GUI_COLWIDTH / 4, GUI_ROWHEIGHT,
		hWinmain_, (HMENU)ID_BTN_RIGHT2, hInstance_, NULL);
	nY += GUI_ROWHEIGHT + GUI_WHITEROW;
	CreateWindowExA(0, WC_BUTTON, "v", WS_CHILD | WS_VISIBLE, nX + GUI_COLWIDTH / 8, nY, GUI_COLWIDTH / 4, GUI_ROWHEIGHT,
		hWinmain_, (HMENU)ID_BTN_DOWN1, hInstance_, NULL);
	CreateWindowExA(0, WC_BUTTON, "v", WS_CHILD | WS_VISIBLE, nX + GUI_COLWIDTH / 8 + GUI_COLWIDTH / 2, nY, GUI_COLWIDTH / 4, GUI_ROWHEIGHT,
		hWinmain_, (HMENU)ID_BTN_DOWN2, hInstance_, NULL);

	nY += GUI_ROWHEIGHT + GUI_WHITEROW;
	nY += GUI_ROWHEIGHT + GUI_WHITEROW;
	CreateWindowExA(0, WC_BUTTON, "Front", WS_CHILD | WS_VISIBLE, nX, nY, GUI_COLWIDTH / 2, GUI_ROWHEIGHT,
		hWinmain_, (HMENU)ID_BTN_CAMFRONT, hInstance_, NULL);
	CreateWindowExA(0, WC_BUTTON, "Bottom", WS_CHILD | WS_VISIBLE, nX + GUI_COLWIDTH / 2, nY, GUI_COLWIDTH / 2, GUI_ROWHEIGHT,
		hWinmain_, (HMENU)ID_BTN_CAMBOTTOM, hInstance_, NULL);

	// Init the compass
	oCompass_.left = GUI_PADDING + GUI_COLWIDTH + GUI_PADDING + GUI_COLWIDTH + GUI_PADDING + GUI_COLWIDTH;
	oCompass_.top = GUI_PADDING;
	oCompass_.right = oCompass_.left + GUI_COLWIDTH;
	oCompass_.bottom = oCompass_.top + GUI_COLWIDTH;

	// Init background image
	hImage_ = (HBITMAP)LoadImageA(hInstance_, "ardrone.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	// Init the camera bottom
	//cv::Mat oImage = oArdrone_.getImage();
	//cv::imshow("camera", oImage);
	oCameraBottom_.left = GUI_PADDING;
	oCameraBottom_.top = GUI_PADDING + (GUI_ROWHEIGHT + GUI_WHITEROW) * 8 + GUI_PADDING * 2;
	oCameraBottom_.right = oCameraBottom_.left + GUI_CAMERA_WIDTH;
	oCameraBottom_.bottom = oCameraBottom_.top + GUI_CAMERA_HEIGHT;

	// Init the timer (100ms)
	SetTimer(hWinmain_, ID_TIMER, 100, (TIMERPROC)NULL);

	// Make the window visible
	ShowWindow(hWinmain_, nCmdShow);
	UpdateWindow(hWinmain_);

	return TRUE;
}

// --------------------------------------------------------------------------
// Message processing
// --------------------------------------------------------------------------
LRESULT CALLBACK WinmainProcessing(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	// WM_DESTROY = clean up and quit the program
	if (nMessage == WM_DESTROY) {

		DeleteObject(hImage_);
		PostQuitMessage(0);
	}

	// WM_CTLCOLORSTATIC = use transparent background behind static controls
	if (nMessage == WM_CTLCOLORSTATIC) {
		HDC hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB(0, 0, 0));
		SetBkMode(hdcStatic, TRANSPARENT);
		return (LRESULT)hBackground_;
	}

	// WM_TIMER = timer event handling
	if (nMessage == WM_TIMER) {
		return WinmainWMTIMER(hWnd, wParam, lParam);
	}

	// WM_SIZE = when the main window changes from size
	if (nMessage == WM_SIZE) {
		return WinmainWMSIZE(hWnd, wParam, lParam);
	}

	// WM_PAINT = redraw the window if needed
	if (nMessage == WM_PAINT) {
		return WinmainWMPAINT(hWnd, wParam, lParam);
	}

	// WM_COMMAND = commands like buttons being pushed
	if (nMessage == WM_COMMAND) {
		return WinmainWMCOMMAND(hWnd, wParam, lParam);
	}

	return DefWindowProc(hWnd, nMessage, wParam, lParam);
}

// --------------------------------------------------------------------------
// Message WM_TIMER
// --------------------------------------------------------------------------
LRESULT WinmainWMTIMER(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// 'Monitor' refresh
	if (wParam == ID_TIMER) {

		CHAR sText[1000];

		// Update the battery status
		sprintf(sText, "%d %%", oArdrone_.getBatteryPercentage());
		SetWindowTextA(hWndBattery_, sText);

		// Update the roll status
		sprintf(sText, "%f rad", oArdrone_.getRoll());
		SetWindowTextA(hWndRoll_, sText);

		// Update the pitch status
		sprintf(sText, "%f rad", oArdrone_.getPitch());
		SetWindowTextA(hWndPitch_, sText);

		// Update the yaw status
		sprintf(sText, "%f rad", oArdrone_.getYaw());
		SetWindowTextA(hWndYaw_, sText);

		// Update the altitude status
		sprintf(sText, "%f m", oArdrone_.getAltitude());
		SetWindowTextA(hWndAltitude_, sText);

		// Update the compass
		InvalidateRect(hWnd, &oCompass_, FALSE);

		// Update the camera bottom
		InvalidateRect(hWnd, &oCameraBottom_, FALSE);
	}

	return 0;
}

// --------------------------------------------------------------------------
// Message WM_SIZE
// --------------------------------------------------------------------------
LRESULT WinmainWMSIZE(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	WORD nNewWidth = LOWORD(lParam);
	WORD nNewHeight = HIWORD(lParam);

	return 0;
}

// --------------------------------------------------------------------------
// Message WM_PAINT
// --------------------------------------------------------------------------
LRESULT WinmainWMPAINT(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// Init the device context
	PAINTSTRUCT oPs;
	RECT oClient;
	BITMAPINFO oBmi;
	BITMAPINFOHEADER* pheader;
	HDC hDC = BeginPaint(hWnd, &oPs);
	HPEN hPenSolid = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	HPEN hPenDot = CreatePen(PS_DOT, 2, RGB(0, 0, 255));
	GetClientRect(hWnd, &oClient);

	// Draw the compass
	double fR = GUI_COLWIDTH / 2 - 5;
	double fA = oArdrone_.getYaw();
	SelectObject(hDC, hPenDot);
	Ellipse(hDC, oCompass_.left, oCompass_.top, oCompass_.right, oCompass_.bottom);
	SelectObject(hDC, hPenSolid);
	MoveToEx(hDC, oCompass_.left + GUI_COLWIDTH / 2, oCompass_.top + GUI_COLWIDTH / 2, 0);
	LineTo(hDC, oCompass_.left + GUI_COLWIDTH / 2 + fR * cos(fA), oCompass_.top + GUI_COLWIDTH / 2 - fR * sin(fA));

	// Show the image
	HDC hDCmem = CreateCompatibleDC(hDC);
	HGDIOBJ hOldbmp = SelectObject(hDCmem, hImage_);
	BITMAP oBmp;
	GetObject(hImage_, sizeof(oBmp), &oBmp);
	BitBlt(hDC, oClient.right - oBmp.bmWidth - GUI_PADDING, oClient.bottom - oBmp.bmHeight - GUI_PADDING, oBmp.bmWidth, oBmp.bmHeight, hDCmem, 0, 0, SRCCOPY);
	SelectObject(hDCmem, hOldbmp);
	DeleteDC(hDCmem);

	// Show the camera bottom
	oArdrone_.setCamera(1);
	cv::Mat oImageBottom = oArdrone_.getImage();
	//cv::imshow("camera", oImage);
	//Rectangle(hDC, oCamera_.left, oCamera_.top, oCamera_.right, oCamera_.bottom);
	pheader = &oBmi.bmiHeader;
	pheader->biSize = sizeof(BITMAPINFOHEADER);
	pheader->biPlanes = 1;
	pheader->biCompression = BI_RGB;
	pheader->biXPelsPerMeter = 100;
	pheader->biYPelsPerMeter = 100;
	pheader->biClrUsed = 0;
	pheader->biClrImportant = 0;
	pheader->biWidth = oImageBottom.cols;
	pheader->biHeight = -oImageBottom.rows;
	pheader->biBitCount = 24;
	oBmi.bmiHeader.biSizeImage = 0;
	StretchDIBits(hDC, oCameraBottom_.left, oCameraBottom_.top, GUI_CAMERA_WIDTH, GUI_CAMERA_HEIGHT, 0, 0, GUI_CAMERA_WIDTH, GUI_CAMERA_HEIGHT,
		oImageBottom.data, &oBmi, DIB_RGB_COLORS, SRCCOPY);

	// Draw arrow over camera bottom
	int x = oCameraBottom_.left + GUI_CAMERA_WIDTH / 2;
	int y = oCameraBottom_.top + GUI_CAMERA_HEIGHT / 2;
	int s = 10;
	MoveToEx(hDC, x - s, y, 0);
	LineTo(hDC, x + s, y);
	MoveToEx(hDC, x, y - s, 0);
	LineTo(hDC, x, y + s);

	// Free
	EndPaint(hWnd, &oPs);

	return 0;
}

// --------------------------------------------------------------------------
// Message WM_COMMAND
// --------------------------------------------------------------------------
LRESULT WinmainWMCOMMAND(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	WORD nId = LOWORD(wParam);
	WORD nEvent = HIWORD(wParam);
	HWND hObject = (HWND)(lParam);
	double fX = 0, fY = 0, fZ = 0, fR = 0;

	// Button Takeoff
	if (nId == ID_BTN_TAKEOFF) {
#ifdef GUI_USE_ARDRONE
		oArdrone_.takeoff();
#endif // GUI_USE_ARDRONE
		return 0;
	}

	// Button Landing
	if (nId == ID_BTN_LANDING) {
#ifdef GUI_USE_ARDRONE
		oArdrone_.landing();
#endif // GUI_USE_ARDRONE
		return 0;
	}

	// Button Emergency
	if (nId == ID_BTN_EMERGENCY) {
#ifdef GUI_USE_ARDRONE
		oArdrone_.emergency();
#endif // GUI_USE_ARDRONE
		return 0;
	}

	// Button Flat trim
	if (nId == ID_BTN_FLATTRIM) {
#ifdef GUI_USE_ARDRONE
		oArdrone_.setFlatTrim();
#endif // GUI_USE_ARDRONE
		return 0;
	}

	// Button up1
	if (nId == ID_BTN_UP1) {
#ifdef GUI_USE_ARDRONE
		oArdrone_.move3D(GUI_SPEEDSTEP, fY, fZ, fR);
#endif // GUI_USE_ARDRONE
		return 0;
	}

	// Button down1
	if (nId == ID_BTN_UP1) {
#ifdef GUI_USE_ARDRONE
		oArdrone_.move3D(-GUI_SPEEDSTEP, fY, fZ, fR);
#endif // GUI_USE_ARDRONE
		return 0;
	}

	// Button left1
	if (nId == ID_BTN_LEFT1) {
#ifdef GUI_USE_ARDRONE
		oArdrone_.move3D(fX, GUI_SPEEDSTEP, fZ, fR);
#endif // GUI_USE_ARDRONE
		return 0;
	}

	// Button right1
	if (nId == ID_BTN_RIGHT1) {
#ifdef GUI_USE_ARDRONE
		oArdrone_.move3D(fX, -GUI_SPEEDSTEP, fZ, fR);
#endif // GUI_USE_ARDRONE
		return 0;
	}

	// Button up2
	if (nId == ID_BTN_UP2) {
#ifdef GUI_USE_ARDRONE
		oArdrone_.move3D(fX, fY, GUI_SPEEDSTEP, fR);
#endif // GUI_USE_ARDRONE
		return 0;
	}

	// Button down2
	if (nId == ID_BTN_UP2) {
#ifdef GUI_USE_ARDRONE
		oArdrone_.move3D(fX, fY, -GUI_SPEEDSTEP, fR);
#endif // GUI_USE_ARDRONE
		return 0;
	}

	// Button left2
	if (nId == ID_BTN_LEFT2) {
#ifdef GUI_USE_ARDRONE
		oArdrone_.move3D(fX, fY, fZ, GUI_SPEEDSTEP);
#endif // GUI_USE_ARDRONE
		return 0;
	}

	// Button right2
	if (nId == ID_BTN_RIGHT2) {
#ifdef GUI_USE_ARDRONE
		oArdrone_.move3D(fX, fY, fZ, -GUI_SPEEDSTEP);
#endif // GUI_USE_ARDRONE
		return 0;
	}

	// Button camera front
	if (nId == ID_BTN_CAMFRONT) {
#ifdef GUI_USE_ARDRONE
		oArdrone_.setCamera(0);
#endif // GUI_USE_ARDRONE
		return 0;
	}

	// Button camera bottom
	if (nId == ID_BTN_CAMBOTTOM) {
#ifdef GUI_USE_ARDRONE
		oArdrone_.setCamera(1);
#endif // GUI_USE_ARDRONE
		return 0;
	}

	return 0;
}
