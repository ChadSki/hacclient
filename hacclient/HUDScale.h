#pragma once

typedef unsigned char BYTE;

namespace HUDScale {
	void ratioFix(bool command = false);
	void reset();
	void adjust(BYTE* pMouseBuffer);
	void toggle();
};