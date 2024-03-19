#include <iostream>
#include "editorState.h"

EditorState::EditorState(ConsoleState state)
{
	this->consoleState = state;
	this->currentMode = EDITOR_MODE_NORMAL;
}

EditorState::~EditorState()
{
	// do nothing
}

void EditorState::setEditorMode(EditorMode mode)
{
	if (this->currentMode != mode) {
		COORD originCursorPos = this->consoleState.getCursorPos();
		this->consoleState.setCursorPos({ 0, this->consoleState.getContentHeight() });
		switch (mode) {
		case EDITOR_MODE_INSERT:
			std::cout << "[INSERT]";
			break;
		case EDITOR_MODE_NORMAL:
			// Çå³ýÐÐÄÚÈÝ
			std::cout << "          ";
			break;
		}
		this->consoleState.setCursorPos(originCursorPos);
		this->currentMode = mode;
	}
}

EditorMode EditorState::getEditorMode()
{
	return this->currentMode;
}