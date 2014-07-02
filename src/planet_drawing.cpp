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

#include "renderer.h"
#include "plugin.h"
#include "memalloc.h"
#include "exceptions.h"
#include "planet_drawing.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Planet;


FaceDrawingNode::FaceDrawingNode( DrawSpace::Interface::Renderer* p_renderer ) :
m_renderer( p_renderer )
{

}

FaceDrawingNode::~FaceDrawingNode( void )
{


}



Drawing::Drawing( void ) :
m_renderer( NULL ),
m_scenegraph( NULL ),
m_planetbody( NULL )
{

}

Drawing::~Drawing( void )
{

}

void Drawing::SetCurrentPlanetBody( Body* p_planetbody )
{
    m_planetbody = p_planetbody;
}

void Drawing::SetRenderer( DrawSpace::Interface::Renderer* p_renderer )
{
    m_renderer = p_renderer;
}

void Drawing::OnRegister( DrawSpace::Scenegraph* p_scenegraph )
{
    for( std::map<dsstring, NodesSet>::iterator it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
    {
        Pass* current_pass = p_scenegraph->GetPass( (*it).first );

        if( NULL == current_pass )
        {
            dsstring msg = "Planet Drawing : pass '";
            msg += (*it).first;
            msg += "' does not exists in scenegraph";

            _DSEXCEPTION( msg )
        }

        for( long i = 0; i < 6; i++ )
        {
            current_pass->GetRenderingQueue()->Add( (*it).second.nodes[i] );
        }
    }
    m_scenegraph = p_scenegraph;
}

void Drawing::on_renderingnode_draw( RenderingNode* p_rendering_node )
{
    if( !m_planetbody )
    {
        return;
    }

    DrawSpace::Utils::Matrix view;
    DrawSpace::Utils::Matrix proj;

    m_scenegraph->GetCurrentCameraView( view );
    m_scenegraph->GetCurrentCameraProj( proj );

    FaceRenderingNode* face_node = static_cast<FaceRenderingNode*>( p_rendering_node );
    face_node->Draw( m_planetbody->m_patchmeshe->GetVertexListSize(), m_planetbody->m_patchmeshe->GetTrianglesListSize(), m_planetbody->m_diameter / 2.0, m_globaltransformation, view, proj );    
}

void Drawing::RegisterPassSlot( const dsstring p_passname )
{
    // TODO
}

