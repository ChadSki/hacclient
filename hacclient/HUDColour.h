#pragma once

typedef unsigned char BYTE;

namespace HUDColour {
	struct RGBColour {
		double red, green, blue;
	};

	struct HSVColour {
		double hue, saturation, value;
	};

	void adjust(BYTE* pMouseBuffer);
	void toggle();
	void colour(const HSVColour& offsets);
	void reset();
	void clear();
	void loadState();
};