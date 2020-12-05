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

#pragma once

#include "systems.h"


namespace DrawSpace
{
namespace Interface
{
struct ITask;
}

namespace Systems
{

class RunnerSequence;

struct RunnerSequenceStep sealed : public Core::ComponentContainer
{
public:
    enum class State
    {        
        RUNNING,
        WAITFORTASK,
        COMPLETED,
    };

private:

    std::function<void(RunnerSequenceStep&, RunnerSequence&)>           m_run_handler;
    std::function<void(RunnerSequenceStep&, RunnerSequence&)>           m_stepcompleted_handler;

    State                                                               m_state{ State::RUNNING };
    Interface::ITask*                                                   m_task{ nullptr };
    dsstring                                                            m_name; // for debug purpose
   
    void run(RunnerSequence& p_sequence);

public:

    State GetState(void) const;
    void  SetRunHandler(const std::function<void(RunnerSequenceStep&, RunnerSequence&)>& p_run_handler);
    void  SetStepCompletedHandler(const std::function<void(RunnerSequenceStep&, RunnerSequence&)>& p_stepcompleted_handler);
    void  SetTask(Interface::ITask* p_task);
    Interface::ITask* RunnerSequenceStep::GetTask(void) const;

    friend class RunnerSequence;
};


class RunnerSequence : public Core::ComponentContainer
{
public:
    enum class State
    {
        RUNNING,
        COMPLETED,
    };

private:

    State                                   m_state{ State::RUNNING };
    std::map<dsstring, RunnerSequenceStep>  m_steps;
    dsstring                                m_current_step;
    dsstring                                m_name; // for debug purpose

    void run(void);

public:   
    void SetCurrentStep(const dsstring& p_stepid);
    
    void RegisterStep(const dsstring& p_id, RunnerSequenceStep& p_step);

    void DeclareCompleted(void);

    bool IsCompleted(void);

    RunnerSequenceStep& GetStep(const dsstring& p_stepid);

    friend class RunnerSystem;
};


class RunnerSystem : public Interface::System
{
public:
    static std::set<std::pair<dsstring, dsstring>>  m_finished_tasks;

protected:
    std::map<dsstring, RunnerSequence>              m_sequences;
    

    void run(EntityGraph::EntityNodeGraph* p_entitygraph);

public:
    RunnerSystem(void);
    ~RunnerSystem(void);

    dsstring GetSystemId(void) const { return "RunnerSystem"; };

    void RegisterSequence(const dsstring& p_sequenceid, RunnerSequence& p_sequence);
    void RemoveSequence(const dsstring& p_sequenceid);

    bool HasSequence(const dsstring& p_sequenceid);
    bool IsSequenceCompleted(const dsstring& p_sequenceid);

    void StartupRunner(void);
    void ShutdownRunner(void);

};

}
}