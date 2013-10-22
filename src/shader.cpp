/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#include "shader.h"
#include "file.h"
#include "memalloc.h"

using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

Shader::Shader( const dsstring& p_path, bool p_compiled ) : m_path( p_path ), m_data( NULL ), m_compiled( p_compiled ), m_datasize( -1 )
{
}

Shader::~Shader( void )
{
}

bool Shader::IsCompiled( void )
{
    return m_compiled;
}

void* Shader::GetData( void )
{
    return m_data;
}

size_t Shader::GetDataSize( void )
{
    return m_datasize;
}

bool Shader::LoadFromFile( void )
{
    long size;
    void* data = File::LoadAndAllocBinaryFile( m_path, &size );
    if( !data )
    {
        return false;
    }
    m_data = data;
    m_datasize = size;
    return true;
}

void Shader::ReleaseData( void )
{
    if( m_data )
    {
        _DRAWSPACE_DELETE_N_( m_data );
        m_data = NULL;
    }
}

void Shader::Serialize( Factory& p_factory, Archive& p_archive  )
{


}

void Shader::Unserialize( Factory& p_factory, Archive& p_archive )
{

}

void Shader::SetParam( long p_register, const Vector& p_values )
{
    m_params[p_register] = p_values;
}

void Shader::GetParamsTable( std::map<long, Vector>& p_table )
{
    p_table = m_params;
}

void Shader::SetText( const dsstring& p_text )
{
    m_data = (void *)_DRAWSPACE_NEW_EXPLICIT_SIZE_( char, char[p_text.size()], p_text.size() );
    m_datasize = p_text.size();
    memcpy( m_data, (void *)p_text.c_str(), p_text.size() );
}