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

#ifndef _SHADER_H_
#define _SHADER_H_

#include "drawspace_commons.h"
#include "vector.h"


namespace DrawSpace
{
namespace Core
{

class Shader 
{
protected:
    static dsstring                 m_rootpath;
    static bool                     m_addshaderspath;
    dsstring                        m_filepath;
    void*                           m_data;
    size_t                          m_datasize;
    bool                            m_compiled;
    bool                            m_dataowner;

    dsstring compute_final_path( void ) const;

public:
    Shader( void );
    Shader( const dsstring& p_filepath, bool p_compiled );
    Shader( bool p_compiled );
    ~Shader( void );


    bool	        IsCompiled( void ) const;
    void*	        GetData( void ) const;
    size_t	        GetDataSize( void ) const;
    void            GetPath(dsstring& p_path) const;
    void            GetBasePath(dsstring& p_path) const;

    static void     EnableShadersDescrInFinalPath(bool p_state);
    static void     SetRootPath(const dsstring& p_path);

    bool	        LoadFromFile( void );
    bool            LoadFromFile( const dsstring& p_filepath, bool p_compiled );
    void	        ReleaseData( void );

	void			SetCompilationFlag(bool p_compiled);

    void            SetText( const dsstring& p_text );
    void            SetData(void* p_data, long p_size);
};
}
}

#endif
