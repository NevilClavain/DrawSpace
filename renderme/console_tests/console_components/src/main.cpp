
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
#include "componentcontainer.h"

using namespace renderMe;

class Foo 
{
public:
	Foo() = default;
	~Foo()
	{
		std::cout << "Foo ctor call\n";
	}
};

int main( int argc, char* argv[] )
{    
	std::cout << "Component container test !\n";
	core::ComponentContainer c;

	c.addComponent<std::string>("theString", "abcdefg");
	c.addComponent<double>("pi", 3.1415927);
	c.addComponent<Foo>("foo");

	std::cout << "Number of components : " << core::ComponentContainer::getUIDCount() << "\n";

	{
		const auto pi{ c.getComponent<double>("pi") };
		const auto theString{ c.getComponent<std::string>("theString") };

		std::cout << pi->getPurpose() << "\n";
		std::cout << theString->getPurpose() << "\n";
	}

	// update
	{
		auto& pi{ c.getComponent<double>("pi")->getPurpose() };
		pi = 3.12;
	}
	///
	{
		const auto pi{ c.getComponent<double>("pi") };
		std::cout << pi->getPurpose() << "\n";
	}

	c.removeComponent<std::string>("theString");
	c.removeComponent<double>("pi");
	c.removeComponent<Foo>("foo");

	std::cout << "Number of components : " << core::ComponentContainer::getUIDCount() << "\n";


    return 0;
}
