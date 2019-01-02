/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

#include "pass.h"
#include "renderer.h"
#include "plugin.h"
#include "misc_utils.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

Pass::Pass( void ) : 
m_viewportquad( NULL ),
m_initialized( false )
{
}

Pass::~Pass( void )
{
    if( m_viewportquad )
    {
        _DRAWSPACE_DELETE_( m_viewportquad );
    }
}




RenderingQueue* Pass::GetRenderingQueue( void )
{
    if( !m_initialized )
    {
        _DSEXCEPTION( "pass " + m_name + " not initialized (call Initialize() method)" );
    }
    return m_renderingqueue;
}

void Pass::CreateViewportQuad( dsreal p_z_offset )
{
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    DrawSpace::Interface::Renderer::Characteristics renderer_characteristics;
    renderer->GetRenderCharacteristics( renderer_characteristics );

    if( "" == m_name )
    {
        _DSEXCEPTION( "pass name cannot be empty" );
    }
    m_viewportquad = _DRAWSPACE_NEW_( ViewportQuad, ViewportQuad( renderer_characteristics.width_viewport, renderer_characteristics.height_viewport, p_z_offset ) );

    m_renderingqueue->Add( m_viewportquad );
}

void Pass::CreateViewportQuad( dsreal p_viewport_width, dsreal p_viewport_height, dsreal p_z_offset )
{
    if( "" == m_name )
    {
        _DSEXCEPTION( "pass name cannot be empty" );
    }

    m_viewportquad = _DRAWSPACE_NEW_( ViewportQuad, ViewportQuad( p_viewport_width, p_viewport_height, p_z_offset ) );
    m_renderingqueue->Add( m_viewportquad );
}

ViewportQuad* Pass::GetViewportQuad( void ) const
{
    return m_viewportquad;
}

void Pass::SetSpecificName( const dsstring& p_name )
{
    m_name = p_name;
}

void Pass::GetSpecificName( dsstring& p_name )
{
    p_name = m_name;
}
/////////////////////////////////////////////////////////////////////////////////////////////

FinalPass::FinalPass( void )
{
}

FinalPass::FinalPass( const dsstring& p_name )
{
    m_name = p_name;
}

FinalPass::~FinalPass( void )
{	
    _DRAWSPACE_DELETE_( m_renderingqueue );
}

bool FinalPass::Initialize( void )
{
    m_renderingqueue = _DRAWSPACE_NEW_( RenderingQueue, RenderingQueue );
    m_initialized = true;
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////

IntermediatePass::IntermediatePass( void ) :
m_targetdims_fromrenderer( true ),
m_targetdims_width( 256 ),
m_targetdims_height( 256 ),
m_renderpurpose( Texture::RENDERPURPOSE_COLOR ),
m_rendertarget( Texture::RENDERTARGET_GPU )
{

}



IntermediatePass::IntermediatePass( const dsstring& p_name ) :
m_targetdims_fromrenderer( true ),
m_targetdims_width( 256 ),
m_targetdims_height( 256 ),
m_renderpurpose( Texture::RENDERPURPOSE_COLOR ),
m_rendertarget( Texture::RENDERTARGET_GPU )
{
    m_name = p_name;
}

IntermediatePass::~IntermediatePass( void )
{
    _DRAWSPACE_DELETE_( m_renderingqueue );
    _DRAWSPACE_DELETE_( m_targettexture );
}

void IntermediatePass::SetTargetDimsFromRenderer( bool p_state )
{
    m_targetdims_fromrenderer = p_state; 
}

void IntermediatePass::SetTargetDims( long p_width, long p_height )
{
    m_targetdims_width = p_width;
    m_targetdims_height = p_height;
}

void IntermediatePass::SetRenderPurpose( Core::Texture::RenderPurpose p_renderpurpose )
{
    m_renderpurpose = p_renderpurpose;
}

void IntermediatePass::SetRenderTarget( Core::Texture::RenderTarget p_rendertarget )
{
    m_rendertarget = p_rendertarget;
}

bool IntermediatePass::Initialize( void )
{
    long h_resol, w_resol;
    //////// creation texture target
    if( "" == m_name )
    {
        _DSEXCEPTION( "pass name cannot be empty" );
    }

    if( m_targetdims_fromrenderer )
    {
        DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

        DrawSpace::Interface::Renderer::Characteristics renderer_characteristics;
        renderer->GetRenderCharacteristics( renderer_characteristics );

        h_resol = renderer_characteristics.height_resol;
        w_resol = renderer_characteristics.width_resol;

    }
    else
    {
        h_resol = m_targetdims_height;
        w_resol = m_targetdims_width;
    }

    m_targettexture = _DRAWSPACE_NEW_( Texture, Texture( m_name + dsstring( "/target" ), true, w_resol, h_resol, m_renderpurpose, m_rendertarget ) );
    m_renderingqueue = _DRAWSPACE_NEW_( RenderingQueue, RenderingQueue( m_targettexture ) );

    m_initialized = true;
    return true;
}

Core::Texture* IntermediatePass::GetTargetTexture( void ) const
{
    return m_targettexture;
}
