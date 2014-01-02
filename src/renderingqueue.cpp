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

#include "renderingqueue.h"
#include "renderer.h"
#include "plugin.h"
#include <algorithm>

using namespace DrawSpace::Core;
using namespace DrawSpace::Interface;

RenderingQueue::RenderingQueue( void ) : 
m_target( NULL ),
m_clear_depth( false ),
m_clear_target( false ),
m_target_clear_color_r( 0 ),
m_target_clear_color_g( 0 ),
m_target_clear_color_b( 0 ),
m_switches_cost( 0 )
{


}

RenderingQueue::RenderingQueue( Texture* p_target ) : 
m_target( p_target ),
m_clear_depth( false ),
m_clear_target( false ),
m_target_clear_color_r( 0 ),
m_target_clear_color_g( 0 ),
m_target_clear_color_b( 0 ),
m_switches_cost( 0 )
{


}

RenderingQueue::~RenderingQueue( void )
{

}

bool RenderingQueue::nodes_comp( RenderingNode* p_n1, RenderingNode* p_n2 )
{
    return ( p_n1->GetOrderNumber() < p_n2->GetOrderNumber() );
}

void RenderingQueue::Draw( void )
{
    m_switches_cost = 0;

    Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;

    if( m_target )
    {
        renderer->BeginTarget( m_target );
    }
    else
    {
        renderer->BeginScreen();
    }

    if( m_clear_depth )
    {
        renderer->ClearDepth();
    }
    if( m_clear_target )
    {
        renderer->ClearScreen( m_target_clear_color_r, m_target_clear_color_g, m_target_clear_color_b );
    }

    for( size_t i = 0; i < m_outputqueue.size(); i++ )
    {
        Operation curr_operation = m_outputqueue[i];

        switch( curr_operation.type )
        {
            case SET_TEXTURE:

                renderer->SetTexture( curr_operation.data, curr_operation.texture_stage );
                m_switches_cost++;
                break;

            case UNSET_TEXTURE:

                renderer->UnsetTexture( curr_operation.texture_stage );
                m_switches_cost++;
                break;

            case SET_FX:

                renderer->SetFx( curr_operation.data );
                m_switches_cost++;
                break;

            case UNSET_FX:

                renderer->UnsetFx( curr_operation.data );
                m_switches_cost++;
                break;

            case SET_MESHE:

                renderer->SetMeshe( curr_operation.data );
                m_switches_cost++;
                break;

            case SET_SHADERS_PARAMS:

                renderer->SetFxShaderParams( curr_operation.shader_index, curr_operation.param_register, curr_operation.param_values );
                break;

            case DRAW_NODE:

                curr_operation.node->OnDraw();
                break;
        }
    }

    if( m_target )
    {
        renderer->EndTarget( m_target );
    }
    else
    {
        renderer->EndScreen();
    }
}

void RenderingQueue::Add( RenderingNode* p_node )
{
    m_nodes.push_back( p_node );

    // a chaque ajout, refaire un sort
    std::sort( m_nodes.begin(), m_nodes.end(), RenderingQueue::nodes_comp );
}

void RenderingQueue::EnableDepthClearing( bool p_enable )
{
    m_clear_depth = p_enable;
}

void RenderingQueue::EnableTargetClearing( bool p_enable )
{
    m_clear_target = p_enable;
}

void RenderingQueue::SetTargetClearingColor( unsigned char p_r, unsigned char p_g, unsigned char p_b )
{
    m_target_clear_color_r = p_r;
    m_target_clear_color_g = p_g;
    m_target_clear_color_b = p_b;
}

void RenderingQueue::UpdateOutputQueue( void )
{
    // I/ classement au niveau des listes RenderingNode*
    

    build_output_list( m_nodes );
    cleanup_output_list();
}

bool RenderingQueue::build_output_list( std::vector<RenderingNode*>& p_input_list )
{
    Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;

    void* fx_data;
    void* tx_data;
    void* meshe_data;

    m_tx_datas.clear();
    m_fx_datas.clear();
    m_meshe_datas.clear();

    for( size_t i = 0; i < p_input_list.size(); i++ )
    {
        RenderingNode* node = p_input_list[i];

        Fx* current_fx = node->GetFx();

        if( false == renderer->CreateFx( current_fx, &fx_data ) )
        {
            return false;
        }
        m_fx_datas[node] = fx_data;

        for( long j = 0; j < node->GetTextureListSize(); j++ )
        {
            Texture* current_tx = node->GetTexture( j );

            if( NULL != current_tx )
            {
                if( false == renderer->CreateTexture( current_tx, &tx_data ) )
                {
                    return false;
                }
                m_tx_datas[node].push_back( tx_data );
            }
        }

        Meshe* current_meshe = node->GetMeshe();

        if( NULL != current_meshe )
        {
            if( false == renderer->CreateMeshe( current_meshe, &meshe_data ) )
            {
                return false;
            }
            m_meshe_datas[node] = meshe_data;
        }
    }

    m_outputqueue.clear();

    for( size_t i = 0; i < p_input_list.size(); i++ )
    {
        Operation operation;
        RenderingNode* node = p_input_list[i];

        if( m_fx_datas.count( node ) )
        {
            operation.type = SET_FX;
            operation.data = m_fx_datas[node];
            m_outputqueue.push_back( operation );
        }

        if( m_tx_datas.count( node ) )
        {
            for( size_t j = 0; j < m_tx_datas[node].size(); j++ )
            {
               operation.type = SET_TEXTURE;
               operation.data = m_tx_datas[node][j];
               operation.texture_stage = (long)j;
               m_outputqueue.push_back( operation );
            }
        }

        if( m_meshe_datas.count( node ) )
        {
            operation.type = SET_MESHE;
            operation.data = m_meshe_datas[node];
            m_outputqueue.push_back( operation );
        }

        std::map<dsstring, RenderingNode::ShadersParams> node_shaders_params;
        node->GetShadersParams( node_shaders_params );

        for( std::map<dsstring, RenderingNode::ShadersParams>::iterator it = node_shaders_params.begin(); it != node_shaders_params.end(); ++it )
        {
            operation.type = SET_SHADERS_PARAMS;

            operation.shader_index = (*it).second.shader_index;
            operation.param_register = (*it).second.param_register;
            operation.param_values = (*it).second.param_values;
                            
            m_outputqueue.push_back( operation );                
        }

        operation.type = DRAW_NODE;
        operation.node = node;
        m_outputqueue.push_back( operation );

        if( m_tx_datas.count( node ) )
        {
            for( size_t j = 0; j < m_tx_datas[node].size(); j++ )
            {
               operation.type = UNSET_TEXTURE;
               operation.texture_stage = (long)j;
               m_outputqueue.push_back( operation );
            }
        }

        if( m_fx_datas.count( node ) )
        {
            operation.type = UNSET_FX;
            operation.data = m_fx_datas[node];
            m_outputqueue.push_back( operation );
        }
    }
    return true;
}

void RenderingQueue::cleanup_output_list( void )
{
    
}

long RenderingQueue::GetSwitchesCost( void )
{
    return m_switches_cost;
}

long RenderingQueue::GetTheoricalSwitchesCost( void )
{
    long cost = 0;

    for( size_t i = 0; i < m_nodes.size(); i++ )
    {
        RenderingNode* node = m_nodes[i];

        if( node->GetFx() )
        {
            cost += 2; // SET_FX, UNSET_FX;
        }

        for( long j = 0; j < node->GetTextureListSize(); j++ )
        {
            Texture* current_tx = node->GetTexture( j );
            if( NULL != current_tx )
            {
                cost += 2; // SET_TEX, UNSET_TEX;
            }
        }
       
        if( node->GetMeshe() )
        {
            cost++; //SET_MESHE
        }
    }
    return cost;
}