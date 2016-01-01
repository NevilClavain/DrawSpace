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

#ifndef _PASS_H_
#define _PASS_H_

#include "texture.h"
#include "renderingqueue.h"
#include "viewportquad.h"
#include "configurable.h"

#define FINALPASS_TEXT_KEYWORD              "FinalPass"
#define FINALPASS_ARC_MAGICNUMBER           0x4041

#define INTERMEDIATEPASS_TEXT_KEYWORD       "IntermediatePass"
#define INTERMEDIATEPASS_ARC_MAGICNUMBER    0x4042

namespace DrawSpace
{

class Pass : public Core::Configurable
{
public:
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


protected:

    Core::RenderingQueue*   m_renderingqueue;
    ViewportQuad*           m_viewportquad;

    dsstring                m_name;

    bool                    m_initialized;

    virtual bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

    // pour empecher l'instanciation
    Pass( void );    
public:
    virtual ~Pass( void );

    virtual Core::RenderingQueue* GetRenderingQueue( void );

    virtual void CreateViewportQuad( void );
    virtual void CreateViewportQuad( dsreal p_viewport_width, dsreal p_viewport_height );

    virtual ViewportQuad* GetViewportQuad( void );

    //virtual void SetName( const dsstring& p_name );
    //virtual void GetName( dsstring& p_name );


    virtual void Serialize( Utils::Archive& p_archive  );
    virtual bool Unserialize( Utils::Archive& p_archive );

    virtual void DumpProperties( dsstring& p_text );
    virtual bool ParseProperties( const dsstring& p_text );

    virtual void ApplyProperties( void );

    virtual void SetSpecificName( const dsstring& p_name );
    virtual void GetSpecificName( dsstring& p_name );


};

class FinalPass : public Pass
{
protected:

    //virtual bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

public:
    FinalPass( void );
    FinalPass( const dsstring& p_name );
    virtual ~FinalPass( void );

    bool Initialize( void );

    void DumpProperties( dsstring& p_text );

    void GetKeyword( dsstring& p_outkeyword );

    virtual void ApplyProperties( void );

    static Configurable* Instanciate( void );
};

class IntermediatePass : public Pass
{
protected:
    Core::Texture*                  m_targettexture;

    bool                            m_targetdims_fromrenderer;
    long                            m_targetdims_width;
    long                            m_targetdims_height;

    Core::Texture::RenderPurpose    m_renderpurpose;

    void init_properties( void );
    virtual bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

public:
    IntermediatePass( void );
    IntermediatePass( const dsstring& p_name );
    virtual ~IntermediatePass( void );

    void SetTargetDimsFromRenderer( bool p_state );
    void SetTargetDims( long p_width, long p_height );
    void SetRenderPurpose( Core::Texture::RenderPurpose p_renderpurpose );

    bool Initialize( void );
    
    Core::Texture* GetTargetTexture( void );

    virtual void DumpProperties( dsstring& p_text );
    virtual void ApplyProperties( void );

    void GetKeyword( dsstring& p_outkeyword );

    static Configurable* Instanciate( void );

};
}

#endif
