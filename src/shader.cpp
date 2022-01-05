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

#include "shader.h"
#include "file.h"
#include "memalloc.h"
#include "exceptions.h"
#include "misc_utils.h"
#include "renderer.h"
#include "plugin.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface;

dsstring Shader::m_rootpath = ".";
bool Shader::m_addshaderspath = false;

Shader::Shader( void ) :
m_data( NULL ), 
m_compiled( false ), 
m_datasize( -1 ),
m_dataowner( false )
{
}

Shader::Shader( const dsstring& p_filepath, bool p_compiled ) : 
m_filepath( p_filepath ), 
m_data( NULL ), 
m_compiled( p_compiled ), 
m_datasize( -1 ),
m_dataowner(false)
{
}

Shader::Shader( bool p_compiled ) : 
m_data( NULL ), 
m_compiled( p_compiled ), 
m_datasize( -1 ),
m_dataowner(false)
{
}

Shader::~Shader( void )
{
    ReleaseData();
}

void* Shader::GetData( void ) const
{
    return m_data;
}

size_t Shader::GetDataSize( void ) const
{
    return m_datasize;
}

void Shader::GetPath(dsstring& p_path) const
{
    p_path = compute_final_path();
}

void Shader::GetBasePath(dsstring& p_path) const
{
    p_path = m_filepath;
}


bool Shader::LoadFromFile( void )
{
    if( m_data )
    {
        // nettoyer
        ReleaseData();
    }

    if( "" == m_filepath )
    {
        _DSEXCEPTION( "Shader filepath not initialized!" );
    }
    long size;
    void* data = Utils::File::LoadAndAllocBinaryFile( compute_final_path(), &size );
    if( !data )
    {
        //return false;
        _DSEXCEPTION( "Unable to load shader " << m_filepath );
    }
    m_data = data;
    m_datasize = size;

    m_dataowner = true;
    return true;
}

void Shader::EnableShadersDescrInFinalPath(bool p_state)
{
    m_addshaderspath = p_state;
}

void Shader::SetRootPath(const dsstring& p_path)
{
    m_rootpath = p_path;
}

bool Shader::IsCompiled(void) const
{
    return m_compiled;
}

void Shader::SetCompilationFlag(bool p_compiled)
{
	m_compiled = p_compiled;
}

bool Shader::LoadFromFile( const dsstring& p_filepath, bool p_compiled )
{
    m_filepath = p_filepath;
    m_compiled = p_compiled;
    return LoadFromFile();
}

void Shader::ReleaseData( void )
{
    if( m_data && m_dataowner )
    {
        _DRAWSPACE_DELETE_N_( m_data );
        m_data = NULL;
        m_datasize = -1;
    }
}

void Shader::SetText( const dsstring& p_text )
{
    m_data = (void *)_DRAWSPACE_NEW_EXPLICIT_SIZE_( char, char[p_text.size()], p_text.size() );
    m_datasize = p_text.size();
    memcpy( m_data, (void *)p_text.c_str(), p_text.size() );
}

dsstring Shader::compute_final_path( void ) const
{
    dsstring final_path = m_rootpath + "/";
    if( m_addshaderspath )    
    {
        Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;
        dsstring m_shader_descr;
        renderer->GetShadersDescr( m_shader_descr );
        final_path += m_shader_descr + "/";
    }
    final_path += m_filepath;

    return final_path;
}

void Shader::SetData(void* p_data, long p_size)
{
    m_data = p_data;
    m_datasize = p_size;
}
