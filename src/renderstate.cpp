/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#include "renderstate.h"

std::map<DrawSpace::Core::RenderState::Operation, dsstring> DrawSpace::Core::RenderState::m_tostring;
std::map<dsstring, DrawSpace::Core::RenderState::Operation> DrawSpace::Core::RenderState::m_fromstring;

using namespace DrawSpace::Core;

RenderState::RenderState( Operation p_operation, const dsstring& p_arg ) : m_operation( p_operation ), m_arg( p_arg )
{
}

RenderState::RenderState( void ) : m_operation( NONE )
{
}

RenderState::~RenderState( void )
{
}

void RenderState::InitStringMaps( void )
{
    m_tostring[NONE] = "NONE";
    m_tostring[SETCLEARMASK] = "SETCLEARMASK";
    m_tostring[SETCLEARCOLOR] = "SETCLEARCOLOR";
    m_tostring[SETCULLING] = "SETCULLING";
    m_tostring[ENABLEZBUFFER] = "ENABLEZBUFFER";
    m_tostring[SETTEXTUREFILTERTYPE] = "SETTEXTUREFILTERTYPE";
    m_tostring[SETFILLMODE] = "SETFILLMODE";
    m_tostring[ALPHABLENDENABLE] = "ALPHABLENDENABLE";
    m_tostring[ALPHABLENDOP] = "ALPHABLENDOP";
    m_tostring[ALPHABLENDFUNC] = "ALPHABLENDFUNC";
    m_tostring[ALPHABLENDDEST] = "ALPHABLENDDEST";
    m_tostring[ALPHABLENDSRC] = "ALPHABLENDSRC";

    m_fromstring["NONE"] = NONE;
    m_fromstring["SETCLEARMASK"] = SETCLEARMASK;
    m_fromstring["SETCLEARCOLOR"] = SETCLEARCOLOR;
    m_fromstring["SETCULLING"] = SETCULLING;
    m_fromstring["ENABLEZBUFFER"] = ENABLEZBUFFER;
    m_fromstring["SETTEXTUREFILTERTYPE"] = SETTEXTUREFILTERTYPE;
    m_fromstring["SETFILLMODE"] = SETFILLMODE;
    m_fromstring["ALPHABLENDENABLE"] = ALPHABLENDENABLE;
    m_fromstring["ALPHABLENDOP"] = ALPHABLENDOP;
    m_fromstring["ALPHABLENDFUNC"] = ALPHABLENDFUNC;
    m_fromstring["ALPHABLENDDEST"] = ALPHABLENDDEST;
    m_fromstring["ALPHABLENDSRC"] = ALPHABLENDSRC;
}

void RenderState::SetOperation( Operation p_operation )
{
    m_operation = p_operation;
}

void RenderState::SetOperationFromString( const dsstring& p_operation )
{
    SetOperation( m_fromstring[p_operation] );
}

void RenderState::SetArg( const dsstring& p_arg )
{
    m_arg = p_arg;
}

RenderState::Operation RenderState::GetOperation( void )
{
    return m_operation;
}

void RenderState::GetOperationToString( dsstring& p_operation )
{
    p_operation = m_tostring[m_operation];
}

void RenderState::GetArg( dsstring& p_arg )
{
    p_arg = m_arg;
}

