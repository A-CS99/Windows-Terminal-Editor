#pragma once
#include <string>
#include <fstream>
#include <vector>

struct ContentWithLine {
	int lineNumber;
	std::string content;
};

class FileContent {
private:
	bool isFileExist;
	bool hasChanged;
	int fileLineCount;
	std::string filePath;
	std::vector<ContentWithLine> content;
public:
	FileContent();
	FileContent(std::string filePath);
	int getFileLineCount();
	bool getHasChanged();
	std::vector<ContentWithLine> getContentBetween(int start, int end);
	void updateLine(int lineNo, std::string newLine);
	void saveFile();
	~FileContent();
};