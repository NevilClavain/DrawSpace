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

#include "folder.h"

Folder::Folder(const dsstring& p_path) :
m_path(p_path)
{
}

Folder Folder::CloneTo(const dsstring& p_path) const
{
	const auto copy_options{ std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive };
	std::filesystem::copy(m_path, p_path, copy_options);
	Folder folder(p_path);
	return folder;
}