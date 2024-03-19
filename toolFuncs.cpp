#include "toolFuncs.h"

void jumpPrint(COORD pos, std::string str, ConsoleState state) {
	// ��¼��ǰ���λ�ã�����ַ�������ת��ԭλ��
	COORD originPos;
	state.updateCursorPos();
	originPos = state.getCursorPos();
	// ��ת��ָ��λ������ַ���
	state.setCursorPos(pos);
	std::cout << str;
	// ��ת��ԭλ��
	state.setCursorPos(originPos);
}

void ScrollConsoleContent(ConsoleState state, int stepY) {
	// ���¹�������̨���ݣ�stepY Ϊ����������
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