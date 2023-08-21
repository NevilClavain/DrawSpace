
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
#include "filesystem.h"
#include "json.h"
#include "exceptions.h"

int main( int argc, char* argv[] )
{    
	std::cout << "JSON parser\n";

	try
	{
		renderMe::core::fileContent<char> fc("./console_jsonparser_assets/log.json");
		fc.load();

		const auto dataSize{ fc.getDataSize() };
		const std::string data(fc.getData(), dataSize);

		renderMe::core::json jsonParser;

		jsonParser.setCallback([] (jsmntype_t p_type, const std::string& p_id, const std::string& p_value)
		{
			switch (p_type)
			{
				case JSMN_OBJECT:

					std::cout << ">> object of id : " << p_id << "\n";
					break;

				case JSMN_ARRAY:

					std::cout << ">> array of id : " << p_id << "\n";
					break;

				case JSMN_STRING:

					std::cout << ">> string of id : " << p_id <<  " with value = " << p_value << "\n";
					break;

				case JSMN_PRIMITIVE:

					std::cout << ">> primitive of id : " << p_id << " with value = " << p_value << "\n";
					break;
			}
		});

		const auto parseStatus{ jsonParser.parse(data) };

		std::cout << "Parser status = " << (parseStatus > -1 ? "OK" : "KO") << "\n";

	}
	catch (const std::exception& e)
	{
		const auto what{ e.what() };
		std::cout << what << "\n";
	}

    return 0;
}
