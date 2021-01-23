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

#ifndef _RENDERPASSNODEGRAPH_H_
#define _RENDERPASSNODEGRAPH_H_

#include "renderpassnode.h"
#include "renderingaspectimpl.h"
#include <queue>

namespace DrawSpace
{
namespace Systems
{
class Hub;
}

namespace RenderGraph
{
class RenderPassNodeGraph
{
public:
    using PassDescrTree = st_tree::tree<RenderPassNode::PassDescr*>;

	using RenderPassEvent = enum
	{
		RENDERINGQUEUE_UPDATED,
        RENDERINGQUEUE_PASS_BEGIN,
        RENDERINGQUEUE_PASS_END,
	};

	using RenderPassEventHandler = DrawSpace::Core::BaseCallback2<void, RenderPassEvent, const dsstring&>;

private:

    using Signals = enum
    {
        SIGNAL_UPDATED_RENDERINGQUEUES,
        SIGNAL_DISABLE_PASS,
        SIGNAL_ENABLE_PASS,
    };

    mutable PassDescrTree				m_tree;
    
    std::queue<Signals>					m_signals;

    // args for signals
    dsstring                            m_pass_to_disable;
    dsstring                            m_pass_to_enable;

	std::set<RenderPassEventHandler*>	m_evt_handlers;

    Systems::Hub*                       m_hub{ nullptr };

    void cleanup_treenodes( void );

public:
    RenderPassNodeGraph(void);
    ~RenderPassNodeGraph( void );

    RenderPassNode CreateRoot( const dsstring& p_name );
    void Erase( void );
    void Accept( DrawSpace::Interface::AspectImplementations::RenderingAspectImpl* p_renderingaspectimpl );

    void ProcessSignals( void );

    void PushSignal_UpdatedRenderingQueues( void );
    void PushSignal_DisablePass(const dsstring& p_pass);
    void PushSignal_EnablePass(const dsstring& p_pass);

	void RegisterRenderPassEvtHandler(RenderPassEventHandler* p_handler);
	void UnregisterRenderPassEvtHandler(RenderPassEventHandler* p_handler);

    void SetSystemsHub(Systems::Hub* p_hub);
};

}
}

#endif
