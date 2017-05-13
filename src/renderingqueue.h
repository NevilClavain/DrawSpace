/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#ifndef _RENDERINGQUEUE_H_
#define _RENDERINGQUEUE_H_

#include "drawspace_commons.h"
#include "renderingnode.h"
#include <list>

namespace DrawSpace
{
namespace Core
{
class RenderingQueue
{
protected:

    typedef enum
    {        
        //FX_LIST,
        SHADERS_LIST,
        RS_LIST,
        MESHE_LIST,
        TEXTURE_LIST

    } SortedListType;


    typedef struct
    {
        SortedListType  type;
        long            stage;  // pour type TEXTURE_LIST

    } SortCategory;

    typedef enum
    {
        SET_TEXTURE,
        SET_VERTEXTEXTURE,
        SET_SHADERS,
        SET_RENDERSTATES_IN,
        SET_RENDERSTATES_OUT,
        SET_MESHE,
        SET_SHADERS_PARAMS,
        DRAW_NODE,
        UNSET_TEXTURE,
        UNSET_VERTEXTEXTURE,

    } OperationType;


    typedef struct
    {
        OperationType                                   type;
        void*                                           data;

        RenderingNode::ShadersParams*                   shader_params;
        long                                            texture_stage;

        RenderingNode*                                  node;
        dsstring                                        comment; // for debug purpose only

    } Operation;

    typedef struct
    {
        std::list<RenderingQueue::Operation>::iterator  pos;
        long                                            index;

    } erase_infos;

    typedef std::vector<erase_infos>                                        OperationsGroup;
 
    std::map<long, std::vector<RenderingNode*>>                             m_renderingorder_nodes;

    std::vector<RenderingNode*>	                                            m_nodes;

    std::list<Operation>                                                    m_outputqueue;

    Texture*                                                                m_target;
    bool                                                                    m_clear_depth;
    bool                                                                    m_clear_target;
    unsigned char                                                           m_target_clear_color_r;
    unsigned char                                                           m_target_clear_color_g;
    unsigned char                                                           m_target_clear_color_b;
    unsigned char                                                           m_target_clear_color_a;

    std::map<RenderingNode*, void*>                                         m_sh_datas;
    std::map<RenderingNode*, void*>                                         m_rs_datas;


    std::map<RenderingNode*, std::vector<std::pair<void*, dsstring> > >     m_tx_datas;
    std::map<RenderingNode*, std::vector<std::pair<void*, dsstring> > >     m_vtx_datas;


    std::map<RenderingNode*, std::pair<void*, dsstring> >                   m_meshe_datas;

    long                                                                    m_switches_cost;


    std::vector<OperationsGroup>                                            m_setmeshe_groups;
    std::vector<OperationsGroup>                                            m_setshaders_groups;

    std::vector<OperationsGroup>                                            m_setrsin_groups;
    std::vector<OperationsGroup>                                            m_setrsout_groups;

    std::vector<OperationsGroup>                                            m_settexture_groups[RenderingNode::NbMaxTextures];
    std::vector<OperationsGroup>                                            m_unsettexture_groups[RenderingNode::NbMaxTextures];

    std::vector<OperationsGroup>                                            m_setvtexture_groups[RenderingNode::NbMaxTextures];
    std::vector<OperationsGroup>                                            m_unsetvtexture_groups[RenderingNode::NbMaxTextures];


    std::map<dsstring, DrawSpace::Core::Fx*>                                m_fx_bases;


    static bool nodes_comp( RenderingNode* p_n1, RenderingNode* p_n2 );

    void sort_list( std::vector<RenderingNode*>& p_input_list, std::vector<RenderingNode*>& p_output_list );

    void sort_step( std::vector<SortCategory>& p_todo, std::vector<RenderingNode*>& p_input_list, std::vector<RenderingNode*>& p_output_list );
    void sort_list_by( SortedListType p_type, long p_texturestage, std::vector<RenderingNode*>& p_in_list, std::map<dsstring, std::vector<RenderingNode*>>& p_out_lists );

    void build_output_list( std::vector<RenderingNode*>& p_input_list );


    void search_op_groups( OperationType p_type, std::vector<OperationsGroup>& p_groups );
    void search_op_textures_groups( OperationType p_type, int p_stage, std::vector<OperationsGroup>& p_groups );

    void cleanup_output_list( void );



    double lists_score( std::map<dsstring, std::vector<RenderingNode*>>& p_lists );

    

public:
    RenderingQueue( void );
    RenderingQueue( Texture* p_target );
    ~RenderingQueue( void );

    void Add( RenderingNode* p_node );
    void Remove( RenderingNode* p_node );
    void Draw( void );
    
    void EnableDepthClearing( bool p_enable );
    void EnableTargetClearing( bool p_enable );
    void SetTargetClearingColor( unsigned char p_r, unsigned char p_g, unsigned char p_b, unsigned char p_a );

    void UpdateOutputQueue( void );
    void UpdateOutputQueueNoOpt( void );

    long GetTheoricalSwitchesCost( void );
    long GetSwitchesCost( void );
};
}
}

#endif
