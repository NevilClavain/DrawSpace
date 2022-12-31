/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

#pragma once
#include "drawspace_commons.h"
#include <filesystem>

class Folder
{
private:
	Folder(void) {};
	std::filesystem::path m_path;
public:
	Folder(const dsstring& p_path);
	Folder CloneTo(const dsstring& p_path) const;
	
	template<class T>
	void Accept(const T& p_visitor) const
	{
		for (const auto& dir_entry : std::filesystem::recursive_directory_iterator{ m_path })
		{
			std::filesystem::path current_path{ dir_entry };
			p_visitor.ProcessPath(current_path);
		}
	}
};


