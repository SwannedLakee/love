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

#include "wrap_Math.h"
#include "wrap_RandomGenerator.h"
#include "wrap_BezierCurve.h"
#include "wrap_Transform.h"
#include "MathModule.h"
#include "BezierCurve.h"
#include "Transform.h"

#include <cmath>
#include <iostream>
#include <algorithm>

// Put the Lua code directly into a raw string literal.
static const char math_lua[] =
#include "wrap_Math.lua"
;

namespace love
{
namespace math
{

#define instance() (Module::getInstance<Math>(Module::M_MATH))

int w__getRandomGenerator(lua_State *L)
{
	RandomGenerator *t = instance()->getRandomGenerator();
	luax_pushtype(L, t);
	return 1;
}

int w_newRandomGenerator(lua_State *L)
{
	RandomGenerator::Seed s;
	if (lua_gettop(L) > 0)
		s = luax_checkrandomseed(L, 1);

	RandomGenerator *t = instance()->newRandomGenerator();

	if (lua_gettop(L) > 0)
	{
		bool should_error = false;

		try
		{
			t->setSeed(s);
		}
		catch (love::Exception &e)
		{
			t->release();
			should_error = true;
			lua_pushstring(L, e.what());
		}

		if (should_error)
			return luaL_error(L, "%s", lua_tostring(L, -1));
	}

	luax_pushtype(L, t);
	t->release();
	return 1;
}

int w_newBezierCurve(lua_State *L)
{
	std::vector<Vector2> points;
	if (lua_istable(L, 1))
	{
		int top = (int) luax_objlen(L, 1);
		points.reserve(top / 2);
		for (int i = 1; i <= top; i += 2)
		{
			lua_rawgeti(L, 1, i);
			lua_rawgeti(L, 1, i+1);

			Vector2 v;
			v.x = (float) luaL_checknumber(L, -2);
			v.y = (float) luaL_checknumber(L, -1);
			points.push_back(v);

			lua_pop(L, 2);
		}
	}
	else
	{
		int top = (int) lua_gettop(L);
		points.reserve(top / 2);
		for (int i = 1; i <= top; i += 2)
		{
			Vector2 v;
			v.x = (float) luaL_checknumber(L, i);
			v.y = (float) luaL_checknumber(L, i+1);
			points.push_back(v);
		}
	}

	BezierCurve *curve = instance()->newBezierCurve(points);
	luax_pushtype(L, curve);
	curve->release();
	return 1;
}

int w_newTransform(lua_State *L)
{
	Transform *t = nullptr;

	if (lua_isnoneornil(L, 1))
		t = instance()->newTransform();
	else
	{
		float x =  (float) luaL_checknumber(L, 1);
		float y =  (float) luaL_checknumber(L, 2);
		float a =  (float) luaL_optnumber(L, 3, 0.0);
		float sx = (float) luaL_optnumber(L, 4, 1.0);
		float sy = (float) luaL_optnumber(L, 5, sx);
		float ox = (float) luaL_optnumber(L, 6, 0.0);
		float oy = (float) luaL_optnumber(L, 7, 0.0);
		float kx = (float) luaL_optnumber(L, 8, 0.0);
		float ky = (float) luaL_optnumber(L, 9, 0.0);
		t = instance()->newTransform(x, y, a, sx, sy, ox, oy, kx, ky);
	}

	luax_pushtype(L, t);
	t->release();
	return 1;
}

int w_triangulate(lua_State *L)
{
	std::vector<love::Vector2> vertices;
	if (lua_istable(L, 1))
	{
		int top = (int) luax_objlen(L, 1);
		vertices.reserve(top / 2);
		for (int i = 1; i <= top; i += 2)
		{
			lua_rawgeti(L, 1, i);
			lua_rawgeti(L, 1, i+1);

			Vector2 v;
			v.x = (float) luaL_checknumber(L, -2);
			v.y = (float) luaL_checknumber(L, -1);
			vertices.push_back(v);

			lua_pop(L, 2);
		}
	}
	else
	{
		int top = (int) lua_gettop(L);
		vertices.reserve(top / 2);
		for (int i = 1; i <= top; i += 2)
		{
			Vector2 v;
			v.x = (float) luaL_checknumber(L, i);
			v.y = (float) luaL_checknumber(L, i+1);
			vertices.push_back(v);
		}
	}

	if (vertices.size() < 3)
		return luaL_error(L, "Need at least 3 vertices to triangulate (got %d).", (int)vertices.size());

	std::vector<Triangle> triangles;

	luax_catchexcept(L, [&]() {
		if (vertices.size() == 3)
			triangles.push_back(Triangle(vertices[0], vertices[1], vertices[2]));
		else
			triangles = triangulate(vertices);
	});

	lua_createtable(L, (int) triangles.size(), 0);
	for (int i = 0; i < (int) triangles.size(); ++i)
	{
		const Triangle &tri = triangles[i];

		lua_createtable(L, 6, 0);
		lua_pushnumber(L, tri.a.x);
		lua_rawseti(L, -2, 1);
		lua_pushnumber(L, tri.a.y);
		lua_rawseti(L, -2, 2);
		lua_pushnumber(L, tri.b.x);
		lua_rawseti(L, -2, 3);
		lua_pushnumber(L, tri.b.y);
		lua_rawseti(L, -2, 4);
		lua_pushnumber(L, tri.c.x);
		lua_rawseti(L, -2, 5);
		lua_pushnumber(L, tri.c.y);
		lua_rawseti(L, -2, 6);

		lua_rawseti(L, -2, i+1);
	}

	return 1;
}

int w_isConvex(lua_State *L)
{
	std::vector<love::Vector2> vertices;
	if (lua_istable(L, 1))
	{
		int top = (int) luax_objlen(L, 1);
		vertices.reserve(top / 2);
		for (int i = 1; i <= top; i += 2)
		{
			lua_rawgeti(L, 1, i);
			lua_rawgeti(L, 1, i+1);

			love::Vector2 v;
			v.x = (float) luaL_checknumber(L, -2);
			v.y = (float) luaL_checknumber(L, -1);
			vertices.push_back(v);

			lua_pop(L, 2);
		}
	}
	else
	{
		int top = lua_gettop(L);
		vertices.reserve(top / 2);
		for (int i = 1; i <= top; i += 2)
		{
			love::Vector2 v;
			v.x = (float) luaL_checknumber(L, i);
			v.y = (float) luaL_checknumber(L, i+1);
			vertices.push_back(v);
		}
	}

	luax_pushboolean(L, isConvex(vertices));
	return 1;
}

static int getGammaArgs(lua_State *L, float color[4])
{
	int numcomponents = 0;

	if (lua_istable(L, 1))
	{
		int n = (int) luax_objlen(L, 1);
		for (int i = 1; i <= n && i <= 4; i++)
		{
			lua_rawgeti(L, 1, i);
			color[i - 1] = (float) luax_checknumberclamped01(L, -1);
			numcomponents++;
		}

		lua_pop(L, numcomponents);
	}
	else
	{
		int n = lua_gettop(L);
		for (int i = 1; i <= n && i <= 4; i++)
		{
			color[i - 1] = (float) luax_checknumberclamped01(L, i);
			numcomponents++;
		}
	}

	if (numcomponents == 0)
		luaL_checknumber(L, 1);

	return numcomponents;
}

int w_gammaToLinear(lua_State *L)
{
	float color[4];
	int numcomponents = getGammaArgs(L, color);

	for (int i = 0; i < numcomponents; i++)
	{
		// Alpha should always be linear.
		if (i < 3)
			color[i] = gammaToLinear(color[i]);
		lua_pushnumber(L, color[i]);
	}

	return numcomponents;
}

int w_linearToGamma(lua_State *L)
{
	float color[4];
	int numcomponents = getGammaArgs(L, color);

	for (int i = 0; i < numcomponents; i++)
	{
		// Alpha should always be linear.
		if (i < 3)
			color[i] = linearToGamma(color[i]);
		lua_pushnumber(L, color[i]);
	}

	return numcomponents;
}

int w_noise(lua_State *L)
{
	luax_markdeprecated(L, 1, "love.math.noise", API_FUNCTION, DEPRECATED_REPLACED, "love.math.perlinNoise or love.math.simplexNoise");

	int nargs = std::min(std::max(lua_gettop(L), 1), 4);
	double args[4];

	for (int i = 0; i < nargs; i++)
		args[i] = luaL_checknumber(L, i + 1);

	double val = 0.0;

	switch (nargs)
	{
	case 1:
		val = simplexNoise1(args[0]);
		break;
	case 2:
		val = simplexNoise2(args[0], args[1]);
		break;
	case 3:
		val = perlinNoise3(args[0], args[1], args[2]);
		break;
	case 4:
		val = perlinNoise4(args[0], args[1], args[2], args[3]);
		break;
	}

	lua_pushnumber(L, (lua_Number) val);
	return 1;
}

int w_perlinNoise(lua_State* L)
{
	int nargs = std::min(std::max(lua_gettop(L), 1), 4);
	double args[4];

	for (int i = 0; i < nargs; i++)
		args[i] = luaL_checknumber(L, i + 1);

	double val = 0.0;

	switch (nargs)
	{
	case 1:
		val = perlinNoise1(args[0]);
		break;
	case 2:
		val = perlinNoise2(args[0], args[1]);
		break;
	case 3:
		val = perlinNoise3(args[0], args[1], args[2]);
		break;
	case 4:
		val = perlinNoise4(args[0], args[1], args[2], args[3]);
		break;
	}

	lua_pushnumber(L, (lua_Number) val);
	return 1;
}

int w_simplexNoise(lua_State* L)
{
	int nargs = std::min(std::max(lua_gettop(L), 1), 4);
	double args[4];

	for (int i = 0; i < nargs; i++)
		args[i] = luaL_checknumber(L, i + 1);

	double val = 0.0;

	switch (nargs)
	{
	case 1:
		val = simplexNoise1(args[0]);
		break;
	case 2:
		val = simplexNoise2(args[0], args[1]);
		break;
	case 3:
		val = simplexNoise3(args[0], args[1], args[2]);
		break;
	case 4:
		val = simplexNoise4(args[0], args[1], args[2], args[3]);
		break;
	}

	lua_pushnumber(L, (lua_Number) val);
	return 1;
}

// C functions in a struct, necessary for the FFI versions of math functions.
struct FFI_Math
{
	double (*snoise1)(double x);
	double (*snoise2)(double x, double y);
	double (*snoise3)(double x, double y, double z);
	double (*snoise4)(double x, double y, double z, double w);
	double (*pnoise1)(double x);
	double (*pnoise2)(double x, double y);
	double (*pnoise3)(double x, double y, double z);
	double (*pnoise4)(double x, double y, double z, double w);

	float (*gammaToLinear)(float c);
	float (*linearToGamma)(float c);
};

static FFI_Math ffifuncs =
{
	simplexNoise1,
	simplexNoise2,
	simplexNoise3,
	simplexNoise4,

	perlinNoise1,
	perlinNoise2,
	perlinNoise3,
	perlinNoise4,

	gammaToLinear,
	linearToGamma,
};

// List of functions to wrap.
static const luaL_Reg functions[] =
{
	// love.math.random etc. are defined in wrap_Math.lua.

	{ "_getRandomGenerator", w__getRandomGenerator },
	{ "newRandomGenerator", w_newRandomGenerator },
	{ "newBezierCurve", w_newBezierCurve },
	{ "newTransform", w_newTransform },
	{ "triangulate", w_triangulate },
	{ "isConvex", w_isConvex },
	{ "gammaToLinear", w_gammaToLinear },
	{ "linearToGamma", w_linearToGamma },
	{ "noise", w_noise },
	{ "perlinNoise", w_perlinNoise },
	{ "simplexNoise", w_simplexNoise },

	{ 0, 0 }
};

static const lua_CFunction types[] =
{
	luaopen_randomgenerator,
	luaopen_beziercurve,
	luaopen_transform,
	0
};

extern "C" int luaopen_love_math(lua_State *L)
{
	Math *instance = instance();
	if (instance == nullptr)
	{
		luax_catchexcept(L, [&](){ instance = new Math(); });
	}
	else
		instance->retain();

	WrappedModule w;
	w.module = instance;
	w.name = "math";
	w.type = &Module::type;
	w.functions = functions;
	w.types = types;

	int n = luax_register_module(L, w);

	// Execute wrap_Math.lua, sending the math table and ffifuncs pointer as args.
	luaL_loadbuffer(L, math_lua, sizeof(math_lua), "=[love \"wrap_Math.lua\"]");
	lua_pushvalue(L, -2);
	luax_pushpointerasstring(L, &ffifuncs);
	lua_call(L, 2, 0);

	return n;
}

} // math
} // love
