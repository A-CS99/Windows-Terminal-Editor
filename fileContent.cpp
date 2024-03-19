#include "fileContent.h"
#include <iostream>

FileContent::FileContent() {
	this->filePath = "";
	this->fileLineCount = 0;
	ContentWithLine lineInfo;
	lineInfo.lineNumber = 0;
	lineInfo.content = " ";
	content.push_back(lineInfo);
}

FileContent::FileContent(std::string filePath) {
	this->filePath = filePath;
	std::ifstream file(filePath);
	if (file.is_open()) {
		std::string line;
		int lineNumber = 1;
		ContentWithLine lineInfo;
		lineInfo.lineNumber = 0;
		lineInfo.content = " ";
		content.push_back(lineInfo);
		for (; ; lineNumber++) {
			ContentWithLine lineInfo;
			lineInfo.lineNumber = lineNumber;
			if (getline(file, line)) {
				if (line.empty()) {
					line = " ";
				}
				lineInfo.content = line;
				content.push_back(lineInfo);
			}
			else {
				this->fileLineCount = lineNumber - 1;
				break;
			}
		}
	}
	file.close();
}

int FileContent::getFileLineCount() {
	return fileLineCount;
}

std::vector<ContentWithLine> FileContent::getContentBetween(int start, int end) {
	std::vector<ContentWithLine> result;
	for (int i = start; i <= end; i++) {
		result.push_back(this->content[i]);
	}
	return result;
}

FileContent::~FileContent() {
	content.clear();
}