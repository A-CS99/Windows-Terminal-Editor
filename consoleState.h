#pragma once
#include <windows.h>

// ���·������еĸ߶�
const int COMMAND_LINE_HEIGHT = 1;
const enum CursorDirection { CURSOR_UP, CURSOR_DOWN, CURSOR_LEFT, CURSOR_RIGHT };

// ����̨����������Ϣ
class ConsoleState {
private:
	HANDLE hConsoleInput;
	HANDLE hConsoleOutput;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	CHAR_INFO* buffer;
	COORD cursorPos;
	short ConsoleWidth;
	short ConsoleHeight;
	short ContentHeight;
public:
	ConsoleState();
	~ConsoleState();
	void updateCSBI();
	CONSOLE_SCREEN_BUFFER_INFO getCSBI();
	HANDLE getHConsoleInput();
	HANDLE getHConsoleOutput();
	void initScreenSize();
	COORD getConsoleSize();
	short getContentHeight();
	COORD getCursorPos();
	void updateCursorPos();
	void setCursorPos(COORD pos);
	void moveCursor(CursorDirection direction, int step);
	ConsoleState SaveConsoleState();
	void RestoreConsoleState(ConsoleState state);
};