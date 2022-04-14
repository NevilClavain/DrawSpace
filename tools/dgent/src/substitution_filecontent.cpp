/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

#include <fstream>
#include <sstream>

#include "substitution_filecontent.h"
#include "folder.h"


FilecontentSubstitution::FilecontentSubstitution(const SubstitutionTable& p_substitution_table):
m_substitution_table(p_substitution_table)
{
}

void FilecontentSubstitution::Process(const Folder& p_folder) const
{
	p_folder.Accept<FilecontentSubstitution>(*this);
}

void FilecontentSubstitution::ProcessPath(const std::filesystem::path& p_path) const
{
	if (std::filesystem::file_type::regular == std::filesystem::status(p_path).type())
	{
		const std::string path{ p_path.u8string() };
		std::ifstream f(path);
		if (f)
		{
			std::ostringstream ss;
			ss << f.rdbuf();
			auto file_content{ ss.str() };
			f.close();

			for (const auto& e : m_substitution_table)
			{
				// build pattern to search and replace
				const std::string key{ e.first };
				const std::string pattern_to_replace = std::string("??") + key + std::string("??");

				const std::string new_string{ e.second };

				//search and replace

				size_t pos = file_content.find(pattern_to_replace);
				while (pos != std::string::npos)
				{
					file_content.replace(pos, pattern_to_replace.size(), new_string);
					pos = file_content.find(pattern_to_replace, pos + new_string.size());
				}
			}

			std::ofstream f_out(path);
			if (f_out)
			{
				f_out << file_content;
				f_out.close();
			}
		}
	}
}