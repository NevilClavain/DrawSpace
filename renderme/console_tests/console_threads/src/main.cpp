
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
#include <vector>

//#include <tuple>
//#include <functional>

#include "runner.h"

#include "filesystem.h"


class Loader : public renderMe::property::AsyncTask
{
public:
	Loader(const std::string& p_path, std::string& p_dest): AsyncTask("loading text", "std::string"),
	m_path(p_path),
	m_dest(p_dest)
	{
	}

private:
	std::string m_path;
	std::string& m_dest;

	void execute(renderMe::core::Runner* p_runner)
	{
		renderMe::core::FileContent<const char> reader(m_path);
		reader.load();

		const std::string textIn(reader.getData(), reader.getDataSize());

		m_dest = textIn;
	}

};


int main( int argc, char* argv[] )
{    
	std::cout << "Threads tests... !\n";

	std::string text1;
	std::string text2;
	
	renderMe::core::SimpleAsyncTask<const std::string&> it(
		[&text1](const std::string& p_path)
		{
			std::cout << "Hello from path : " << p_path << "\n";
		},

		"./console_threads_assets/gmreadme.txt"
	);
	
	renderMe::core::SimpleAsyncTask<> it2(

		[](void)
		{
			std::cout << "Hello from NOBODY !\n";

		}
	);


	Loader loader("./console_threads_assets/gmreadme.txt", text1);

	Loader loader2("./console_threads_assets/title.txt", text2);


	renderMe::core::Runner runner;
	runner.m_mailbox_in.push<renderMe::property::AsyncTask*>(&it);

	runner.m_mailbox_in.push<renderMe::property::AsyncTask*>(&it2);


	runner.m_mailbox_in.push<renderMe::property::AsyncTask*>(&loader);
	runner.m_mailbox_in.push<renderMe::property::AsyncTask*>(&renderMe::core::RunnerKiller());

	renderMe::core::Runner runner2;
	runner2.m_mailbox_in.push<renderMe::property::AsyncTask*>(&loader2);
	runner2.m_mailbox_in.push<renderMe::property::AsyncTask*>(&renderMe::core::RunnerKiller());


	runner.startup();
	runner2.startup();



	runner.join();
	runner2.join();
	std::cout << "ALL joined\n";
	
	std::cout << "*****text1***************\n";
	std::cout << text1 << "\n";

	std::cout << "*****text2***************\n";
	std::cout << text2 << "\n";
	

	std::cout << "bye...\n";

    return 0;
}
