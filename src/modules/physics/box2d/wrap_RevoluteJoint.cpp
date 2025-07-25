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

#include "wrap_RevoluteJoint.h"
#include "wrap_Physics.h"

namespace love
{
namespace physics
{
namespace box2d
{

RevoluteJoint *luax_checkrevolutejoint(lua_State *L, int idx)
{
	RevoluteJoint *j = luax_checktype<RevoluteJoint>(L, idx);
	if (!j->isValid())
		luaL_error(L, "Attempt to use destroyed joint.");
	return j;
}

int w_RevoluteJoint_getJointAngle(lua_State *L)
{
	RevoluteJoint *t = luax_checkrevolutejoint(L, 1);
	lua_pushnumber(L, t->getJointAngle());
	return 1;
}

int w_RevoluteJoint_getJointSpeed(lua_State *L)
{
	RevoluteJoint *t = luax_checkrevolutejoint(L, 1);
	lua_pushnumber(L, t->getJointSpeed());
	return 1;
}

int w_RevoluteJoint_setMotorEnabled(lua_State *L)
{
	RevoluteJoint *t = luax_checkrevolutejoint(L, 1);
	bool arg1 = luax_checkboolean(L, 2);
	t->setMotorEnabled(arg1);
	return 0;
}

int w_RevoluteJoint_isMotorEnabled(lua_State *L)
{
	RevoluteJoint *t = luax_checkrevolutejoint(L, 1);
	luax_pushboolean(L, t->isMotorEnabled());
	return 1;
}

int w_RevoluteJoint_setMaxMotorTorque(lua_State *L)
{
	RevoluteJoint *t = luax_checkrevolutejoint(L, 1);
	float arg1 = (float)luaL_checknumber(L, 2);
	t->setMaxMotorTorque(arg1);
	return 0;
}

int w_RevoluteJoint_setMotorSpeed(lua_State *L)
{
	RevoluteJoint *t = luax_checkrevolutejoint(L, 1);
	float arg1 = (float)luaL_checknumber(L, 2);
	t->setMotorSpeed(arg1);
	return 0;
}

int w_RevoluteJoint_getMotorSpeed(lua_State *L)
{
	RevoluteJoint *t = luax_checkrevolutejoint(L, 1);
	lua_pushnumber(L, t->getMotorSpeed());
	return 1;
}

int w_RevoluteJoint_getMotorTorque(lua_State *L)
{
	RevoluteJoint *t = luax_checkrevolutejoint(L, 1);
	float dt = (float)luaL_checknumber(L, 2);
	lua_pushnumber(L, t->getMotorTorque(dt));
	return 1;
}

int w_RevoluteJoint_getMaxMotorTorque(lua_State *L)
{
	RevoluteJoint *t = luax_checkrevolutejoint(L, 1);
	lua_pushnumber(L, t->getMaxMotorTorque());
	return 1;
}

int w_RevoluteJoint_setLimitsEnabled(lua_State *L)
{
	RevoluteJoint *t = luax_checkrevolutejoint(L, 1);
	bool arg1 = luax_checkboolean(L, 2);
	t->setLimitsEnabled(arg1);
	return 0;
}

int w_RevoluteJoint_areLimitsEnabled(lua_State *L)
{
	RevoluteJoint *t = luax_checkrevolutejoint(L, 1);
	luax_pushboolean(L, t->areLimitsEnabled());
	return 1;
}

int w_RevoluteJoint_setUpperLimit(lua_State *L)
{
	RevoluteJoint *t = luax_checkrevolutejoint(L, 1);
	float arg1 = (float)luaL_checknumber(L, 2);
	luax_catchexcept(L, [&](){ t->setUpperLimit(arg1); });
	return 0;
}

int w_RevoluteJoint_setLowerLimit(lua_State *L)
{
	RevoluteJoint *t = luax_checkrevolutejoint(L, 1);
	float arg1 = (float)luaL_checknumber(L, 2);
	luax_catchexcept(L, [&](){ t->setLowerLimit(arg1); });
	return 0;
}

int w_RevoluteJoint_setLimits(lua_State *L)
{
	RevoluteJoint *t = luax_checkrevolutejoint(L, 1);
	float arg1 = (float)luaL_checknumber(L, 2);
	float arg2 = (float)luaL_checknumber(L, 3);
	luax_catchexcept(L, [&](){ t->setLimits(arg1, arg2); });
	return 0;
}

int w_RevoluteJoint_getLowerLimit(lua_State *L)
{
	RevoluteJoint *t = luax_checkrevolutejoint(L, 1);
	lua_pushnumber(L, t->getLowerLimit());
	return 1;
}

int w_RevoluteJoint_getUpperLimit(lua_State *L)
{
	RevoluteJoint *t = luax_checkrevolutejoint(L, 1);
	lua_pushnumber(L, t->getUpperLimit());
	return 1;
}

int w_RevoluteJoint_getLimits(lua_State *L)
{
	RevoluteJoint *t = luax_checkrevolutejoint(L, 1);
	lua_remove(L, 1);
	return t->getLimits(L);
}

int w_RevoluteJoint_getReferenceAngle(lua_State *L)
{
	RevoluteJoint *t = luax_checkrevolutejoint(L, 1);
	lua_pushnumber(L, t->getReferenceAngle());
	return 1;
}

static const luaL_Reg w_RevoluteJoint_functions[] =
{
	{ "getJointAngle", w_RevoluteJoint_getJointAngle },
	{ "getJointSpeed", w_RevoluteJoint_getJointSpeed },
	{ "setMotorEnabled", w_RevoluteJoint_setMotorEnabled },
	{ "isMotorEnabled", w_RevoluteJoint_isMotorEnabled },
	{ "setMaxMotorTorque", w_RevoluteJoint_setMaxMotorTorque },
	{ "setMotorSpeed", w_RevoluteJoint_setMotorSpeed },
	{ "getMotorSpeed", w_RevoluteJoint_getMotorSpeed },
	{ "getMotorTorque", w_RevoluteJoint_getMotorTorque },
	{ "getMaxMotorTorque", w_RevoluteJoint_getMaxMotorTorque },
	{ "setLimitsEnabled", w_RevoluteJoint_setLimitsEnabled },
	{ "areLimitsEnabled", w_RevoluteJoint_areLimitsEnabled },
	{ "setUpperLimit", w_RevoluteJoint_setUpperLimit },
	{ "setLowerLimit", w_RevoluteJoint_setLowerLimit },
	{ "setLimits", w_RevoluteJoint_setLimits },
	{ "getLowerLimit", w_RevoluteJoint_getLowerLimit },
	{ "getUpperLimit", w_RevoluteJoint_getUpperLimit },
	{ "getLimits", w_RevoluteJoint_getLimits },
	{ "getReferenceAngle", w_RevoluteJoint_getReferenceAngle },

	{ 0, 0 }
};

extern "C" int luaopen_revolutejoint(lua_State *L)
{
	return luax_register_type(L, &RevoluteJoint::type, w_Joint_functions, w_RevoluteJoint_functions, nullptr);
}

} // box2d
} // physics
} // love
