#include "fileContent.h"
#include "toolFuncs.h"
#include <iostream>

FileContent::FileContent() {
	this->isFileExist = false;
	this->hasChanged = false;
	this->filePath = "";
	this->fileLineCount = 0;
	ContentWithLine lineInfo;
	lineInfo.lineNumber = 0;
	lineInfo.content = " ";
	content.push_back(lineInfo);
}

FileContent::FileContent(std::string filePath) {
	this->hasChanged = false;
	this->filePath = filePath;
	std::ifstream file(filePath);
	if (file.is_open()) {
		this->isFileExist = true;
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
		file.close();
	}
	else {
		this->isFileExist = false;
		ContentWithLine lineInfo;
		lineInfo.lineNumber = 1;
		lineInfo.content = "Error: Unable to open file";
		content.push_back(lineInfo);
		this->fileLineCount = 1;
	}
}

int FileContent::getFileLineCount() {
	return fileLineCount;
}

bool FileContent::getHasChanged() {
	return hasChanged;
}

std::vector<ContentWithLine> FileContent::getContentBetween(int start, int end) {
	if (this->fileLineCount == 0) {
		return std::vector<ContentWithLine>();
	}
	std::vector<ContentWithLine> result;
	for (int i = start; i <= end; i++) {
		if (content.size() > i) {
			result.push_back(this->content[i]);
		}
		else {
			ContentWithLine lineInfo;
			lineInfo.lineNumber = i;
			lineInfo.content = "~";
			result.push_back(lineInfo);
		}
	}
	return result;
}

void FileContent::updateLine(int lineNo, std::string newLine) {
	if (lineNo < 1 || lineNo > this->fileLineCount) {
		throw std::runtime_error("RangeOutException");
	}
	this->content[lineNo].content = newLine;
	this->hasChanged = true;
}

void FileContent::saveFile() {
	if (this->hasChanged) {
		std::ofstream file(this->filePath);
		if (file.is_open()) {
			for (int i = 1; i <= this->fileLineCount; i++) {
				file << this->content[i].content << std::endl;
			}
			file.close();
			this->hasChanged = false;
		}
		else {
			std::cout << "Error: Unable to open file " << this->filePath << std::endl;
		}
	}
}

FileContent::~FileContent() {
	content.clear();
}