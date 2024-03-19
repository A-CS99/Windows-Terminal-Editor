#pragma once
#include "fileContent.h"
#include "consoleState.h"

class ConsoleText {
private:
	bool with_line_number;
	int text_start;
	int text_end;
	int textHeight;
	std::vector<ContentWithLine> text;
	FileContent fileContent;
	ConsoleState consoleState;
public:
	ConsoleText(ConsoleState state, FileContent fileContent, int textHeight);
	bool isTextEmpty();
	bool isFileStart();
	bool isFileEnd();
	void updateText(int start, int end);
	void updateText(int stepY);
	void show();
	void showLine(int lineNo);
	void switchLineNumberFlag();
	void insertChar(char c);
	~ConsoleText();
};
