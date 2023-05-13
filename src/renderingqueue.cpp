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

#include "renderingqueue.h"
#include "renderer.h"
#include "plugin.h"
#include "exceptions.h"
#include <algorithm>

using namespace DrawSpace::Core;
using namespace DrawSpace::Interface;

DrawSpace::Logger::Sink rd_logger("RenderingQueue", DrawSpace::Logger::Configuration::GetInstance());

RenderingQueue::RenderingQueue(const dsstring& p_id) :
m_target( nullptr ),
m_target_slice(0),
m_id( p_id),
m_current_outputqueue(&m_outputqueue_0),
m_back_outputqueue(&m_outputqueue_1)
{


}

RenderingQueue::RenderingQueue(const dsstring& p_id, Texture* p_target ) :
m_target( p_target ),
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

    const auto renderer{ SingletonPlugin<Renderer>::GetInstance()->m_interface };
    if( m_target )
    {
        if (!m_ready)
        {
            m_status = Status::ERROR_NOTREADY;
            return;
        }

        const auto texture_render_data{ m_target->GetRenderData() };
        if( nullptr == texture_render_data )
        {
            //_DSEXCEPTION( "target texture not initialised in renderer" )
            m_status = Status::ERROR_NOTARGET;
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
        m_status = Status::ERROR_NOTREADY;
        return;
    }
    else
    {
        int operation_counter{ 0 }; // to help setting breakpoint

        for( auto& curr_operation : *m_current_outputqueue)
        {            
            if(63 == operation_counter)
            {
                _asm nop // set breakpoint here
            }
                       
            switch( curr_operation.type )
            {
                case OperationType::SET_TEXTURE:

                    renderer->SetTexture( curr_operation.data, curr_operation.texture_stage );
                    m_switches_cost++;
                    break;

                case OperationType::UNSET_TEXTURE:

                    renderer->UnsetTexture( curr_operation.texture_stage );
                    m_switches_cost++;
                    break;

                case OperationType::SET_VERTEXTEXTURE:

                    renderer->SetVertexTexture( curr_operation.data, curr_operation.texture_stage );
                    m_switches_cost++;
                    break;

                case OperationType::UNSET_VERTEXTEXTURE:

                    renderer->UnsetVertexTexture( curr_operation.texture_stage );
                    m_switches_cost++;
                    break;

                case OperationType::SET_SHADERS:

                    renderer->SetShaders( curr_operation.data );
                    m_switches_cost++;
                    break;

                case OperationType::SET_RENDERSTATES_IN:

                    renderer->ApplyRenderStatesIn( curr_operation.data );
                    m_switches_cost++;
                    break;

                case OperationType::SET_RENDERSTATES_OUT:

                    renderer->ApplyRenderStatesOut( curr_operation.data );
                    m_switches_cost++;
                    break;

                case OperationType::SET_MESHE:
                    renderer->SetMeshe( curr_operation.data );
                    m_switches_cost++;
                    break;

                case OperationType::SET_LINEMESHE:
                    renderer->SetLineMeshe(curr_operation.data);
                    m_switches_cost++;
                    break;

                case OperationType::SET_SHADERS_PARAMS:
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

                case OperationType::SET_SHADERS_ARRAY_PARAMS:
				    {					
					    renderer->SetShaderVectorBuffer(curr_operation.shader_array_param->shader_index, curr_operation.shader_array_param->begin_register, curr_operation.shader_array_param->array);
				    }
				    break;

                case OperationType::DRAW_NODE:

                    if( curr_operation.node->m_drawing_enabled )
                    {
                        curr_operation.node->OnDraw();
                    }
                    break;
            }
            operation_counter++;
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

	m_status = Status::OK;
}

void RenderingQueue::Add( RenderingNode* p_node )
{
    m_renderingorder_nodes[p_node->GetOrderNumber()].push_back( p_node );

    // a new node was added; we cannot consider we're 'ready' until a new call to UpdateOutputQueue() or UpdateOutputQueueNoOpt() has been made (new node probably need resources creation in renderer plugin...)
    m_ready = false;
}

void RenderingQueue::Remove( RenderingNode* p_node )
{
    const auto order_num{ p_node->GetOrderNumber() };
    std::vector<RenderingNode*>& list{ m_renderingorder_nodes[order_num] };

    for( auto it = list.begin(); it != list.end(); ++it )
    {
        if( *it == p_node )
        {
            list.erase( it );
            break;
        }
    }

    m_ready = false;
}

bool RenderingQueue::IsReady(void) const
{
    return m_ready;
}

void RenderingQueue::DeclareReady(void)
{
    m_ready = true;
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

void RenderingQueue::EraseOutputQueue(void)
{
    m_back_outputqueue->clear();
}

void RenderingQueue::UpdateOutputQueue( void )
{
    m_nodes.clear();
    m_fx_bases.clear();

    for(auto& e : m_renderingorder_nodes)
    {
        std::vector<RenderingNode*> sorted_list;
        sort_list( e.second, sorted_list );

        //for( size_t i = 0; i < sorted_list.size(); i++ )
        for(auto& s : sorted_list)
        {
            m_nodes.push_back( /*sorted_list[i]*/ s);

            // a chaque ajout, refaire un sort
            std::sort( m_nodes.begin(), m_nodes.end(), RenderingQueue::nodes_comp );            
        }
    }
    
    build_output_list( m_nodes );

    m_setmeshe_groups.clear();
    m_setlinemeshe_groups.clear();
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
}

void RenderingQueue::UpdateOutputQueueNoOpt( void )
{
    m_nodes.clear();
    m_fx_bases.clear();

    for(auto& e : m_renderingorder_nodes)
    {
        for (size_t i = 0; i < e.second.size(); i++)
        {     
            m_nodes.push_back(e.second[i]);

            // a chaque ajout, refaire un sort
            std::sort( m_nodes.begin(), m_nodes.end(), RenderingQueue::nodes_comp );            
        }
    }
    build_output_list( m_nodes );

    m_setmeshe_groups.clear();
    m_setlinemeshe_groups.clear();
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
}

double RenderingQueue::lists_score( std::map<dsstring, std::vector<RenderingNode*>>& p_lists )
{
    long count{ 0 };
    long sum{ 0 };

    if( 0 == p_lists.size() )
    {
        return 0.0;
    }

    for(auto& e : p_lists)
    {
        if (e.second.size() > 1)
        {
            count++;
            sum += (long)(e.second.size());
        }
    }
    
    if( count > 0 )
    {
        const auto alpha{ (double)count / (double)p_lists.size() };

        // moyenne
        const auto avg{ (double)sum / (double)count };

        //compter nbre de nodes
        long count2{ 0 };
        for (auto& e : p_lists)
        {
            count2 += (long)(e.second.size());
        }

        // normaliser la moyenne
        const auto beta{ avg / count2 };

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
    std::vector<SortCategory> todo{ { SortedListType::SHADERS_LIST, 0 }, { SortedListType::RS_LIST, 0 }, { SortedListType::MESHE_LIST, 0 }, { SortedListType::LINEMESHE_LIST, 0 } };

    // add TEXTURE_LIST entries
    for (int i = 0; i < RenderingNode::NbMaxTextures; i++)
    {
        const SortCategory category{ SortedListType::TEXTURE_LIST, i };
        todo.push_back(category);
    }
    sort_step( todo, p_input_list, p_output_list );
}


void RenderingQueue::sort_step( std::vector<SortCategory>& p_todo, std::vector<RenderingNode*>& p_input_list, std::vector<RenderingNode*>& p_output_list )
{
    std::map<dsstring, std::vector<RenderingNode*>>     shaders_lists;
    std::map<dsstring, std::vector<RenderingNode*>>     rs_lists;
    std::map<dsstring, std::vector<RenderingNode*>>     meshe_lists;
    std::map<dsstring, std::vector<RenderingNode*>>     linemeshe_lists;
    std::map<dsstring, std::vector<RenderingNode*>>     texture_lists;
    
    SortedListType                                      sel_type;
    long                                                sel_stage;
    std::map<dsstring, std::vector<RenderingNode*>>     selected_lists;
    bool                                                sel_flag{ false };

    double max_score{ 0.0 };

    for(const auto& category : p_todo)
    {
        const auto listType{ category.type };

        switch( listType )
        {
            case SortedListType::SHADERS_LIST:
                {
                    sort_list_by(listType, 0, p_input_list, shaders_lists );

                    const auto local_score{ lists_score(shaders_lists) };
                    if( local_score > max_score )
                    {
                        max_score = local_score;
                        selected_lists = shaders_lists;
                        sel_type = listType;
                        sel_flag = true;
                    }
                }
                break;

            case SortedListType::RS_LIST:
                {
                    sort_list_by(listType, 0, p_input_list, rs_lists );

                    const auto local_score{ lists_score(rs_lists) };
                    if( local_score > max_score )
                    {
                        max_score = local_score;
                        selected_lists = rs_lists;
                        sel_type = listType;
                        sel_flag = true;
                    }
                }
                break;

            case SortedListType::MESHE_LIST:
                {
                    sort_list_by(listType, 0, p_input_list, meshe_lists );

                    const auto local_score{ lists_score(meshe_lists) };
                    if( local_score > max_score )
                    {
                        max_score = local_score;
                        selected_lists = meshe_lists;
                        sel_type = listType;
                        sel_flag = true;
                    }
                }
                break;

            case SortedListType::LINEMESHE_LIST:
                {
                    sort_list_by(listType, 0, p_input_list, linemeshe_lists);

                    const auto local_score{ lists_score(linemeshe_lists) };
                    if (local_score > max_score)
                    {
                        max_score = local_score;
                        selected_lists = linemeshe_lists;
                        sel_type = listType;
                        sel_flag = true;
                    }
                }
                break;

            case SortedListType::TEXTURE_LIST:
                {
                    sort_list_by(listType, category.stage, p_input_list, texture_lists );

                    const auto local_score { lists_score(texture_lists) };
                    if( local_score > max_score )
                    {
                        max_score = local_score;
                        selected_lists = texture_lists;
                        sel_type = listType;
                        sel_stage = category.stage;
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
        if(SortedListType::TEXTURE_LIST == sel_type )
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
    //for( auto it = selected_lists.begin(); it != selected_lists.end(); ++it )
    for (auto e: selected_lists)
    {
        //if( (*it).second.size() > 2 )
        if (e.second.size() > 2)
        {
            if( p_todo.size() > 0 )
            {
                std::vector<RenderingNode*> output_list;              
                sort_step( p_todo, e.second, output_list );

                for( size_t i = 0; i < output_list.size(); i++ )
                {
                    p_output_list.push_back( output_list[i] ); 
                }
            }
            else
            {
                for( size_t i = 0; i < e.second.size(); i++ )
                {
                    p_output_list.push_back( e.second[i] ); 
                }
            }
        }
        else
        {
            for( size_t i = 0; i < e.second.size(); i++ )
            {
                p_output_list.push_back( e.second[i] );
            }
        }
    }
}

void RenderingQueue::sort_list_by( SortedListType p_type, long p_texturestage, std::vector<RenderingNode*>& p_in_list, std::map<dsstring, std::vector<RenderingNode*>>& p_out_lists )
{
    std::map<dsstring, std::vector<RenderingNode*> > out_lists;
    char buff[64];

    for(const auto& curr_node : p_in_list)
    {
        switch( p_type )
        {
            case SortedListType::SHADERS_LIST:
                {
                    const auto curr_fx{ curr_node->GetFx() };
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
                        const auto id{ dsstring("void_fx:") + dsstring(buff) };
                        out_lists[id].push_back( curr_node );
                    }
                }
                break;

            case SortedListType::RS_LIST:
                {
                    const auto curr_fx{ curr_node->GetFx() };
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
                        const auto id{ dsstring("void_fx:") + dsstring(buff) };
                        out_lists[id].push_back( curr_node );
                    }
                }
                break;

            case SortedListType::MESHE_LIST:
                {
                    const auto curr_meshe{ curr_node->GetMeshe() };
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
                        const auto id{ dsstring("void_meshe:") + dsstring(buff) };
                        out_lists[id].push_back( curr_node );
                    }
                }
                break;

            case SortedListType::LINEMESHE_LIST:
                {
                    const auto curr_line_meshe{ curr_node->GetLineMeshe() };
                    if (curr_line_meshe)
                    {
                        const auto curr_meshe_md5{ curr_line_meshe->GetMD5() };
                        out_lists[curr_meshe_md5].push_back(curr_node);
                    }
                    else
                    {
                        // pas de line meshe : ne doit pas influer sur les comptages de redondances
                        // s'assurer que l'id est unique, empechant ainsi la mise en place de fausses redondances
                        sprintf(buff, "%x", (long long)curr_node);
                        const auto id{ dsstring("void_linemeshe:") + dsstring(buff) };
                        out_lists[id].push_back(curr_node);
                    }

                }
                break;

            case SortedListType::TEXTURE_LIST:
                {
                    const auto curr_tx{ curr_node->GetTexture(p_texturestage) };
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
    const auto renderer{ SingletonPlugin<Renderer>::GetInstance()->m_interface };

    void* sh_data;
    void* rs_data;
    void* tx_data;
    

    m_tx_datas.clear();
    m_vtx_datas.clear();
    m_sh_datas.clear();
    m_rs_datas.clear();

    m_meshe_datas.clear();

    for(const auto& node : p_input_list)
    {
        const auto current_fx{ node->GetFx() };

        if( !current_fx )
        {
            _DSEXCEPTION( "Missing fx for rendering node" );
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
            rs_data = static_cast<void *>(m_fx_bases.at(hash));
        }
        else
        {
            rs_data = static_cast<void*>(current_fx);
            m_fx_bases[hash] = current_fx;
        }

        m_rs_datas[node] = rs_data;
        /////////////////////////////////////////////

        const auto texture_list_size{ node->GetTextureListSize() };
        for( long j = 0; j < texture_list_size; j++ )
        {
            auto current_tx{ node->GetTexture(j) };
            if( nullptr != current_tx )
            {
                if( false == renderer->CreateTexture( current_tx, &tx_data ) )
                {
                    dsstring path;
                    current_tx->GetPath( path );

                    dsstring excp_msg = "Cannot create Texture ";
                    excp_msg += path;
                    _DSEXCEPTION( excp_msg  )
                }

                dsstring texture_id;
                current_tx->GetPath( texture_id );
                std::pair<void*, dsstring> txt_infos( tx_data, texture_id );
                m_tx_datas[node].push_back( txt_infos );
            }
            else
            {
                std::pair<void*, dsstring> txt_infos( nullptr, "" );
                m_tx_datas[node].push_back( txt_infos );
            }


            /// vertex textures

            current_tx = node->GetVertexTexture( j );

            if( nullptr != current_tx )
            {
                if( false == renderer->CreateTexture( current_tx, &tx_data ) )
                {
                    dsstring path;
                    current_tx->GetPath( path );

                    dsstring excp_msg = "Cannot create Vertex Texture ";
                    excp_msg += path;
                    _DSEXCEPTION( excp_msg  )
                }

                dsstring texture_id;
                current_tx->GetPath( texture_id );
                std::pair<void*, dsstring> txt_infos( tx_data, texture_id );
                m_vtx_datas[node].push_back( txt_infos );
            }
            else
            {
                std::pair<void*, dsstring> txt_infos( nullptr, "" );
                m_vtx_datas[node].push_back( txt_infos );
            }
        }

        const auto current_meshe{ node->GetMeshe() };
        if( nullptr != current_meshe )
        {
            auto meshe_data{ current_meshe->GetRenderData() };

            dsstring meshe_path;
            current_meshe->GetPath( meshe_path );

            // optimisation pour les meshes : pas recreer dans le renderer un meshe deja cree (evite la verif via md5 hash, couteuse
            // pour les gros meshes)

            if( nullptr == meshe_data )
            {
                if( false == renderer->CreateMeshe( current_meshe, &meshe_data ) )
                {
                    _DSEXCEPTION( "Cannot create Meshe" )
                }
            }

            std::pair<void*, dsstring> meshe_infos( meshe_data, meshe_path );
            m_meshe_datas[node] = meshe_infos;
        }

        const auto current_linemeshe{ node->GetLineMeshe() };
        if (nullptr != current_linemeshe)
        {
            auto linemeshe_data{ current_linemeshe->GetRenderData() };
            const auto linemeshe_name{ current_linemeshe->GetName() };            
            // optimisation pour les meshes : pas recreer dans le renderer un meshe deja cree (evite la verif via md5 hash, couteuse
            // pour les gros meshes)

            if (nullptr == linemeshe_data)
            {
                if (false == renderer->CreateLineMeshe(current_linemeshe, &linemeshe_data))
                {
                    _DSEXCEPTION("Cannot create LineMeshe")    
                }
            }

            std::pair<void*, dsstring> linemeshe_infos(linemeshe_data, linemeshe_name);
            m_linemeshe_datas[node] = linemeshe_infos;
        }
    }

    m_back_outputqueue->clear();

    //for( size_t i = 0; i < p_input_list.size(); i++ )
    for(const auto& node : p_input_list)
    {        
        //const auto node{ p_input_list[i] };
        
        if( m_sh_datas.count( node ) )
        {
            Operation operation;
            operation.type = OperationType::SET_SHADERS;
            operation.data = m_sh_datas[node].first;

            operation.comments.push_back(m_sh_datas[node].second);

            m_back_outputqueue->push_back( operation );
        }

        if( m_rs_datas.count( node ) )
        {
            Operation operation;
            operation.type = OperationType::SET_RENDERSTATES_IN;
            operation.data = m_rs_datas[node];

            const auto curr_fx { static_cast<Fx*>(m_rs_datas[node]) };
            const auto rs{ curr_fx->GetRenderStatesSetRef() };

            for (size_t i = 0; i < rs->GetRenderStatesInListSize(); i++)
            {
                operation.comments.push_back(rs->GetRenderStateIn(i).ToString() + " = " + rs->GetRenderStateIn(i).GetArg());
            }

            m_back_outputqueue->push_back( operation );
        }

        if( m_tx_datas.count( node ) )
        {
            for( size_t j = 0; j < m_tx_datas[node].size(); j++ )
            {
                Operation operation;
                operation.type = OperationType::SET_TEXTURE;
                operation.data = m_tx_datas[node][j].first;
                operation.texture_stage = (long)j;
                operation.comments.push_back( m_tx_datas[node][j].second );
                
                if( operation.data != nullptr )
                {
                    m_back_outputqueue->push_back( operation );
                }
            }
        }

        if( m_vtx_datas.count( node ) )
        {
            for( size_t j = 0; j < m_vtx_datas[node].size(); j++ )
            {
                Operation operation;
                operation.type = OperationType::SET_VERTEXTEXTURE;
                operation.data = m_vtx_datas[node][j].first;
                operation.texture_stage = (long)j;
                operation.comments.push_back(m_vtx_datas[node][j].second);

                if( operation.data != nullptr )
                {
                    m_back_outputqueue->push_back( operation );
                }
            }
        }

        if( m_meshe_datas.count( node ) )
        {
            Operation operation;
            operation.type = OperationType::SET_MESHE;
            operation.data = m_meshe_datas[node].first;
            operation.comments.push_back( m_meshe_datas[node].second );
            m_back_outputqueue->push_back( operation );
        }

        if (m_linemeshe_datas.count(node))
        {
            Operation operation;
            operation.type = OperationType::SET_LINEMESHE;
            operation.data = m_linemeshe_datas[node].first;
            operation.comments.push_back( m_linemeshe_datas[node].second );
            m_back_outputqueue->push_back(operation);
        }

		//////Shaders params//////////////

        std::map<dsstring, RenderingNode::ShadersParams*> node_shaders_params;
        node->GetShadersParams( node_shaders_params );

        for(const auto& e : node_shaders_params)
        {
            Operation operation;
            operation.type = OperationType::SET_SHADERS_PARAMS;
            
            operation.shader_params = e.second;
            
            auto params_descr{ "shader index = " + std::to_string(operation.shader_params->shader_index) + " register " + std::to_string(operation.shader_params->param_register) };

            if (operation.shader_params->vector)
            {
                params_descr += " vector : ";

                params_descr += std::to_string(operation.shader_params->param_values[0]) + " ";
                params_descr += std::to_string(operation.shader_params->param_values[1]) + " ";
                params_descr += std::to_string(operation.shader_params->param_values[2]) + " ";
                params_descr += std::to_string(operation.shader_params->param_values[3]);
            }
            else
            {
                params_descr += " matrix";
            }

            operation.comments.push_back(params_descr);

            m_back_outputqueue->push_back(operation);
        }

		//////Shaders arrays params//////////////

		std::map<dsstring, RenderingNode::ShadersArrayParam*> node_shaders_array_params;
		node->GetShadersArrayParams(node_shaders_array_params);

		for (const auto& e: node_shaders_array_params)
		{
            Operation operation;
			operation.type = OperationType::SET_SHADERS_ARRAY_PARAMS;

			operation.shader_array_param = e.second;
            dsstring params_descr = "shader index = " + std::to_string(operation.shader_array_param->shader_index) + " begin register " + std::to_string(operation.shader_array_param->begin_register);

            m_back_outputqueue->push_back( operation );
		}
		

		/////////////////////////////////////////
        {
            Operation operation;
            operation.type = OperationType::DRAW_NODE;
            operation.node = node;
            operation.comments.push_back( node->m_debug_id );
            m_back_outputqueue->push_back(operation);
        }

        if( m_tx_datas.count( node ) )
        {
            for( size_t j = 0; j < m_tx_datas[node].size(); j++ )
            {
                Operation operation;
                operation.type = OperationType::UNSET_TEXTURE;
                operation.data = m_tx_datas[node][j].first;
                operation.texture_stage = (long)j;
                operation.comments.push_back( m_tx_datas[node][j].second );

                if( operation.data != nullptr )
                {
                    m_back_outputqueue->push_back( operation );
                }
            }
        }

        if( m_vtx_datas.count( node ) )
        {
            for( size_t j = 0; j < m_vtx_datas[node].size(); j++ )
            {
                Operation operation;
                operation.type = OperationType::UNSET_VERTEXTEXTURE;
                operation.data = m_vtx_datas[node][j].first;
                operation.texture_stage = (long)j;
                operation.comments.push_back( m_vtx_datas[node][j].second );

                if( operation.data != nullptr )
                {
                    m_back_outputqueue->push_back( operation );
                }
            }
        }

        if( m_rs_datas.count( node ) )
        {
            Operation operation;
            operation.type = OperationType::SET_RENDERSTATES_OUT;
            operation.data = m_rs_datas[node];

            const auto curr_fx{ static_cast<Fx*>(m_rs_datas[node]) };
            const auto rs{ curr_fx->GetRenderStatesSetRef() };

            for (size_t i = 0; i < rs->GetRenderStatesOutListSize(); i++)
            {
                operation.comments.push_back(rs->GetRenderStateOut(i).ToString() + " = " + rs->GetRenderStateOut(i).GetArg());
            }

            m_back_outputqueue->push_back( operation );
        }
    }
}

void RenderingQueue::search_op_textures_groups( OperationType p_type, int p_stage, std::vector<OperationsGroup>& p_groups )
{
    void* curr_data{ nullptr };
    OperationsGroup group;
    long index{ 0 };

    for( auto it = m_back_outputqueue->begin(); it != m_back_outputqueue->end(); ++it, index++ )
    {
        const auto curr_operation{ *it };

        if( p_type == curr_operation.type && p_stage == curr_operation.texture_stage )
        {
            if( nullptr == curr_data )
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
    void* curr_data{ nullptr };
    OperationsGroup group;
    long index{ 0 };

    for( auto it = m_back_outputqueue->begin(); it != m_back_outputqueue->end(); ++it, index++ )
    {
        const auto curr_operation{ *it };

        if( p_type == curr_operation.type )
        {
            if( nullptr == curr_data )
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
    
    search_op_groups( OperationType::SET_MESHE, m_setmeshe_groups );
    search_op_groups( OperationType::SET_LINEMESHE, m_setlinemeshe_groups);

    search_op_groups( OperationType::SET_SHADERS, m_setshaders_groups );

    search_op_groups( OperationType::SET_RENDERSTATES_IN, m_setrsin_groups );
    search_op_groups( OperationType::SET_RENDERSTATES_OUT, m_setrsout_groups );

    for( int i = 0; i < RenderingNode::NbMaxTextures; i++ )
    {
        search_op_textures_groups( OperationType::SET_TEXTURE, i, m_settexture_groups[i] );
        search_op_textures_groups( OperationType::UNSET_TEXTURE, i, m_unsettexture_groups[i] );

        search_op_textures_groups( OperationType::SET_VERTEXTEXTURE, i, m_setvtexture_groups[i] );
        search_op_textures_groups( OperationType::UNSET_VERTEXTEXTURE, i, m_unsetvtexture_groups[i] );
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

    // SET_LINEMESHE : pour chaque groupe effacer toutes les entrees sauf le 1er
    for (size_t i = 0; i < m_setlinemeshe_groups.size(); i++)
    {
        if (m_setlinemeshe_groups[i].size() > 1)
        {
            for (size_t j = 1; j < m_setlinemeshe_groups[i].size(); j++)
            {
                m_back_outputqueue->erase(m_setlinemeshe_groups[i][j].pos);
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
    long cost{ 0 };
    for(const auto& node : m_nodes)
    {
        if( node->GetFx() )
        {
            cost += 2; // SET_FX, UNSET_FX;
        }

        for( long j = 0; j < node->GetTextureListSize(); j++ )
        {
            const auto current_tx{ node->GetTexture(j) };
            if( nullptr != current_tx )
            {
                cost += 2; // SET_TEX, UNSET_TEX;
            }
        }
       
        if( node->GetMeshe() )
        {
            cost++; //SET_MESHE
        }

        if (node->GetLineMeshe())
        {
            cost++; //SET_LINEMESHE
        }

    }
    return cost;
}

dsstring RenderingQueue::GetId(void) const
{
    return m_id;
}
