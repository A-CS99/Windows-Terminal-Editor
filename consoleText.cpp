#include <iostream>
#include "consoleText.h"

ConsoleText::ConsoleText(ConsoleState state, FileContent fileContent, int textHeight) {
	this->consoleState = state;
	this->with_line_number = false;
	this->text_start = 1;
	this->text_end = textHeight;
	this->textHeight = textHeight;
	this->fileContent = fileContent;
	text = fileContent.getContentBetween(1, textHeight);
}

bool ConsoleText::isTextEmpty() {
	return this->text_start == this->text_end;
}

bool ConsoleText::isFileStart() {
	return this->text_start == 1;
}

bool ConsoleText::isFileEnd() {
	return this->text_end == this->fileContent.getFileLineCount();
}

void ConsoleText::updateText(int start, int end) {
	this->text_start = start;
	this->text_end = end;
	text = fileContent.getContentBetween(start, end);
}

void ConsoleText::updateText(int stepY) {
	this->text_start += stepY;
	this->text_end += stepY;
	text = fileContent.getContentBetween(this->text_start, this->text_end);
}

void ConsoleText::show() {
	consoleState.setCursorPos({ 0, consoleState.getCursorPos().Y });
	for (int i = 1; i <= textHeight; i++) {
		this->showLine(i);
	}
}

void ConsoleText::showLine(int lineNo) {
	if (lineNo < 0) {
		lineNo = this->textHeight + lineNo + 1;
	}
	lineNo--;
	consoleState.setCursorPos({ 0, consoleState.getCursorPos().Y });
	if (this->with_line_number) {
		std::cout << text[lineNo].lineNumber << "\t";
	}
	std::cout << text[lineNo].content << std::endl;
}

void ConsoleText::switchLineNumberFlag() {
	this->with_line_number = !this->with_line_number;
}

void ConsoleText::insertChar(char c) {
	COORD pos = consoleState.getCursorPos();
	if (pos.X < 0 || pos.X > text[pos.Y].content.length()) {
		return;
	}
	text[pos.Y].content.insert(pos.X, 1, c);
	this->showLine(pos.Y + 1);
	pos.X++;
	consoleState.setCursorPos(pos);
}

ConsoleText::~ConsoleText() {
	text.clear();
}