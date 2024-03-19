#include "toolFuncs.h"

void jumpPrint(COORD pos, std::string str, ConsoleState state) {
	// 记录当前光标位置，输出字符串后跳转到原位置
	COORD originPos;
	state.updateCursorPos();
	originPos = state.getCursorPos();
	// 跳转到指定位置输出字符串
	state.setCursorPos(pos);
	std::cout << str;
	// 跳转回原位置
	state.setCursorPos(originPos);
}

void ScrollConsoleContent(ConsoleState state, int stepY) {
	// 向下滚动控制台内容，stepY 为滚动的行数
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	HANDLE hConsole = state.getHConsoleOutput();
	COORD ConsoleSize = state.getConsoleSize();
	short ConsoleHeight = ConsoleSize.Y;
	short ContentHeight = state.getContentHeight();
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
		jumpPrint({ 0, ContentHeight }, "GetConsoleScreenBufferInfo failed", state);
	}
	COORD coordDest;
	SMALL_RECT srctScrollRect, srctClipRect;
	CHAR_INFO chiFill;
	srctScrollRect.Top = csbi.dwSize.Y - ConsoleHeight - 1;
	srctScrollRect.Bottom = csbi.dwSize.Y - 2;
	srctScrollRect.Left = 0;
	srctScrollRect.Right = csbi.dwSize.X - 1;
	coordDest.X = 0;
	coordDest.Y = csbi.dwSize.Y - ConsoleHeight - 1 + stepY;
	srctClipRect = srctScrollRect;
	chiFill.Attributes = 0;
	chiFill.Char.UnicodeChar = (char)'~';
	if (!ScrollConsoleScreenBuffer(hConsole, &srctScrollRect, &srctClipRect, coordDest, &chiFill)) {
		jumpPrint({ 0, ContentHeight }, "ScrollConsoleScreenBuffer failed", state);
	}
}