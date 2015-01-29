/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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
#include "exceptions.h"
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

    //for( size_t i = 0; i < m_outputqueue.size(); i++ )
    for( std::list<Operation>::iterator it = m_outputqueue.begin(); it != m_outputqueue.end(); ++it )
    {
        //Operation curr_operation = m_outputqueue[i];
        Operation curr_operation = (*it);

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
               
                renderer->SetFxShaderParams( curr_operation.shader_params->shader_index, curr_operation.shader_params->param_register, curr_operation.shader_params->param_values );
                break;

            case DRAW_NODE:

                if( curr_operation.node->m_drawing_enabled )
                {
                    curr_operation.node->OnDraw();
                }
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
    m_renderingorder_nodes[p_node->GetOrderNumber()].push_back( p_node );
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
    m_nodes.clear();

    for( std::map<long, std::vector<RenderingNode*>>::iterator it = m_renderingorder_nodes.begin(); it != m_renderingorder_nodes.end(); ++it )
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
    cleanup_output_list();
}

double RenderingQueue::lists_score( std::map<dsstring, std::vector<RenderingNode*>>& p_lists )
{
    long count = 0;
    long sum = 0;

    if( 0 == p_lists.size() )
    {
        return 0.0;
    }

    for( std::map<dsstring, std::vector<RenderingNode*>>::iterator it = p_lists.begin(); it != p_lists.end(); ++it )
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
        for( std::map<dsstring, std::vector<RenderingNode*>>::iterator it = p_lists.begin(); it != p_lists.end(); ++it )
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

    category.type = FX_LIST;
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
    std::map<dsstring, std::vector<RenderingNode*>>     fx_lists;
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
    for( std::vector<SortCategory>::iterator it = p_todo.begin(); it != p_todo.end(); ++it )
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
    for( std::map<dsstring, std::vector<RenderingNode*>>::iterator it = selected_lists.begin(); it != selected_lists.end(); ++it )
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
            case FX_LIST:
                {
                    Fx* curr_fx = curr_node->GetFx();

                    if( curr_fx )
                    {
                        dsstring curr_fx_md5;
                        curr_fx->GetMD5( curr_fx_md5 );
                        out_lists[curr_fx_md5].push_back( curr_node );
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
                        dsstring path;
                        curr_tx->GetPath( path );

                        char buff[64];
                        dsstring signature = path + dsstring( ":" ) + itoa( p_texturestage, buff, 10 );

                        out_lists[signature].push_back( curr_node );
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

    void* fx_data;
    void* tx_data;
    void* meshe_data;

    m_tx_datas.clear();
    m_vtx_datas.clear();
    m_fx_datas.clear();
    m_meshe_datas.clear();

    for( size_t i = 0; i < p_input_list.size(); i++ )
    {
        RenderingNode* node = p_input_list[i];

        Fx* current_fx = node->GetFx();

        if( false == renderer->CreateFx( current_fx, &fx_data ) )
        {
            _DSEXCEPTION( "Cannot create FX" )
        }
        m_fx_datas[node] = fx_data;

        for( long j = 0; j < node->GetTextureListSize(); j++ )
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
                m_tx_datas[node].push_back( tx_data );
            }
            else
            {
                m_tx_datas[node].push_back( NULL );
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
                m_vtx_datas[node].push_back( tx_data );
            }
            else
            {
                m_vtx_datas[node].push_back( NULL );
            }
        }

        Meshe* current_meshe = node->GetMeshe();

        if( NULL != current_meshe )
        {
            if( false == renderer->CreateMeshe( current_meshe, &meshe_data ) )
            {
                _DSEXCEPTION( "Cannot create Meshe" )
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

                if( operation.data != NULL )
                {
                    m_outputqueue.push_back( operation );
                }
            }
        }

        if( m_vtx_datas.count( node ) )
        {
            for( size_t j = 0; j < m_vtx_datas[node].size(); j++ )
            {
                operation.type = SET_VERTEXTEXTURE;
                operation.data = m_vtx_datas[node][j];
                operation.texture_stage = (long)j;

                if( operation.data != NULL )
                {
                    m_outputqueue.push_back( operation );
                }
            }
        }


        if( m_meshe_datas.count( node ) )
        {
            operation.type = SET_MESHE;
            operation.data = m_meshe_datas[node];
            m_outputqueue.push_back( operation );
        }

        std::map<dsstring, RenderingNode::ShadersParams*> node_shaders_params;
        node->GetShadersParams( node_shaders_params );

        for( std::map<dsstring, RenderingNode::ShadersParams*>::iterator it = node_shaders_params.begin(); it != node_shaders_params.end(); ++it )
        {
            operation.type = SET_SHADERS_PARAMS;
            
            operation.shader_params = (*it).second;
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
                operation.data = m_tx_datas[node][j];
                operation.texture_stage = (long)j;

                if( operation.data != NULL )
                {
                    m_outputqueue.push_back( operation );
                }
            }
        }

        if( m_vtx_datas.count( node ) )
        {
            for( size_t j = 0; j < m_vtx_datas[node].size(); j++ )
            {
                operation.type = UNSET_VERTEXTEXTURE;
                operation.data = m_vtx_datas[node][j];
                operation.texture_stage = (long)j;

                if( operation.data != NULL )
                {
                    m_outputqueue.push_back( operation );
                }
            }
        }


        if( m_fx_datas.count( node ) )
        {
            operation.type = UNSET_FX;
            operation.data = m_fx_datas[node];
            m_outputqueue.push_back( operation );
        }
    }
}




void RenderingQueue::cleanup_output_list( void )
{
    Operation                current_setfx_ope;
    bool                     current_setfx_ope_set = false;

    Operation                current_settex_ope[RenderingNode::NbMaxTextures];
    bool                     current_settex_ope_set[RenderingNode::NbMaxTextures];

    Operation                current_setvtex_ope[RenderingNode::NbMaxTextures];
    bool                     current_setvtex_ope_set[RenderingNode::NbMaxTextures];


    Operation                current_setmeshe_ope;
    bool                     current_setmeshe_ope_set = false;

    std::vector<erase_infos> to_erase_list;

    for( long i = 0; i < RenderingNode::NbMaxTextures; i++ )
    {
        current_settex_ope_set[i] = false;
        current_setvtex_ope_set[i] = false;
    }

    long index = 0;
    for( std::list<Operation>::iterator it = m_outputqueue.begin(); it != m_outputqueue.end(); ++it, index++ )
    {
        Operation curr_operation = (*it);

        switch( curr_operation.type )
        {
            case SET_VERTEXTEXTURE:

                if( !current_setvtex_ope_set[curr_operation.texture_stage] )
                {
                    current_setvtex_ope[curr_operation.texture_stage] = curr_operation;
                    current_setvtex_ope_set[curr_operation.texture_stage] = true;
                }
                else
                {
                    if( current_setvtex_ope[curr_operation.texture_stage].data == curr_operation.data )                 
                    {
                        erase_infos ei;
                        ei.index = index;
                        ei.pos = it;

                        to_erase_list.push_back( ei );

                        // remonter pour rechercher le unset_tx precedent
                        
                        std::list<Operation>::iterator it2 = it;
                        long index2 = index;

                        while( it2 != m_outputqueue.begin() )
                        {
                            Operation curr_operation_2 = (*it2);

                            if( UNSET_VERTEXTEXTURE == curr_operation_2.type && current_setvtex_ope[curr_operation.texture_stage].data == curr_operation_2.data )                              
                            {
                                ei.index = index2;
                                ei.pos = it2;
                                to_erase_list.push_back( ei );  

                                break;
                            }
                            it2--;
                            index2--;
                        }
                    }
                    else
                    {
                        current_setvtex_ope[curr_operation.texture_stage] = curr_operation;
                    }
                }
                break;


            case SET_TEXTURE:

                if( !current_settex_ope_set[curr_operation.texture_stage] )
                {
                    current_settex_ope[curr_operation.texture_stage] = curr_operation;
                    current_settex_ope_set[curr_operation.texture_stage] = true;
                }
                else
                {
                    if( current_settex_ope[curr_operation.texture_stage].data == curr_operation.data )                 
                    {
                        erase_infos ei;
                        ei.index = index;
                        ei.pos = it;

                        to_erase_list.push_back( ei );

                        // remonter pour rechercher le unset_tx precedent
                        
                        std::list<Operation>::iterator it2 = it;
                        long index2 = index;

                        while( it2 != m_outputqueue.begin() )
                        {
                            Operation curr_operation_2 = (*it2);

                            if( UNSET_TEXTURE == curr_operation_2.type && current_settex_ope[curr_operation.texture_stage].data == curr_operation_2.data )                              
                            {
                                ei.index = index2;
                                ei.pos = it2;
                                to_erase_list.push_back( ei );  

                                break;
                            }
                            it2--;
                            index2--;
                        }
                    }
                    else
                    {
                        current_settex_ope[curr_operation.texture_stage] = curr_operation;
                    }
                }
                break;

            case SET_FX:

                if( !current_setfx_ope_set )
                {
                    current_setfx_ope = curr_operation;
                    current_setfx_ope_set = true;                    
                }
                else
                {
                    if( current_setfx_ope.data == curr_operation.data )
                    {
                        erase_infos ei;
                        ei.index = index;
                        ei.pos = it;

                        to_erase_list.push_back( ei );

                        // remonter pour rechercher le unset_fx precedent
                        
                        std::list<Operation>::iterator it2 = it;
                        long index2 = index;

                        while( it2 != m_outputqueue.begin() )
                        {
                            Operation curr_operation_2 = (*it2);

                            if( UNSET_FX == curr_operation_2.type && current_setfx_ope.data == curr_operation_2.data )
                            {
                                ei.index = index2;
                                ei.pos = it2;
                                to_erase_list.push_back( ei );  

                                break;
                            }
                            it2--;
                            index2--;
                        }
                    }
                    else
                    {
                        current_setfx_ope = curr_operation;
                    }
                }
                
                break;

            case SET_MESHE:

                if( !current_setmeshe_ope_set )
                {
                    current_setmeshe_ope = curr_operation;
                    current_setmeshe_ope_set = true;                    
                }
                else
                {
                    if( current_setmeshe_ope.data == curr_operation.data )
                    {
                        erase_infos ei;
                        ei.index = index;
                        ei.pos = it;

                        to_erase_list.push_back( ei );
                    }
                    else
                    {
                        current_setmeshe_ope = curr_operation;
                    }
                }

                break;

        }
    }

    for( size_t i = 0; i < to_erase_list.size(); i++ )
    {
        m_outputqueue.erase( to_erase_list[i].pos );
    }
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
