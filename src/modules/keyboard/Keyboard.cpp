/**
 * Copyright (c) 2006-2025 LOVE Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

#include "common/config.h"
#include "libraries/utf8/utf8.h"

#include "Keyboard.h"

namespace love
{
namespace keyboard
{

Keyboard::Keyboard(const char *name)
	: Module(M_KEYBOARD, name)
{
}

bool Keyboard::getConstant(const char *in, Key &out)
{
	auto it = stringToKey.find(in);
	if (it != stringToKey.end())
	{
		out = (Key)it->second;
		return true;
	}

	std::string text(in);
	uint32 codepoint = 0;

	try
	{
		codepoint = utf8::peek_next(text.begin(), text.end());
	}
	catch (utf8::exception &)
	{
		return false;
	}

	if (codepoint > 0)
	{
		stringToKey[text] = codepoint;
		out = (Key)codepoint;
		return true;
	}

	return false;
}

bool Keyboard::getConstant(Key in, const char *&out)
{
	if (keyToString.empty())
	{
		for (const auto &kvp : stringToKey)
			keyToString[kvp.second] = kvp.first;
	}

	auto it = keyToString.find(in);
	if (it != keyToString.end())
	{
		out = it->second.c_str();
		return true;
	}

	char u[5] = { 0, 0, 0, 0, 0 };
	ptrdiff_t length = 0;

	try
	{
		char *end = utf8::unchecked::append(in, u);
		length = end - u;
	}
	catch (utf8::exception &)
	{
		return false;
	}

	if (length > 0)
	{
		keyToString[in] = std::string(u, length);
		out = keyToString[in].c_str();
		return true;
	}

	return false;
}

bool Keyboard::getConstant(const char *in, Scancode &out)
{
	return scancodes.find(in, out);
}

bool Keyboard::getConstant(Scancode in, const char *&out)
{
	return scancodes.find(in, out);
}

bool Keyboard::getConstant(const char *in, ModifierKey &out)
{
	return modifiers.find(in, out);
}

bool Keyboard::getConstant(ModifierKey in, const char *&out)
{
	return modifiers.find(in, out);
}

std::map<std::string, uint32> Keyboard::stringToKey =
{
	{"unknown", Keyboard::KEY_UNKNOWN},

	{"return", Keyboard::KEY_RETURN},
	{"escape", Keyboard::KEY_ESCAPE},
	{"backspace", Keyboard::KEY_BACKSPACE},
	{"tab", Keyboard::KEY_TAB},
	{"space", Keyboard::KEY_SPACE},
	{"!", Keyboard::KEY_EXCLAIM},
	{"\"", Keyboard::KEY_QUOTEDBL},
	{"#", Keyboard::KEY_HASH},
	{"%", Keyboard::KEY_PERCENT},
	{"$", Keyboard::KEY_DOLLAR},
	{"&", Keyboard::KEY_AMPERSAND},
	{"'", Keyboard::KEY_QUOTE},
	{"(", Keyboard::KEY_LEFTPAREN},
	{")", Keyboard::KEY_RIGHTPAREN},
	{"*", Keyboard::KEY_ASTERISK},
	{"+", Keyboard::KEY_PLUS},
	{",", Keyboard::KEY_COMMA},
	{"-", Keyboard::KEY_MINUS},
	{".", Keyboard::KEY_PERIOD},
	{"/", Keyboard::KEY_SLASH},
	{"0", Keyboard::KEY_0},
	{"1", Keyboard::KEY_1},
	{"2", Keyboard::KEY_2},
	{"3", Keyboard::KEY_3},
	{"4", Keyboard::KEY_4},
	{"5", Keyboard::KEY_5},
	{"6", Keyboard::KEY_6},
	{"7", Keyboard::KEY_7},
	{"8", Keyboard::KEY_8},
	{"9", Keyboard::KEY_9},
	{":", Keyboard::KEY_COLON},
	{";", Keyboard::KEY_SEMICOLON},
	{"<", Keyboard::KEY_LESS},
	{"=", Keyboard::KEY_EQUALS},
	{">", Keyboard::KEY_GREATER},
	{"?", Keyboard::KEY_QUESTION},
	{"@", Keyboard::KEY_AT},

	{"[", Keyboard::KEY_LEFTBRACKET},
	{"\\", Keyboard::KEY_BACKSLASH},
	{"]", Keyboard::KEY_RIGHTBRACKET},
	{"^", Keyboard::KEY_CARET},
	{"_", Keyboard::KEY_UNDERSCORE},
	{"`", Keyboard::KEY_BACKQUOTE},
	{"a", Keyboard::KEY_A},
	{"b", Keyboard::KEY_B},
	{"c", Keyboard::KEY_C},
	{"d", Keyboard::KEY_D},
	{"e", Keyboard::KEY_E},
	{"f", Keyboard::KEY_F},
	{"g", Keyboard::KEY_G},
	{"h", Keyboard::KEY_H},
	{"i", Keyboard::KEY_I},
	{"j", Keyboard::KEY_J},
	{"k", Keyboard::KEY_K},
	{"l", Keyboard::KEY_L},
	{"m", Keyboard::KEY_M},
	{"n", Keyboard::KEY_N},
	{"o", Keyboard::KEY_O},
	{"p", Keyboard::KEY_P},
	{"q", Keyboard::KEY_Q},
	{"r", Keyboard::KEY_R},
	{"s", Keyboard::KEY_S},
	{"t", Keyboard::KEY_T},
	{"u", Keyboard::KEY_U},
	{"v", Keyboard::KEY_V},
	{"w", Keyboard::KEY_W},
	{"x", Keyboard::KEY_X},
	{"y", Keyboard::KEY_Y},
	{"z", Keyboard::KEY_Z},

	{"capslock", Keyboard::KEY_CAPSLOCK},

	{"f1", Keyboard::KEY_F1},
	{"f2", Keyboard::KEY_F2},
	{"f3", Keyboard::KEY_F3},
	{"f4", Keyboard::KEY_F4},
	{"f5", Keyboard::KEY_F5},
	{"f6", Keyboard::KEY_F6},
	{"f7", Keyboard::KEY_F7},
	{"f8", Keyboard::KEY_F8},
	{"f9", Keyboard::KEY_F9},
	{"f10", Keyboard::KEY_F10},
	{"f11", Keyboard::KEY_F11},
	{"f12", Keyboard::KEY_F12},

	{"printscreen", Keyboard::KEY_PRINTSCREEN},
	{"scrolllock", Keyboard::KEY_SCROLLLOCK},
	{"pause", Keyboard::KEY_PAUSE},
	{"insert", Keyboard::KEY_INSERT},
	{"home", Keyboard::KEY_HOME},
	{"pageup", Keyboard::KEY_PAGEUP},
	{"delete", Keyboard::KEY_DELETE},
	{"end", Keyboard::KEY_END},
	{"pagedown", Keyboard::KEY_PAGEDOWN},
	{"right", Keyboard::KEY_RIGHT},
	{"left", Keyboard::KEY_LEFT},
	{"down", Keyboard::KEY_DOWN},
	{"up", Keyboard::KEY_UP},

	{"numlock", Keyboard::KEY_NUMLOCKCLEAR},
	{"kp/", Keyboard::KEY_KP_DIVIDE},
	{"kp*", Keyboard::KEY_KP_MULTIPLY},
	{"kp-", Keyboard::KEY_KP_MINUS},
	{"kp+", Keyboard::KEY_KP_PLUS},
	{"kpenter", Keyboard::KEY_KP_ENTER},
	{"kp0", Keyboard::KEY_KP_0},
	{"kp1", Keyboard::KEY_KP_1},
	{"kp2", Keyboard::KEY_KP_2},
	{"kp3", Keyboard::KEY_KP_3},
	{"kp4", Keyboard::KEY_KP_4},
	{"kp5", Keyboard::KEY_KP_5},
	{"kp6", Keyboard::KEY_KP_6},
	{"kp7", Keyboard::KEY_KP_7},
	{"kp8", Keyboard::KEY_KP_8},
	{"kp9", Keyboard::KEY_KP_9},
	{"kp.", Keyboard::KEY_KP_PERIOD},
	{"kp,", Keyboard::KEY_KP_COMMA},
	{"kp=", Keyboard::KEY_KP_EQUALS},

	{"application", Keyboard::KEY_APPLICATION},
	{"power", Keyboard::KEY_POWER},
	{"f13", Keyboard::KEY_F13},
	{"f14", Keyboard::KEY_F14},
	{"f15", Keyboard::KEY_F15},
	{"f16", Keyboard::KEY_F16},
	{"f17", Keyboard::KEY_F17},
	{"f18", Keyboard::KEY_F18},
	{"f19", Keyboard::KEY_F19},
	{"f20", Keyboard::KEY_F20},
	{"f21", Keyboard::KEY_F21},
	{"f22", Keyboard::KEY_F22},
	{"f23", Keyboard::KEY_F23},
	{"f24", Keyboard::KEY_F24},
	{"execute", Keyboard::KEY_EXECUTE},
	{"help", Keyboard::KEY_HELP},
	{"menu", Keyboard::KEY_MENU},
	{"select", Keyboard::KEY_SELECT},
	{"stop", Keyboard::KEY_STOP},
	{"again", Keyboard::KEY_AGAIN},
	{"undo", Keyboard::KEY_UNDO},
	{"cut", Keyboard::KEY_CUT},
	{"copy", Keyboard::KEY_COPY},
	{"paste", Keyboard::KEY_PASTE},
	{"find", Keyboard::KEY_FIND},
	{"mute", Keyboard::KEY_MUTE},
	{"volumeup", Keyboard::KEY_VOLUMEUP},
	{"volumedown", Keyboard::KEY_VOLUMEDOWN},

	{"alterase", Keyboard::KEY_ALTERASE},
	{"sysreq", Keyboard::KEY_SYSREQ},
	{"cancel", Keyboard::KEY_CANCEL},
	{"clear", Keyboard::KEY_CLEAR},
	{"prior", Keyboard::KEY_PRIOR},
	{"return2", Keyboard::KEY_RETURN2},
	{"separator", Keyboard::KEY_SEPARATOR},
	{"out", Keyboard::KEY_OUT},
	{"oper", Keyboard::KEY_OPER},
	{"clearagain", Keyboard::KEY_CLEARAGAIN},

	{"thousandsseparator", Keyboard::KEY_THOUSANDSSEPARATOR},
	{"decimalseparator", Keyboard::KEY_DECIMALSEPARATOR},
	{"currencyunit", Keyboard::KEY_CURRENCYUNIT},
	{"currencysubunit", Keyboard::KEY_CURRENCYSUBUNIT},

	{"lctrl", Keyboard::KEY_LCTRL},
	{"lshift", Keyboard::KEY_LSHIFT},
	{"lalt", Keyboard::KEY_LALT},
	{"lgui", Keyboard::KEY_LGUI},
	{"rctrl", Keyboard::KEY_RCTRL},
	{"rshift", Keyboard::KEY_RSHIFT},
	{"ralt", Keyboard::KEY_RALT},
	{"rgui", Keyboard::KEY_RGUI},

	{"mode", Keyboard::KEY_MODE},

	{"audionext", Keyboard::KEY_AUDIONEXT},
	{"audioprev", Keyboard::KEY_AUDIOPREV},
	{"audiostop", Keyboard::KEY_AUDIOSTOP},
	{"audioplay", Keyboard::KEY_AUDIOPLAY},
	{"audiomute", Keyboard::KEY_AUDIOMUTE},
	{"mediaselect", Keyboard::KEY_MEDIASELECT},
	{"appsearch", Keyboard::KEY_APP_SEARCH},
	{"apphome", Keyboard::KEY_APP_HOME},
	{"appback", Keyboard::KEY_APP_BACK},
	{"appforward", Keyboard::KEY_APP_FORWARD},
	{"appstop", Keyboard::KEY_APP_STOP},
	{"apprefresh", Keyboard::KEY_APP_REFRESH},
	{"appbookmarks", Keyboard::KEY_APP_BOOKMARKS},

	{"eject", Keyboard::KEY_EJECT},
	{"sleep", Keyboard::KEY_SLEEP},
};

std::map<uint32, std::string> Keyboard::keyToString;

StringMap<Keyboard::Scancode, Keyboard::SCANCODE_MAX_ENUM>::Entry Keyboard::scancodeEntries[] =
{
	{"unknown", SCANCODE_UNKNOWN},

	{"a", SCANCODE_A},
	{"b", SCANCODE_B},
	{"c", SCANCODE_C},
	{"d", SCANCODE_D},
	{"e", SCANCODE_E},
	{"f", SCANCODE_F},
	{"g", SCANCODE_G},
	{"h", SCANCODE_H},
	{"i", SCANCODE_I},
	{"j", SCANCODE_J},
	{"k", SCANCODE_K},
	{"l", SCANCODE_L},
	{"m", SCANCODE_M},
	{"n", SCANCODE_N},
	{"o", SCANCODE_O},
	{"p", SCANCODE_P},
	{"q", SCANCODE_Q},
	{"r", SCANCODE_R},
	{"s", SCANCODE_S},
	{"t", SCANCODE_T},
	{"u", SCANCODE_U},
	{"v", SCANCODE_V},
	{"w", SCANCODE_W},
	{"x", SCANCODE_X},
	{"y", SCANCODE_Y},
	{"z", SCANCODE_Z},

	{"1", SCANCODE_1},
	{"2", SCANCODE_2},
	{"3", SCANCODE_3},
	{"4", SCANCODE_4},
	{"5", SCANCODE_5},
	{"6", SCANCODE_6},
	{"7", SCANCODE_7},
	{"8", SCANCODE_8},
	{"9", SCANCODE_9},
	{"0", SCANCODE_0},

	{"return", SCANCODE_RETURN},
	{"escape", SCANCODE_ESCAPE},
	{"backspace", SCANCODE_BACKSPACE},
	{"tab", SCANCODE_TAB},
	{"space", SCANCODE_SPACE},

	{"-", SCANCODE_MINUS},
	{"=", SCANCODE_EQUALS},
	{"[", SCANCODE_LEFTBRACKET},
	{"]", SCANCODE_RIGHTBRACKET},
	{"\\", SCANCODE_BACKSLASH},
	{"nonus#", SCANCODE_NONUSHASH},
	{";", SCANCODE_SEMICOLON},
	{"'", SCANCODE_APOSTROPHE},
	{"`", SCANCODE_GRAVE},
	{",", SCANCODE_COMMA},
	{".", SCANCODE_PERIOD},
	{"/", SCANCODE_SLASH},

	{"capslock", SCANCODE_CAPSLOCK},

	{"f1", SCANCODE_F1},
	{"f2", SCANCODE_F2},
	{"f3", SCANCODE_F3},
	{"f4", SCANCODE_F4},
	{"f5", SCANCODE_F5},
	{"f6", SCANCODE_F6},
	{"f7", SCANCODE_F7},
	{"f8", SCANCODE_F8},
	{"f9", SCANCODE_F9},
	{"f10", SCANCODE_F10},
	{"f11", SCANCODE_F11},
	{"f12", SCANCODE_F12},

	{"printscreen", SCANCODE_PRINTSCREEN},
	{"scrolllock", SCANCODE_SCROLLLOCK},
	{"pause", SCANCODE_PAUSE},
	{"insert", SCANCODE_INSERT},
	{"home", SCANCODE_HOME},
	{"pageup", SCANCODE_PAGEUP},
	{"delete", SCANCODE_DELETE},
	{"end", SCANCODE_END},
	{"pagedown", SCANCODE_PAGEDOWN},
	{"right", SCANCODE_RIGHT},
	{"left", SCANCODE_LEFT},
	{"down", SCANCODE_DOWN},
	{"up", SCANCODE_UP},

	{"numlock", SCANCODE_NUMLOCKCLEAR},
	{"kp/", SCANCODE_KP_DIVIDE},
	{"kp*", SCANCODE_KP_MULTIPLY},
	{"kp-", SCANCODE_KP_MINUS},
	{"kp+", SCANCODE_KP_PLUS},
	{"kpenter", SCANCODE_KP_ENTER},
	{"kp1", SCANCODE_KP_1},
	{"kp2", SCANCODE_KP_2},
	{"kp3", SCANCODE_KP_3},
	{"kp4", SCANCODE_KP_4},
	{"kp5", SCANCODE_KP_5},
	{"kp6", SCANCODE_KP_6},
	{"kp7", SCANCODE_KP_7},
	{"kp8", SCANCODE_KP_8},
	{"kp9", SCANCODE_KP_9},
	{"kp0", SCANCODE_KP_0},
	{"kp.", SCANCODE_KP_PERIOD},

	{"nonusbackslash", SCANCODE_NONUSBACKSLASH},
	{"application", SCANCODE_APPLICATION},
	{"power", SCANCODE_POWER},
	{"kp=", SCANCODE_KP_EQUALS},
	{"f13", SCANCODE_F13},
	{"f14", SCANCODE_F14},
	{"f15", SCANCODE_F15},
	{"f16", SCANCODE_F16},
	{"f17", SCANCODE_F17},
	{"f18", SCANCODE_F18},
	{"f19", SCANCODE_F19},
	{"f20", SCANCODE_F20},
	{"f21", SCANCODE_F21},
	{"f22", SCANCODE_F22},
	{"f23", SCANCODE_F23},
	{"f24", SCANCODE_F24},
	{"execute", SCANCODE_EXECUTE},
	{"help", SCANCODE_HELP},
	{"menu", SCANCODE_MENU},
	{"select", SCANCODE_SELECT},
	{"stop", SCANCODE_STOP},
	{"again", SCANCODE_AGAIN},
	{"undo", SCANCODE_UNDO},
	{"cut", SCANCODE_CUT},
	{"copy", SCANCODE_COPY},
	{"paste", SCANCODE_PASTE},
	{"find", SCANCODE_FIND},
	{"mute", SCANCODE_MUTE},
	{"volumeup", SCANCODE_VOLUMEUP},
	{"volumedown", SCANCODE_VOLUMEDOWN},
	{"kp,", SCANCODE_KP_COMMA},
	{"kp=400", SCANCODE_KP_EQUALSAS400},

	{"international1", SCANCODE_INTERNATIONAL1},
	{"international2", SCANCODE_INTERNATIONAL2},
	{"international3", SCANCODE_INTERNATIONAL3},
	{"international4", SCANCODE_INTERNATIONAL4},
	{"international5", SCANCODE_INTERNATIONAL5},
	{"international6", SCANCODE_INTERNATIONAL6},
	{"international7", SCANCODE_INTERNATIONAL7},
	{"international8", SCANCODE_INTERNATIONAL8},
	{"international9", SCANCODE_INTERNATIONAL9},
	{"lang1", SCANCODE_LANG1},
	{"lang2", SCANCODE_LANG2},
	{"lang3", SCANCODE_LANG3},
	{"lang4", SCANCODE_LANG4},
	{"lang5", SCANCODE_LANG5},
	{"lang6", SCANCODE_LANG6},
	{"lang7", SCANCODE_LANG7},
	{"lang8", SCANCODE_LANG8},
	{"lang9", SCANCODE_LANG9},

	{"alterase", SCANCODE_ALTERASE},
	{"sysreq", SCANCODE_SYSREQ},
	{"cancel", SCANCODE_CANCEL},
	{"clear", SCANCODE_CLEAR},
	{"prior", SCANCODE_PRIOR},
	{"return2", SCANCODE_RETURN2},
	{"separator", SCANCODE_SEPARATOR},
	{"out", SCANCODE_OUT},
	{"oper", SCANCODE_OPER},
	{"clearagain", SCANCODE_CLEARAGAIN},
	{"crsel", SCANCODE_CRSEL},
	{"exsel", SCANCODE_EXSEL},

	{"kp00", SCANCODE_KP_00},
	{"kp000", SCANCODE_KP_000},
	{"thousandsseparator", SCANCODE_THOUSANDSSEPARATOR},
	{"decimalseparator", SCANCODE_DECIMALSEPARATOR},
	{"currencyunit", SCANCODE_CURRENCYUNIT},
	{"currencysubunit", SCANCODE_CURRENCYSUBUNIT},
	{"kp(", SCANCODE_KP_LEFTPAREN},
	{"kp)", SCANCODE_KP_RIGHTPAREN},
	{"kp{", SCANCODE_KP_LEFTBRACE},
	{"kp}", SCANCODE_KP_RIGHTBRACE},
	{"kptab", SCANCODE_KP_TAB},
	{"kpbackspace", SCANCODE_KP_BACKSPACE},
	{"kpa", SCANCODE_KP_A},
	{"kpb", SCANCODE_KP_B},
	{"kpc", SCANCODE_KP_C},
	{"kpd", SCANCODE_KP_D},
	{"kpe", SCANCODE_KP_E},
	{"kpf", SCANCODE_KP_F},
	{"kpxor", SCANCODE_KP_XOR},
	{"kpower", SCANCODE_KP_POWER},
	{"kp%", SCANCODE_KP_PERCENT},
	{"kp<", SCANCODE_KP_LESS},
	{"kp>", SCANCODE_KP_GREATER},
	{"kp&", SCANCODE_KP_AMPERSAND},
	{"kp&&", SCANCODE_KP_DBLAMPERSAND},
	{"kp|", SCANCODE_KP_VERTICALBAR},
	{"kp||", SCANCODE_KP_DBLVERTICALBAR},
	{"kp:", SCANCODE_KP_COLON},
	{"kp#", SCANCODE_KP_HASH},
	{"kp ", SCANCODE_KP_SPACE},
	{"kp@", SCANCODE_KP_AT},
	{"kp!", SCANCODE_KP_EXCLAM},
	{"kpmemstore", SCANCODE_KP_MEMSTORE},
	{"kpmemrecall", SCANCODE_KP_MEMRECALL},
	{"kpmemclear", SCANCODE_KP_MEMCLEAR},
	{"kpmem+", SCANCODE_KP_MEMADD},
	{"kpmem-", SCANCODE_KP_MEMSUBTRACT},
	{"kpmem*", SCANCODE_KP_MEMMULTIPLY},
	{"kpmem/", SCANCODE_KP_MEMDIVIDE},
	{"kp+-", SCANCODE_KP_PLUSMINUS},
	{"kpclear", SCANCODE_KP_CLEAR},
	{"kpclearentry", SCANCODE_KP_CLEARENTRY},
	{"kpbinary", SCANCODE_KP_BINARY},
	{"kpoctal", SCANCODE_KP_OCTAL},
	{"kpdecimal", SCANCODE_KP_DECIMAL},
	{"kphex", SCANCODE_KP_HEXADECIMAL},

	{"lctrl", SCANCODE_LCTRL},
	{"lshift", SCANCODE_LSHIFT},
	{"lalt", SCANCODE_LALT},
	{"lgui", SCANCODE_LGUI},
	{"rctrl", SCANCODE_RCTRL},
	{"rshift", SCANCODE_RSHIFT},
	{"ralt", SCANCODE_RALT},
	{"rgui", SCANCODE_RGUI},

	{"mode", SCANCODE_MODE},

	{"audionext", SCANCODE_AUDIONEXT},
	{"audioprev", SCANCODE_AUDIOPREV},
	{"audiostop", SCANCODE_AUDIOSTOP},
	{"audioplay", SCANCODE_AUDIOPLAY},
	{"audiomute", SCANCODE_AUDIOMUTE},
	{"mediaselect", SCANCODE_MEDIASELECT},
	{"acsearch", SCANCODE_AC_SEARCH},
	{"achome", SCANCODE_AC_HOME},
	{"acback", SCANCODE_AC_BACK},
	{"acforward", SCANCODE_AC_FORWARD},
	{"acstop", SCANCODE_AC_STOP},
	{"acrefresh", SCANCODE_AC_REFRESH},
	{"acbookmarks", SCANCODE_AC_BOOKMARKS},

	{"eject", SCANCODE_EJECT},
	{"sleep", SCANCODE_SLEEP},
};

StringMap<Keyboard::Scancode, Keyboard::SCANCODE_MAX_ENUM> Keyboard::scancodes(Keyboard::scancodeEntries, sizeof(Keyboard::scancodeEntries));

StringMap<Keyboard::ModifierKey, Keyboard::MODKEY_MAX_ENUM>::Entry Keyboard::modifierEntries[] =
{
	{"numlock", MODKEY_NUMLOCK},
	{"capslock", MODKEY_CAPSLOCK},
	{"scrolllock", MODKEY_SCROLLLOCK},
	{"mode", MODKEY_MODE},
};

StringMap<Keyboard::ModifierKey, Keyboard::MODKEY_MAX_ENUM> Keyboard::modifiers(Keyboard::modifierEntries, sizeof(Keyboard::modifierEntries));

} // keyboard
} // love
