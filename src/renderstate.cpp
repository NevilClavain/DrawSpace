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

#include "renderstate.h"


using namespace DrawSpace::Core;

RenderState::RenderState( Operation p_operation, const dsstring& p_arg ) : 
m_operation( p_operation ), 
m_arg( p_arg )
{
}

RenderState::RenderState( void )
{
}

void RenderState::SetOperation( Operation p_operation )
{
    m_operation = p_operation;
}

void RenderState::SetArg( const dsstring& p_arg )
{
    m_arg = p_arg;
}

void RenderState::SetExtendedArgs(const std::vector<dsstring>& p_args)
{
    m_extendedargs = p_args;
}

RenderState::Operation RenderState::GetOperation( void ) const
{
    return m_operation;
}

void RenderState::GetArg( dsstring& p_arg ) const
{
    p_arg = m_arg;
}

std::vector<dsstring> RenderState::GetExtendedArgs(void) const
{
    return m_extendedargs;
}

