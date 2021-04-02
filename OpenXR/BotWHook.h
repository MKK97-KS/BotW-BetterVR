#pragma once

enum class HOOK_MODE : byte {
	DISABLED = 0,
	GFX_PACK_ENABLED = 1,
	GFX_PACK_NOT_ENABLED = 2,
	BOTH_ENABLED_GFX_CALC = 3,
	BOTH_ENABLED_PRECALC = 4
};

void InitializeCemuHooking();
void SetBotWPositions(XrView leftScreen, XrView rightScreen);
HWND getCemuHWND();
HOOK_MODE getHookMode();
void setCemuFullScreen(bool enabled);
bool getCemuFullScreen();
