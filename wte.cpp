#include <iostream>
#include <string>
#include <array>
#include "config.h"
#include "fileContent.h"
#include "consoleState.h"
#include "consoleText.h"
#include "editorState.h"
#include "toolFuncs.h"

// ȫ�ֱ���
std::array<std::string, 13> validPlainExtensions{ "txt", "cpp", "h", "java", "cs", "py", "html", "css", "js", "php", "json", "xml", "yaml" };

// ���б�־
bool running_flag = true;

// ����״̬
ConsoleState consoleState{};
EditorState editorState{ consoleState };
ConsoleText consoleText{};

// ����
char backTillChar(CursorDirection direct) {
	bool findChar = false;
	char targetChar = ' ';
	short moveX = direct == CURSOR_LEFT ? -1 : 0;
	while (!findChar) {
		findChar = true;
		try {
			COORD currentCursorPos = consoleState.getCursorPos();
			targetChar = consoleText.getCharAt(currentCursorPos.Y, currentCursorPos.X + moveX);
		}
		catch (const std::runtime_error& e) {
			if (e.what() == std::string{ "RangeOutException" }) {
				consoleState.moveCursor(CURSOR_LEFT, 1);
				findChar = false;
			}
		}
	}
	return targetChar;
}

int main(int argc, char* argv[]) {
	// ��ȡ�ļ����·��
	if (argc != 2) {
		std::cout << "Expecting for a path arguement." << std::endl;
		std::cout << "Usage:" << std::endl << "wte <file_path>" << std::endl;
		return 1;
	}
	std::string filePath = argv[1];
	std::string fileName = filePath.substr(filePath.find_last_of("\\") + 1);
	std::string extension = fileName.substr(fileName.find_last_of(".") + 1);
	if (fileName.empty()) {
		std::cout << "Invalid File Path." << std::endl;
		return 1;
	}
	else if (fileName.find_last_of(".") == std::string::npos || std::find(validPlainExtensions.begin(), validPlainExtensions.end(), extension) == validPlainExtensions.end()) {
		std::cout << "Can only open plain text files." << std::endl;
		return 1;
	}

	// �������̨ԭʼ״̬������
	ConsoleState initialState = ConsoleState{}.SaveConsoleState();

	// ��տ���̨
	system("cls");

	// ��ȡ����̨���ڴ�С
	consoleState.initScreenSize();

	// ��ȡ�ļ�����
	FileContent fileContent(filePath);
	system("cls");

	// ��ʾ�ļ�����
	consoleText = ConsoleText{ consoleState, &fileContent, consoleState.getContentHeight() };
	consoleText.show();

	// ��ӡ��ǰ�ļ���
	bool justOpened = true;
	consoleState.printCmd("Opened: " + fileName, Config::getAttr(INFO_FRONT));

	// �����λ������Ϊ����̨�����������Ͻ�
	consoleState.setCursorPos({ 0, 0 });

	// �������̺�����¼�
	INPUT_RECORD inputRecord;
	DWORD numEventsRead;
	INPUT_RECORD cmdInputRecord;
	DWORD cmdNumEventsRead;

	while (running_flag) {
		if (ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &inputRecord, 1, &numEventsRead)) {
			if (numEventsRead > 0) {
				if (inputRecord.EventType == KEY_EVENT) {
					if (inputRecord.Event.KeyEvent.bKeyDown) {
						if (justOpened) {
							justOpened = false;
							consoleState.printCmd("");
						}
						COORD currentCursorPos = consoleState.getCursorPos();
						if (editorState.getEditorMode() == EDITOR_MODE_INSERT) {
							if (inputRecord.Event.KeyEvent.wVirtualKeyCode >= 0x41 && inputRecord.Event.KeyEvent.wVirtualKeyCode <= 0x5A) {
								// ������ĸ��
								consoleText.insertChar(inputRecord.Event.KeyEvent.uChar.AsciiChar);
							}
							else if (inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_BACK) {
								// �����˸��
								consoleText.deleteChar();
							}
							else if (inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_DELETE) {
								consoleText.deleteCharBehind();
							}
							else if (inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_RETURN) {
								// ���»س���
								//consoleText.newLine();
							}
							else if (inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_SPACE) {
								// ���¿ո��
								consoleText.insertChar(' ');
							}
							else if (inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_TAB) {
								// ����Tab��
								for (int i = 0; i < consoleState.TabWidth; i++) {
									consoleText.insertChar(' ');
								}
							}
						}
						switch (inputRecord.Event.KeyEvent.wVirtualKeyCode) {
						case VK_UP:
						{
							// ���ϼ�
							if (currentCursorPos.Y > 0) {
								consoleState.moveCursor(CURSOR_UP, 1);
								backTillChar(CURSOR_UP);
							}
							else {
								if (consoleText.isFileStart()) {
									continue;
								}
								consoleText.updateText(-1);
								ScrollConsoleContent(consoleState, 1);
								consoleText.showLine(1);
								consoleState.setCursorPos({ currentCursorPos.X, 0 });
							}
							break;
						}
						case VK_DOWN:
						{
							// ���¼�
							if (currentCursorPos.Y < consoleText.getTextHeight() - 1) {
								consoleState.moveCursor(CURSOR_DOWN, 1);
								backTillChar(CURSOR_DOWN);
							}
							else {
								if (consoleText.isFileEnd()) {
									continue;
								}
								consoleText.updateText(1);
								ScrollConsoleContent(consoleState, -1);
								consoleText.showLine(-1);
								short cursorY = consoleState.getContentHeight() - 1;
								consoleState.setCursorPos({ currentCursorPos.X, cursorY });
							}
							break;
						}
						case VK_RIGHT:
						{
							// ���Ҽ�
							if (currentCursorPos.X < consoleState.getConsoleSize().X - 1) {
								if (backTillChar(CURSOR_RIGHT) == '\t')
									consoleState.moveCursor(CURSOR_RIGHT, consoleState.TabWidth);
								else {
									consoleState.moveCursor(CURSOR_RIGHT, 1);
								}
							}
							break;
						}
						case VK_LEFT:
						{
							// �����
							if (currentCursorPos.X > 0) {
								if (backTillChar(CURSOR_LEFT) == '\t')
									consoleState.moveCursor(CURSOR_LEFT, consoleState.TabWidth);
								else consoleState.moveCursor(CURSOR_LEFT, 1);
							}
							break;
						}
						case VK_OEM_1:
						{
							//����ð��: ����������ģʽ
							COORD currentCursorPos = consoleState.getCursorPos();
							consoleState.setCursorPos({ 0, consoleState.getContentHeight() });
							EditorMode currentMode = editorState.getEditorMode();
							editorState.setEditorMode(EDITOR_MODE_COMMAND);
							std::string cmdStr = "";
							consoleState.printCmd("");
							printWithColor(consoleState, "^_^", BACKGROUND_GREEN | 0x0f);
							std::cout << ": ";
							while (editorState.getEditorMode() == EDITOR_MODE_COMMAND) {
								ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &cmdInputRecord, 1, &cmdNumEventsRead);
								if (cmdInputRecord.EventType == KEY_EVENT) {
									if (cmdInputRecord.Event.KeyEvent.bKeyDown) {
										if (cmdInputRecord.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
											// ����ESC�˳�������ģʽ
											editorState.setEditorMode(currentMode);
											consoleState.setCursorPos(currentCursorPos);
										}
										else if (cmdInputRecord.Event.KeyEvent.wVirtualKeyCode == VK_RETURN) {
											// ���»س���ִ������
											if (cmdStr == "q") {
												if (fileContent.getHasChanged()) {
													consoleState.printErr("Has Unsaved Changes. Use :wq to save and quit or :q! to force to quit.");
												}
												else {
													running_flag = false;
													editorState.setEditorMode(currentMode);
												}
											}
											else if (cmdStr == "q!") {
												running_flag = false;
												editorState.setEditorMode(currentMode);
											}
											else if (cmdStr == "w") {
												editorState.setEditorMode(currentMode);
												if (fileContent.getHasChanged()) {
													fileContent.saveFile();
													consoleState.printInfo("File Saved.");
												}
												else {
													consoleState.printWarn("No changes need to be saved.");
												}
											}
											else if (cmdStr == "wq") {
												if (fileContent.getHasChanged()) {
													fileContent.saveFile();
													running_flag = false;
													editorState.setEditorMode(currentMode);
												}
												else {
													editorState.setEditorMode(currentMode);
													consoleState.printWarn("No changes need to be saved. Try q Only.");
												}
											}
											else if (cmdStr == "set color") {
												consoleState.printCmd("");
												consoleState.setCursorPos({ 0, consoleState.getContentHeight() });
												printWithColor(consoleState, "^_^", BACKGROUND_BLUE | 0x0f);
												std::cout << ": ";
												INPUT_RECORD setInputRecord;
												DWORD setNumEventsRead;
												std::string setStr = "";
												while (true) {
													ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &setInputRecord, 1, &setNumEventsRead);
													if (setInputRecord.EventType == KEY_EVENT) {
														if (setInputRecord.Event.KeyEvent.bKeyDown) {
															if (setInputRecord.Event.KeyEvent.wVirtualKeyCode == VK_RETURN) {
																break;
															}
															else if (setInputRecord.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
																editorState.setEditorMode(currentMode);
																break;
															}
															else if (setInputRecord.Event.KeyEvent.wVirtualKeyCode == VK_BACK) {
																// �����˸��
																if (setStr.length() > 0) {
																	setStr.pop_back();
																	consoleState.printCmd("");
																	consoleState.setCursorPos({ 0, consoleState.getContentHeight() });
																	printWithColor(consoleState, "^_^", BACKGROUND_BLUE | 0x0f);
																	std::cout << ": ";
																	std::cout << setStr;
																}
															}
															else if (setInputRecord.Event.KeyEvent.wVirtualKeyCode == VK_SPACE) {
																// ���¿ո��
																setStr += " ";
																std::cout << " ";
															}
															else if (setInputRecord.Event.KeyEvent.wVirtualKeyCode == VK_OEM_PLUS) {
																setStr += "=";
																std::cout << "=";
															}
															else if (setInputRecord.Event.KeyEvent.wVirtualKeyCode == VK_OEM_5) {
																setStr += "|";
																std::cout << "|";
															}
															else if (setInputRecord.Event.KeyEvent.wVirtualKeyCode >= 0x41 && setInputRecord.Event.KeyEvent.wVirtualKeyCode <= 0x5A) {
																setStr += setInputRecord.Event.KeyEvent.uChar.AsciiChar;
																std::cout << setInputRecord.Event.KeyEvent.uChar.AsciiChar;
															}
														}
													}
												}
												std::string setType = setStr.substr(0, setStr.find_first_of("="));
												std::string setColor = setStr.substr(setStr.find_first_of("=") + 1);
												std::string frontColor = setColor.substr(0, setColor.find_first_of("|"));
												std::string backColor = setColor.substr(setColor.find_first_of("|") + 1);
												if (Config::getFrontColor(frontColor) == -1 || Config::getBackColor(backColor) == -1) {
													consoleState.printErr("Unknown Color: " + frontColor + " or " + backColor);
												}
												else {
													if (setType == "base") {
														Config::setAttr(BASE_FRONT, frontColor);
														Config::setAttr(BASE_BACK, backColor);
														editorState.setEditorMode(currentMode);
														consoleText.show();
													}
													else if (setType == "info") {
														Config::setAttr(INFO_FRONT, frontColor);
														Config::setAttr(INFO_BACK, backColor);
														editorState.setEditorMode(currentMode);
													}
													else if (setType == "warn") {
														Config::setAttr(WARN_FRONT, frontColor);
														Config::setAttr(WARN_BACK, backColor);
														editorState.setEditorMode(currentMode);
													}
													else if (setType == "err") {
														Config::setAttr(ERR_FRONT, frontColor);
														Config::setAttr(ERR_BACK, backColor);
														editorState.setEditorMode(currentMode);
													}
													else {
														consoleState.printErr("Unknown Command: " + setStr + ". Please try <type>=<foregroundColor>|[backgroundColor]");
													}
												}
											}
											else {
												consoleState.printErr("Unknown Command: " + cmdStr);
											}
											cmdStr = "";
											consoleState.setCursorPos(currentCursorPos);
										}
										else if (cmdInputRecord.Event.KeyEvent.wVirtualKeyCode == VK_BACK) {
											// �����˸��
											if (cmdStr.length() > 0) {
												cmdStr.pop_back();
												consoleState.printCmd("");
												consoleState.setCursorPos({ 0, consoleState.getContentHeight() });
												printWithColor(consoleState, "^_^", BACKGROUND_GREEN | 0x0f);
												std::cout << ": ";
												std::cout << cmdStr;
											}
										}
										else if (cmdInputRecord.Event.KeyEvent.wVirtualKeyCode == VK_SPACE) {
											// ���¿ո��
											cmdStr += " ";
											std::cout << " ";
										}
										else if (cmdInputRecord.Event.KeyEvent.wVirtualKeyCode >= 0x41 && cmdInputRecord.Event.KeyEvent.wVirtualKeyCode <= 0x5A) {
											cmdStr += cmdInputRecord.Event.KeyEvent.uChar.AsciiChar;
											std::cout << cmdInputRecord.Event.KeyEvent.uChar.AsciiChar;
										}
									}
								}
							}
							break;
						}
						case 0x49:
						{
							// ����i�����������ģʽ
							editorState.setEditorMode(EDITOR_MODE_INSERT);
							break;
						}
						case 0x4C:
						{
							// ����l���������к���ʾ
							consoleText.switchLineNumberFlag();
							consoleText.show();
						}
						case VK_ESCAPE:
						{
							// ����ESC�˳�����
							editorState.setEditorMode(EDITOR_MODE_NORMAL);
							break;
						}
						}
					}
				}
			}
		}
	}

	// ��տ���̨
	system("cls");

	// �ָ�����̨ԭʼ״̬������
	initialState.RestoreConsoleState(initialState);

	return 0;
}