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
#include "common/config.h"
#include "wrap_Filesystem.h"
#include "wrap_File.h"
#include "wrap_NativeFile.h"
#include "wrap_FileData.h"
#include "data/wrap_Data.h"
#include "data/wrap_DataModule.h"

#include "physfs/Filesystem.h"

#ifdef LOVE_ANDROID
#include "common/android.h"
#endif

// SDL
#include <SDL3/SDL_loadso.h>

// STL
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

namespace love
{
namespace filesystem
{

#define instance() (Module::getInstance<Filesystem>(Module::M_FILESYSTEM))

int w_init(lua_State *L)
{
	const char *arg0 = luaL_checkstring(L, 1);
	luax_catchexcept(L, [&](){ instance()->init(arg0); });
	return 0;
}

int w_setFused(lua_State *L)
{
	// no error checking needed, everything, even nothing
	// can be converted to a boolean
	instance()->setFused(luax_toboolean(L, 1));
	return 0;
}

int w_isFused(lua_State *L)
{
	luax_pushboolean(L, instance()->isFused());
	return 1;
}

int w_setAndroidSaveExternal(lua_State *L)
{
	bool useExternal = luax_optboolean(L, 1, false);
	instance()->setAndroidSaveExternal(useExternal);
	return 0;
}

int w_setIdentity(lua_State *L)
{
	const char *arg = luaL_checkstring(L, 1);
	bool append = luax_optboolean(L, 2, false);

	if (!instance()->setIdentity(arg, append))
		return luaL_error(L, "Could not set write directory.");

	return 0;
}

int w_getIdentity(lua_State *L)
{
	lua_pushstring(L, instance()->getIdentity());
	return 1;
}

int w_setSource(lua_State *L)
{
	const char *arg = luaL_checkstring(L, 1);

	if (!instance()->setSource(arg))
		return luaL_error(L, "Could not set source.");

	return 0;
}

int w_getSource(lua_State *L)
{
	lua_pushstring(L, instance()->getSource());
	return 1;
}

int w_mount(lua_State *L)
{
	std::string archive;

	if (luax_istype(L, 1, Data::type))
	{
		Data *data = love::data::luax_checkdata(L, 1);
		int startidx = 2;

		if (luax_istype(L, 1, FileData::type) && !lua_isstring(L, 3))
		{
			FileData *filedata = luax_checkfiledata(L, 1);
			archive = filedata->getFilename();
			startidx = 2;
		}
		else
		{
			archive = luax_checkstring(L, 2);
			startidx = 3;
		}

		const char *mountpoint = luaL_checkstring(L, startidx + 0);
		bool append = luax_optboolean(L, startidx + 1, false);

		luax_pushboolean(L, instance()->mount(data, archive.c_str(), mountpoint, append));
		return 1;
	}
	else if (luax_istype(L, 1, NativeFile::type))
	{
		NativeFile *file = luax_totype<NativeFile>(L, 1);
		archive = file->getFilename();
	}
	else
		archive = luax_checkstring(L, 1);

	const char *mountpoint = luaL_checkstring(L, 2);
	bool append = luax_optboolean(L, 3, false);

	luax_pushboolean(L, instance()->mount(archive.c_str(), mountpoint, append));
	return 1;
}

int w_mountFullPath(lua_State *L)
{
	const char *fullpath = luaL_checkstring(L, 1);
	const char *mountpoint = luaL_checkstring(L, 2);

	auto permissions = Filesystem::MOUNT_PERMISSIONS_READ;
	if (!lua_isnoneornil(L, 3))
	{
		const char *permissionstr = luaL_checkstring(L, 3);
		if (!Filesystem::getConstant(permissionstr, permissions))
			return luax_enumerror(L, "mount permissions", Filesystem::getConstants(permissions), permissionstr);
	}

	bool append = luax_optboolean(L, 4, false);

	luax_pushboolean(L, instance()->mountFullPath(fullpath, mountpoint, permissions, append));
	return 1;
}

int w_mountCommonPath(lua_State *L)
{
	const char *commonpathstr = luaL_checkstring(L, 1);
	Filesystem::CommonPath commonpath;
	if (!Filesystem::getConstant(commonpathstr, commonpath))
		return luax_enumerror(L, "common path", Filesystem::getConstants(commonpath), commonpathstr);

	const char *mountpoint = luaL_checkstring(L, 2);

	auto permissions = Filesystem::MOUNT_PERMISSIONS_READ;
	if (!lua_isnoneornil(L, 3))
	{
		const char *permissionstr = luaL_checkstring(L, 3);
		if (!Filesystem::getConstant(permissionstr, permissions))
			return luax_enumerror(L, "mount permissions", Filesystem::getConstants(permissions), permissionstr);
	}

	bool append = luax_optboolean(L, 4, false);

	luax_pushboolean(L, instance()->mountCommonPath(commonpath, mountpoint, permissions, append));
	return 1;
}

int w_unmount(lua_State *L)
{
	if (luax_istype(L, 1, Data::type))
	{
		Data *data = love::data::luax_checkdata(L, 1);
		luax_pushboolean(L, instance()->unmount(data));
	}
	else
	{
		const char *archive = luaL_checkstring(L, 1);
		luax_pushboolean(L, instance()->unmount(archive));
	}
	return 1;
}

int w_unmountFullPath(lua_State *L)
{
	const char *fullpath = luaL_checkstring(L, 1);
	luax_pushboolean(L, instance()->unmountFullPath(fullpath));
	return 1;
}

int w_unmountCommonPath(lua_State *L)
{
	const char *commonpathstr = luaL_checkstring(L, 1);
	Filesystem::CommonPath commonpath;
	if (!Filesystem::getConstant(commonpathstr, commonpath))
		return luax_enumerror(L, "common path", Filesystem::getConstants(commonpath), commonpathstr);

	luax_pushboolean(L, instance()->unmount(commonpath));
	return 1;
}

int w_openFile(lua_State *L)
{
	const char *filename = luaL_checkstring(L, 1);
	const char *modestr = luaL_checkstring(L, 2);

	File::Mode mode = File::MODE_CLOSED;
	if (!File::getConstant(modestr, mode))
		return luax_enumerror(L, "file open mode", File::getConstants(mode), modestr);

	File *t = nullptr;
	try
	{
		t = instance()->openFile(filename, mode);
	}
	catch (love::Exception &e)
	{
		return luax_ioError(L, "%s", e.what());
	}

	luax_pushtype(L, t);
	t->release();
	return 1;
}

int w_openNativeFile(lua_State *L)
{
	const char *path = luaL_checkstring(L, 1);
	const char *modestr = luaL_checkstring(L, 2);

	File::Mode mode = File::MODE_CLOSED;
	if (!File::getConstant(modestr, mode))
		return luax_enumerror(L, "file open mode", File::getConstants(mode), modestr);

	File *t = nullptr;
	try
	{
		t = instance()->openNativeFile(path, mode);
	}
	catch (love::Exception &e)
	{
		return luax_ioError(L, "%s", e.what());
	}

	luax_pushtype(L, t);
	t->release();
	return 1;
}

int w_newFile(lua_State* L)
{
	luax_markdeprecated(L, 1, "love.filesystem.newFile", API_FUNCTION, DEPRECATED_RENAMED, "love.filesystem.openFile");

	const char* filename = luaL_checkstring(L, 1);

	File::Mode mode = File::MODE_CLOSED;

	if (!lua_isnoneornil(L, 2))
	{
		const char* modestr = luaL_checkstring(L, 2);
		if (!File::getConstant(modestr, mode))
			return luax_enumerror(L, "file open mode", File::getConstants(mode), modestr);
	}

	File* t = nullptr;
	try
	{
		t = instance()->openFile(filename, mode);
	}
	catch (love::Exception& e)
	{
		return luax_ioError(L, "%s", e.what());
	}

	luax_pushtype(L, t);
	t->release();
	return 1;
}

File *luax_getfile(lua_State *L, int idx)
{
	File *file = nullptr;
	if (lua_isstring(L, idx))
	{
		const char *filename = luaL_checkstring(L, idx);
		try
		{
			file = instance()->openFile(filename, File::MODE_CLOSED);
		}
		catch (love::Exception &e)
		{
			luax_ioError(L, "%s", e.what());
		}
	}
	else
	{
		file = luax_checkfile(L, idx);
		file->retain();
	}

	return file;
}

FileData *luax_getfiledata(lua_State *L, int idx, bool ioerror, int &nresults)
{
	FileData *data = nullptr;
	File *file = nullptr;
	nresults = 0;

	if (lua_isstring(L, idx) || luax_istype(L, idx, File::type))
	{
		file = luax_getfile(L, idx);
	}
	else if (luax_istype(L, idx, FileData::type))
	{
		data = luax_checkfiledata(L, idx);
		data->retain();
	}

	if (!data && !file)
	{
		nresults = luaL_argerror(L, idx, "filename, File, or FileData expected");
		return nullptr; // Never reached.
	}
	else if (file && !data)
	{
		try
		{
			data = file->read();
		}
		catch (love::Exception &e)
		{
			file->release();
			if (ioerror)
				nresults = luax_ioError(L, "%s", e.what());
			else
				nresults = luaL_error(L, "%s", e.what());
			return nullptr; // Never reached if ioerror is false.
		}

		file->release();
	}

	return data;
}

FileData *luax_getfiledata(lua_State *L, int idx)
{
	int nresults = 0;
	return luax_getfiledata(L, idx, false, nresults);
}

Data *luax_getdata(lua_State *L, int idx)
{
	Data *data = nullptr;
	File *file = nullptr;

	if (lua_isstring(L, idx) || luax_istype(L, idx, File::type))
	{
		file = luax_getfile(L, idx);
	}
	else if (luax_istype(L, idx, Data::type))
	{
		data = data::luax_checkdata(L, idx);
		data->retain();
	}

	if (!data && !file)
	{
		luaL_argerror(L, idx, "filename, File, or Data expected");
		return nullptr; // Never reached.
	}

	if (file)
	{
		luax_catchexcept(L,
			[&]() { data = file->read(); },
			[&](bool) { file->release(); }
		);
	}

	return data;
}

bool luax_cangetfile(lua_State *L, int idx)
{
	return lua_isstring(L, idx) || luax_istype(L, idx, File::type);
}

bool luax_cangetfiledata(lua_State *L, int idx)
{
	return lua_isstring(L, idx) || luax_istype(L, idx, File::type) || luax_istype(L, idx, FileData::type);
}

bool luax_cangetdata(lua_State *L, int idx)
{
	return lua_isstring(L, idx) || luax_istype(L, idx, File::type) || luax_istype(L, idx, Data::type);
}

int w_newFileData(lua_State *L)
{
	// Single argument: treat as filepath or File.
	if (lua_gettop(L) == 1)
	{
		int nresults = 0;
		FileData *data = luax_getfiledata(L, 1, true, nresults);
		if (data == nullptr)
			return nresults;
		luax_pushtype(L, data);
		data->release();
		return 1;
	}

	size_t length = 0;
	const void *ptr = nullptr;
	if (luax_istype(L, 1, Data::type))
	{
		Data *data = data::luax_checkdata(L, 1);
		ptr = data->getData();
		length = data->getSize();
	}
	else if (lua_isstring(L, 1))
		ptr = luaL_checklstring(L, 1, &length);
	else
		return luaL_argerror(L, 1, "string or Data expected");

	const char *filename = luaL_checkstring(L, 2);

	FileData *t = nullptr;
	luax_catchexcept(L, [&](){ t = instance()->newFileData(ptr, length, filename); });

	luax_pushtype(L, t);
	t->release();
	return 1;
}

int w_getFullCommonPath(lua_State *L)
{
	const char *commonpathstr = luaL_checkstring(L, 1);
	Filesystem::CommonPath commonpath;
	if (!Filesystem::getConstant(commonpathstr, commonpath))
		return luax_enumerror(L, "common path", Filesystem::getConstants(commonpath), commonpathstr);

	luax_pushstring(L, instance()->getFullCommonPath(commonpath));
	return 1;
}

int w_getWorkingDirectory(lua_State *L)
{
	lua_pushstring(L, instance()->getWorkingDirectory());
	return 1;
}

int w_getUserDirectory(lua_State *L)
{
	luax_pushstring(L, instance()->getUserDirectory());
	return 1;
}

int w_getAppdataDirectory(lua_State *L)
{
	luax_pushstring(L, instance()->getAppdataDirectory());
	return 1;
}

int w_getSaveDirectory(lua_State *L)
{
	luax_pushstring(L, instance()->getSaveDirectory());
	return 1;
}

int w_getSourceBaseDirectory(lua_State *L)
{
	luax_pushstring(L, instance()->getSourceBaseDirectory());
	return 1;
}

int w_getRealDirectory(lua_State *L)
{
	const char *filename = luaL_checkstring(L, 1);
	std::string dir;

	try
	{
		dir = instance()->getRealDirectory(filename);
	}
	catch (love::Exception &e)
	{
		return luax_ioError(L, "%s", e.what());
	}

	lua_pushstring(L, dir.c_str());
	return 1;
}

int w_canonicalizeRealPath(lua_State *L)
{
	const char *path = luaL_checkstring(L, 1);
	luax_pushstring(L, instance()->canonicalizeRealPath(path));
	return 1;
}

int w_getExecutablePath(lua_State *L)
{
	luax_pushstring(L, instance()->getExecutablePath());
	return 1;
}

int w_exists(lua_State *L)
{
	const char *path = luaL_checkstring(L, 1);
	luax_pushboolean(L, instance()->exists(path));
	return 1;
}

int w_getInfo(lua_State *L)
{
	const char *filepath = luaL_checkstring(L, 1);
	Filesystem::Info info = {};

	int startidx = 2;
	Filesystem::FileType filtertype = Filesystem::FILETYPE_MAX_ENUM;
	if (lua_isstring(L, startidx))
	{
		const char *typestr = luaL_checkstring(L, startidx);
		if (!Filesystem::getConstant(typestr, filtertype))
			return luax_enumerror(L, "file type", Filesystem::getConstants(filtertype), typestr);

		startidx++;
	}

	if (instance()->getInfo(filepath, info))
	{
		if (filtertype != Filesystem::FILETYPE_MAX_ENUM && info.type != filtertype)
		{
			lua_pushnil(L);
			return 1;
		}

		const char *typestr = nullptr;
		if (!Filesystem::getConstant(info.type, typestr))
			return luaL_error(L, "Unknown file type.");

		if (lua_istable(L, startidx))
			lua_pushvalue(L, startidx);
		else
			lua_createtable(L, 0, 3);

		lua_pushstring(L, typestr);
		lua_setfield(L, -2, "type");

		luax_pushboolean(L, info.readonly);
		lua_setfield(L, -2, "readonly");

		// Lua numbers (doubles) can't fit the full range of 64 bit ints.
		info.size = std::min<int64>(info.size, 0x20000000000000LL);
		if (info.size >= 0)
		{
			lua_pushnumber(L, (lua_Number) info.size);
			lua_setfield(L, -2, "size");
		}

		info.modtime = std::min<int64>(info.modtime, 0x20000000000000LL);
		if (info.modtime >= 0)
		{
			lua_pushnumber(L, (lua_Number) info.modtime);
			lua_setfield(L, -2, "modtime");
		}
	}
	else
		lua_pushnil(L);

	return 1;
}

int w_createDirectory(lua_State *L)
{
	const char *arg = luaL_checkstring(L, 1);
	luax_pushboolean(L, instance()->createDirectory(arg));
	return 1;
}

int w_remove(lua_State *L)
{
	const char *arg = luaL_checkstring(L, 1);
	luax_pushboolean(L, instance()->remove(arg));
	return 1;
}

int w_read(lua_State *L)
{
	love::data::ContainerType ctype = love::data::CONTAINER_STRING;
	int startidx = 1;

	if (lua_type(L, 2) == LUA_TSTRING)
	{
		ctype = love::data::luax_checkcontainertype(L, 1);
		startidx = 2;
	}

	const char *filename = luaL_checkstring(L, startidx + 0);
	int64 len = (int64) luaL_optinteger(L, startidx + 1, -1);

	FileData *data = nullptr;
	try
	{
		if (len >= 0)
			data = instance()->read(filename, len);
		else
			data = instance()->read(filename);
	}
	catch (love::Exception &e)
	{
		return luax_ioError(L, "%s", e.what());
	}

	if (data == nullptr)
		return luax_ioError(L, "File could not be read.");

	if (ctype == love::data::CONTAINER_DATA)
		luax_pushtype(L, data);
	else
		lua_pushlstring(L, (const char *) data->getData(), data->getSize());

	lua_pushinteger(L, data->getSize());

	// Lua has a copy now, so we can free it.
	data->release();

	return 2;
}

static int w_write_or_append(lua_State *L, File::Mode mode)
{
	const char *filename = luaL_checkstring(L, 1);

	const char *input = nullptr;
	size_t len = 0;

	if (luax_istype(L, 2, love::Data::type))
	{
		love::Data *data = luax_totype<love::Data>(L, 2);
		input = (const char *) data->getData();
		len = data->getSize();
	}
	else if (lua_isstring(L, 2))
		input = lua_tolstring(L, 2, &len);
	else
		return luaL_argerror(L, 2, "string or Data expected");

	// Get how much we should write. Length of string default.
	len = luaL_optinteger(L, 3, len);

	try
	{
		if (mode == File::MODE_APPEND)
			instance()->append(filename, (const void *) input, len);
		else
			instance()->write(filename, (const void *) input, len);
	}
	catch (love::Exception &e)
	{
		return luax_ioError(L, "%s", e.what());
	}

	luax_pushboolean(L, true);
	return 1;
}

int w_write(lua_State *L)
{
	return w_write_or_append(L, File::MODE_WRITE);
}

int w_append(lua_State *L)
{
	return w_write_or_append(L, File::MODE_APPEND);
}

int w_getDirectoryItems(lua_State *L)
{
	const char *dir = luaL_checkstring(L, 1);
	std::vector<std::string> items;

	instance()->getDirectoryItems(dir, items);

	lua_createtable(L, (int) items.size(), 0);

	for (int i = 0; i < (int) items.size(); i++)
	{
		lua_pushstring(L, items[i].c_str());
		lua_rawseti(L, -2, i + 1);
	}

	// Return the table.
	return 1;
}

int w_lines(lua_State *L)
{
	if (lua_isstring(L, 1))
	{
		File *file = nullptr;
		luax_catchexcept(L, [&]() { file = instance()->openFile(lua_tostring(L, 1), File::MODE_READ); });

		luax_pushtype(L, file);
		file->release();
	}
	else
		return luaL_argerror(L, 1, "expected filename.");

	lua_pushstring(L, ""); // buffer
	lua_pushstring(L, 0); // buffer offset
	lua_pushcclosure(L, w_File_lines_i, 3);
	return 1;
}

int w_load(lua_State *L)
{
	std::string filename = std::string(luaL_checkstring(L, 1));

	Filesystem::LoadMode loadMode = Filesystem::LOADMODE_ANY;

	if (!lua_isnoneornil(L, 2))
	{
		const char *mode = luaL_checkstring(L, 2);

		if (!Filesystem::getConstant(mode, loadMode))
			return luax_enumerror(L, "load mode", Filesystem::getConstants(loadMode), mode);
	}

	Data *data = nullptr;
	try
	{
		data = instance()->read(filename.c_str());
	}
	catch (love::Exception &e)
	{
		return luax_ioError(L, "%s", e.what());
	}

	int status;

#if (LUA_VERSION_NUM > 501) || defined(LUA_JITLIBNAME)
	// LuaJIT support this Lua 5.2 function.
	const char *mode;
	Filesystem::getConstant(loadMode, mode);

	status = luaL_loadbufferx(L, (const char *)data->getData(), data->getSize(), ("@" + filename).c_str(), mode);
#else
	if (loadMode == Filesystem::LOADMODE_ANY)
		status = luaL_loadbuffer(L, (const char *)data->getData(), data->getSize(), ("@" + filename).c_str());
	else
	{
		// Unsupported
		data->release();
		return luaL_error(L, "only \"bt\" is supported on this Lua interpreter\n");
	}
#endif

	data->release();

	// Load the chunk, but don't run it.
	switch (status)
	{
	case LUA_ERRMEM:
		return luaL_error(L, "Memory allocation error: %s\n", lua_tostring(L, -1));
	case LUA_ERRSYNTAX:
		return luaL_error(L, "Syntax error: %s\n", lua_tostring(L, -1));
	default: // success
		return 1;
	}
}

int w_setSymlinksEnabled(lua_State *L)
{
	instance()->setSymlinksEnabled(luax_checkboolean(L, 1));
	return 0;
}

int w_areSymlinksEnabled(lua_State *L)
{
	luax_pushboolean(L, instance()->areSymlinksEnabled());
	return 1;
}

int w_getRequirePath(lua_State *L)
{
	std::stringstream path;
	bool seperator = false;
	for (auto &element : instance()->getRequirePath())
	{
		if (seperator)
			path << ";";
		else
			seperator = true;

		path << element;
	}

	luax_pushstring(L, path.str());
	return 1;
}

int w_getCRequirePath(lua_State *L)
{
	std::stringstream path;
	bool seperator = false;
	for (auto &element : instance()->getCRequirePath())
	{
		if (seperator)
			path << ";";
		else
			seperator = true;

		path << element;
	}

	luax_pushstring(L, path.str());
	return 1;
}

int w_setRequirePath(lua_State *L)
{
	std::string element = luax_checkstring(L, 1);
	auto &requirePath = instance()->getRequirePath();

	requirePath.clear();
	std::stringstream path;
	path << element;

	while(std::getline(path, element, ';'))
		requirePath.push_back(element);

	return 0;
}

int w_setCRequirePath(lua_State *L)
{
	std::string element = luax_checkstring(L, 1);
	auto &requirePath = instance()->getCRequirePath();

	requirePath.clear();
	std::stringstream path;
	path << element;

	while(std::getline(path, element, ';'))
		requirePath.push_back(element);

	return 0;
}

static void replaceAll(std::string &str, const std::string &substr, const std::string &replacement)
{
	std::vector<size_t> locations;
	size_t pos = 0;
	size_t sublen = substr.length();

	while ((pos = str.find(substr, pos)) != std::string::npos)
	{
		locations.push_back(pos);
		pos += sublen;
	}

	for (int i = (int) locations.size() - 1; i >= 0; i--)
		str.replace(locations[i], sublen, replacement);
}

int loader(lua_State *L)
{
	std::string modulename = luax_checkstring(L, 1);
	bool hasSlash = modulename.find('/') != std::string::npos;

	for (char &c : modulename)
	{
		if (c == '.')
			c = '/';
	}

	auto *inst = instance();
	for (std::string element : inst->getRequirePath())
	{
		replaceAll(element, "?", modulename);

		Filesystem::Info info = {};
		if (inst->getInfo(element.c_str(), info) && info.type != Filesystem::FILETYPE_DIRECTORY)
		{
			if (hasSlash)
				luax_markdeprecated(L, 2, "character in require string (forward slashes), use dots instead.", API_CUSTOM);

			lua_pop(L, 1);
			lua_pushstring(L, element.c_str());
			return w_load(L);
		}
	}

	std::string errstr = "\n\tno '%s' in LOVE game directories.";

	lua_pushfstring(L, errstr.c_str(), modulename.c_str());
	return 1;
}

static const char *library_extensions[] =
{
#ifdef LOVE_WINDOWS
	".dll"
#elif defined(LOVE_MACOS) || defined(LOVE_IOS)
	".dylib", ".so"
#else
	".so"
#endif
};

int extloader(lua_State *L)
{
	std::string filename = luax_checkstring(L, 1);
	std::string tokenized_name(filename);
	std::string tokenized_function(filename);

	// We need both the tokenized filename (dots replaced with slashes)
	// and the tokenized function name (dots replaced with underscores)
	// NOTE: Lua's loader queries more names than this one.
	for (unsigned int i = 0; i < tokenized_name.size(); i++)
	{
		if (tokenized_name[i] == '.')
		{
			tokenized_name[i] = '/';
			tokenized_function[i] = '_';
		}
	}

	SDL_SharedObject *handle = nullptr;
	auto *inst = instance();

	std::string loaderror = "";

#ifdef LOVE_ANDROID
	// Specifically Android, look the library path based on getCRequirePath first
	std::string androidPath(love::android::getCRequirePath());

	if (!androidPath.empty())
	{
		// Replace ? with just the dotted filename (not tokenized_name)
		replaceAll(androidPath, "?", filename);

		// Load directly, don't check for existence.
		handle = SDL_LoadObject(androidPath.c_str());
	}

	if (!handle)
	{
		loaderror += std::string(" ") + std::string(SDL_GetError());
#endif // LOVE_ANDROID

	for (const std::string &el : inst->getCRequirePath())
	{
		for (const char *ext : library_extensions)
		{
			std::string element = el;

			// Replace ?? with the filename and extension
			replaceAll(element, "??", tokenized_name + ext);

			// And ? with just the filename
			replaceAll(element, "?", tokenized_name);

			Filesystem::Info info = {};
			if (!inst->getInfo(element.c_str(), info) || info.type == Filesystem::FILETYPE_DIRECTORY)
				continue;

			// Now resolve the full path, as we're bypassing physfs for the next part.
			std::string filepath = inst->getRealDirectory(element.c_str()) + LOVE_PATH_SEPARATOR + element;

			handle = SDL_LoadObject(filepath.c_str());
			// Can fail, for instance if it turned out the source was a zip
			if (handle)
				break;
			else
				loaderror += std::string(" ") + std::string(SDL_GetError());
		}

		if (handle)
			break;
	}

#ifdef LOVE_ANDROID
	} // if (!handle)
#endif

	if (!handle)
	{
		lua_pushfstring(L, "\n\tno valid C library '%s' in LOVE paths.%s", tokenized_name.c_str(), loaderror.c_str());
		return 1;
	}

	// We look for both loveopen_ and luaopen_, so libraries with specific love support
	// can tell when they've been loaded by love.
	SDL_FunctionPointer func = SDL_LoadFunction(handle, ("loveopen_" + tokenized_function).c_str());
	if (!func)
		func = SDL_LoadFunction(handle, ("luaopen_" + tokenized_function).c_str());

	if (!func)
	{
		SDL_UnloadObject(handle);
		lua_pushfstring(L, "\n\tC library '%s' is incompatible.", tokenized_name.c_str());
		return 1;
	}

	lua_pushcfunction(L, (lua_CFunction) func);
	return 1;
}

// List of functions to wrap.
static const luaL_Reg functions[] =
{
	{ "init", w_init },
	{ "setFused", w_setFused },
	{ "isFused", w_isFused },
	{ "_setAndroidSaveExternal", w_setAndroidSaveExternal },
	{ "setIdentity", w_setIdentity },
	{ "getIdentity", w_getIdentity },
	{ "setSource", w_setSource },
	{ "getSource", w_getSource },
	{ "mount", w_mount },
	{ "mountFullPath", w_mountFullPath },
	{ "mountCommonPath", w_mountCommonPath },
	{ "unmount", w_unmount },
	{ "unmountFullPath", w_unmountFullPath },
	{ "unmountCommonPath", w_unmountCommonPath },
	{ "openFile", w_openFile },
	{ "openNativeFile", w_openNativeFile },
	{ "getFullCommonPath", w_getFullCommonPath },
	{ "getWorkingDirectory", w_getWorkingDirectory },
	{ "getUserDirectory", w_getUserDirectory },
	{ "getAppdataDirectory", w_getAppdataDirectory },
	{ "getSaveDirectory", w_getSaveDirectory },
	{ "getSourceBaseDirectory", w_getSourceBaseDirectory },
	{ "getRealDirectory", w_getRealDirectory },
	{ "canonicalizeRealPath", w_canonicalizeRealPath },
	{ "getExecutablePath", w_getExecutablePath },
	{ "createDirectory", w_createDirectory },
	{ "remove", w_remove },
	{ "read", w_read },
	{ "write", w_write },
	{ "append", w_append },
	{ "getDirectoryItems", w_getDirectoryItems },
	{ "lines", w_lines },
	{ "load", w_load },
	{ "exists", w_exists },
	{ "getInfo", w_getInfo },
	{ "setSymlinksEnabled", w_setSymlinksEnabled },
	{ "areSymlinksEnabled", w_areSymlinksEnabled },
	{ "newFileData", w_newFileData },
	{ "getRequirePath", w_getRequirePath },
	{ "setRequirePath", w_setRequirePath },
	{ "getCRequirePath", w_getCRequirePath },
	{ "setCRequirePath", w_setCRequirePath },

	// Deprecated
	{ "newFile", w_newFile },

	{ 0, 0 }
};

static const lua_CFunction types[] =
{
	luaopen_file,
	luaopen_nativefile,
	luaopen_filedata,
	0
};

extern "C" int luaopen_love_filesystem(lua_State *L)
{
	Filesystem *instance = instance();
	if (instance == nullptr)
	{
		luax_catchexcept(L, [&](){ instance = new physfs::Filesystem(); });
	}
	else
		instance->retain();

	// The love loaders should be tried after package.preload.
	love::luax_register_searcher(L, loader, 2);
	love::luax_register_searcher(L, extloader, 3);

	WrappedModule w;
	w.module = instance;
	w.name = "filesystem";
	w.type = &Filesystem::type;
	w.functions = functions;
	w.types = types;

	return luax_register_module(L, w);
}

} // filesystem
} // love
