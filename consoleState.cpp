#include "consoleState.h"

ConsoleState::ConsoleState() {
	// 设置控制台输出编码为 UTF-8
	SetConsoleOutputCP(CP_UTF8);
	this->hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
	this->hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	this->initScreenSize();
}

ConsoleState::~ConsoleState() {
	// do nothing
}

void ConsoleState::updateCSBI() {
	GetConsoleScreenBufferInfo(this->hConsoleOutput, &this->csbi);
}

CONSOLE_SCREEN_BUFFER_INFO ConsoleState::getCSBI() {
	this->updateCSBI();
	return this->csbi;
}

HANDLE ConsoleState::getHConsoleInput() {
	return this->hConsoleInput;
}

HANDLE ConsoleState::getHConsoleOutput() {
	return this->hConsoleOutput;
}

void ConsoleState::initScreenSize() {
	this->updateCSBI();
	this->ConsoleWidth = this->csbi.srWindow.Right - this->csbi.srWindow.Left + 1;
	this->ConsoleHeight = this->csbi.srWindow.Bottom - this->csbi.srWindow.Top + 1;
	this->ContentHeight = this->ConsoleHeight - COMMAND_LINE_HEIGHT;
}

COORD ConsoleState::getConsoleSize() {
	return { this->ConsoleWidth, this->ConsoleHeight };
}

short ConsoleState::getContentHeight() {
	return this->ContentHeight;
}

COORD ConsoleState::getCursorPos() {
	this->updateCursorPos();
	return this->cursorPos;
}

void ConsoleState::updateCursorPos() {
	this->updateCSBI();
	this->cursorPos = this->csbi.dwCursorPosition;
}

void ConsoleState::setCursorPos(COORD pos) {
	this->cursorPos = pos;
	SetConsoleCursorPosition(this->hConsoleOutput, pos);
}

void ConsoleState::moveCursor(CursorDirection direction, int step) {
	COORD pos = this->getCursorPos();
	switch (direction) {
	case CURSOR_UP:
		pos.Y -= step;
		if (pos.Y < 0)
			pos.Y = 0;
		break;
	case CURSOR_DOWN:
		pos.Y += step;
		if (pos.Y > this->getContentHeight() - 1)
			pos.Y = this->getContentHeight() - 1;
		break;
	case CURSOR_LEFT:
		pos.X -= step;
		if (pos.X < 0)
			pos.X = 0;
		break;
	case CURSOR_RIGHT:
		pos.X += step;
		if (pos.X > this->getConsoleSize().X - 1)
			pos.X = this->getConsoleSize().X - 1;
		break;
	}
	this->setCursorPos(pos);
}

// 保存控制台状态和内容
ConsoleState ConsoleState::SaveConsoleState() {
	HANDLE hConsoleOutput = this->hConsoleOutput;
	ConsoleState state;
	GetConsoleScreenBufferInfo(hConsoleOutput, &state.csbi);

	int bufferSize = state.csbi.dwSize.X * state.csbi.dwSize.Y;
	state.buffer = new CHAR_INFO[bufferSize];
	COORD bufferCoord = { 0, 0 };
	ReadConsoleOutput(hConsoleOutput, state.buffer, state.csbi.dwSize, bufferCoord, &state.csbi.srWindow);

	state.cursorPos = state.csbi.dwCursorPosition;

	return state;
}

// 恢复控制台状态和内容
void ConsoleState::RestoreConsoleState(ConsoleState state) {
	COORD bufferSize = { state.csbi.dwSize.X, state.csbi.dwSize.Y };
	COORD bufferCoord = { 0, 0 };
	WriteConsoleOutput(state.hConsoleOutput, state.buffer, bufferSize, bufferCoord, const_cast<PSMALL_RECT>(&state.csbi.srWindow));

	SetConsoleCursorPosition(state.hConsoleOutput, state.cursorPos);

	delete[] buffer;
}