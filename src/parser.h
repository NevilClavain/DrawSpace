/***************************************************************************
*                                                                          *
* DrawSpace Rendering engine                                               *
* Emmanuel Chaumont Copyright (c) 2013-2014                                *
*                                                                          *
* This file is part of DrawSpace.                                          *
*                                                                          *
*    DrawSpace is free software: you can redistribute it and/or modify     *
*    it under the terms of the GNU General Public License as published by  *
*    the Free Software Foundation, either version 3 of the License, or     *
*    (at your option) any later version.                                   *
*                                                                          *
*    DrawSpace is distributed in the hope that it will be useful,          *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*    GNU General Public License for more details.                          *
*                                                                          *
*    You should have received a copy of the GNU General Public License     *
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.    *
*                                                                          *
***************************************************************************/

#ifndef _PARSER_H_
#define _PARSER_H_

#include "drawspace_commons.h"

namespace DrawSpace
{
namespace Utils
{
class Parser
{
protected:

    void split_line( const dsstring& p_line, const dsstring& p_separators, std::vector<dsstring>& p_words );
    virtual bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words ) = 0;
public:

    Parser( void );
    virtual ~Parser( void );
    bool Run( const dsstring& p_filepath, const dsstring& p_separators );
};
}
}

#endif