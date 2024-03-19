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

// ����

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
	ConsoleText consoleText(consoleState, fileContent, consoleState.getContentHeight());
	consoleText.show();

	// �����λ������Ϊ����̨�����������Ͻ�
	consoleState.setCursorPos({ 0, 0 });

	// �������̺�����¼�
	INPUT_RECORD inputRecord;
	DWORD numEventsRead;

	while (running_flag) {
		if (ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &inputRecord, 1, &numEventsRead)) {
			if (numEventsRead > 0) {
				if (inputRecord.EventType == KEY_EVENT) {
					if (inputRecord.Event.KeyEvent.bKeyDown) {
						COORD currentCursorPos = consoleState.getCursorPos();
						if (inputRecord.Event.KeyEvent.wVirtualKeyCode >= 0x41 && inputRecord.Event.KeyEvent.wVirtualKeyCode <= 0x5A) {
							// ������ĸ��
							if (editorState.getEditorMode() == EDITOR_MODE_INSERT) {
								consoleText.insertChar(inputRecord.Event.KeyEvent.uChar.AsciiChar);
							}
						}
						switch (inputRecord.Event.KeyEvent.wVirtualKeyCode) {
						case VK_UP:
							// ���ϼ�
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
							// ���¼�
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
							// ���Ҽ�
							if (currentCursorPos.X < consoleState.getConsoleSize().X - 1) {
								consoleState.moveCursor(CURSOR_RIGHT, 1);
							}
							break;
						case VK_LEFT:
							// �����
							if (currentCursorPos.X > 0) {
								consoleState.moveCursor(CURSOR_LEFT, 1);
							}
							break;
						case VK_OEM_1:
							//����ð��: ����������ģʽ
							consoleState.setCursorPos({ 0, consoleState.getContentHeight() });
							std::cout << ":";
							if (std::cin.get() == 'q') {
								running_flag = false;
							}
							break;
						case 0x49:
							// ����i�����������ģʽ
							editorState.setEditorMode(EDITOR_MODE_INSERT);
							break;
						case VK_ESCAPE:
							// ����ESC�˳�����
							editorState.setEditorMode(EDITOR_MODE_NORMAL);
							break;
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