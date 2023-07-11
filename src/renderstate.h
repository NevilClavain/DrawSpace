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

#ifndef _RENDERSTATE_H_
#define _RENDERSTATE_H_

#include <vector>
#include "drawspace_commons.h"

namespace DrawSpace
{
namespace Core
{

class RenderState
{
public:

    using Operation = enum
    {
        NONE,
        SETCULLING,
        ENABLEZBUFFER,
        SETTEXTUREFILTERTYPE,
        SETVERTEXTEXTUREFILTERTYPE,
        SETFILLMODE,
        ALPHABLENDENABLE,
        ALPHABLENDOP,
        ALPHABLENDFUNC,
        ALPHABLENDDEST,
        ALPHABLENDSRC,
    };

    RenderState( void );
    RenderState( Operation p_operation, const dsstring& p_arg );

    void SetOperation( Operation p_operation );
    void SetArg( const dsstring& p_arg );
    void SetExtendedArgs(const std::vector<dsstring>& p_args);

    void ClearExtendedArgs(void);
    void PushExtendedArg(const dsstring& p_arg);

    dsstring ToString(void) const;

    Operation GetOperation( void ) const;
    dsstring GetArg( void ) const;
    std::vector<dsstring> GetExtendedArgs(void) const;

private:
    dsstring                                m_arg;         //argument operation renderstate, sous forme de chaine ascii
    std::vector<dsstring>                   m_extendedargs;

    Operation                               m_operation{ NONE };
};
}
}

#endif
