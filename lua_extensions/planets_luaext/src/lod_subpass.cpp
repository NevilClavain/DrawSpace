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

#include "lod_subpass.h"

using namespace DrawSpace;
using namespace LOD;

SubPass::SubPass( void ) :
m_subpass( NULL ),
m_subpass_node( NULL ),
m_timer_ready_flag( false )
{
}

SubPass::~SubPass( void )
{
}

void SubPass::DrawSubPass( void )
{
    m_subpass->GetRenderingQueue()->Draw();
}

DrawSpace::Core::RenderingNode* SubPass::GetNode( void ) const
{
    return m_subpass_node;
}

DrawSpace::IntermediatePass* SubPass::GetPass( void ) const
{
    return m_subpass;
}

void SubPass::remove_entry_from_queue( const EntryInfos& p_entryInfos )
{
    const auto passDestination{ static_cast<Destination>(p_entryInfos.queue_id) };

    switch(passDestination)
    {        
        case Destination::DELAYED_SINGLE_SUBPASS:
            {
                p_entryInfos.singleshot_subpasses_stack->erase(p_entryInfos.singleshot_subpasses_stack_position);
            }
            break;

        case Destination::IMMEDIATE_SINGLE_SUBPASS:
            {
                const auto it{ p_entryInfos.singleshot_subpasses_position };
                p_entryInfos.singleshot_subpasses->erase( it );
            }
            break;
    }
}

void SubPass::SetTimerReadyFlag( bool p_flag )
{
    m_timer_ready_flag = p_flag;
}

bool SubPass::GetTimerReadyFlag( void ) const
{
    return m_timer_ready_flag;
}

void SubPass::RequestAbortion(void)
{
    m_request_for_abort = true;
}

bool SubPass::IsRequestedForAbortion(void) const
{
    return m_request_for_abort;
}