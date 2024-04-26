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
		switch (mode) {
		case EDITOR_MODE_INSERT:
			consoleState.printCmd("[INSERT]", FOREGROUND_BLUE | BACKGROUND_GREEN);
			break;
		case EDITOR_MODE_NORMAL:
			consoleState.printCmd("");
			break;
		}
		this->currentMode = mode;
	}
}

EditorMode EditorState::getEditorMode()
{
	return this->currentMode;
}