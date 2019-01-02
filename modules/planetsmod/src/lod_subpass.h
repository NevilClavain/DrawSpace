/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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


#include "pass.h"
#include "renderingnode.h"

namespace LOD
{
class SubPass abstract
{
public:

    using singleshot_subpasses_stack    = std::list<SubPass*>;
    using singleshot_subpasses          = std::vector<SubPass*>;
    using permanent_subpasses           = std::vector<SubPass*>;

    using EntryInfos = struct
    {    
        int queue_id; // 0 ->singleshot_subpasses_stack 1 -> singleshot_subpasses 2 -> permanent_subpasses

        singleshot_subpasses_stack*             singleshot_subpasses_stack;
        singleshot_subpasses*                   singleshot_subpasses;
        permanent_subpasses*                    permanent_subpasses;

        singleshot_subpasses_stack::iterator    singleshot_subpasses_stack_position;
        singleshot_subpasses::iterator          singleshot_subpasses_position;
        permanent_subpasses::iterator           permanent_subpasses_position;

    };

protected:

    DrawSpace::Core::RenderingNode*             m_subpass_node;
    DrawSpace::IntermediatePass*                m_subpass;
    bool                                        m_timer_ready_flag;

    static void remove_entry_from_queue( const EntryInfos& p_entryInfos );

public:

    SubPass( void );
    virtual ~SubPass( void );

    virtual void                                        DrawSubPass( void );
    virtual void                                        SubPassDone( void ) = 0;

    virtual DrawSpace::Core::RenderingNode*             GetNode( void ) const;
    virtual DrawSpace::IntermediatePass*                GetPass( void ) const;

    virtual void                                        SetTimerReadyFlag( bool p_flag );
    virtual bool                                        GetTimerReadyFlag( void ) const;

};
}
