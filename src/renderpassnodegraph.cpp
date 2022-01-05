/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

#include "renderpassnodegraph.h"
#include "memalloc.h"
#include "renderer.h"
#include "plugin.h"
#include "rendersystem.h"
#include "passesrenderingaspectimpl.h"
#include "runnersystem.h"
#include "hub.h"
#include "updatequeuetask.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::RenderGraph;
using namespace DrawSpace::Interface::AspectImplementations;
using namespace DrawSpace::Systems;

RenderPassNodeGraph::RenderPassNodeGraph()
{
}

RenderPassNodeGraph::~RenderPassNodeGraph( void )
{
    cleanup_treenodes();
}

void RenderPassNodeGraph::cleanup_treenodes( void )
{
    for( auto it = m_tree.df_post_begin(); it != m_tree.df_post_end(); ++it ) 
    {
        it->data()->CleanUp();
        _DRAWSPACE_DELETE_( it->data() );
    }
}

void RenderPassNodeGraph::SetSystemsHub(Systems::Hub* p_hub)
{
    m_hub = p_hub;
}

RenderPassNode RenderPassNodeGraph::CreateRoot( const dsstring& p_name )
{
    // Creer PassDescr dont la cible rendu est l'ecran (final pass)

    Core::RenderingQueue* renderingqueue = _DRAWSPACE_NEW_( Core::RenderingQueue, Core::RenderingQueue(p_name) );


    RenderPassNode::PassDescr* descr = _DRAWSPACE_NEW_( RenderPassNode::PassDescr, RenderPassNode::PassDescr( p_name ) );
    descr->m_renderingqueue = renderingqueue;

    m_tree.insert( descr );

    RenderPassNode node( &m_tree.root() );
    return node;
}

// nettoie et efface toute la hierarchie
void RenderPassNodeGraph::Erase( void )
{
	cleanup_treenodes();
    m_tree.root().erase();
}

void RenderPassNodeGraph::Accept( RenderingAspectImpl* p_renderingaspectimpl )
{
    for( auto it = m_tree.df_post_begin(); it != m_tree.df_post_end(); ++it )
    {
        auto& pass_descr { it->data() };

        if (pass_descr->m_enabled)
        {
            const auto pass_id{ pass_descr->m_name };

            for (auto& e : m_evt_handlers)
            {
                (*e)(RenderPassEvent::RENDERINGQUEUE_PASS_BEGIN, pass_id);
            }

            bool updated_queue = p_renderingaspectimpl->VisitRenderPassDescr(pass_descr->m_name, pass_descr->m_renderingqueue);

            for (auto& e : m_evt_handlers)
            {
                (*e)(RenderPassEvent::RENDERINGQUEUE_PASS_END, pass_id);
            }

            pass_descr->m_renderingqueue_update_flag = pass_descr->m_renderingqueue_update_flag | updated_queue;
        }        
    }
}

void RenderPassNodeGraph::PushSignal_UpdatedRenderingQueues( void )
{
    m_signals.push( SIGNAL_UPDATED_RENDERINGQUEUES );
}

void RenderPassNodeGraph::PushSignal_DisablePass(const dsstring& p_pass)
{
    m_pass_to_disable = p_pass;
    m_signals.push(SIGNAL_DISABLE_PASS);
}

void RenderPassNodeGraph::PushSignal_EnablePass(const dsstring& p_pass)
{
    m_pass_to_enable = p_pass;
    m_signals.push(SIGNAL_ENABLE_PASS);    
}

void RenderPassNodeGraph::PushSignal_CleanupRenderingQueues(void)
{
    m_signals.push(SIGNAL_CLEANUP_RENDERINGQUEUES);
}

void RenderPassNodeGraph::ProcessSignals( void )
{   
    while( !m_signals.empty() )
    {
        Signals sig = m_signals.front();

        if (SIGNAL_CLEANUP_RENDERINGQUEUES == sig)
        {
            for (auto it = m_tree.df_post_begin(); it != m_tree.df_post_end(); ++it)
            {
                RenderPassNode::PassDescr* passdescr{ it->data() };
                passdescr->m_renderingqueue->EraseOutputQueue();
            }
        }
        else if( SIGNAL_UPDATED_RENDERINGQUEUES == sig )
        {
            if (!m_hub)
            {
                _DSEXCEPTION("systems hub has not been set in rendergraph ! ");
            }

            RunnerSystem& runner_system{ m_hub->GetSystem<RunnerSystem>("RunnerSystem") };

            RunnerSequenceStep update_queue_step;

            dsstring sequence_id{ std::to_string(GetTickCount()) + dsstring("/") };
            std::vector<RenderPassNode::PassDescr*> passes_descr;


            for (auto it = m_tree.df_post_begin(); it != m_tree.df_post_end(); ++it)
            {
                if (it->data()->m_renderingqueue_update_flag)
                {
                    RenderPassNode::PassDescr* passdescr{ it->data() };
                    sequence_id = sequence_id + passdescr->m_name + dsstring("/");

                    passes_descr.push_back(passdescr);
                }
            }

            update_queue_step.AddComponent<std::vector<RenderPassNode::PassDescr*>>("passes_descr", passes_descr);
            update_queue_step.AddComponent<dsstring>("sequence_id", sequence_id);

            update_queue_step.SetRunHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
            {
                UpdateQueueTask* task{ _DRAWSPACE_NEW_(UpdateQueueTask, UpdateQueueTask) };

                auto passes_descr{ p_step.GetComponent<std::vector<RenderPassNode::PassDescr*>>("passes_descr")->getPurpose() };
                auto sequence_id{ p_step.GetComponent<dsstring>("sequence_id")->getPurpose() };

                std::vector<DrawSpace::Core::RenderingQueue*> queues;
                for (auto& pass : passes_descr)
                {
                    queues.push_back(pass->m_renderingqueue);
                }

                task->SetTargetDescr(sequence_id);
                task->SetActionDescr("UPDATEQUEUES");
                task->SetRenderingQueue(queues);
                
                p_step.SetTask(task);
            });

            update_queue_step.SetStepCompletedHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
            {
                auto passes_descr{ p_step.GetComponent<std::vector<RenderPassNode::PassDescr*>>("passes_descr")->getPurpose() };

                UpdateQueueTask* task{ static_cast<UpdateQueueTask*>(p_step.GetTask()) };

                for (auto& pass : passes_descr)
                {
                    // flip output buffers
                    pass->m_renderingqueue->FlipOutputQueues();
                    pass->m_renderingqueue->DeclareReady();

                    // reset flag
                    pass->m_renderingqueue_update_flag = false;
                }

                _DRAWSPACE_DELETE_(task);
                p_seq.DeclareCompleted();

                p_step.RemoveComponent<std::vector<RenderPassNode::PassDescr*>>("passes_descr");
                p_step.RemoveComponent<dsstring>("sequence_id");
            });




            RunnerSequence sequence;
            sequence.RegisterStep(dsstring("updateQueueStep"), update_queue_step);
            sequence.SetCurrentStep(dsstring("updateQueueStep"));

            runner_system.RegisterSequence(sequence_id, sequence);

            for (auto& e : m_evt_handlers)
            {
                (*e)(RenderPassEvent::RENDERINGQUEUE_UPDATED, "");
            }
        }
        else if (SIGNAL_DISABLE_PASS == sig)
        {
            for (auto& it = m_tree.df_post_begin(); it != m_tree.df_post_end(); ++it)
            {
                auto& pass_descr { it->data() };
                if (pass_descr->m_name == m_pass_to_disable)
                {
                    pass_descr->m_enabled = false;
                }
            }
        }
        else if (SIGNAL_ENABLE_PASS == sig)
        {
            for (auto& it = m_tree.df_post_begin(); it != m_tree.df_post_end(); ++it)
            {
                auto& pass_descr{ it->data() };
                if (pass_descr->m_name == m_pass_to_disable)
                {
                    pass_descr->m_enabled = true;
                }
            }
        }

        m_signals.pop();
    }
}

void RenderPassNodeGraph::RegisterRenderPassEvtHandler(RenderPassEventHandler* p_handler)
{
	m_evt_handlers.insert(p_handler);
}

void RenderPassNodeGraph::UnregisterRenderPassEvtHandler(RenderPassEventHandler* p_handler)
{
	m_evt_handlers.erase(p_handler);
}
