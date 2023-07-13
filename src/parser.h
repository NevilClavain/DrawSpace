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

#include <vector>
#include <functional>

#include "drawspace_commons.h"


namespace DrawSpace
{

    namespace Parser
    {
        using ParserCallback = std::function<void(const dsstring&, long, const std::vector<dsstring>&)>;

        void run(const dsstring& p_filepath, 
                    const dsstring& p_separators, 
                    const ParserCallback& p_callback
                );
    }

}

/*
namespace DrawSpace
{
namespace Utils
{
class Parser
{
protected:

    dsstring m_lasterror;

    void split_line( const dsstring& p_line, const dsstring& p_separators, std::vector<dsstring>& p_words );
    void split_text( const dsstring& p_text, std::vector<dsstring>& p_lines );

    virtual bool on_new_line( const dsstring& p_line, long p_line_num, const std::vector<dsstring>& p_words ) = 0;


    virtual void error_message( long p_line_num, const dsstring& p_msg );
public:

    Parser( void );
    virtual ~Parser( void );
    bool Run( const dsstring& p_filepath, const dsstring& p_separators );
    bool RunOnTextChunk( const dsstring& p_text, const dsstring& p_separators );

    void GetLastError( dsstring& p_lasterror );
};
}
}
*/

