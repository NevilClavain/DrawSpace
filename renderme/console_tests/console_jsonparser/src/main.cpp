
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
#include "filesystem.h"
#include "json.h"
#include "exceptions.h"

int main( int argc, char* argv[] )
{    
	std::cout << "JSON parser\n";

	try
	{
		mage::core::FileContent<char> fc("./console_jsonparser_assets/log.json");
		fc.load();

		const auto dataSize{ fc.getDataSize() };
		const std::string data(fc.getData(), dataSize);

		mage::core::Json<> jsonParser;

		jsonParser.registerSubscriber([] (mage::core::JSONEvent p_event, const std::string& p_id, int p_index, const std::string& p_value, const std::optional<mage::core::DefaultUserData*>&)
		{
			switch (p_event)
			{
				case mage::core::JSONEvent::OBJECT_BEGIN:

					std::cout << ">> begin object of id : " << p_id << "\n";
					break;

				case mage::core::JSONEvent::OBJECT_END:

					std::cout << ">> end object of id : " << p_id << "\n";
					break;


				case mage::core::JSONEvent::ARRAY_BEGIN:

					std::cout << ">> begin array of id : " << p_id << "\n";
					break;

				case mage::core::JSONEvent::ARRAY_END:

					std::cout << ">> end array of id : " << p_id << "\n";
					break;

				case mage::core::JSONEvent::STRING:

					std::cout << "	>> string of id : " << p_id <<  " with value = " << p_value << "\n";
					break;

				case mage::core::JSONEvent::PRIMITIVE:

					std::cout << "	>> primitive of id : " << p_id << " with value = " << p_value << "\n";
					break;
			}
		});

		const int parseStatus{ jsonParser.parse(data) };

		std::cout << "Parser status = " << (parseStatus > -1 ? "OK" : "KO") << "\n";

	}
	catch (const std::exception& e)
	{
		const auto what{ e.what() };
		std::cout << what << "\n";
	}

    return 0;
}
