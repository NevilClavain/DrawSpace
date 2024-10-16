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

#pragma once

#include <map>

#include "ds_types.h"
#include "renderingnode.h"
#include <list>

namespace DrawSpace
{
namespace Core
{
class RenderingQueue
{
public:

    enum class Status
    {
        OK,
        ERROR_NOTREADY,
        ERROR_NOTARGET,
    };

    RenderingQueue( const dsstring& p_id );
    RenderingQueue(const dsstring& p_id, Texture* p_target );
    ~RenderingQueue( void );

    void SetTargetSlice(int p_target_slice);

    long GetTheoricalSwitchesCost(void) const;
    long GetSwitchesCost(void) const;
    bool IsReady(void) const;
    void DeclareReady(void);

    void Add( RenderingNode* p_node );
    void Remove( RenderingNode* p_node );
    void Draw( void );
    
    void EnableDepthClearing( bool p_enable );
    void EnableTargetClearing( bool p_enable );
    void SetTargetClearingColor( unsigned char p_r, unsigned char p_g, unsigned char p_b, unsigned char p_a );

    void UpdateOutputQueue( void );
    void UpdateOutputQueueNoOpt( void );
    void EraseOutputQueue(void);
    void FlipOutputQueues(void);

    dsstring GetId(void) const;

	Status GetStatus(void) const { return m_status; };

private:

    enum class SortedListType
    {
        SHADERS_LIST,
        RS_LIST,
        MESHE_LIST,
        LINEMESHE_LIST,
        TEXTURE_LIST
    };

    struct SortCategory
    {
        SortedListType  type;
        long            stage{ 0 };  // pour type TEXTURE_LIST

    };

    enum class OperationType
    {
        SET_TEXTURE,
        SET_VERTEXTEXTURE,
        SET_SHADERS,
        SET_RENDERSTATES_IN,
        SET_RENDERSTATES_OUT,
        SET_MESHE,
        SET_LINEMESHE,
        SET_SHADERS_PARAMS,
        SET_SHADERS_ARRAY_PARAMS,
        DRAW_NODE,
        UNSET_TEXTURE,
        UNSET_VERTEXTEXTURE,
    };

    using Operation = struct
    {
        OperationType                                   type;
        void* data;

        RenderingNode::ShadersParams* shader_params;
        RenderingNode::ShadersArrayParam* shader_array_param;
        long                                            texture_stage;

        RenderingNode* node;
        //dsstring                                        comment; // for debug purpose only
        std::vector<dsstring>                           comments;

    };

    using erase_infos = struct
    {
        std::list<RenderingQueue::Operation>::iterator  pos;
        long                                            index;

    };


    using OperationsGroup = std::vector<erase_infos>;

    std::map<long, std::vector<RenderingNode*>>                             m_renderingorder_nodes;

    std::vector<RenderingNode*>	                                            m_nodes;

    std::list<Operation>                                                    m_outputqueue_0;
    std::list<Operation>                                                    m_outputqueue_1;

    std::list<Operation>* m_current_outputqueue; // rendered output queue
    std::list<Operation>* m_back_outputqueue;    // prepared output queue 

    Texture*                                                                m_target{ nullptr };
    bool                                                                    m_clear_depth{ false };
    bool                                                                    m_clear_target{ false };
    unsigned char                                                           m_target_clear_color_r{ 0 };
    unsigned char                                                           m_target_clear_color_g{ 0 };
    unsigned char                                                           m_target_clear_color_b{ 0 };
    unsigned char                                                           m_target_clear_color_a{ 0 };

    std::map<RenderingNode*, std::pair<void*, dsstring>>                    m_sh_datas;
    std::map<RenderingNode*, void*>                                         m_rs_datas;


    std::map<RenderingNode*, std::vector<std::pair<void*, dsstring> > >     m_tx_datas;
    std::map<RenderingNode*, std::vector<std::pair<void*, dsstring> > >     m_vtx_datas;


    std::map<RenderingNode*, std::pair<void*, dsstring> >                   m_meshe_datas;
    std::map<RenderingNode*, std::pair<void*, dsstring> >                   m_linemeshe_datas;

    long                                                                    m_switches_cost{ 0 };


    std::vector<OperationsGroup>                                            m_setmeshe_groups;
    std::vector<OperationsGroup>                                            m_setlinemeshe_groups;
    std::vector<OperationsGroup>                                            m_setshaders_groups;

    std::vector<OperationsGroup>                                            m_setrsin_groups;
    std::vector<OperationsGroup>                                            m_setrsout_groups;

    std::vector<OperationsGroup>                                            m_settexture_groups[RenderingNode::NbMaxTextures];
    std::vector<OperationsGroup>                                            m_unsettexture_groups[RenderingNode::NbMaxTextures];

    std::vector<OperationsGroup>                                            m_setvtexture_groups[RenderingNode::NbMaxTextures];
    std::vector<OperationsGroup>                                            m_unsetvtexture_groups[RenderingNode::NbMaxTextures];


    std::map<dsstring, DrawSpace::Core::Fx*>                                m_fx_bases;

    bool                                                                    m_ready{ false };   // true if UpdateOutputQueue() UpdateOutputQueueNoOpt() were called at least once

    Status																	m_status{ Status::OK };

    int                                                                     m_target_slice{ 0 }; // quand > 0, on a affaire a une slice de texture3D

    dsstring                                                                m_id;
   
    void sort_list(std::vector<RenderingNode*>& p_input_list, std::vector<RenderingNode*>& p_output_list);

    void sort_step(std::vector<SortCategory>& p_todo, std::vector<RenderingNode*>& p_input_list, std::vector<RenderingNode*>& p_output_list);
    void sort_list_by(SortedListType p_type, long p_texturestage, std::vector<RenderingNode*>& p_in_list, std::map<dsstring, std::vector<RenderingNode*>>& p_out_lists);

    void build_output_list(std::vector<RenderingNode*>& p_input_list);

    void search_op_groups(OperationType p_type, std::vector<OperationsGroup>& p_groups);
    void search_op_textures_groups(OperationType p_type, int p_stage, std::vector<OperationsGroup>& p_groups);

    void cleanup_output_list(void);

    double lists_score(std::map<dsstring, std::vector<RenderingNode*>>& p_lists);

    static bool nodes_comp(RenderingNode* p_n1, RenderingNode* p_n2);
};
}
}

