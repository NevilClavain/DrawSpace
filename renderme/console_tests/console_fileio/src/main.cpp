
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

int main( int argc, char* argv[] )
{    
	std::cout << "file IO tests \n";

	{
		// read/write of human readable data...

		const std::string textOut("abcdefghijklmnopqrstuvwxyz1234567890");

		renderMe::core::FileContent<const char> writer("./mytext.txt");
		writer.save(textOut.c_str(), textOut.size());

		renderMe::core::FileContent<const char> reader("./mytext.txt");
		reader.load();

		const std::string textIn(reader.getData(), reader.getDataSize());

		std::cout << "text is : " << textIn << "\n";
	}

	{
		// read/write of double buffer data...

		const double dataOut[]{ 0.1, 0.3, 0.666, 0.75, 3.1415927 };

		renderMe::core::FileContent<const double> writer("./mydata.dat");
		writer.save(dataOut, 5);

		renderMe::core::FileContent<const double> reader("./mydata.dat");
		reader.load();

		const auto nbElements = reader.getDataSize() / sizeof(const double);
		std::cout << "nb of doubles = " << nbElements << "\n";

		auto ptr{ reader.getData() };

		for (size_t i = 0; i < nbElements; i++)
		{
			std::cout << *ptr << "\n";
			ptr++;
		}
	}

    return 0;
}
