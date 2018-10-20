/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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


#include "nebulaerenderingaspectimpl.h"
#include "renderingaspect.h"
#include "transformaspect.h"
#include "maths.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::RenderGraph;
using namespace DrawSpace::Utils;


NebulaeRenderingAspectImpl::PassSlot::PassSlot( const dsstring& p_pass_name ) :
    m_pass_name( p_pass_name )
{
    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    // BUILD MESHE HERE

    dsreal angle = 90.0;
    int nb_vertex = 0;
    
    m_meshe = _DRAWSPACE_NEW_(Core::Meshe, Core::Meshe);
    
    Vector pos0( 0.0, 0.0, 0.0, 1.0);
    Vector color0(0.0, 0.0, 0.0, 1.0);
    dsreal scale0 = 1.0;
    create_bloc(pos0, color0, scale0, nb_vertex);

    Vector pos1(0.3, 0.2, 0.0, 1.0);
    Vector color1(0.99, 0.0, 0.9, 1.0);
    dsreal scale1 = 1.2;
    create_bloc(pos1, color1, scale1, nb_vertex);

    Vector pos2(0.0, -0.45, 0.27, 1.0);
    Vector color2(0.99, 0.0, 0.9, 1.0);
    dsreal scale2 = 1.9;
    create_bloc(pos2, color2, scale2, nb_vertex);

    Vector pos3(0.67, -0.55, 0.07, 1.0);
    Vector color3(0.99, 0.0, 0.9, 1.0);
    dsreal scale3 = 1.3;
    create_bloc(pos3, color3, scale3, nb_vertex);

    /////////////////
    
    m_cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &PassSlot::on_renderingnode_draw ) );

    m_rendering_node = _DRAWSPACE_NEW_( RenderingNode, RenderingNode );
    m_rendering_node->RegisterHandler( m_cb );
    m_rendering_node->SetMeshe( m_meshe );

    m_world.Identity();
    
    m_view.Identity();
    m_proj.Identity();
}

NebulaeRenderingAspectImpl::PassSlot::~PassSlot( void )
{    
    _DRAWSPACE_DELETE_(m_rendering_node);
    _DRAWSPACE_DELETE_(m_meshe);
    _DRAWSPACE_DELETE_(m_cb);
}

void NebulaeRenderingAspectImpl::PassSlot::create_bloc(const Utils::Vector& p_pos, const Utils::Vector& p_color, dsreal p_scale, int& p_nb_vertex)
{
    int step = 15;
    create_axis_quad( p_pos, p_color, p_scale, X_AXIS, step, p_nb_vertex );
    create_axis_quad( p_pos, p_color, p_scale, Y_AXIS, step, p_nb_vertex );
    create_axis_quad( p_pos, p_color, p_scale, Z_AXIS, step, p_nb_vertex );
}

void NebulaeRenderingAspectImpl::PassSlot::create_axis_quad(const Utils::Vector& p_pos, const Utils::Vector& p_color, dsreal p_scale, QuadAxis p_axis, int p_angle_step, int& p_nb_vertex)
{
    Vector vo1;
    Vector vo2;
    Vector vo3;
    Vector vo4;

    dsreal angle = 0.0;

    int nb_loop = 360 / p_angle_step;

    for (int i = 0; i < nb_loop; i++)
    {
        Vertex v1, v2, v3, v4;

        switch( p_axis )
        {
            case X_AXIS:
            {
                Vector vi1(-0.5, 0.5, 0.0, 1.0);
                Vector vi2(0.5, 0.5, 0.0, 1.0);
                Vector vi3(0.5, -0.5, 0.0, 1.0);
                Vector vi4(-0.5, -0.5, 0.0, 1.0);

                Matrix mrot;
                mrot.Rotation( Vector( 1.0, 0.0, 0.0, 1.0 ), Utils::Maths::DegToRad( angle ) );

                angle += p_angle_step;

                mrot.Transform( &vi1, &vo1 );
                mrot.Transform( &vi2, &vo2 );
                mrot.Transform( &vi3, &vo3 );
                mrot.Transform( &vi4, &vo4 );
            }
            break;

            case Y_AXIS:
            {
                Vector vi1(-0.5, 0.5, 0.0, 1.0);
                Vector vi2(0.5, 0.5, 0.0, 1.0);
                Vector vi3(0.5, -0.5, 0.0, 1.0);
                Vector vi4(-0.5, -0.5, 0.0, 1.0);

                Matrix mrot;
                mrot.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), Utils::Maths::DegToRad( angle ) );

                angle += p_angle_step;

                mrot.Transform( &vi1, &vo1 );
                mrot.Transform( &vi2, &vo2 );
                mrot.Transform( &vi3, &vo3 );
                mrot.Transform( &vi4, &vo4 );
            }

            break;

            case Z_AXIS:
            {
                Vector vi1(-0.5, 0.0, 0.5, 1.0);
                Vector vi2(0.5, 0.0, 0.5, 1.0);
                Vector vi3(0.5, 0.0, -0.5, 1.0);
                Vector vi4(-0.5, 0.0, -0.5, 1.0);

                Matrix mrot;
                mrot.Rotation(Vector(0.0, 0.0, 1.0, 1.0), Utils::Maths::DegToRad(angle));

                angle += p_angle_step;

                mrot.Transform(&vi1, &vo1);
                mrot.Transform(&vi2, &vo2);
                mrot.Transform(&vi3, &vo3);
                mrot.Transform(&vi4, &vo4);
            }
            break;
        }

        vo1.Scale(p_scale);
        vo2.Scale(p_scale);
        vo3.Scale(p_scale);
        vo4.Scale(p_scale);

        vo1[0] += p_pos[0];
        vo1[1] += p_pos[1];
        vo1[2] += p_pos[2];

        vo2[0] += p_pos[0];
        vo2[1] += p_pos[1];
        vo2[2] += p_pos[2];

        vo3[0] += p_pos[0];
        vo3[1] += p_pos[1];
        vo3[2] += p_pos[2];

        vo4[0] += p_pos[0];
        vo4[1] += p_pos[1];
        vo4[2] += p_pos[2];


        v1.x = vo1[0];
        v1.y = vo1[1];
        v1.z = vo1[2];

        v2.x = vo2[0];
        v2.y = vo2[1];
        v2.z = vo2[2];

        v3.x = vo3[0];
        v3.y = vo3[1];
        v3.z = vo3[2];

        v4.x = vo4[0];
        v4.y = vo4[1];
        v4.z = vo4[2];

        dsreal tu1, tv1;
        dsreal tu2, tv2;

        generate_uvcoords( tu1, tv1, tu2, tv2 );

        v1.tu[0] = tu1;
        v1.tv[0] = tv1;

        v2.tu[0] = tu2;
        v2.tv[0] = tv1;

        v3.tu[0] = tu2;
        v3.tv[0] = tv2;

        v4.tu[0] = tu1;
        v4.tv[0] = tv2;

        v1.tu[1] = 0.0;
        v1.tv[1] = 0.0;

        v2.tu[1] = 1.0;
        v2.tv[1] = 0.0;

        v3.tu[1] = 1.0;
        v3.tv[1] = 1.0;

        v4.tu[1] = 0.0;
        v4.tv[1] = 1.0;



        /////////////////////

        


        v1.tu[2] = p_color[0];
        v1.tv[2] = p_color[1];
        v1.tw[2] = p_color[2];
        v1.ta[2] = p_color[3];

        v2.tu[2] = p_color[0];
        v2.tv[2] = p_color[1];
        v2.tw[2] = p_color[2];
        v2.ta[2] = p_color[3];

        v3.tu[2] = p_color[0];
        v3.tv[2] = p_color[1];
        v3.tw[2] = p_color[2];
        v3.ta[2] = p_color[3];

        v4.tu[2] = p_color[0];
        v4.tv[2] = p_color[1];
        v4.tw[2] = p_color[2];
        v4.ta[2] = p_color[3];




        /////////////////////


        m_meshe->AddVertex(v1);
        m_meshe->AddVertex(v2);
        m_meshe->AddVertex(v3);
        m_meshe->AddVertex(v4);

        int index = 4 * i;

        index += p_nb_vertex;

        m_meshe->AddTriangle(Triangle(index, index + 3, index + 1));
        m_meshe->AddTriangle(Triangle(index + 1, index + 3, index + 2));
    }

    m_meshe->ComputeNormales();

    p_nb_vertex += 4 * nb_loop;
}


void NebulaeRenderingAspectImpl::PassSlot::on_renderingnode_draw( RenderingNode* p_rendering_node )
{
    m_renderer->DrawMeshe( m_world, m_view, m_proj );
}

void NebulaeRenderingAspectImpl::PassSlot::generate_uvcoords(dsreal& p_u1, dsreal& p_v1, dsreal& p_u2, dsreal& p_v2)
{
    static int u;
    static int v;

    dsreal step = 1.0 / atlasResolution;

    dsreal u1, u2;
    dsreal v1, v2;

    u1 = u * step;
    u2 = u1 + step;
    
    v1 = v * step;
    v2 = v1 + step;

    p_u1 = u1;
    p_u2 = u2;

    p_v1 = v1;
    p_v2 = v2;

    u++;
    if( 8 == u )
    {
        u = 0;
        v++;
        if( 8 == v )
        {
            v = 0;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

NebulaeRenderingAspectImpl::NebulaeRenderingAspectImpl( void )
{
}

bool NebulaeRenderingAspectImpl::VisitRenderPassDescr( const dsstring& p_name, DrawSpace::Core::RenderingQueue* p_passqueue )
{
    bool updated_queue = false;

    for( size_t i = 0; i < m_pass_slots.size(); i++ )
    {
        if( m_pass_slots[i]->m_pass_name == p_name )
        {
            if( m_add_in_rendergraph )
            {
                // ajout du renderingnode dans la renderingqueue  
                p_passqueue->Add( m_pass_slots[i]->m_rendering_node );
            }
            else
            {
                // suppression du renderingnode de la renderingqueue
                p_passqueue->Remove( m_pass_slots[i]->m_rendering_node );
            }
            updated_queue = true;
        }
    }
    return updated_queue;
}

void NebulaeRenderingAspectImpl::RegisterToRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph )
{
    init_rendering_objects();

    m_add_in_rendergraph = true;
    p_rendergraph.Accept( this );
}

void NebulaeRenderingAspectImpl::UnregisterFromRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph )
{
    release_rendering_objects();

    m_add_in_rendergraph = false;
    p_rendergraph.Accept( this );
}

void NebulaeRenderingAspectImpl::Run( DrawSpace::Core::Entity* p_entity )
{
    TransformAspect* transform_aspect = p_entity->GetAspect<TransformAspect>();

    if( transform_aspect )
    {
        Matrix world;        
        transform_aspect->GetWorldTransform( world );

        Matrix view;
        transform_aspect->GetViewTransform( view );

        Matrix proj;
        transform_aspect->GetProjTransform( proj ); 

        for( size_t i = 0; i < m_pass_slots.size(); i++ )
        {
            m_pass_slots[i]->m_world = world;
            m_pass_slots[i]->m_view = view;
            m_pass_slots[i]->m_proj = proj;
        }
    }

    update_shader_params();

    ////////////////////////////////////////////////////////
}

void NebulaeRenderingAspectImpl::init_rendering_objects( void )
{
    ComponentList<std::vector<dsstring>> passes;
    m_owner->GetComponentsByType<std::vector<dsstring>>( passes );

    ComponentList<std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>> textures;
    m_owner->GetComponentsByType<std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>(textures);

    ComponentList<Fx*> fxs;
    m_owner->GetComponentsByType<Fx*>( fxs );

    ComponentList<int> ro;
    m_owner->GetComponentsByType<int>( ro );

    //// retrieve specific config....

    ComponentList<DataModel> datamodels;
    m_owner->GetComponentsByType<DataModel>(datamodels);

    m_data_model = datamodels[0]->getPurpose();

    std::vector<dsstring> passes_names = passes[0]->getPurpose();
    
    for( size_t i = 0; i < passes_names.size(); i++ )
    {        
        dsstring pass_name;
        pass_name = passes_names[i];
       
        PassSlot* pass_slot = _DRAWSPACE_NEW_( PassSlot, PassSlot( pass_name ) );

        pass_slot->GetRenderingNode()->SetOrderNumber( ro[i]->getPurpose() );
        pass_slot->GetRenderingNode()->SetFx( fxs[i]->getPurpose() );

        if(textures[0]->getPurpose().size() )
        {
            std::array<Texture*,RenderingNode::NbMaxTextures> textures_set = textures[0]->getPurpose()[0];
            for( size_t k = 0; k < RenderingNode::NbMaxTextures; k++ )
            {
                pass_slot->GetRenderingNode()->SetTexture( textures_set[k], k );
            }
        }        
        m_pass_slots.push_back( pass_slot );
    }
    
    update_shader_params();   
}

void NebulaeRenderingAspectImpl::release_rendering_objects( void )
{
    for( size_t i = 0; i < m_pass_slots.size(); i++ )
    {
        _DRAWSPACE_DELETE_( m_pass_slots[i] );
    }
    m_pass_slots.clear();
}

void NebulaeRenderingAspectImpl::update_shader_params( void ) // for all passes
{
    ////////////////////////////////////////////////////////
    //recup des params shaders

    ComponentList<std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>> neb_shaders_params;

    m_owner->GetComponentsByType<std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>(neb_shaders_params);
    
    for( size_t i = 0; i < m_pass_slots.size(); i++ )
    {       
        // pour chaque passe
        PassSlot* curr_pass = m_pass_slots[i];

        std::vector<std::pair<dsstring, RenderingNode::ShadersParams>> shaders_params = neb_shaders_params[i]->getPurpose();

        for( size_t k = 0; k < shaders_params.size(); k++ )
        {
            std::pair<dsstring, RenderingNode::ShadersParams> shader_params_pair = shaders_params[k];
            curr_pass->GetRenderingNode()->UpdateShaderParams( shader_params_pair.first, shader_params_pair.second );
        }
    }   
}

