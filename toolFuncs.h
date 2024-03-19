#pragma once
#include <iostream>
#include <windows.h>
#include "consoleState.h"

void jumpPrint(COORD pos, std::string str, ConsoleState state);

void ScrollConsoleContent(ConsoleState state, int stepY);
