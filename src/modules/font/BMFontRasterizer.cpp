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

// LOVE
#include "BMFontRasterizer.h"
#include "GenericShaper.h"
#include "filesystem/Filesystem.h"
#include "image/Image.h"

// C++
#include <sstream>
#include <vector>
#include <algorithm>

// C
#include <cstdlib>
#include <cstring>

namespace love
{
namespace font
{

namespace
{

/**
 * Helper class for parsing lines in BMFont definition files.
 * NOTE: Does not properly handle multi-value attributes (e.g. 'padding' or
 * 'spacing'.)
 **/
class BMFontLine
{
public:

	BMFontLine(const std::string &line);

	const std::string &getTag() const { return tag; }

	int getAttributeInt(const char *name) const;
	std::string getAttributeString(const char *name) const;

private:

	std::string tag;
	std::unordered_map<std::string, std::string> attributes;

};

// This is not entirely robust...
BMFontLine::BMFontLine(const std::string &line)
{
	// The tag name should always be at the start of the line.
	tag = line.substr(0, line.find(' '));

	size_t startpos = 0;

	while (startpos < line.length())
	{
		// Find the next '=', which indicates a key-value pair.
		size_t fpos = line.find('=', startpos);
		if (fpos == std::string::npos || fpos + 1 >= line.length())
			break;

		// The key should be between a space character and the '='.
		size_t keystart = line.rfind(' ', fpos);
		if (keystart == std::string::npos)
			break;

		keystart++;

		std::string key = line.substr(keystart, fpos - keystart);

		size_t valstart = fpos + 1;
		size_t valend = valstart + 1;

		if (line[valstart] == '"')
		{
			// Values can be surrounded by quotes (a literal string.)
			valstart++;
			valend = line.find('"', valstart) - 1;
		}
		else
		{
			// Otherwise look for the next space character after the '='.
			valend = line.find(' ', valstart + 1) - 1;
		}

		valend = std::min(valend, line.length() - 1);

		attributes[key] = line.substr(valstart, valend - valstart + 1);

		startpos = valend + 1;
	}
}

int BMFontLine::getAttributeInt(const char *name) const
{
	auto it = attributes.find(name);
	if (it == attributes.end())
		return 0;

	return (int) strtol(it->second.c_str(), nullptr, 10);
}

std::string BMFontLine::getAttributeString(const char *name) const
{
	auto it = attributes.find(name);
	if (it == attributes.end())
		return "";

	return it->second;
}

} // anonymous namespace


BMFontRasterizer::BMFontRasterizer(love::filesystem::FileData *fontdef, const std::vector<image::ImageData *> &imagelist, float dpiscale)
	: fontSize(0)
	, unicode(false)
	, lineHeight(0)
{
	this->dpiScale = dpiscale;

	const std::string &filename = fontdef->getFilename();

	size_t separatorpos = filename.rfind('/');
	if (separatorpos != std::string::npos)
		fontFolder = filename.substr(0, separatorpos);

	// The parseConfig function will try to load any missing page images.
	for (int i = 0; i < (int) imagelist.size(); i++)
	{
		if (imagelist[i]->getFormat() != PIXELFORMAT_RGBA8_UNORM)
			throw love::Exception("Only 32-bit RGBA images are supported in BMFonts.");

		images[i] = imagelist[i];
	}

	std::string configtext((const char *) fontdef->getData(), fontdef->getSize());

	parseConfig(configtext);
}

BMFontRasterizer::~BMFontRasterizer()
{
}

void BMFontRasterizer::parseConfig(const std::string &configtext)
{
	{
		BMFontCharacter nullchar = {};
		nullchar.page = -1;
		nullchar.glyph = 0;
		characters.push_back(nullchar);
		characterIndices[0] = (int)characters.size() - 1;
	}

	std::stringstream ss(configtext);
	std::string line;

	while (std::getline(ss, line))
	{
		BMFontLine cline(line);

		const std::string &tag = cline.getTag();

		if (tag == "info")
		{
			fontSize = cline.getAttributeInt("size");
			unicode  = cline.getAttributeInt("unicode") > 0;
		}
		else if (tag == "common")
		{
			lineHeight = cline.getAttributeInt("lineHeight");
			metrics.ascent = cline.getAttributeInt("base");
			metrics.descent = metrics.ascent - lineHeight; // Negative.
		}
		else if (tag == "page")
		{
			int pageindex = cline.getAttributeInt("id");
			std::string filename = cline.getAttributeString("file");

			// The file name is relative to the font file's folder.
			if (!fontFolder.empty())
				filename = fontFolder + "/" + filename;

			// Load the page file from disk into an ImageData, if necessary.
			if (images[pageindex].get() == nullptr)
			{
				using namespace love::filesystem;
				using namespace love::image;

				auto filesystem  = Module::getInstance<Filesystem>(Module::M_FILESYSTEM);
				auto imagemodule = Module::getInstance<image::Image>(Module::M_IMAGE);

				if (!filesystem)
					throw love::Exception("Filesystem module not loaded!");
				if (!imagemodule)
					throw love::Exception("Image module not loaded!");

				// read() returns a retained ref already.
				StrongRef<FileData> data(filesystem->read(filename.c_str()), Acquire::NORETAIN);

				ImageData *imagedata = imagemodule->newImageData(data.get());

				if (imagedata->getFormat() != PIXELFORMAT_RGBA8_UNORM)
				{
					imagedata->release();
					throw love::Exception("Only 32-bit RGBA images are supported in BMFonts.");
				}

				// Same with newImageData.
				images[pageindex].set(imagedata, Acquire::NORETAIN);
			}
		}
		else if (tag == "char")
		{
			BMFontCharacter c = {};

			uint32 id = (uint32) cline.getAttributeInt("id");

			c.x    = cline.getAttributeInt("x");
			c.y    = cline.getAttributeInt("y");
			c.page = cline.getAttributeInt("page");

			c.metrics.width    =  cline.getAttributeInt("width");
			c.metrics.height   =  cline.getAttributeInt("height");
			c.metrics.bearingX =  cline.getAttributeInt("xoffset");
			c.metrics.bearingY = -cline.getAttributeInt("yoffset");
			c.metrics.advance  =  cline.getAttributeInt("xadvance");

			c.glyph = id;

			characters.push_back(c);
			characterIndices[id] = (int) characters.size() - 1;

			metrics.advance = std::max(metrics.advance, c.metrics.advance);
		}
		else if (tag == "kerning")
		{
			uint32 firstid  = (uint32) cline.getAttributeInt("first");
			uint32 secondid = (uint32) cline.getAttributeInt("second");

			uint64 packedids = ((uint64) firstid << 32) | (uint64) secondid;

			kerning[packedids] = cline.getAttributeInt("amount");
		}
	}

	if (characters.size() == 0)
		throw love::Exception("Invalid BMFont file (no character definitions?)");

	// Try to guess the line height if the lineheight attribute isn't found.
	bool guessheight = lineHeight == 0;

	// Verify the glyph character attributes.
	for (const auto &c : characters)
	{
		if (c.glyph == 0)
			continue;

		int width = c.metrics.width;
		int height = c.metrics.height;

		if (!unicode && c.glyph > 127)
			throw love::Exception("Invalid BMFont character id (only unicode and ASCII are supported)");

		if (c.page < 0 || images[c.page].get() == nullptr)
			throw love::Exception("Invalid BMFont character page id: %d", c.page);

		const image::ImageData *id = images[c.page].get();

		if (!id->inside(c.x, c.y))
			throw love::Exception("Invalid coordinates for BMFont character %u.", c.glyph);

		if (width > 0 && !id->inside(c.x + width - 1, c.y))
			throw love::Exception("Invalid width %d for BMFont character %u.", width, c.glyph);

		if (height > 0 && !id->inside(c.x, c.y + height - 1))
			throw love::Exception("Invalid height %d for BMFont character %u.", height, c.glyph);

		if (guessheight)
			lineHeight = std::max(lineHeight, c.metrics.height);
	}

	metrics.height = lineHeight;
}

int BMFontRasterizer::getLineHeight() const
{
	return lineHeight;
}

int BMFontRasterizer::getGlyphSpacing(uint32 glyph) const
{
	auto it = characterIndices.find(glyph);
	if (it == characterIndices.end())
		return 0;

	return characters[it->second].metrics.advance;
}

int BMFontRasterizer::getGlyphIndex(uint32 glyph) const
{
	auto it = characterIndices.find(glyph);
	if (it == characterIndices.end())
		return 0;
	return it->second;
}

GlyphData *BMFontRasterizer::getGlyphDataForIndex(int index) const
{
	// Return an empty GlyphData if we don't have the glyph character.
	if (index < 0 || index >= (int) characters.size())
		return new GlyphData(0, GlyphMetrics(), PIXELFORMAT_RGBA8_UNORM);

	const BMFontCharacter& c = characters[index];
	const auto &imagepair = images.find(c.page);

	if (imagepair == images.end())
		return new GlyphData(c.glyph, GlyphMetrics(), PIXELFORMAT_RGBA8_UNORM);

	image::ImageData *imagedata = imagepair->second.get();
	GlyphData *g = new GlyphData(c.glyph, c.metrics, PIXELFORMAT_RGBA8_UNORM);

	size_t pixelsize = imagedata->getPixelSize();

	uint8 *pixels = (uint8 *) g->getData();
	const uint8 *ipixels = (const uint8 *) imagedata->getData();

	// Always lock the mutex since the user can't know when to do it.
	love::thread::Lock lock(imagedata->getMutex());

	// Copy the subsection of the texture from the ImageData to the GlyphData.
	for (int y = 0; y < c.metrics.height; y++)
	{
		size_t idindex = ((c.y + y) * imagedata->getWidth() + c.x) * pixelsize;
		memcpy(&pixels[y * c.metrics.width * pixelsize], &ipixels[idindex], pixelsize * c.metrics.width);
	}

	return g;
}

int BMFontRasterizer::getGlyphCount() const
{
	return (int) characters.size();
}

bool BMFontRasterizer::hasGlyph(uint32 glyph) const
{
	return characterIndices.find(glyph) != characterIndices.end();
}

float BMFontRasterizer::getKerning(uint32 leftglyph, uint32 rightglyph) const
{
	uint64 packedglyphs = ((uint64) leftglyph << 32) | (uint64) rightglyph;

	auto it = kerning.find(packedglyphs);
	if (it != kerning.end())
		return it->second;

	return 0.0f;
}

Rasterizer::DataType BMFontRasterizer::getDataType() const
{
	return DATA_IMAGE;
}

TextShaper *BMFontRasterizer::newTextShaper()
{
	return new GenericShaper(this);
}

bool BMFontRasterizer::accepts(love::filesystem::FileData *fontdef)
{
	const char *data = (const char *) fontdef->getData();

	// Check if the "info" tag is at the start of the file. This is a truly
	// crappy test. Is the tag even guaranteed to be at the start?
	return fontdef->getSize() > 4 && memcmp(data, "info", 4) == 0;
}

} // font
} // love
