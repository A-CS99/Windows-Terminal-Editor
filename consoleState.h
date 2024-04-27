#pragma once
#include <windows.h>
#include <string>

// 最下方命令行的高度
const int COMMAND_LINE_HEIGHT = 1;
const enum CursorDirection { CURSOR_UP, CURSOR_DOWN, CURSOR_LEFT, CURSOR_RIGHT };

// 控制台缓冲区的信息
class ConsoleState {
private:
	HANDLE hConsoleInput;
	HANDLE hConsoleOutput;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	CHAR_INFO* buffer;
	COORD cursorPos;
public:
	short ConsoleWidth;
	short ConsoleHeight;
	short ContentHeight;
	short TabWidth;
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
	void printCmd(std::string cmdStr);
	void printCmd(std::string cmdStr, WORD textAttr);
	void printInfo(std::string infoStr);
	void printWarn(std::string warnStr);
	void printErr(std::string errStr);
	void setTextAttr(WORD textAttr);
	ConsoleState SaveConsoleState();
	void RestoreConsoleState(ConsoleState state);
};