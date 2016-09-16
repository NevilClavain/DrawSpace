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

#ifndef _SHADER_H_
#define _SHADER_H_

#include "drawspace_commons.h"
#include "asset.h"
#include "vector.h"

#define SHADER_TEXT_KEYWORD    "Shader"
#define SHADER_ARC_MAGICNUMBER 0x1067

namespace DrawSpace
{
namespace Core
{

class Shader : public Asset
{
protected:
    dsstring                        m_path;
    void*                           m_data;
    size_t                          m_datasize;
    bool                            m_compiled;

    virtual bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

public:
    Shader( void );
    Shader( const dsstring& p_path, bool p_compiled );
    Shader( bool p_compiled );
    ~Shader( void );

    bool	IsCompiled( void );
    void*	GetData( void );
    size_t	GetDataSize( void );

    bool	LoadFromFile( void );
    void	ReleaseData( void );

    virtual bool ApplyProperties( void );

    void	Serialize( Utils::Archive& p_archive );
    bool	Unserialize( Utils::Archive& p_archive );

    void    DumpProperties( dsstring& p_text );
    bool    ParseProperties( const dsstring& p_text );

    void    SetText( const dsstring& p_text );

    void    GetKeyword( dsstring& p_outkeyword );

    void    GetPath( dsstring& p_path );

    static Asset* Instanciate( void );
};
}
}

#endif
