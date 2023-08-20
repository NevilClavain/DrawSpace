
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
#include "jsonparser.h"
#include "exceptions.h"

int main( int argc, char* argv[] )
{    
	std::cout << "JSON parser\n";

	try 
	{
		renderMe::core::fileContent<char> fc("./console_jsonparser_assets/appgraphics.conf");
		fc.load();

		const auto dataSize{ fc.getDataSize() };
		const std::string data(fc.getData(), dataSize);

		renderMe::core::JSONParser parser;

		const auto parseStatus{ parser.parse(data) };

	}
	catch (const std::exception& e)
	{
		const auto what{ e.what() };
		std::cout << what << "\n";
	}

    return 0;
}
