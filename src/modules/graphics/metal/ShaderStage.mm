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

#include "ShaderStage.h"

#include "libraries/glslang/glslang/Public/ShaderLang.h"

namespace love
{
namespace graphics
{
namespace metal
{

ShaderStage::ShaderStage(love::graphics::Graphics *gfx, ShaderStageType stage, const std::string &source, bool gles, const std::string &cachekey)
	: love::graphics::ShaderStage(gfx, stage, source, gles, cachekey)
{
	// Can't store anything in here since the next part of the compilation
	// pipeline (glslang to generate spir-v) requires linking stages together
	// destructively.
}

ShaderStage::~ShaderStage()
{
}

} // metal
} // graphics
} // love
