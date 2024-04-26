#pragma once
#include "consoleState.h"
#include "consoleText.h"
const enum EditorMode { EDITOR_MODE_NORMAL, EDITOR_MODE_INSERT, EDITOR_MODE_VISUAL, EDITOR_MODE_COMMAND };

class EditorState {
private:
	ConsoleState consoleState;
	EditorMode currentMode;
public:
	EditorState(ConsoleState state);
	~EditorState();
	EditorMode getEditorMode();
	void setEditorMode(EditorMode mode);
};
