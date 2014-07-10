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

#include "reticle_widget.h"
#include "maths.h"

using namespace DrawSpace;
using namespace DrawSpace::Gui;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;



ReticleWidget::ReticleWidget( const dsstring& p_name, long p_virtual_width, long p_virtual_height, Scenegraph* p_scenegraph, Widget* p_parentwidget ) :
Widget( p_name, p_virtual_width, p_virtual_height, p_parentwidget ),
m_scenegraph( p_scenegraph ),
m_locked_node( NULL ),
m_locked_body( NULL ),
m_zdepth( 0.0 )
{
    // default clipping params

    m_clipping_params.clipping_policy = CLIPPING_CUT;
    m_clipping_params.xmin = -0.6;
    m_clipping_params.xmax = 0.6;
    m_clipping_params.ymin = -0.5;
    m_clipping_params.ymax = 0.5;

}

ReticleWidget::~ReticleWidget( void )
{
}

void ReticleWidget::LockOnBody( Body* p_locked_body )
{
    m_locked_body = p_locked_body;
}

void ReticleWidget::LockOnTransformNode( DrawSpace::Core::TransformNode* p_locked_node )
{
    m_locked_node = p_locked_node;
}

void ReticleWidget::SetTranslation( dsreal p_x, dsreal p_y )
{
    m_real_posx = p_x;
    m_real_posy = p_y;
}

void ReticleWidget::Transform( void )
{
    Matrix target_mat;
    Vector pos;
    dsreal center_x, center_y;
    dsreal z_proj;

    bool project = false;

    if( m_locked_node )
    {
        m_locked_node->GetSceneWorld( target_mat );
        project = true;
    }
    else if( m_locked_body )
    {
        m_locked_body->GetLastWorldTransformation( target_mat );
        project = true;
    }

    if( project )
    {
        pos[0] = target_mat( 3, 0 );
        pos[1] = target_mat( 3, 1 );
        pos[2] = target_mat( 3, 2 );
        pos[3] = 1.0;

        //////////////////////

        Matrix view;
        Vector view_pos;
        m_scenegraph->GetCurrentCameraView( view );

        view.Transform( &pos, &view_pos );
        m_zdepth = view_pos.Length();
        


        //////////////////////

        m_scenegraph->PointProjection( pos, center_x, center_y, z_proj );

        //

        switch( m_clipping_params.clipping_policy )
        {
            case CLIPPING_CUT:

                if( center_x > m_clipping_params.xmax || center_x < m_clipping_params.xmin ||
                    center_y > m_clipping_params.ymax || center_y < m_clipping_params.ymin ||
                    z_proj < 0.0 )
                {
                    if( m_drawingstate )
                    {
                        SetDrawingState( false );
                    }
                }
                else
                {
                    if( !m_drawingstate )
                    {
                        SetDrawingState( true );
                    }
                }

                break;

            case NO_CLIPPING:

                if( z_proj < 0.0 )
                {
                    if( m_drawingstate )
                    {
                        SetDrawingState( false );
                    }
                }
                else
                {
                    if( !m_drawingstate )
                    {
                        SetDrawingState( true );
                    }
                }
                break;


            case CLIPPING_HOLD:

                if( !m_drawingstate )
                {
                    SetDrawingState( true );
                }

                center_x = Maths::Clamp( m_clipping_params.xmin, m_clipping_params.xmax, center_x );
                center_y = Maths::Clamp( m_clipping_params.ymin, m_clipping_params.ymax, center_y );
                break;
        }

        //

        SetTranslation( center_x, center_y );
    }

    for( size_t i = 0; i < m_children.size(); i++ )
    {
        m_children[i]->Transform();
    }
}

void ReticleWidget::SetClippingParams( const ClippingParams& p_params )
{
    m_clipping_params = p_params;
}

dsreal ReticleWidget::GetLastZDepth( void )
{
    return abs( m_zdepth );
}