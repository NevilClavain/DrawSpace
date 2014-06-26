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

#include "image.h"
#include "transformation.h"
#include "maths.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Utils;

Image::Image( long p_virtual_width, long p_virtual_height ) : /*m_width( p_width ), m_height( p_height ),*/ m_x( 0.0 ), m_y( 0.0 ), m_scale_x( 1.0 ), m_scale_y( 1.0 ), m_rotation_angle( 0.0 )
{
    Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->GetRenderCharacteristics( m_rc );

    m_width = ( (dsreal)p_virtual_width / (dsreal)DRAWSPACE_GUI_WIDTH ) * m_rc.width_viewport;
    m_height = ( (dsreal)p_virtual_height / (dsreal)DRAWSPACE_GUI_HEIGHT ) * m_rc.height_viewport;

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
}

Image::Image( long p_virtual_width, long p_virtual_height, Utils::Vector& p_uv1, Utils::Vector& p_uv2, Utils::Vector& p_uv3, Utils::Vector& p_uv4 ) : 
/*m_width( p_width ), m_height( p_height ),*/ m_x( 0.0 ), m_y( 0.0 ), m_rotation_angle( 0.0 )
{
    Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->GetRenderCharacteristics( m_rc );

    m_width = ( (dsreal)p_virtual_width / (dsreal)DRAWSPACE_GUI_WIDTH ) * m_rc.width_viewport;
    m_height = ( (dsreal)p_virtual_height / (dsreal)DRAWSPACE_GUI_HEIGHT ) * m_rc.height_viewport;

    Vertex v1, v2, v3, v4;
    v1.x = - m_width / 2.0;
    v1.y = m_height / 2.0;
    v1.z = -1.0;
    v1.tu[0] = (float)p_uv1[0];
    v1.tv[0] = (float)p_uv1[1];

    v2.x = m_width / 2.0;
    v2.y = m_height / 2.0;
    v2.z = -1.0;
    v2.tu[0] = (float)p_uv2[0];
    v2.tv[0] = (float)p_uv2[1];

    v3.x = m_width / 2.0;
    v3.y = - m_height / 2.0;
    v3.z = -1.0;
    v3.tu[0] = (float)p_uv3[0];
    v3.tv[0] = (float)p_uv3[1];

    v4.x = - m_width / 2.0;
    v4.y = - m_height / 2.0;
    v4.z = -1.0;
    v4.tu[0] = (float)p_uv4[0];
    v4.tv[0] = (float)p_uv4[1];

    m_meshe = _DRAWSPACE_NEW_( Meshe, Meshe );
    m_meshe->AddVertex( v1 );
    m_meshe->AddVertex( v2 );
    m_meshe->AddVertex( v3 );
    m_meshe->AddVertex( v4 );
    m_meshe->AddTriangle( Triangle( 0, 2, 1 ) );
    m_meshe->AddTriangle( Triangle( 0, 3, 2 ) );
}

Image::Image( dsreal p_width, dsreal p_height ) : m_width( p_width ), m_height( p_height ), m_x( 0.0 ), m_y( 0.0 ), m_scale_x( 1.0 ), m_scale_y( 1.0 ), m_rotation_angle( 0.0 )
{
    Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->GetRenderCharacteristics( m_rc );

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
}

Image::Image( dsreal p_width, dsreal p_height, Utils::Vector& p_uv1, Utils::Vector& p_uv2, Utils::Vector& p_uv3, Utils::Vector& p_uv4 ) : 
m_width( p_width ), m_height( p_height ), m_x( 0.0 ), m_y( 0.0 ), m_rotation_angle( 0.0 )
{
    Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->GetRenderCharacteristics( m_rc );

    Vertex v1, v2, v3, v4;
    v1.x = - m_width / 2.0;
    v1.y = m_height / 2.0;
    v1.z = -1.0;
    v1.tu[0] = (float)p_uv1[0];
    v1.tv[0] = (float)p_uv1[1];

    v2.x = m_width / 2.0;
    v2.y = m_height / 2.0;
    v2.z = -1.0;
    v2.tu[0] = (float)p_uv2[0];
    v2.tv[0] = (float)p_uv2[1];

    v3.x = m_width / 2.0;
    v3.y = - m_height / 2.0;
    v3.z = -1.0;
    v3.tu[0] = (float)p_uv3[0];
    v3.tv[0] = (float)p_uv3[1];

    v4.x = - m_width / 2.0;
    v4.y = - m_height / 2.0;
    v4.z = -1.0;
    v4.tu[0] = (float)p_uv4[0];
    v4.tv[0] = (float)p_uv4[1];

    m_meshe = _DRAWSPACE_NEW_( Meshe, Meshe );
    m_meshe->AddVertex( v1 );
    m_meshe->AddVertex( v2 );
    m_meshe->AddVertex( v3 );
    m_meshe->AddVertex( v4 );
    m_meshe->AddTriangle( Triangle( 0, 2, 1 ) );
    m_meshe->AddTriangle( Triangle( 0, 3, 2 ) );
}



Image::~Image( void )
{
    _DRAWSPACE_DELETE_( m_meshe );
}

void Image::OnDraw( void )
{
    Matrix view, trans, world, scale, rot;
    view.Identity();
    trans.Translation( m_x, m_y, 0.0 );
    scale.Scale( m_scale_x, m_scale_y, 1.0 );
    rot.Rotation( Vector( 0.0, 0.0, 1.0, 1.0 ), m_rotation_angle );
    Transformation chain;

    chain.PushMatrix( trans );
    chain.PushMatrix( rot );
    chain.PushMatrix( scale );
    chain.BuildResult();
    chain.GetResult( &world );

    Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;
    renderer->DrawMeshe( m_meshe->GetVertexListSize(), m_meshe->GetTrianglesListSize(), view, world );
}

void Image::SetTranslation( dsreal p_x, dsreal p_y )
{
    m_x = p_x;
    m_y = p_y;
}

void Image::SetVirtualTranslation( long p_x ,long p_y )
{
    m_x = ( ( ( (dsreal)p_x / (dsreal)DRAWSPACE_GUI_WIDTH ) - 0.5 ) /*/ 0.5 */) * ( m_rc.width_viewport /*/ 2.0*/ );
    m_y = ( ( 0.5 - ( (dsreal)p_y / (dsreal)DRAWSPACE_GUI_HEIGHT ) ) /*/ 0.5 */ ) * ( m_rc.height_viewport /*/ 2.0*/ );

    m_virtual_x = p_x;
    m_virtual_y = p_y;
}

void Image::GetVirtualTranslation( long& p_x, long& p_y )
{
    p_x = m_virtual_x;
    p_y = m_virtual_y;
}

void Image::SetScale( dsreal p_sx, dsreal p_sy )
{
    m_scale_x = p_sx;
    m_scale_y = p_sy;
}

void** Image::GetRenderMesheData( void )
{
    return &m_renderer_meshe_data;
}

void Image::SetRotationAngle( dsreal p_angle )
{
    m_rotation_angle = Maths::DegToRad( p_angle );
}