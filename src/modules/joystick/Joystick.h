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

#ifndef LOVE_JOYSTICK_JOYSTICK_H
#define LOVE_JOYSTICK_JOYSTICK_H

// LOVE
#include "common/Object.h"
#include "common/StringMap.h"
#include "common/int.h"
#include "sensor/Sensor.h"

// stdlib
#include <vector>
#include <string>

namespace love
{
namespace joystick
{

class Joystick : public Object
{
public:

	using Sensor = love::sensor::Sensor;

	static love::Type type;

	// Joystick hat values.
	enum Hat
	{
		HAT_INVALID,
		HAT_CENTERED,
		HAT_UP,
		HAT_RIGHT,
		HAT_DOWN,
		HAT_LEFT,
		HAT_RIGHTUP,
		HAT_RIGHTDOWN,
		HAT_LEFTUP,
		HAT_LEFTDOWN,
		HAT_MAX_ENUM = 16
	};

	enum JoystickType
	{
		JOYSTICK_TYPE_UNKNOWN,
		JOYSTICK_TYPE_GAMEPAD,
		JOYSTICK_TYPE_WHEEL,
		JOYSTICK_TYPE_ARCADE_STICK,
		JOYSTICK_TYPE_FLIGHT_STICK,
		JOYSTICK_TYPE_DANCE_PAD,
		JOYSTICK_TYPE_GUITAR,
		JOYSTICK_TYPE_DRUM_KIT,
		JOYSTICK_TYPE_ARCADE_PAD,
		JOYSTICK_TYPE_THROTTLE,
		JOYSTICK_TYPE_MAX_ENUM
	};

	enum GamepadType
	{
		GAMEPAD_TYPE_UNKNOWN,
		GAMEPAD_TYPE_XBOX360,
		GAMEPAD_TYPE_XBOXONE,
		GAMEPAD_TYPE_PS3,
		GAMEPAD_TYPE_PS4,
		GAMEPAD_TYPE_PS5,
		GAMEPAD_TYPE_NINTENDO_SWITCH_PRO,
		GAMEPAD_TYPE_AMAZON_LUNA,
		GAMEPAD_TYPE_STADIA,
		GAMEPAD_TYPE_VIRTUAL,
		GAMEPAD_TYPE_NVIDIA_SHIELD,
		GAMEPAD_TYPE_JOYCON_LEFT,
		GAMEPAD_TYPE_JOYCON_RIGHT,
		GAMEPAD_TYPE_JOYCON_PAIR,
		GAMEPAD_TYPE_MAX_ENUM
	};

	// Valid Gamepad axes.
	enum GamepadAxis
	{
		GAMEPAD_AXIS_INVALID,
		GAMEPAD_AXIS_LEFTX,
		GAMEPAD_AXIS_LEFTY,
		GAMEPAD_AXIS_RIGHTX,
		GAMEPAD_AXIS_RIGHTY,
		GAMEPAD_AXIS_TRIGGERLEFT,
		GAMEPAD_AXIS_TRIGGERRIGHT,
		GAMEPAD_AXIS_MAX_ENUM
	};

	// Valid Gamepad buttons.
	enum GamepadButton
	{
		GAMEPAD_BUTTON_INVALID,
		GAMEPAD_BUTTON_A,
		GAMEPAD_BUTTON_B,
		GAMEPAD_BUTTON_X,
		GAMEPAD_BUTTON_Y,
		GAMEPAD_BUTTON_BACK,
		GAMEPAD_BUTTON_GUIDE,
		GAMEPAD_BUTTON_START,
		GAMEPAD_BUTTON_LEFTSTICK,
		GAMEPAD_BUTTON_RIGHTSTICK,
		GAMEPAD_BUTTON_LEFTSHOULDER,
		GAMEPAD_BUTTON_RIGHTSHOULDER,
		GAMEPAD_BUTTON_DPAD_UP,
		GAMEPAD_BUTTON_DPAD_DOWN,
		GAMEPAD_BUTTON_DPAD_LEFT,
		GAMEPAD_BUTTON_DPAD_RIGHT,
		GAMEPAD_BUTTON_MISC1, // Xbox Series X share button, PS5 mic button, Switch Pro capture button
		GAMEPAD_BUTTON_PADDLE1,
		GAMEPAD_BUTTON_PADDLE2,
		GAMEPAD_BUTTON_PADDLE3,
		GAMEPAD_BUTTON_PADDLE4,
		GAMEPAD_BUTTON_TOUCHPAD,
		GAMEPAD_BUTTON_MAX_ENUM
	};

	// Different types of inputs for a joystick.
	enum InputType
	{
		INPUT_TYPE_AXIS,
		INPUT_TYPE_BUTTON,
		INPUT_TYPE_HAT,
		INPUT_TYPE_MAX_ENUM
	};

	// Represents a gamepad input value, e.g. the "x" button or the left trigger.
	struct GamepadInput
	{
		InputType type;
		union
		{
			GamepadAxis axis;
			GamepadButton button;
		};
	};

	// Represents a joystick input value, e.g. button 6 or axis 1.
	struct JoystickInput
	{
		InputType type;
		union
		{
			int axis;
			int button;
			struct
			{
				int index;
				Hat value;
			} hat;
		};
	};

	virtual ~Joystick() {}

	virtual bool open(int64 deviceid) = 0;
	virtual void close() = 0;

	virtual bool isConnected() const = 0;

	virtual const char *getName() const = 0;

	virtual JoystickType getJoystickType() const = 0;

	virtual int getAxisCount() const = 0;
	virtual int getButtonCount() const = 0;
	virtual int getHatCount() const = 0;

	virtual float getAxis(int axisindex) const = 0;
	virtual std::vector<float> getAxes() const = 0;
	virtual Hat getHat(int hatindex) const = 0;

	virtual bool isDown(const std::vector<int> &buttonlist) const = 0;

	virtual void setPlayerIndex(int index) = 0;
	virtual int getPlayerIndex() const = 0;

	virtual bool openGamepad(int64 deviceid) = 0;
	virtual bool isGamepad() const = 0;

	virtual GamepadType getGamepadType() const = 0;

	virtual float getGamepadAxis(GamepadAxis axis) const = 0;
	virtual bool isGamepadDown(const std::vector<GamepadButton> &blist) const = 0;

	virtual JoystickInput getGamepadMapping(const GamepadInput &input) const = 0;
	virtual std::string getGamepadMappingString() const = 0;

	virtual void *getHandle() const = 0;

	virtual std::string getGUID() const = 0;
	virtual int getInstanceID() const = 0;
	virtual int getID() const = 0;

	virtual void getDeviceInfo(int &vendorID, int &productID, int &productVersion) const = 0;

	virtual bool isVibrationSupported() = 0;
	virtual bool setVibration(float left, float right, float duration = -1.0f) = 0;
	virtual bool setVibration() = 0;
	virtual void getVibration(float &left, float &right) = 0;

	virtual bool hasSensor(Sensor::SensorType type) const = 0;
	virtual bool isSensorEnabled(Sensor::SensorType type) const = 0;
	virtual void setSensorEnabled(Sensor::SensorType type, bool enabled) = 0;
	virtual std::vector<float> getSensorData(Sensor::SensorType type) const = 0;

	STRINGMAP_CLASS_DECLARE(Hat);
	STRINGMAP_CLASS_DECLARE(JoystickType);
	STRINGMAP_CLASS_DECLARE(GamepadType);
	STRINGMAP_CLASS_DECLARE(GamepadAxis);
	STRINGMAP_CLASS_DECLARE(GamepadButton);
	STRINGMAP_CLASS_DECLARE(InputType);

	static float clampval(float x);

}; // Joystick

} // joystick
} // love


#endif // LOVE_JOYSTICK_JOYSTICK_H
