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
	int fileLineCount;
	std::string filePath;
	std::vector<ContentWithLine> content;
public:
	FileContent();
	FileContent(std::string filePath);
	int getFileLineCount();
	std::vector<ContentWithLine> getContentBetween(int start, int end);
	~FileContent();
};