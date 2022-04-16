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

#pragma once
#include "jsonparser.h"
#include "substitution.h"

struct Config : public DrawSpace::Utils::JSONParser
{
private:
	std::string							m_template_path;
	std::string							m_destination_path;
	SubstitutionTable					m_substitution_table;

	std::string extract_token(int p_tkindex) const;

public:
	void ParseFromFile(const dsstring& p_filepath);

	std::string			GetTemplatePath(void) const;
	std::string			GetDestinationPath(void) const;
	SubstitutionTable	GetSubstitutionTable(void) const;
};
