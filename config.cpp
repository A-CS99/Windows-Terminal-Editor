#include "config.h"

std::array<WORD, _END> Config::textAttrs = { 0x08, 0, 0x0f, 0, FOREGROUND_GREEN, 0, FOREGROUND_BLUE, 0, 0x0e, 0, FOREGROUND_RED, 0 };

void Config::setAttr(textAttrType type, WORD attr) {
	textAttrs[type] = attr;
}

void Config::setAttr(textAttrType type, std::string colorName) {
	textAttrs[type] = type % 2 == 0 ? getFrontColor(colorName) : getBackColor(colorName);
}

WORD Config::getAttr(textAttrType type) {
	return textAttrs[type];
}

WORD Config::getFrontColor(std::string colorName) {
	if (colorName == "black") {
		return 0;
	}
	else if (colorName == "blue") {
		return FOREGROUND_BLUE;
	}
	else if (colorName == "green") {
		return FOREGROUND_GREEN;
	}
	else if (colorName == "red") {
		return FOREGROUND_RED;
	}
	else if (colorName == "yellow") {
		return FOREGROUND_RED | FOREGROUND_GREEN;
	}
	else if (colorName == "purple") {
		return FOREGROUND_RED | FOREGROUND_BLUE;
	}
	else if (colorName == "cyan") {
		return FOREGROUND_GREEN | FOREGROUND_BLUE;
	}
	else if (colorName == "white") {
		return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	}
	else {
		return -0x01;
	}
}

WORD Config::getBackColor(std::string colorName) {
	if (colorName == "black") {
		return 0;
	}
	else if (colorName == "blue") {
		return BACKGROUND_BLUE;
	}
	else if (colorName == "green") {
		return BACKGROUND_GREEN;
	}
	else if (colorName == "red") {
		return BACKGROUND_RED;
	}
	else if (colorName == "yellow") {
		return BACKGROUND_RED | BACKGROUND_GREEN;
	}
	else if (colorName == "purple") {
		return BACKGROUND_RED | BACKGROUND_BLUE;
	}
	else if (colorName == "cyan") {
		return BACKGROUND_GREEN | BACKGROUND_BLUE;
	}
	else if (colorName == "white") {
		return BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
	}
	else {
		return -0x01;
	}
}