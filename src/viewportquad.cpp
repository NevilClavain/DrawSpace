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

#include "viewportquad.h"
#include "renderer.h"
#include "plugin.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Interface;

ViewportQuad::ViewportQuad( const dsstring& p_name, dsreal p_width, dsreal p_height ) : TransformNode( p_name ), m_width( p_width ), m_height( p_height )
{
    Vertex v1, v2, v3, v4;
    v1.x = - m_width / 2.0;
    v1.y = m_height / 2.0;
    v1.z = -1.0;
    v1.tu[0] = 0.0;
    v1.tv[0] = 0.0;

    v2.x = m_width / 2.0;
    v2.y = m_height / 2.0;
    v2.z = -1.0;
    v2.tu[0] = 1.0;
    v2.tv[0] = 0.0;

    v3.x = m_width / 2.0;
    v3.y = - m_height / 2.0;
    v3.z = -1.0;
    v3.tu[0] = 1.0;
    v3.tv[0] = 1.0;

    v4.x = - m_width / 2.0;
    v4.y = - m_height / 2.0;
    v4.z = -1.0;
    v4.tu[0] = 0.0;
    v4.tv[0] = 1.0;

    m_meshe = _DRAWSPACE_NEW_( Meshe, Meshe );
    m_meshe->AddVertex( v1 );
    m_meshe->AddVertex( v2 );
    m_meshe->AddVertex( v3 );
    m_meshe->AddVertex( v4 );
    m_meshe->AddTriangle( Triangle( 0, 2, 1 ) );
    m_meshe->AddTriangle( Triangle( 0, 3, 2 ) );

    // prepare projection matrix
    DrawSpace::Interface::Renderer::Characteristics characteristics;
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->GetRenderCharacteristics( characteristics );
    m_projection.Perspective( characteristics.width_viewport, characteristics.height_viewport, 1.0, 10.0 );
}

ViewportQuad::~ViewportQuad( void )
{
    _DRAWSPACE_DELETE_( m_meshe );
}

void ViewportQuad::OnDraw( void )
{
    Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;
    DrawSpace::Utils::Matrix view;
    view.Identity();

    renderer->DrawMeshe( m_meshe->GetVertexListSize(), m_meshe->GetTrianglesListSize(), m_globaltransformation, view, m_projection );
}
