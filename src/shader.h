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

#ifndef _SHADER_H_
#define _SHADER_H_

#include "drawspace_commons.h"
#include "asset.h"
#include "vector.h"
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

    std::map<long, Utils::Vector>   m_params;

public:
    Shader( const dsstring& p_path, bool p_compiled = true );
    ~Shader( void );

    bool	IsCompiled( void );
    void*	GetData( void );
    size_t	GetDataSize( void );

    bool	LoadFromFile( void );
    void	ReleaseData( void );

    void	Serialize( Core::Factory& p_factory, Utils::Archive& p_archive );
    void	Unserialize( Core::Factory& p_factory, Utils::Archive& p_archive );

    void	SetParam( long p_register, const Utils::Vector& p_values );
    void	GetParamsTable( std::map<long, Utils::Vector>& p_table );

    void    SetText( const dsstring& p_text );
};
}
}

#endif