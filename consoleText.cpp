#include <iostream>
#include <stdexcept>
#include "consoleText.h"

ConsoleText::ConsoleText() {
	this->with_line_number = false;
	this->text_start = 1;
	this->text_end = 1;
	this->textHeight = 1;
	this->fileContent = new FileContent();
}

ConsoleText::ConsoleText(ConsoleState state, FileContent* fileContent, int textHeight) {
	this->consoleState = state;
	this->with_line_number = false;
	this->text_start = 1;
	this->text_end = textHeight;
	this->textHeight = textHeight;
	this->fileContent = fileContent;
	text = fileContent->getContentBetween(1, textHeight);
}

bool ConsoleText::isTextEmpty() {
	return this->text_start == this->text_end;
}

bool ConsoleText::isFileStart() {
	return this->text_start == 1;
}

bool ConsoleText::isFileEnd() {
	return this->text_end == this->fileContent->getFileLineCount();
}

void ConsoleText::updateText(int start, int end) {
	this->text_start = start;
	this->text_end = end;
	text = fileContent->getContentBetween(start, end);
}

void ConsoleText::updateText(int stepY) {
	this->text_start += stepY;
	this->text_end += stepY;
	text = fileContent->getContentBetween(this->text_start, this->text_end);
}

void ConsoleText::show() {
	COORD pos = consoleState.getCursorPos();
	int contentHeight = consoleState.getContentHeight();
	consoleState.setCursorPos({ 0, 0 });
	for (int i = 1; i <= textHeight; i++) {
		this->showLine(i);
	}
	consoleState.setCursorPos(pos);
}

void ConsoleText::showLine(int lineNo) {
	COORD cursorPos = consoleState.getCursorPos();
	if (lineNo < 0) {
		lineNo = this->textHeight + lineNo + 1;
	}
	lineNo--;
	consoleState.setCursorPos({ 0, cursorPos.Y });
	std::string blankLine = "";
	for (int i = 0; i < consoleState.getConsoleSize().X; i++) {
		blankLine += " ";
	}
	std::cout << blankLine;
	consoleState.setCursorPos({ 0, cursorPos.Y });
	if (text.size() == 0) {
		std::cout << " " << std::endl;
		return;
	}
	if (this->with_line_number) {
		// 获取当前文本属性
		WORD oldTextAttr = consoleState.getCSBI().wAttributes;
		SetConsoleTextAttribute(consoleState.getHConsoleOutput(), 0x08);
		std::cout << text[lineNo].lineNumber << "\t";
		// 恢复文本属性
		SetConsoleTextAttribute(consoleState.getHConsoleOutput(), oldTextAttr);
	}
	std::cout << text[lineNo].content << std::endl;
}

void ConsoleText::switchLineNumberFlag() {
	this->with_line_number = !this->with_line_number;
}

char ConsoleText::getCharAt(int lineNo, short pos) {
	int index = -1;
	char curChar;
	std::string contentStr = text[lineNo].content;
	while (pos >= 0) {
		index++;
		if (index < 0 || index >= contentStr.length()) {
			throw std::runtime_error("RangeOutException");
		}
		curChar = contentStr[index];
		if (curChar == '\t') {
			pos -= consoleState.TabWidth;
		}
		else {
			pos--;
		}
	};
	return curChar;
}

void ConsoleText::insertChar(char c) {
	COORD pos = consoleState.getCursorPos();
	if (pos.X < 0 || pos.X > text[pos.Y].content.length()) {
		return;
	}
	text[pos.Y].content.insert(pos.X, 1, c);
	fileContent->updateLine(text[pos.Y].lineNumber, text[pos.Y].content);
	this->showLine(pos.Y + 1);
	pos.X++;
	consoleState.setCursorPos(pos);
}

void ConsoleText::deleteChar() {
	COORD pos = consoleState.getCursorPos();
	if (pos.X < 0 || pos.X >= text[pos.Y].content.length()) {
		return;
	}
	text[pos.Y].content.erase(pos.X - 1, 1);
	fileContent->updateLine(text[pos.Y].lineNumber, text[pos.Y].content);
	this->showLine(pos.Y + 1);
	pos.X--;
	consoleState.setCursorPos(pos);
}

void ConsoleText::deleteCharBehind() {
	COORD pos = consoleState.getCursorPos();
	if (pos.X < 0 || pos.X >= text[pos.Y].content.length()) {
		return;
	}
	text[pos.Y].content.erase(pos.X, 1);
	fileContent->updateLine(text[pos.Y].lineNumber, text[pos.Y].content);
	this->showLine(pos.Y + 1);
	consoleState.setCursorPos(pos);
}

void ConsoleText::newLine() {
	/*COORD pos = consoleState.getCursorPos();
	std::string newLine = text[pos.Y].content.substr(pos.X);
	text[pos.Y].content = text[pos.Y].content.substr(0, pos.X);
	fileContent.updateLine(text[pos.Y].lineNumber, text[pos.Y].content);
	ContentWithLine newLineInfo;
	newLineInfo.lineNumber = text[pos.Y].lineNumber + 1;
	newLineInfo.content = newLine;
	text.insert(text.begin() + pos.Y + 1, newLineInfo);
	fileContent.updateLine(newLineInfo.lineNumber, newLineInfo.content);
	this->updateText(this->text_start, this->text_end + 1);
	this->show();
	pos.Y++;
	pos.X = 0;
	consoleState.setCursorPos(pos);*/
}

ConsoleText::~ConsoleText() {
	text.clear();
}