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

#include "substitution_filenames.h"
#include "folder.h"

FilenamesSubstitution::FilenamesSubstitution(const SubstitutionTable& p_substitution_table):
m_substitution_table(p_substitution_table)
{
}

void FilenamesSubstitution::Process(const Folder& p_folder) const
{
	p_folder.Accept<FilenamesSubstitution>(*this);
}

void FilenamesSubstitution::ProcessPath(const std::filesystem::path& p_path) const
{
	std::string path{ p_path.u8string() };
	for (const auto& e : m_substitution_table)
	{
		// build pattern to search and replace
		const std::string key{ e.first };		
		const std::string pattern_to_replace = std::string("xx") + key + std::string("xx");

		const size_t pos{ path.find(pattern_to_replace) };
		if (pos != std::string::npos)
		{
			const size_t pos_end{ pos + strlen(pattern_to_replace.c_str()) };
			path.replace(path.begin() + pos, path.begin() + pos_end, e.second);

			std::filesystem::rename(p_path, std::filesystem::path(path));
		}
	}
}