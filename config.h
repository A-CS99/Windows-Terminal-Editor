#pragma once
#include "windows.h"
#include <array>
#include <string>

const enum textAttrType { LNO_FRONT, LNO_BACK, BASE_FRONT, BASE_BACK, ACCEPT_FRONT, ACCEPT_BACK, INFO_FRONT, INFO_BACK, WARN_FRONT, WARN_BACK, ERR_FRONT, ERR_BACK, _END };

class Config
{
private:
	static std::array<WORD, _END> textAttrs;
public:
	static void setAttr(textAttrType type, WORD attr);
	static void setAttr(textAttrType type, std::string colorName);
	static WORD getAttr(textAttrType type);
	static WORD getFrontColor(std::string colorName);
	static WORD getBackColor(std::string colorName);
};
