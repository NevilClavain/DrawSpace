/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

#include "renderingqueue.h"
#include "renderer.h"
#include "plugin.h"
#include "exceptions.h"
#include <algorithm>

using namespace DrawSpace::Core;
using namespace DrawSpace::Interface;

DrawSpace::Logger::Sink rd_logger("RenderingQueue", DrawSpace::Logger::Configuration::GetInstance());

RenderingQueue::RenderingQueue(const dsstring& p_id) :
m_target( NULL ),
m_clear_depth( false ),
m_clear_target( false ),
m_target_clear_color_r( 0 ),
m_target_clear_color_g( 0 ),
m_target_clear_color_b( 0 ),
m_target_clear_color_a( 0 ),
m_switches_cost( 0 ),
m_ready( true ),
m_status( OK ),
m_target_slice(0),
m_id( p_id),
m_current_outputqueue(&m_outputqueue_0),
m_back_outputqueue(&m_outputqueue_1)
{


}

RenderingQueue::RenderingQueue(const dsstring& p_id, Texture* p_target ) :
m_target( p_target ),
m_clear_depth( false ),
m_clear_target( false ),
m_target_clear_color_r( 0 ),
m_target_clear_color_g( 0 ),
m_target_clear_color_b( 0 ),
m_target_clear_color_a( 0 ),
m_switches_cost( 0 ),
m_ready( true ),
m_status( OK ),
m_target_slice(0),
m_id(p_id),
m_current_outputqueue(&m_outputqueue_0),
m_back_outputqueue(&m_outputqueue_1)
{


}

void RenderingQueue::FlipOutputQueues(void)
{
    if (m_current_outputqueue == &m_outputqueue_0)
    {
        m_current_outputqueue = &m_outputqueue_1;
        m_back_outputqueue = &m_outputqueue_0;
    }
    else
    {
        m_current_outputqueue = &m_outputqueue_0;
        m_back_outputqueue = &m_outputqueue_1;
    }
}

RenderingQueue::~RenderingQueue( void )
{
}

void RenderingQueue::SetTargetSlice(int p_target_slice)
{
    m_target_slice = p_target_slice;
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
        if (!m_ready)
        {
            m_status = ERROR_NOTREADY;
            return;
        }

        void* texture_render_data = m_target->GetRenderData();
        if( NULL == texture_render_data )
        {
            //_DSEXCEPTION( "target texture not initialised in renderer" )
            m_status = ERROR_NOTARGET;
            return;

        }
        renderer->BeginTarget( texture_render_data, m_target_slice);
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
        renderer->ClearScreen( m_target_clear_color_r, m_target_clear_color_g, m_target_clear_color_b, m_target_clear_color_a );
    }

    if (!m_ready)
    {
        m_status = ERROR_NOTREADY;
        return;
    }
    else
    {
        for( auto& curr_operation : *m_current_outputqueue)
        {
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


                case SET_VERTEXTEXTURE:

                    renderer->SetVertexTexture( curr_operation.data, curr_operation.texture_stage );
                    m_switches_cost++;
                    break;

                case UNSET_VERTEXTEXTURE:

                    renderer->UnsetVertexTexture( curr_operation.texture_stage );
                    m_switches_cost++;
                    break;

                case SET_SHADERS:

                    renderer->SetShaders( curr_operation.data );
                    m_switches_cost++;
                    break;

                case SET_RENDERSTATES_IN:

                    renderer->ApplyRenderStatesIn( curr_operation.data );
                    m_switches_cost++;
                    break;

                case SET_RENDERSTATES_OUT:

                    renderer->ApplyRenderStatesOut( curr_operation.data );
                    m_switches_cost++;
                    break;

                case SET_MESHE:

                    renderer->SetMeshe( curr_operation.data );
                    m_switches_cost++;
                    break;

                case SET_SHADERS_PARAMS:
                    {
                        if( curr_operation.shader_params->vector )
                        {
                            renderer->SetFxShaderParams( curr_operation.shader_params->shader_index, curr_operation.shader_params->param_register, curr_operation.shader_params->param_values );
                        }
                        else
                        {
                            renderer->SetFxShaderMatrix( curr_operation.shader_params->shader_index, curr_operation.shader_params->param_register, curr_operation.shader_params->mat );
                        }
                    }
                    break;

			    case SET_SHADERS_ARRAY_PARAMS:
				    {					
					    renderer->SetShaderVectorBuffer(curr_operation.shader_array_param->shader_index, curr_operation.shader_array_param->begin_register, curr_operation.shader_array_param->array);
				    }
				    break;

                case DRAW_NODE:

                    if( curr_operation.node->m_drawing_enabled )
                    {
                        curr_operation.node->OnDraw();
                    }
                    break;
            }
        }
    }

    if( m_target )
    {
        renderer->EndTarget( m_target->GetRenderData() );
    }
    else
    {
        renderer->EndScreen();
    }

	m_status = OK;
}

void RenderingQueue::Add( RenderingNode* p_node )
{
    m_renderingorder_nodes[p_node->GetOrderNumber()].push_back( p_node );

    // a new node was added; we cannot consider we're 'ready' until a new call to UpdateOutputQueue() or UpdateOutputQueueNoOpt() has been made (new node probably need resources creation in renderer plugin...)
    m_ready = false;
}

void RenderingQueue::Remove( RenderingNode* p_node )
{
    long order_num = p_node->GetOrderNumber();
    std::vector<RenderingNode*>& list = m_renderingorder_nodes[order_num];

    for( auto it = list.begin(); it != list.end(); ++it )
    {
        if( *it == p_node )
        {
            list.erase( it );
            break;
        }
    }
}

bool RenderingQueue::IsReady(void) const
{
    return m_ready;
}

void RenderingQueue::EnableDepthClearing( bool p_enable )
{
    m_clear_depth = p_enable;
}

void RenderingQueue::EnableTargetClearing( bool p_enable )
{
    m_clear_target = p_enable;
}

void RenderingQueue::SetTargetClearingColor( unsigned char p_r, unsigned char p_g, unsigned char p_b, unsigned char p_a )
{
    m_target_clear_color_r = p_r;
    m_target_clear_color_g = p_g;
    m_target_clear_color_b = p_b;
    m_target_clear_color_a = p_a;
}

void RenderingQueue::UpdateOutputQueue( void )
{
    _DSDEBUG(rd_logger, dsstring("Begin : " << m_id));

    m_nodes.clear();
    m_fx_bases.clear();

    for( auto it = m_renderingorder_nodes.begin(); it != m_renderingorder_nodes.end(); ++it )
    {
        std::vector<RenderingNode*> sorted_list;
        sort_list( (*it).second, sorted_list );

        for( size_t i = 0; i < sorted_list.size(); i++ )
        {
            m_nodes.push_back( sorted_list[i] );

            // a chaque ajout, refaire un sort
            std::sort( m_nodes.begin(), m_nodes.end(), RenderingQueue::nodes_comp );            
        }
    }
    
    build_output_list( m_nodes );

    m_setmeshe_groups.clear();
    m_setshaders_groups.clear();
    m_setrsin_groups.clear();
    m_setrsout_groups.clear();

    for( int i = 0; i < RenderingNode::NbMaxTextures; i++ )
    {
        m_settexture_groups[i].clear();
        m_unsettexture_groups[i].clear();

        m_setvtexture_groups[i].clear();
        m_unsetvtexture_groups[i].clear();
    }

    cleanup_output_list();

    m_ready = true;

    _DSDEBUG(rd_logger, dsstring("End : " << m_id));
}

void RenderingQueue::UpdateOutputQueueNoOpt( void )
{
    _DSDEBUG(rd_logger, dsstring("Begin : " << m_id));

    m_nodes.clear();
    m_fx_bases.clear();

    for( auto it = m_renderingorder_nodes.begin(); it != m_renderingorder_nodes.end(); ++it )
    {
        for( size_t i = 0; i < (*it).second.size(); i++ )
        {
            m_nodes.push_back( (*it).second[i] );

            // a chaque ajout, refaire un sort
            std::sort( m_nodes.begin(), m_nodes.end(), RenderingQueue::nodes_comp );            
        }
    }
    build_output_list( m_nodes );

    m_setmeshe_groups.clear();
    m_setshaders_groups.clear();
    m_setrsin_groups.clear();
    m_setrsout_groups.clear();

    for( int i = 0; i < RenderingNode::NbMaxTextures; i++ )
    {
        m_settexture_groups[i].clear();
        m_unsettexture_groups[i].clear();

        m_setvtexture_groups[i].clear();
        m_unsetvtexture_groups[i].clear();
    }

    cleanup_output_list();
    m_ready = true;

    _DSDEBUG(rd_logger, dsstring("End : " << m_id));
}

double RenderingQueue::lists_score( std::map<dsstring, std::vector<RenderingNode*>>& p_lists )
{
    long count = 0;
    long sum = 0;

    if( 0 == p_lists.size() )
    {
        return 0.0;
    }

    for( auto it = p_lists.begin(); it != p_lists.end(); ++it )
    {
        if( (*it).second.size() > 1 )
        {
            count++;
            sum += (long)( (*it).second.size() );
        }
    }
    
    if( count > 0 )
    {
        double alpha = (double)count / (double)p_lists.size();

        // moyenne
        double avg = (double)sum / (double)count;

        //compter nbre de nodes
        long count2 = 0;
        for( auto it = p_lists.begin(); it != p_lists.end(); ++it )
        {
            count2 += (long)( (*it).second.size() );
        }

        // normaliser la moyenne
        double beta = avg / count2;

        return alpha * beta;
    }
    else
    {
        return 0.0;
    }
}

void RenderingQueue::sort_list( std::vector<RenderingNode*>& p_input_list, std::vector<RenderingNode*>& p_output_list )
{
    p_output_list.clear();

    std::vector<SortCategory> todo;
    SortCategory category;

    /*
    category.type = FX_LIST;
    todo.push_back( category );
    */

    category.type = SHADERS_LIST;
    todo.push_back( category );

    category.type = RS_LIST;
    todo.push_back( category );

    category.type = MESHE_LIST;
    todo.push_back( category );

    category.type = TEXTURE_LIST;
    for( category.stage = 0; category.stage < RenderingNode::NbMaxTextures; category.stage++ )
    {
        todo.push_back( category );
    }

    sort_step( todo, p_input_list, p_output_list );
}


void RenderingQueue::sort_step( std::vector<SortCategory>& p_todo, std::vector<RenderingNode*>& p_input_list, std::vector<RenderingNode*>& p_output_list )
{
    //std::map<dsstring, std::vector<RenderingNode*>>     fx_lists;

    std::map<dsstring, std::vector<RenderingNode*>>     shaders_lists;
    std::map<dsstring, std::vector<RenderingNode*>>     rs_lists;
    std::map<dsstring, std::vector<RenderingNode*>>     meshe_lists;
    std::map<dsstring, std::vector<RenderingNode*>>     texture_lists;
    
    SortedListType                                      sel_type;
    long                                                sel_stage;
    std::map<dsstring, std::vector<RenderingNode*>>     selected_lists;
    bool                                                sel_flag = false;

    double max_score = 0.0;

    for( size_t i = 0; i < p_todo.size(); i++ )
    {
        switch( p_todo[i].type )
        {
            /*
            case FX_LIST:
                {
                    //long local_occ = 0;
                    sort_list_by( FX_LIST, 0, p_input_list, fx_lists );

                    double local_score = lists_score( fx_lists );
                    if( local_score > max_score )
                    {
                        max_score = local_score;
                        selected_lists = fx_lists;
                        sel_type = FX_LIST;
                        sel_flag = true;
                    }
                }
                break;
                */

            case SHADERS_LIST:
                {
                    //long local_occ = 0;
                    sort_list_by( SHADERS_LIST, 0, p_input_list, shaders_lists );

                    double local_score = lists_score( shaders_lists );
                    if( local_score > max_score )
                    {
                        max_score = local_score;
                        selected_lists = shaders_lists;
                        sel_type = SHADERS_LIST;
                        sel_flag = true;
                    }
                }
                break;

            case RS_LIST:
                {
                    //long local_occ = 0;
                    sort_list_by( RS_LIST, 0, p_input_list, rs_lists );

                    double local_score = lists_score( rs_lists );
                    if( local_score > max_score )
                    {
                        max_score = local_score;
                        selected_lists = rs_lists;
                        sel_type = RS_LIST;
                        sel_flag = true;
                    }
                }
                break;


            case MESHE_LIST:
                {
                    //long local_occ = 0;
                    sort_list_by( MESHE_LIST, 0, p_input_list, meshe_lists );

                    double local_score = lists_score( meshe_lists );
                    if( local_score > max_score )
                    {
                        max_score = local_score;
                        selected_lists = meshe_lists;
                        sel_type = MESHE_LIST;
                        sel_flag = true;
                    }
                }
                break;

            case TEXTURE_LIST:
                {
                    //long local_occ = 0;
                    sort_list_by( TEXTURE_LIST, p_todo[i].stage, p_input_list, texture_lists );

                    double local_score = lists_score( texture_lists );
                    if( local_score > max_score )
                    {
                        max_score = local_score;
                        selected_lists = texture_lists;
                        sel_type = TEXTURE_LIST;
                        sel_stage = p_todo[i].stage;
                        sel_flag = true;
                    }
                }
                break;
        }
    }

    if( !sel_flag )
    {
        // chaque node n'a rien en commun avec tout les autres
        p_output_list = p_input_list;
        return;
    }

    // type selectionne : retirer de p_todo
    for( auto it = p_todo.begin(); it != p_todo.end(); ++it )
    {
        if( TEXTURE_LIST == sel_type )
        {
            if( (*it).type == sel_type && (*it).stage == sel_stage )
            {
                p_todo.erase( it );
                break;
            }
        }
        else
        {
            if( (*it).type == sel_type )
            {
                p_todo.erase( it );
                break;
            }
        }
    }

    // appels recursifs
    for( auto it = selected_lists.begin(); it != selected_lists.end(); ++it )
    {
        if( (*it).second.size() > 2 )
        {
            if( p_todo.size() > 0 )
            {
                std::vector<RenderingNode*> output_list;              
                sort_step( p_todo, (*it).second, output_list );

                for( size_t i = 0; i < output_list.size(); i++ )
                {
                    p_output_list.push_back( output_list[i] ); 
                }
            }
            else
            {
                for( size_t i = 0; i < (*it).second.size(); i++ )
                {
                    p_output_list.push_back( (*it).second[i] ); 
                }
            }
        }
        else
        {
            for( size_t i = 0; i < (*it).second.size(); i++ )
            {
                p_output_list.push_back( (*it).second[i] );
            }
        }
    }
}

void RenderingQueue::sort_list_by( SortedListType p_type, long p_texturestage, std::vector<RenderingNode*>& p_in_list, std::map<dsstring, std::vector<RenderingNode*>>& p_out_lists )
{
    std::map<dsstring, std::vector<RenderingNode*> > out_lists;
    char buff[64];

    for( size_t i = 0; i < p_in_list.size(); i++ )
    {
        RenderingNode* curr_node = p_in_list[i];

        switch( p_type )
        {

            case SHADERS_LIST:
                {
                    Fx* curr_fx = curr_node->GetFx();

                    if( curr_fx )
                    {
                        dsstring curr_md5;
                        curr_fx->GetShadersMD5( curr_md5 );
                        out_lists[curr_md5].push_back( curr_node );
                    }
                    else
                    {
                        // pas de fx : ne doit pas influer sur les comptages de redondances
                        // s'assurer que l'id est unique, empechant ainsi la mise en place de fausses redondances
                        sprintf( buff, "%x", (long long)curr_node );
                        dsstring id = dsstring( "void_fx:" ) + dsstring( buff );
                        out_lists[id].push_back( curr_node );
                    }
                }
                break;

            case RS_LIST:
                {
                    Fx* curr_fx = curr_node->GetFx();

                    if( curr_fx )
                    {
                        dsstring curr_md5;
                        curr_fx->GetRenderStatesSetMD5( curr_md5 );
                        out_lists[curr_md5].push_back( curr_node );
                    }
                    else
                    {
                        // pas de fx : ne doit pas influer sur les comptages de redondances
                        // s'assurer que l'id est unique, empechant ainsi la mise en place de fausses redondances
                        sprintf( buff, "%x", (long long)curr_node );
                        dsstring id = dsstring( "void_fx:" ) + dsstring( buff );
                        out_lists[id].push_back( curr_node );
                    }
                }
                break;

            case MESHE_LIST:
                {
                    Meshe* curr_meshe = curr_node->GetMeshe();

                    if( curr_meshe )
                    {
                        dsstring curr_meshe_md5;
                        curr_meshe->GetMD5( curr_meshe_md5 );
                        out_lists[curr_meshe_md5].push_back( curr_node );
                    }
                    else
                    {
                        // pas de meshe : ne doit pas influer sur les comptages de redondances
                        // s'assurer que l'id est unique, empechant ainsi la mise en place de fausses redondances
                        sprintf( buff, "%x", (long long)curr_node );
                        dsstring id = dsstring( "void_meshe:" ) + dsstring( buff );
                        out_lists[id].push_back( curr_node );
                    }
                }
                break;

            case TEXTURE_LIST:
                {
                    Texture* curr_tx = curr_node->GetTexture( p_texturestage );
                    if( curr_tx )
                    {
                        dsstring curr_txt_md5;
                        curr_tx->GetMD5( curr_txt_md5 );
                        dsstring stage( itoa( p_texturestage, buff, 10 ) );
                        dsstring signature = curr_txt_md5 + ":" + stage;

                    }
                    else
                    {
                        // pas de texture pour ce stage : ne doit pas influer sur les comptages de redondances
                        // s'assurer que l'id est unique, empechant ainsi la mise en place de fausses redondances                       
                        sprintf( buff, "%d:%x", p_texturestage, (long long)curr_node );
                        dsstring id = dsstring( "void_texture:" ) + dsstring( buff );
                        out_lists[id].push_back( curr_node );
                    }
                }
                break;
        }
    }

    p_out_lists = out_lists;
}

void RenderingQueue::build_output_list( std::vector<RenderingNode*>& p_input_list )
{
    Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;

    void* sh_data;
    void* rs_data;
    void* tx_data;
    void* meshe_data;

    m_tx_datas.clear();
    m_vtx_datas.clear();
    m_sh_datas.clear();
    m_rs_datas.clear();

    m_meshe_datas.clear();

    for( size_t i = 0; i < p_input_list.size(); i++ )
    {
        RenderingNode* node = p_input_list[i];

        Fx* current_fx = node->GetFx();
        if( !current_fx )
        {
            _DSEXCEPTION( "Missing fx for rendering node " );
        }

        if( current_fx->GetShadersListSize() )
        {
            if( false == renderer->CreateShaders( current_fx, &sh_data ) )
            {
                _DSEXCEPTION( "Cannot create Shaders" )
            }

            dsstring shaders_infos;

            for( long i = 0; i < current_fx->GetShadersListSize(); i++ )
            {
                 dsstring curr_shader_path;
                 current_fx->GetShader( i )->GetPath( curr_shader_path );

                 shaders_infos += curr_shader_path;
                 shaders_infos += "    ";
            }

            m_sh_datas[node].first = sh_data;
            m_sh_datas[node].second = shaders_infos;
        }

        /////////////////////////////////////////////
        dsstring hash;
        current_fx->GetRenderStatesSetMD5( hash );

        if( m_fx_bases.count( hash ) > 0 )
        {
            rs_data = (void *)m_fx_bases[hash];
        }
        else
        {
            rs_data = (void*)current_fx;
            m_fx_bases[hash] = current_fx;
        }

        m_rs_datas[node] = rs_data;
        /////////////////////////////////////////////

        long texture_list_size{ node->GetTextureListSize() };
        for( long j = 0; j < texture_list_size; j++ )
        {
            Texture* current_tx = node->GetTexture( j );

            if( NULL != current_tx )
            {
                if( false == renderer->CreateTexture( current_tx, &tx_data ) )
                {
                    dsstring path;
                    current_tx->GetPath( path );

                    dsstring excp_msg = "Cannot create Texture ";
                    excp_msg += path;
                    _DSEXCEPTION( excp_msg  )
                }
                //m_tx_datas[node].push_back( tx_data );
                dsstring texture_id;
                current_tx->GetPath( texture_id );
                std::pair<void*, dsstring> txt_infos( tx_data, texture_id );
                m_tx_datas[node].push_back( txt_infos );
            }
            else
            {
                std::pair<void*, dsstring> txt_infos( NULL, "" );
                m_tx_datas[node].push_back( txt_infos );
            }


            /// vertex textures

            current_tx = node->GetVertexTexture( j );

            if( NULL != current_tx )
            {
                if( false == renderer->CreateTexture( current_tx, &tx_data ) )
                {
                    dsstring path;
                    current_tx->GetPath( path );

                    dsstring excp_msg = "Cannot create Vertex Texture ";
                    excp_msg += path;
                    _DSEXCEPTION( excp_msg  )
                }
                //m_vtx_datas[node].push_back( tx_data );
                dsstring texture_id;
                current_tx->GetPath( texture_id );
                std::pair<void*, dsstring> txt_infos( tx_data, texture_id );
                m_vtx_datas[node].push_back( txt_infos );
            }
            else
            {
                //m_vtx_datas[node].push_back( NULL );
                std::pair<void*, dsstring> txt_infos( NULL, "" );
                m_vtx_datas[node].push_back( txt_infos );
            }
        }

        Meshe* current_meshe = node->GetMeshe();

        if( NULL != current_meshe )
        {
            meshe_data = current_meshe->GetRenderData();

            dsstring meshe_path;
            current_meshe->GetPath( meshe_path );

            // optimisation pour les meshes : pas recreer dans le renderer un meshe deja cree (evite la verif via md5 hash, couteuse
            // pour les gros meshes)

            if( NULL == meshe_data )
            {
                if( false == renderer->CreateMeshe( current_meshe, &meshe_data ) )
                {
                    _DSEXCEPTION( "Cannot create Meshe" )
                }
            }
            //m_meshe_datas[node] = meshe_data;
            std::pair<void*, dsstring> meshe_infos( meshe_data, meshe_path );
            m_meshe_datas[node] = meshe_infos;
        }
    }

    //m_outputqueue.clear();
    m_back_outputqueue->clear();

    for( size_t i = 0; i < p_input_list.size(); i++ )
    {
        Operation operation;
        RenderingNode* node = p_input_list[i];

        if( m_sh_datas.count( node ) )
        {
            operation.type = SET_SHADERS;
            operation.data = m_sh_datas[node].first;
            operation.comment = m_sh_datas[node].second;
            m_back_outputqueue->push_back( operation );
        }

        if( m_rs_datas.count( node ) )
        {
            operation.type = SET_RENDERSTATES_IN;
            operation.data = m_rs_datas[node];
            m_back_outputqueue->push_back( operation );
        }

        if( m_tx_datas.count( node ) )
        {
            for( size_t j = 0; j < m_tx_datas[node].size(); j++ )
            {
                operation.type = SET_TEXTURE;
                //operation.data = m_tx_datas[node][j];
                operation.data = m_tx_datas[node][j].first;
                operation.texture_stage = (long)j;
                operation.comment = m_tx_datas[node][j].second;
                

                if( operation.data != NULL )
                {
                    m_back_outputqueue->push_back( operation );
                }
            }
        }

        if( m_vtx_datas.count( node ) )
        {
            for( size_t j = 0; j < m_vtx_datas[node].size(); j++ )
            {
                operation.type = SET_VERTEXTEXTURE;
                //operation.data = m_vtx_datas[node][j];
                operation.data = m_vtx_datas[node][j].first;
                operation.texture_stage = (long)j;
                operation.comment = m_vtx_datas[node][j].second;

                if( operation.data != NULL )
                {
                    m_back_outputqueue->push_back( operation );
                }
            }
        }


        if( m_meshe_datas.count( node ) )
        {
            operation.type = SET_MESHE;
            operation.data = m_meshe_datas[node].first;
            operation.comment = m_meshe_datas[node].second;
            m_back_outputqueue->push_back( operation );
        }

		//////Shaders params//////////////

        std::map<dsstring, RenderingNode::ShadersParams*> node_shaders_params;
        node->GetShadersParams( node_shaders_params );

        for(auto& e : node_shaders_params)
        {
            operation.type = SET_SHADERS_PARAMS;
            
            operation.shader_params = e.second;
            m_back_outputqueue->push_back( operation );
        }

		//////Shaders arrays params//////////////

		std::map<dsstring, RenderingNode::ShadersArrayParam*> node_shaders_array_params;
		node->GetShadersArrayParams(node_shaders_array_params);

		for (auto& e: node_shaders_array_params)
		{
			operation.type = SET_SHADERS_ARRAY_PARAMS;

			operation.shader_array_param = e.second;
            m_back_outputqueue->push_back( operation );
		}
		

		/////////////////////////////////////////

        operation.type = DRAW_NODE;
        operation.node = node;
        operation.comment = node->m_debug_id;
        m_back_outputqueue->push_back( operation );

        if( m_tx_datas.count( node ) )
        {
            for( size_t j = 0; j < m_tx_datas[node].size(); j++ )
            {
                operation.type = UNSET_TEXTURE;
                //operation.data = m_tx_datas[node][j];
                operation.data = m_tx_datas[node][j].first;
                operation.texture_stage = (long)j;
                operation.comment = m_tx_datas[node][j].second;

                if( operation.data != NULL )
                {
                    m_back_outputqueue->push_back( operation );
                }
            }
        }

        if( m_vtx_datas.count( node ) )
        {
            for( size_t j = 0; j < m_vtx_datas[node].size(); j++ )
            {
                operation.type = UNSET_VERTEXTEXTURE;
                //operation.data = m_vtx_datas[node][j];
                operation.data = m_vtx_datas[node][j].first;
                operation.texture_stage = (long)j;
                operation.comment = m_vtx_datas[node][j].second;

                if( operation.data != NULL )
                {
                    m_back_outputqueue->push_back( operation );
                }
            }
        }

        if( m_rs_datas.count( node ) )
        {
            operation.type = SET_RENDERSTATES_OUT;
            operation.data = m_rs_datas[node];
            m_back_outputqueue->push_back( operation );
        }
    }
}

void RenderingQueue::search_op_textures_groups( OperationType p_type, int p_stage, std::vector<OperationsGroup>& p_groups )
{
    void* curr_data{ NULL };
    OperationsGroup group;
    long index{ 0 };

    for( auto it = m_back_outputqueue->begin(); it != m_back_outputqueue->end(); ++it, index++ )
    {
        Operation curr_operation{ *it };

        if( p_type == curr_operation.type && p_stage == curr_operation.texture_stage )
        {
            if( NULL == curr_data )
            {
                curr_data = curr_operation.data;

                erase_infos ei;
                ei.pos = it;
                ei.index = index;
                group.push_back( ei ); 
            }
            else
            {
                if( curr_data == curr_operation.data )
                {
                    // add to currentgroup

                    erase_infos ei;
                    ei.pos = it;
                    ei.index = index;
                    group.push_back( ei );   
                }
                else
                {

                    p_groups.push_back( group );
                    // start new group
                    group.clear();

                    erase_infos ei;
                    ei.pos = it;
                    ei.index = index;
                    group.push_back( ei );

                    curr_data = curr_operation.data;
                }
            }
        }
    }

    if( group.size() > 0 )
    {
        p_groups.push_back( group );
    }
}


void RenderingQueue::search_op_groups( OperationType p_type, std::vector<OperationsGroup>& p_groups )
{
    void* curr_data = NULL;

    OperationsGroup group;

    long index = 0;
    for( auto it = m_back_outputqueue->begin(); it != m_back_outputqueue->end(); ++it, index++ )
    {
        Operation curr_operation = (*it);

        if( p_type == curr_operation.type )
        {
            if( NULL == curr_data )
            {
                curr_data = curr_operation.data;

                erase_infos ei;
                ei.pos = it;
                ei.index = index;
                group.push_back( ei ); 
            }
            else
            {
                if( curr_data == curr_operation.data )
                {
                    // add to currentgroup

                    erase_infos ei;
                    ei.pos = it;
                    ei.index = index;
                    group.push_back( ei );   
                }
                else
                {

                    p_groups.push_back( group );
                    // start new group
                    group.clear();

                    erase_infos ei;
                    ei.pos = it;
                    ei.index = index;
                    group.push_back( ei );

                    curr_data = curr_operation.data;
                }
            }
        }
    }

    if( group.size() > 0 )
    {
        p_groups.push_back( group );
    }
}


void RenderingQueue::cleanup_output_list( void )
{
    // etape de creation des groupes
    
    search_op_groups( SET_MESHE, m_setmeshe_groups );
    search_op_groups( SET_SHADERS, m_setshaders_groups );

    search_op_groups( SET_RENDERSTATES_IN, m_setrsin_groups );
    search_op_groups( SET_RENDERSTATES_OUT, m_setrsout_groups );

    for( int i = 0; i < RenderingNode::NbMaxTextures; i++ )
    {
        search_op_textures_groups( SET_TEXTURE, i, m_settexture_groups[i] );
        search_op_textures_groups( UNSET_TEXTURE, i, m_unsettexture_groups[i] );

        search_op_textures_groups( SET_VERTEXTEXTURE, i, m_setvtexture_groups[i] );
        search_op_textures_groups( UNSET_VERTEXTEXTURE, i, m_unsetvtexture_groups[i] );
    }


    // etape de nettoyage

    // SET_MESHE : pour chaque groupe effacer toutes les entrees sauf le 1er
    for( size_t i = 0; i < m_setmeshe_groups.size(); i++ )
    {
        if( m_setmeshe_groups[i].size() > 1 )
        {
            for( size_t j = 1; j < m_setmeshe_groups[i].size(); j++ )
            {
                m_back_outputqueue->erase( m_setmeshe_groups[i][j].pos );
            }
        }
    }

    // SET_SHADERS : pour chaque groupe effacer toutes les entrees sauf le 1er
    for( size_t i = 0; i < m_setshaders_groups.size(); i++ )
    {
        if( m_setshaders_groups[i].size() > 1 )
        {
            for( size_t j = 1; j < m_setshaders_groups[i].size(); j++ )
            {
                m_back_outputqueue->erase( m_setshaders_groups[i][j].pos );
            }
        }
    }

    // SET_RENDERSTATES_IN : pour chaque groupe effacer toutes les entrees sauf le 1er
    for( size_t i = 0; i < m_setrsin_groups.size(); i++ )
    {
        if( m_setrsin_groups[i].size() > 1 )
        {
            for( size_t j = 1; j < m_setrsin_groups[i].size(); j++ )
            {
                m_back_outputqueue->erase( m_setrsin_groups[i][j].pos );
            }
        }
    }

    // SET_RENDERSTATES_OUT : pour chaque groupe effacer toutes les entrees sauf le dernier
    for( size_t i = 0; i < m_setrsout_groups.size(); i++ )
    {
        if( m_setrsout_groups[i].size() > 1 )
        {
            for( size_t j = 0; j < m_setrsout_groups[i].size() - 1; j++ )
            {
                m_back_outputqueue->erase( m_setrsout_groups[i][j].pos );
            }
        }
    }


    for( int i0 = 0; i0 < RenderingNode::NbMaxTextures; i0++ )
    {
        // SET_TEXTURE : pour chaque groupe effacer toutes les entrees sauf le 1er
        for( size_t i = 0; i < m_settexture_groups[i0].size(); i++ )
        {
            if( m_settexture_groups[i0][i].size() > 1 )
            {
                for( size_t j = 1; j < m_settexture_groups[i0][i].size(); j++ )
                {
                    m_back_outputqueue->erase( m_settexture_groups[i0][i][j].pos );
                }
            }
        }  

        // UNSET_TEXTURE : pour chaque groupe effacer toutes les entrees sauf le dernier
        for( size_t i = 0; i < m_unsettexture_groups[i0].size(); i++ )
        {
            if( m_unsettexture_groups[i0][i].size() > 1 )
            {
                for( size_t j = 0; j < m_unsettexture_groups[i0][i].size() - 1; j++ )
                {
                    m_back_outputqueue->erase( m_unsettexture_groups[i0][i][j].pos );
                }
            }
        }


        // SET_VTEXTURE : pour chaque groupe effacer toutes les entrees sauf le 1er
        for( size_t i = 0; i < m_setvtexture_groups[i0].size(); i++ )
        {
            if( m_setvtexture_groups[i0][i].size() > 1 )
            {
                for( size_t j = 1; j < m_setvtexture_groups[i0][i].size(); j++ )
                {
                    m_back_outputqueue->erase( m_setvtexture_groups[i0][i][j].pos );
                }
            }
        }  

        // UNSET_VTEXTURE : pour chaque groupe effacer toutes les entrees sauf le dernier
        for( size_t i = 0; i < m_unsetvtexture_groups[i0].size(); i++ )
        {
            if( m_unsetvtexture_groups[i0][i].size() > 1 )
            {
                for( size_t j = 0; j < m_unsetvtexture_groups[i0][i].size() - 1; j++ )
                {
                    m_back_outputqueue->erase( m_unsetvtexture_groups[i0][i][j].pos );
                }
            }
        }
    }
}

long RenderingQueue::GetSwitchesCost( void ) const
{
    return m_switches_cost;
}

long RenderingQueue::GetTheoricalSwitchesCost( void ) const
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
