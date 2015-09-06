/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#include "procedural.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Procedural;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Atomic::GetId( dsstring& p_id )
{
    p_id = m_id;
}

void Atomic::SetId( const dsstring& p_id )
{
    m_id = p_id;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Publisher::Publisher( void ) : m_child( NULL ), m_handler( NULL )
{
}

void Publisher::Apply( void )
{
    if( m_handler && m_child )
    {
        (*m_handler)( m_child->GetResultValue() );
    }
}

Atomic* Publisher::GetResultValue( void )
{
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Source::Source( void ) : m_rules( NULL )
{

}

Source::~Source( void )
{

}

void Source::RegisterHandler( BaseCallback<void, Atomic*>* p_handler )
{
    m_handler = p_handler;
}

void Source::SetRules( Atomic* p_rules )
{
    if( !m_handler )
    {
        return;
    }
    m_rules = p_rules;
    set_rules_handler( m_rules );
}

void Source::Run( void )
{
    if( !m_rules )
    {
        return;
    }

    m_rules->Apply();
}

void Source::set_rules_handler( Atomic* p_atom )
{
    Publisher* pub = dynamic_cast<Publisher*>( p_atom );
    if( pub )
    {
        pub->SetHandler( m_handler );

        // appel recursif
        set_rules_handler( pub->GetChild() );
    }
}
