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

#include "pass.h"
#include "renderingnode.h"

namespace LOD
{
class SubPass abstract
{
public:
  
    using singleshot_subpasses_stack    = std::list<SubPass*>;
    using singleshot_subpasses          = std::list<SubPass*>;
    using permanent_subpasses           = std::vector<SubPass*>;

    enum class Destination
    {
        DELAYED_SINGLE_SUBPASS      = 0,
        IMMEDIATE_SINGLE_SUBPASS    = 1
    };

    using EntryInfos = struct
    {    
        int queue_id; // 0 ->singleshot_subpasses_stack 1 -> singleshot_subpasses 2 -> permanent_subpasses

        singleshot_subpasses_stack*             singleshot_subpasses_stack;
        singleshot_subpasses*                   singleshot_subpasses;

        singleshot_subpasses_stack::iterator    singleshot_subpasses_stack_position;
        singleshot_subpasses::iterator          singleshot_subpasses_position;

    };

    using SubPassCreationHandler = DrawSpace::Core::BaseCallback2<SubPass::EntryInfos, SubPass*, SubPass::Destination>;

protected:

    //DrawSpace::Core::RenderingNode*             m_subpass_node;
    //DrawSpace::IntermediatePass*                m_subpass;

    std::vector< DrawSpace::Core::RenderingNode*>   m_subpass_node_list;
    std::vector<DrawSpace::IntermediatePass*>       m_subpass_list;

    bool                                        m_timer_ready_flag;

    bool                                        m_request_for_abort{ false };

    static void remove_entry_from_queue( const EntryInfos& p_entryInfos );

public:

    SubPass( void );
    virtual ~SubPass( void );

    virtual void                                DrawSubPass( void );

    virtual void                                SubPassDone( void ) = 0;
    virtual void                                SubPassAborted( void ) = 0;

    //DrawSpace::Core::RenderingNode*             GetNode( void ) const;
    //DrawSpace::IntermediatePass*                GetPass( void ) const;

    std::vector< DrawSpace::Core::RenderingNode*>   GetNodeList(void) const;
    std::vector<DrawSpace::IntermediatePass*>       GetPassList(void) const;


    void                                        SetTimerReadyFlag( bool p_flag );
    bool                                        GetTimerReadyFlag( void ) const;

    void                                        RequestAbortion(void);

    bool                                        IsRequestedForAbortion(void) const;

};
}
