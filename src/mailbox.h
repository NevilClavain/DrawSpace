/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of DrawSpace.
*
*    DrawSpace is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    DrawSpace is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#pragma once
#include "drawspace_commons.h"

#define _push_code_ \
	m_mutex.lock();\
	m_messages.push_front(p_object);\
	m_mutex.unlock();

#define _popnext_code_ \
	auto task{ p_default }; \
	if (m_mutex.try_lock()) \
	{ \
		if (m_messages.size() > 0) \
		{ \
			task = m_messages.back(); \
			m_messages.pop_back(); \
		} \
		m_mutex.unlock(); \
	} \
	return task;

namespace DrawSpace
{
namespace Threading
{

template<typename T>
struct Mailbox
{
private:

	std::list<T>	    m_messages;
	mutable std::mutex	m_mutex;

public:

	// build arg by copy if ptr or integral(fundamental) type
	template<typename Type>
	using copyArg = typename std::enable_if_t<std::is_pointer<Type>::value || std::is_integral<Type>::value, T>;

	// build arg by const ref if not ptr and not integral(fundamental) type (i.e classes, struct...)
	template<typename Type>
	using constRefArg = typename std::enable_if_t<!std::is_pointer<Type>::value && !std::is_integral<Type>::value, const T&>;


	//if ptr or integral type, build method signature : Push(T p_object)
	template<typename Arg>
	inline void Push(copyArg<Arg> p_object)
	{
		_push_code_
	}

	//if not ptr and not integral type, build method signature : Push(const T& p_object)
	template<typename Arg>
	inline void Push(constRefArg<Arg> p_object)
	{
		_push_code_
	}

	//if ptr or integral type, build method signature : PopNext(T p_object)
	template<typename Arg>
	inline T PopNext(copyArg<Arg> p_default)
	{
		_popnext_code_
	}

	//if not ptr and not integral type, build method signature : PopNext(const T& p_object)
	template<typename Arg>
	inline T PopNext(constRefArg<Arg> p_default)
	{
		_popnext_code_
	}

	inline int GetBoxSize(void) const
	{
		int size = -1;
		m_mutex.lock();
		size = m_messages.size();
		m_mutex.unlock();
		return size;
	}
};

}
}

