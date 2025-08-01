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

#include "Event.h"

using love::thread::Mutex;
using love::thread::Lock;

namespace love
{
namespace event
{

Message::Message(const std::string &name, const std::vector<Variant> &vargs)
	: name(name)
	, args(vargs)
{
}

Message::~Message()
{
}

Event::Event(const char *name)
	: Module(M_EVENT, name)
	, modalDrawData()
	, defaultModalDrawData()
{
}

Event::~Event()
{
	if (modalDrawData.cleanup != nullptr)
		modalDrawData.cleanup(modalDrawData.context);

	if (defaultModalDrawData.cleanup != nullptr)
		defaultModalDrawData.cleanup(defaultModalDrawData.context);
}

void Event::push(Message *msg)
{
	push(msg, false);
}

void Event::push(Message *msg, bool pushFront)
{
	Lock lock(mutex);
	msg->retain();
	if (pushFront)
		queue.push_front(msg);
	else
		queue.push_back(msg);
}

bool Event::poll(Message *&msg)
{
	Lock lock(mutex);
	if (queue.empty())
		return false;
	msg = queue.front();
	queue.pop_front();
	return true;
}

void Event::clear()
{
	Lock lock(mutex);
	while (!queue.empty())
	{
		queue.front()->release();
		queue.pop_front();
	}
}

void Event::setModalDrawData(const ModalDrawData &data)
{
	if (modalDrawData.cleanup != nullptr)
		modalDrawData.cleanup(modalDrawData.context);

	modalDrawData = data;
}

void Event::setDefaultModalDrawData(const ModalDrawData &data)
{
	if (defaultModalDrawData.cleanup != nullptr)
		defaultModalDrawData.cleanup(defaultModalDrawData.context);

	defaultModalDrawData = data;
}

void Event::modalDraw()
{
	// Skip the draw if a previous one generated an unprocessed exception.
	if (!deferredExceptionMessage.empty())
		return;

	// Also skip the draw if a previous one generated a return value that
	// needs to be passed down as a quit event.
	if (deferredReturnValues[0].getType() != Variant::NIL)
		return;

	try
	{
		if (modalDrawData.draw != nullptr)
			modalDrawData.draw(modalDrawData.context, &deferredReturnValues[0], &deferredReturnValues[1]);
		else if (defaultModalDrawData.draw != nullptr)
			defaultModalDrawData.draw(defaultModalDrawData.context, &deferredReturnValues[0], &deferredReturnValues[1]);
	}
	catch (std::exception &e)
	{
		deferredExceptionMessage = e.what();
	}
}

} // event
} // love
