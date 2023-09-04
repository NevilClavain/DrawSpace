
/* -*-LIC_BEGIN-*- */
/*
*
* renderMe grafx framework
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of renderMe.
*
*    renderMe is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    renderMe is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with renderMe.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#include <iostream>
#include <functional>
#include <vector>

template<class... Args>
class Event
{
public:
	using Callback = std::function<void(Args&&...)>;

	void registerSubscriber(const Callback& p_callback)
	{
		m_callbacks.push_back(p_callback);
	}

protected:
	std::vector<Callback> m_callbacks;

};


template<class Emiter>
class EventSubscriber
{
public:
	virtual typename Emiter::Callback getCallback() const
	{
		return m_cb;
	}

protected:
	typename Emiter::Callback m_cb;
};

/////////////////////////////////////////////////////////////////////////////

class Processing : public Event<const std::string&, int, double>
{
public:
	Processing() = default;
	~Processing() = default;

	void processSomething()
	{
		std::cout << "Processing : Processing begin...\n";

		
		for (const auto& call : m_callbacks)
		{
			call("myId", 666, 3.1415927);
		}
		

		std::cout << "Processing : Processing end...\n";
	}	
};


class Subscriber1 : public EventSubscriber<Processing>
{
public:

	Subscriber1() 
	{
		m_cb = [&, this](const std::string& p_id, int p_ivalue, double p_dvalue)
		{
			this->m_memId = p_id;
			this->m_memInt = p_ivalue;
			this->m_memDouble = p_dvalue;

			display_values();
		};
	}
	~Subscriber1() = default;

private:

	
	std::string			m_memId;
	int					m_memInt;
	double				m_memDouble;

	void display_values()
	{
		std::cout << "Subscriber1 -> " << " " << m_memId << " " << m_memInt << " " << m_memDouble << "\n";
	}
};


//////////////////////////////////////////////////////////////////////////

int main( int argc, char* argv[] )
{    
	std::cout << "callback system test...\n";

	Processing		caller;
	Subscriber1		subscriber;

	caller.registerSubscriber(subscriber.getCallback());

	caller.processSomething();


    return 0;
}
