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

// 函数

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
	ConsoleText consoleText(consoleState, fileContent, consoleState.getContentHeight());
	consoleText.show();

	// 将光标位置设置为控制台缓冲区的左上角
	consoleState.setCursorPos({ 0, 0 });

	// 监听键盘和鼠标事件
	INPUT_RECORD inputRecord;
	DWORD numEventsRead;

	while (running_flag) {
		if (ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &inputRecord, 1, &numEventsRead)) {
			if (numEventsRead > 0) {
				if (inputRecord.EventType == KEY_EVENT) {
					if (inputRecord.Event.KeyEvent.bKeyDown) {
						COORD currentCursorPos = consoleState.getCursorPos();
						if (inputRecord.Event.KeyEvent.wVirtualKeyCode >= 0x41 && inputRecord.Event.KeyEvent.wVirtualKeyCode <= 0x5A) {
							// 按下字母键
							if (editorState.getEditorMode() == EDITOR_MODE_INSERT) {
								consoleText.insertChar(inputRecord.Event.KeyEvent.uChar.AsciiChar);
							}
						}
						switch (inputRecord.Event.KeyEvent.wVirtualKeyCode) {
						case VK_UP:
							// 按上键
							if (currentCursorPos.Y > 0) {
								consoleState.moveCursor(CURSOR_UP, 1);
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
						case VK_DOWN:
							// 按下键
							if (currentCursorPos.Y < consoleState.getContentHeight() - 1) {
								consoleState.moveCursor(CURSOR_DOWN, 1);
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
						case VK_RIGHT:
							// 按右键
							if (currentCursorPos.X < consoleState.getConsoleSize().X - 1) {
								consoleState.moveCursor(CURSOR_RIGHT, 1);
							}
							break;
						case VK_LEFT:
							// 按左键
							if (currentCursorPos.X > 0) {
								consoleState.moveCursor(CURSOR_LEFT, 1);
							}
							break;
						case VK_OEM_1:
							//按下冒号: 进入命令行模式
							consoleState.setCursorPos({ 0, consoleState.getContentHeight() });
							std::cout << ":";
							if (std::cin.get() == 'q') {
								running_flag = false;
							}
							break;
						case 0x49:
							// 按下i键，进入插入模式
							editorState.setEditorMode(EDITOR_MODE_INSERT);
							break;
						case VK_ESCAPE:
							// 按下ESC退出程序
							editorState.setEditorMode(EDITOR_MODE_NORMAL);
							break;
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