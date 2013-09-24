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

#ifndef _FONT_H_
#define _FONT_H_

#include "asset.h"
#include "image.h"
#include "texture.h"
#include "fx.h"

namespace DrawSpace
{
namespace Interface
{
class FontImport;
}
namespace Core
{
class Font : public Asset
{
public:
    typedef struct
    {
        float   u, v;
        float   width, height;
        Image*  image;

    } CharMapping;

protected:
    std::map<unsigned char, CharMapping>    m_charset;
    DrawSpace::Interface::FontImport*       m_importer;
    dsstring                                m_texturepath;

    //DrawSpace::Core::Texture*             m_texture;

public:
    Font( void );
    virtual ~Font( void );

    void SetImporter( DrawSpace::Interface::FontImport* p_importer );
    bool Build( const dsstring& p_texturefilepath, const dsstring& p_metricsfilepath );

    void AddCharMapping( unsigned char p_ch, float p_u, float p_v, float p_w, float p_h );
    bool GetCharMapping( unsigned char p_char, CharMapping& p_map );

    void GetTexturePath( dsstring& p_texturepath );

    virtual void Serialize( Core::Factory& p_factory, Utils::Archive& p_archive  );
    virtual void Unserialize( Core::Factory& p_factory, Utils::Archive& p_archive );

    
};
}
}
#endif