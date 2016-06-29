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

#include "procedural_seeds.h"
#include <random>

using namespace DrawSpace;
using namespace DrawSpace::Procedural;

SeedsBase::SeedsBase( void )
{
    memset( m_base, 0, BaseSize * sizeof( int ) );
}

SeedsBase::~SeedsBase( void )
{
}

void SeedsBase::Initialize( unsigned int p_globalseed )
{
    std::default_random_engine generator;
    std::uniform_int_distribution<unsigned int> rand( 0, 1000000000 );

    generator.seed( p_globalseed );

    for( int i = 0; i < BaseSize; i++ )
    {
        m_base[i] = rand( generator );
    }
}

void SeedsBase::InitializeFromCurrentTime( void )
{
    std::default_random_engine generator;
    std::uniform_int_distribution<unsigned int> rand( 0, 1000000000 );

    generator.seed( ::GetTickCount() );

    for( int i = 0; i < BaseSize; i++ )
    {
        m_base[i] = rand( generator );
    }
}

int SeedsBase::GetSeed( unsigned int p_index )
{
    if( p_index < BaseSize )
    {
        return m_base[p_index];
    }
    return  0;
}
