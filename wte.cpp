#include <iostream>
#include <string>
#include "fileContent.h"
#include "consoleState.h"
#include "consoleText.h"
#include "editorState.h"
#include "toolFuncs.h"

// ȫ�ֱ���
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
		std::cout << std::endl << "Usage:" << std::endl << "Lab2 <file_path>" << std::endl;
		return 1;
	}
	std::string filePath = argv[1];

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
	std::string fileName = filePath.substr(filePath.find_last_of("\\") + 1);
	consoleState.printCmd("Opened: " + fileName, FOREGROUND_GREEN);

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
					if (justOpened) {
						justOpened = false;
						consoleState.printCmd("");
					}
					if (inputRecord.Event.KeyEvent.bKeyDown) {
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
							std::cout << ":";
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
											// �����˸��
											if (cmdStr.length() > 0) {
												cmdStr.pop_back();
												consoleState.printCmd("");
												consoleState.setCursorPos({ 0, consoleState.getContentHeight() });
												std::cout << ":" << cmdStr;
											}
										}
										else if (cmdInputRecord.Event.KeyEvent.wVirtualKeyCode == VK_SPACE) {
											// ���¿ո��
											cmdStr += " ";
											std::cout << " ";
										}
										else if (cmdInputRecord.Event.KeyEvent.wVirtualKeyCode == VK_SHIFT) {
											// ����Shift��
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