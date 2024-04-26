#include <iostream>
#include <string>
#include "fileContent.h"
#include "consoleState.h"
#include "consoleText.h"
#include "editorState.h"
#include "toolFuncs.h"

// 全局变量
// 运行标志
bool running_flag = true;

// 运行状态
ConsoleState consoleState{};
EditorState editorState{ consoleState };
ConsoleText consoleText{};

// 函数
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
	// 读取文件相对路径
	if (argc != 2) {
		std::cout << std::endl << "Usage:" << std::endl << "Lab2 <file_path>" << std::endl;
		return 1;
	}
	std::string filePath = argv[1];

	// 保存控制台原始状态和内容
	ConsoleState initialState = ConsoleState{}.SaveConsoleState();

	// 清空控制台
	system("cls");

	// 获取控制台窗口大小
	consoleState.initScreenSize();

	// 读取文件内容
	FileContent fileContent(filePath);
	system("cls");

	// 显示文件内容
	consoleText = ConsoleText{ consoleState, &fileContent, consoleState.getContentHeight() };
	consoleText.show();

	// 打印当前文件名
	bool justOpened = true;
	std::string fileName = filePath.substr(filePath.find_last_of("\\") + 1);
	consoleState.printCmd("Opened: " + fileName, FOREGROUND_GREEN);

	// 将光标位置设置为控制台缓冲区的左上角
	consoleState.setCursorPos({ 0, 0 });

	// 监听键盘和鼠标事件
	INPUT_RECORD inputRecord;
	DWORD numEventsRead;
	INPUT_RECORD cmdInputRecord;
	DWORD cmdNumEventsRead;

	while (running_flag) {
		if (ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &inputRecord, 1, &numEventsRead)) {
			if (numEventsRead > 0) {
				if (inputRecord.EventType == KEY_EVENT) {
					if (justOpened) {
						justOpened = false;
						consoleState.printCmd("");
					}
					if (inputRecord.Event.KeyEvent.bKeyDown) {
						COORD currentCursorPos = consoleState.getCursorPos();
						if (editorState.getEditorMode() == EDITOR_MODE_INSERT) {
							if (inputRecord.Event.KeyEvent.wVirtualKeyCode >= 0x41 && inputRecord.Event.KeyEvent.wVirtualKeyCode <= 0x5A) {
								// 按下字母键
								consoleText.insertChar(inputRecord.Event.KeyEvent.uChar.AsciiChar);
							}
							else if (inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_BACK) {
								// 按下退格键
								consoleText.deleteChar();
							}
							else if (inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_DELETE) {
								consoleText.deleteCharBehind();
							}
							else if (inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_RETURN) {
								// 按下回车键
								//consoleText.newLine();
							}
							else if (inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_SPACE) {
								// 按下空格键
								consoleText.insertChar(' ');
							}
							else if (inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_TAB) {
								// 按下Tab键
								for (int i = 0; i < consoleState.TabWidth; i++) {
									consoleText.insertChar(' ');
								}
							}
						}
						switch (inputRecord.Event.KeyEvent.wVirtualKeyCode) {
						case VK_UP:
						{
							// 按上键
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
							// 按下键
							if (currentCursorPos.Y < consoleState.getContentHeight() - 1) {
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
							// 按右键
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
							// 按左键
							if (currentCursorPos.X > 0) {
								if (backTillChar(CURSOR_LEFT) == '\t')
									consoleState.moveCursor(CURSOR_LEFT, consoleState.TabWidth);
								else consoleState.moveCursor(CURSOR_LEFT, 1);
							}
							break;
						}
						case VK_OEM_1:
						{
							//按下冒号: 进入命令行模式
							COORD currentCursorPos = consoleState.getCursorPos();
							consoleState.setCursorPos({ 0, consoleState.getContentHeight() });
							EditorMode currentMode = editorState.getEditorMode();
							editorState.setEditorMode(EDITOR_MODE_COMMAND);
							std::string cmdStr = "";
							consoleState.printCmd("");
							std::cout << ":";
							while (editorState.getEditorMode() == EDITOR_MODE_COMMAND) {
								ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &cmdInputRecord, 1, &cmdNumEventsRead);
								if (cmdInputRecord.EventType == KEY_EVENT) {
									if (cmdInputRecord.Event.KeyEvent.bKeyDown) {
										if (cmdInputRecord.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
											// 按下ESC退出命令行模式
											editorState.setEditorMode(currentMode);
											consoleState.setCursorPos(currentCursorPos);
										}
										else if (cmdInputRecord.Event.KeyEvent.wVirtualKeyCode == VK_RETURN) {
											// 按下回车键执行命令
											consoleState.setCursorPos(currentCursorPos);
											editorState.setEditorMode(currentMode);
											if (cmdStr == "q") {
												if (fileContent.getHasChanged()) {
													consoleState.printCmd("Has Unsaved Changes. Use :wq to save and quit or :q! to force to quit.", FOREGROUND_RED);
												}
												else {
													running_flag = false;
												}
											}
											else if (cmdStr == "q!") {
												running_flag = false;
											}
											else if (cmdStr == "w") {
												if (fileContent.getHasChanged()) {
													fileContent.saveFile();
													consoleState.printCmd("File Saved.", FOREGROUND_GREEN);
												}
												else {
													consoleState.printCmd("No changes need to be saved.", 0x0e);
												}
											}
											else if (cmdStr == "wq") {
												if (fileContent.getHasChanged()) {
													fileContent.saveFile();
													running_flag = false;
												}
												else {
													consoleState.printCmd("No changes need to be saved.", 0x0e);
												}
											}
											else {
												consoleState.printCmd("Unknown Command: " + cmdStr, FOREGROUND_RED);
											}
											cmdStr = "";
										}
										else if (cmdInputRecord.Event.KeyEvent.wVirtualKeyCode == VK_BACK) {
											// 按下退格键
											if (cmdStr.length() > 0) {
												cmdStr.pop_back();
												consoleState.printCmd("");
												consoleState.setCursorPos({ 0, consoleState.getContentHeight() });
												std::cout << ":" << cmdStr;
											}
										}
										else if (cmdInputRecord.Event.KeyEvent.wVirtualKeyCode == VK_SPACE) {
											// 按下空格键
											cmdStr += " ";
											std::cout << " ";
										}
										else if (cmdInputRecord.Event.KeyEvent.wVirtualKeyCode == VK_SHIFT) {
											// 按下Shift键
											continue;
										}
										else {
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
							// 按下i键，进入插入模式
							editorState.setEditorMode(EDITOR_MODE_INSERT);
							break;
						}
						case 0x4C:
						{
							// 按下l键，开启行号显示
							consoleText.switchLineNumberFlag();
							consoleText.show();
						}
						case VK_ESCAPE:
						{
							// 按下ESC退出程序
							editorState.setEditorMode(EDITOR_MODE_NORMAL);
							break;
						}
						}
					}
				}
			}
		}
	}

	// 清空控制台
	system("cls");

	// 恢复控制台原始状态和内容
	initialState.RestoreConsoleState(initialState);

	return 0;
}