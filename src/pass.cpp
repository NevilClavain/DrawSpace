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

#include "pass.h"
#include "renderer.h"
#include "plugin.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

Pass::Pass( const dsstring& p_name ) : 
m_name( p_name ),
m_viewportquad( NULL )
{	
}

Pass::~Pass( void )
{
    if( m_viewportquad )
    {
        _DRAWSPACE_DELETE_( m_viewportquad );
    }
}

void Pass::GetName( dsstring& p_name )
{
    p_name = m_name;
}


RenderingQueue* Pass::GetRenderingQueue( void )
{
    return m_renderingqueue;
}

void Pass::CreateViewportQuad( void )
{
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    DrawSpace::Interface::Renderer::Characteristics renderer_characteristics;
    renderer->GetRenderCharacteristics( renderer_characteristics );
    m_viewportquad = _DRAWSPACE_NEW_( ViewportQuad, ViewportQuad( m_name + dsstring( "/viewportquad" ), renderer_characteristics.width_viewport, renderer_characteristics.height_viewport ) );

    m_renderingqueue->Add( m_viewportquad );
}

void Pass::CreateViewportQuad( dsreal p_viewport_width, dsreal p_viewport_height )
{
    m_viewportquad = _DRAWSPACE_NEW_( ViewportQuad, ViewportQuad( m_name + dsstring( "/viewportquad" ), p_viewport_width, p_viewport_height ) );
    m_renderingqueue->Add( m_viewportquad );
}

ViewportQuad* Pass::GetViewportQuad( void )
{
    return m_viewportquad;
}

/////////////////////////////////////////////////////////////////////////////////////////////

FinalPass::FinalPass( const dsstring& p_name ) : Pass( p_name )
{
    m_renderingqueue = _DRAWSPACE_NEW_( RenderingQueue, RenderingQueue );
}

FinalPass::~FinalPass( void )
{	
    _DRAWSPACE_DELETE_( m_renderingqueue );
}

/////////////////////////////////////////////////////////////////////////////////////////////

IntermediatePass::IntermediatePass( const dsstring& p_name ) : Pass( p_name )
{
    //////// creation texture target
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    DrawSpace::Interface::Renderer::Characteristics renderer_characteristics;
    renderer->GetRenderCharacteristics( renderer_characteristics );

    m_targettexture = _DRAWSPACE_NEW_( Texture, Texture( m_name + dsstring( "/target" ), true, renderer_characteristics.width_resol, renderer_characteristics.height_resol ) );

    m_renderingqueue = _DRAWSPACE_NEW_( RenderingQueue, RenderingQueue( m_targettexture ) );
}

IntermediatePass::IntermediatePass( const dsstring& p_name, long p_target_width, long p_target_height ) : Pass( p_name )
{
    //////// creation texture target
    m_targettexture = _DRAWSPACE_NEW_( Texture, Texture( m_name + dsstring( "/target" ), true, p_target_width, p_target_height ) );
    m_renderingqueue = _DRAWSPACE_NEW_( RenderingQueue, RenderingQueue( m_targettexture ) );
}

IntermediatePass::~IntermediatePass( void )
{
    _DRAWSPACE_DELETE_( m_renderingqueue );
    _DRAWSPACE_DELETE_( m_targettexture );

}

Core::Texture* IntermediatePass::GetTargetTexture( void )
{
    return m_targettexture;
}