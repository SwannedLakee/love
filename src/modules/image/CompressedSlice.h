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

#pragma once

// LOVE
#include "common/int.h"
#include "common/pixelformat.h"
#include "data/ByteData.h"
#include "ImageDataBase.h"

namespace love
{
namespace image
{

using ByteData = love::data::ByteData;

// Compressed image data can have multiple mipmap levels, each represented by a
// sub-image.
class CompressedSlice : public ImageDataBase
{
public:

	CompressedSlice(PixelFormat format, int width, int height, ByteData *memory, size_t offset, size_t size);
	CompressedSlice(const CompressedSlice &slice);
	virtual ~CompressedSlice();

	CompressedSlice *clone() const override;
	void *getData() const override { return (uint8 *) memory->getData() + offset; }
	size_t getSize() const override { return dataSize; }
	size_t getOffset() const { return offset; }

private:

	StrongRef<ByteData> memory;
	size_t offset;
	size_t dataSize;

}; // CompressedSlice

} // image
} // love
