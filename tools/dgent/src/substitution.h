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
#include "drawspace_commons.h"

//fwd decl
class Folder;

using SubstitutionTable = std::map<dsstring, dsstring>;

class ISubstitutionContainer
{
public:
	virtual void Run(const Folder& p_folder) const = 0;
};

template<class T>
class SubstitutionContainer : public ISubstitutionContainer
{
private:
	SubstitutionTable m_substitution_table;
public:
	SubstitutionContainer(const SubstitutionTable& p_table):
	m_substitution_table(p_table)
	{
	}

	void Run(const Folder& p_folder) const
	{
		const T substitution_method(m_substitution_table);
		substitution_method.Process(p_folder);
	}
	friend Folder& operator>>(Folder& p_in, const ISubstitutionContainer& p_obj);
};
