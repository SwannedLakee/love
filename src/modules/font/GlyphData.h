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

#ifndef LOVE_FONT_GLYPH_DATA_H
#define LOVE_FONT_GLYPH_DATA_H

// LOVE
#include "common/config.h"
#include "common/Data.h"
#include "common/Exception.h"
#include "common/StringMap.h"
#include "common/int.h"
#include "common/pixelformat.h"

// stdlib
#include <string>

namespace love
{
namespace font
{

/**
 * Holds the specific glyph data.
 **/
struct GlyphMetrics
{
	int height;
	int width;
	int advance;
	int bearingX;
	int bearingY;
};

/**
 * Holds data for a specic glyph object.
 **/
class GlyphData : public Data
{
public:

	static love::Type type;

	GlyphData(uint32 glyph, GlyphMetrics glyphMetrics, PixelFormat f);
	GlyphData(const GlyphData &c);
	virtual ~GlyphData();

	// Implements Data.
	GlyphData *clone() const;
	void *getData() const;
	size_t getSize() const;

	/**
	 * Gets the data starting at a specific pixel in the glyph.
	 **/
	void *getData(int x, int y) const;

	/**
	 * Gets the size in bytes of each pixel in the glyph.
	 **/
	size_t getPixelSize() const;

	/**
	 * Gets the height of the glyph.
	 **/
	virtual int getHeight() const;

	/**
	 * Gets the width of the glyph.
	 **/
	virtual int getWidth() const;

	/**
	 * Gets the glyph codepoint itself.
	 **/
	uint32 getGlyph() const;

	/**
	 * Gets the glyph as a UTF-8 string (instead of a UTF-8 code point.)
	 **/
	std::string getGlyphString() const;

	/**
	 * Gets the advance (the space the glyph takes up) of the glyph.
	 **/
	int getAdvance() const;

	/**
	 * Gets bearing (the spacing from origin) along the x-axis of the glyph.
	 **/
	int getBearingX() const;

	/**
	 * Gets bearing (the spacing from origin) along the y-axis of the glyph.
	 **/
	int getBearingY() const;

	/**
	 * Gets the min x value of the glyph.
	 **/
	int getMinX() const;

	/**
	 * Gets the min y value of the glyph.
	 **/
	int getMinY() const;

	/**
	 * Gets the max x value of the glyph.
	 **/
	int getMaxX() const;

	/**
	 * Gets the max y value of the glyph.
	 **/
	int getMaxY() const;

	/**
	 * Gets the format of the glyph data.
	 **/
	PixelFormat getFormat() const;

private:

	// The glyph codepoint itself.
	uint32 glyph;

	// Glyph metrics.
	GlyphMetrics metrics;

	// Glyph texture data.
	uint8 *data;

	// The format the data's in.
	PixelFormat format;

}; // GlyphData

} // font
} // love

#endif // LOVE_FONT_GLYPH_DATA_H
