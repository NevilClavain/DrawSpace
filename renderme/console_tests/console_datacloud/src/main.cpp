
/* -*-LIC_BEGIN-*- */
/*
*
* MaGE rendering framework
* Emmanuel Chaumont Copyright (c) 2013-2025
*
* This file is part of MaGE.
*
*    MaGE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    MaGE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with MaGE.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#include <iostream>

#include "datacloud.h"
#include "tvector.h"

using namespace mage;
using namespace mage::core::maths;

int main( int argc, char* argv[] )
{    
	std::cout << "Datacloud tests\n";

	const auto dataCloud { mage::rendering::Datacloud::getInstance() };

	const auto cb{
		[](mage::rendering::DatacloudEvent p_event, const std::string& p_id, const std::string& p_typeid)
		{
			switch (p_event)
			{
				case mage::rendering::DatacloudEvent::DATA_ADDED:

					std::cout << "DATA_ADDED : " << p_id << " of type " << p_typeid << "\n";
					break;

				case mage::rendering::DatacloudEvent::DATA_REMOVED:

					std::cout << "DATA_REMOVED : " << p_id << " of type " << p_typeid << "\n";
					break;

				case mage::rendering::DatacloudEvent::DATA_UPDATED:

					std::cout << "DATA_UPDATED : " << p_id << " of type " << p_typeid << "\n";
					break;

			}
		}
	};
		
	dataCloud->registerSubscriber(cb);

		

	dataCloud->registerData<Real4Vector>("mycolor");

	{
		const auto mycolorVal{ dataCloud->readDataValue<Real4Vector>("mycolor") };
		std::cout << "mycolor = " << mycolorVal[0] << " " << mycolorVal[1] << " " << mycolorVal[2] << " " << mycolorVal[3] << "\n";
	}

	dataCloud->updateDataValue<Real4Vector>("mycolor", Real4Vector(0.3, 0.2, 0.1, 1.0));

	{
		const auto mycolorVal{ dataCloud->readDataValue<Real4Vector>("mycolor") };
		std::cout << "mycolor = " << mycolorVal[0] << " " << mycolorVal[1] << " " << mycolorVal[2] << " " << mycolorVal[3] << "\n";
	}


    return 0;
}
