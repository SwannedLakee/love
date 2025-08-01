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

#ifndef LOVE_FONT_RASTERIZER_H
#define LOVE_FONT_RASTERIZER_H

// LOVE
#include "common/Object.h"
#include "common/int.h"
#include "GlyphData.h"

namespace love
{
namespace font
{

class TextShaper;

/**
 * Holds the specific font metrics.
 **/
struct FontMetrics
{
	int advance;
	int ascent;
	int descent;
	int height;
};

/**
 * Holds data for a font object.
 **/
class Rasterizer : public Object
{
public:

	enum DataType
	{
		DATA_TRUETYPE,
		DATA_IMAGE,
	};

	static love::Type type;

	virtual ~Rasterizer();

	/**
	 * Gets the max height of the glyphs.
	 **/
	virtual int getHeight() const;

	/**
	 * Gets the max advance of the glyphs.
	 **/
	virtual int getAdvance() const;

	/**
	 * Gets the max ascent (height above baseline) for the font.
	 **/
	virtual int getAscent() const;

	/**
	 * Gets the max descent (height below baseline) for the font.
	 **/
	virtual int getDescent() const;

	/**
	 * Gets the line height of the font.
	 **/
	virtual int getLineHeight() const = 0;

	/**
	 * Gets the spacing of the given unicode glyph.
	 **/
	virtual int getGlyphSpacing(uint32 glyph) const = 0;

	/**
	 * Gets a rasterizer-specific index associated with the given glyph.
	 **/
	virtual int getGlyphIndex(uint32 glyph) const = 0;

	/**
	 * Gets a specific glyph.
	 * @param glyph The (UNICODE) glyph codepoint to get data for.
	 **/
	GlyphData *getGlyphData(uint32 glyph) const;

	/**
	 * Gets a specific glyph.
	 * @param text The (UNICODE) glyph character to get the data for.
	 **/
	GlyphData *getGlyphData(const std::string &text) const;

	/**
	 * Gets a specific glyph for the given rasterizer glyph index.
	 **/
	virtual GlyphData *getGlyphDataForIndex(int index) const = 0;

	/**
	 * Gets the number of glyphs the rasterizer has data for.
	 **/
	virtual int getGlyphCount() const = 0;

	/**
	 * Gets whether this Rasterizer has a specific glyph.
	 * @param glyph The (UNICODE) glyph codepoint.
	 **/
	virtual bool hasGlyph(uint32 glyph) const = 0;

	/**
	 * Gets whether this Rasterizer has all the glyphs in a string.
	 * @param text The (UTF-8) string.
	 **/
	virtual bool hasGlyphs(const std::string &text) const;

	/**
	 * Gets the amount of horizontal kerning between two glyphs.
	 **/
	virtual float getKerning(uint32 leftglyph, uint32 rightglyph) const;

	virtual DataType getDataType() const = 0;

	virtual ptrdiff_t getHandle() const { return 0; }

	virtual TextShaper *newTextShaper() = 0;

	float getDPIScale() const;

protected:

	FontMetrics metrics = {};
	float dpiScale = 1.0f;
	bool sdf = false;

}; // Rasterizer

} // font
} // love

#endif // LOVE_FONT_RASTERIZER_H
