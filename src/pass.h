/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

#pragma once

#include "texture.h"
#include "renderingqueue.h"
#include "viewportquad.h"

namespace DrawSpace
{
class Pass
{
public:
    enum TextureSource
    {
        TEXTURE_NAME,
        PASS_NAME,
    };

    struct
    {
        TextureSource   source;
        dsstring        name;
    } TextureSourceName;

protected:

    Core::RenderingQueue*   m_renderingqueue{ nullptr };
    ViewportQuad*           m_viewportquad{ nullptr };
    dsstring                m_name;
    bool                    m_initialized{ false };

    // pour empecher l'instanciation
    Pass( void );    
public:
    virtual ~Pass( void );

    virtual Core::RenderingQueue*   GetRenderingQueue( void ) const;
    virtual ViewportQuad*           GetViewportQuad(void) const;
    virtual void                    GetSpecificName(dsstring& p_name) const;

    virtual void CreateViewportQuad( dsreal p_z_offset = 0.0 );
    virtual void CreateViewportQuad( dsreal p_viewport_width, dsreal p_viewport_height, dsreal p_z_offset = 0.0 );

    virtual void SetSpecificName( const dsstring& p_name );    
};

class FinalPass : public Pass
{
public:
    FinalPass( void );
    FinalPass( const dsstring& p_name );
    virtual ~FinalPass( void );

    bool Initialize( void );
};

class IntermediatePass : public Pass
{
protected:
    Core::Texture*                  m_targettexture{ nullptr };

    bool                            m_targetdims_fromrenderer;
    long                            m_targetdims_width;
    long                            m_targetdims_height;

    Core::Texture::RenderPurpose    m_renderpurpose;
    Core::Texture::RenderTarget     m_rendertarget;

public:
    IntermediatePass( void );
    IntermediatePass( const dsstring& p_name );
    virtual ~IntermediatePass( void );

    void SetTargetDimsFromRenderer( bool p_state );
    void SetTargetDims( long p_width, long p_height );
    void SetRenderPurpose( Core::Texture::RenderPurpose p_renderpurpose );
    void SetRenderTarget( Core::Texture::RenderTarget p_rendertarget );

    bool Initialize( void );
    
    Core::Texture* GetTargetTexture( void ) const;

};
}

