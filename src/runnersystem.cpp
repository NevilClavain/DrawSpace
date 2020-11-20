/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2020
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

#include "runnersystem.h"
#include "runner.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Systems;
using namespace DrawSpace::Threading;

std::set<std::pair<dsstring, dsstring>> RunnerSystem::m_finished_tasks;

void RunnerSequenceStep::run(RunnerSequence& p_sequence)
{
    if (State::RUNNING == m_state)
    {
        m_run_handler(*this, p_sequence);

        if (m_task)
        {
            m_state = State::WAITFORTASK;

            Threading::Runner* runner{ Threading::Runner::GetInstance() };
            runner->m_mailbox_in.Push<Interface::ITask*>(m_task);
        }
        else
        {
            m_state = State::COMPLETED;
            m_stepcompleted_handler(*this, p_sequence);
        }
    }
    else if (State::WAITFORTASK == m_state)
    {
        for (auto& e : RunnerSystem::m_finished_tasks)
        {
            if (m_task->GetActionDescr() == e.first && m_task->GetTargetDescr() == e.second)
            {
                m_state = State::COMPLETED;
                m_stepcompleted_handler(*this, p_sequence);

                RunnerSystem::m_finished_tasks.erase(std::make_pair(e.first, e.second));

                break;
            }
        }
    }
}

RunnerSequenceStep::State RunnerSequenceStep::GetState(void) const
{
    return m_state;
}

void RunnerSequenceStep::SetRunHandler(const std::function<void(RunnerSequenceStep&, RunnerSequence&)>& p_run_handler)
{
    m_run_handler = p_run_handler;
}

void RunnerSequenceStep::SetStepCompletedHandler(const std::function<void(RunnerSequenceStep&, RunnerSequence&)>& p_stepcompleted_handler)
{
    m_stepcompleted_handler = p_stepcompleted_handler;
}

void RunnerSequenceStep::SetTask(Interface::ITask* p_task)
{
    m_task = p_task;
}

Interface::ITask* RunnerSequenceStep::GetTask(void) const
{
    return m_task;
}



void RunnerSequence::run(void)
{
    if (State::RUNNING == m_state)
    {
        if (m_steps.count(m_current_step))
        {
            m_steps.at(m_current_step).run(*this);
        }
        else
        {
            _DSEXCEPTION("Unknown step id : " + m_current_step);
        }      
    }
}

void RunnerSequence::SetCurrentStep(const dsstring& p_stepid)
{
    m_current_step = p_stepid;
}

void RunnerSequence::RegisterStep(const dsstring& p_id, RunnerSequenceStep& p_step)
{    
    p_step.m_name = p_id;
    m_steps[p_id] = p_step;    
}

void RunnerSequence::DeclareCompleted(void)
{
    m_state = State::COMPLETED;
}

bool RunnerSequence::IsCompleted(void)
{
    return (m_state == State::COMPLETED);
}

RunnerSequenceStep& RunnerSequence::GetStep(const dsstring& p_stepid)
{
    if (m_steps.count(p_stepid))
    {
        return m_steps.at(p_stepid);
    }
    else
    {
        _DSEXCEPTION("Unknown step id : " + p_stepid);
    }
}



RunnerSystem::RunnerSystem(void)
{

}

RunnerSystem::~RunnerSystem(void)
{

}

void RunnerSystem::run(EntityGraph::EntityNodeGraph* p_entitygraph)
{
    for (auto& e : m_sequences)
    {
        e.second.run();
    }

    Threading::Runner* runner{ Threading::Runner::GetInstance() };
    int nb_tasks_done{ runner->m_mailbox_out.GetBoxSize() };

    for (int i = 0; i < nb_tasks_done; ++i)
    {
        auto task_descr{ runner->m_mailbox_out.PopNext<std::pair<std::string, std::string>>(std::make_pair<dsstring, dsstring>("","")) };
        m_finished_tasks.insert(std::make_pair(task_descr.first, task_descr.second));
    }
}

void RunnerSystem::StartupRunner(void)
{
    //startup runner thread
    Runner::GetInstance()->Startup();
}

void RunnerSystem::ShutdownRunner(void)
{
    // stop and join runner

    Runner* runner{ Runner::GetInstance() };

    runner->m_mailbox_in.Push<Interface::ITask*>(&RunnerKiller());
    runner->Join();
}

void RunnerSystem::RegisterSequence(const dsstring& p_sequenceid, RunnerSequence& p_sequence)
{
    m_sequences[p_sequenceid] = p_sequence;
    m_sequences[p_sequenceid].m_name = p_sequenceid;
}

void RunnerSystem::RemoveSequence(const dsstring& p_sequenceid)
{
    if (m_sequences.count(p_sequenceid))
    {
        m_sequences.erase(p_sequenceid);
    }
}

bool RunnerSystem::HasSequence(const dsstring& p_sequenceid)
{
    return (m_sequences.find(p_sequenceid) != m_sequences.end());
}

bool RunnerSystem::IsSequenceCompleted(const dsstring& p_sequenceid)
{
    if (m_sequences.count(p_sequenceid))
    {
        return m_sequences.at(p_sequenceid).IsCompleted();
    }
    else
    {
        _DSEXCEPTION("Unknown sequence id : " + p_sequenceid);
    }
}