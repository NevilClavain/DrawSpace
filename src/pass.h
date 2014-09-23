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

#ifndef _PASS_H_
#define _PASS_H_

#include "texture.h"
#include "transformqueue.h"
#include "renderingqueue.h"
#include "viewportquad.h"
#include "configurable.h"

#define PASS_TEXT_KEYWORD    "Pass"
#define PASS_ARC_MAGICNUMBER 0x4041


namespace DrawSpace
{

class Pass : public Core::Configurable
{
protected:

    typedef enum
    {
        TEXTURE_NAME,
        PASS_NAME,

    } TextureSource;

    typedef struct
    {
        TextureSource   source;
        dsstring        name;

    } TextureSourceName;


    Core::RenderingQueue*   m_renderingqueue;
    ViewportQuad*           m_viewportquad;

    dsstring                m_name;

    virtual bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

    // pour empecher l'instanciation
    Pass( const dsstring& p_name );
public:
    virtual ~Pass( void );

    virtual Core::RenderingQueue* GetRenderingQueue( void );

    virtual void CreateViewportQuad( void );
    virtual void CreateViewportQuad( dsreal p_viewport_width, dsreal p_viewport_height );

    virtual ViewportQuad* GetViewportQuad( void );

    virtual void GetName( dsstring& p_name );


    virtual void Serialize( Utils::Archive& p_archive  );
    virtual bool Unserialize( Utils::Archive& p_archive );

    virtual void DumpProperties( dsstring& p_text );
    virtual bool ParseProperties( const dsstring& p_text );


    virtual void ApplyProperties( void );

};

class FinalPass : public Pass
{
protected:

    //virtual bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

public:
    FinalPass( const dsstring& p_name );
    virtual ~FinalPass( void );

    /*
    virtual void Serialize( Utils::Archive& p_archive  );
    virtual bool Unserialize( Utils::Archive& p_archive );

    virtual void DumpProperties( dsstring& p_text );
    virtual bool ParseProperties( const dsstring& p_text );


    virtual void ApplyProperties( void );
    */

};

class IntermediatePass : public Pass
{
protected:
    Core::Texture*          m_targettexture;

    //virtual bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

public:
    IntermediatePass( const dsstring& p_name );
    IntermediatePass( const dsstring& p_name, long p_target_width, long p_target_height );
    virtual ~IntermediatePass( void );

    Core::Texture* GetTargetTexture( void );

    /*

    virtual void Serialize( Utils::Archive& p_archive  );
    virtual bool Unserialize( Utils::Archive& p_archive );

    virtual void DumpProperties( dsstring& p_text );
    virtual bool ParseProperties( const dsstring& p_text );


    virtual void ApplyProperties( void );
    */

};
}

#endif
