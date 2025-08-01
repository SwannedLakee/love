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

#ifndef LOVE_PHYSICS_BODY_H
#define LOVE_PHYSICS_BODY_H

// LOVE
#include "common/Object.h"
#include "common/StringMap.h"

namespace love
{
namespace physics
{

class Body : public Object
{
public:

	static love::Type type;

	enum Type
	{
		BODY_INVALID,
		BODY_STATIC,
		BODY_DYNAMIC,
		BODY_KINEMATIC,
		BODY_MAX_ENUM
	};

	virtual ~Body();

	static bool getConstant(const char *in, Type &out);
	static bool getConstant(Type in, const char  *&out);
	static std::vector<std::string> getConstants(Type);

private:

	static StringMap<Type, BODY_MAX_ENUM>::Entry typeEntries[];
	static StringMap<Type, BODY_MAX_ENUM> types;
};

} // physics
} // love

#endif // LOVE_PHYSICS_BODY_H
