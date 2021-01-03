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

#pragma once

#include "systems.h"
#include "entitynodegraph.h"
#include "animationsaspect.h"
#include "renderingaspect.h"
#include "timeaspect.h"

namespace DrawSpace
{
namespace Systems
{
class AnimationsSystem : public Interface::System
{
public:
	enum AnimationEvent
	{
		ANIMATION_BEGIN,
		ANIMATION_END
	};

	using AnimationEventHandler = DrawSpace::Core::BaseCallback3<void, const dsstring&, AnimationEvent, const dsstring&>;

	static const int		bonesBuffer0StartReg					= 31;
	static const int		bonesBuffer0Length						= 69;

	static const int		bonesBuffer1StartReg					= 124;
	static const int		bonesBuffer1Length						= 132;

	static const dsstring	bonesBuffer0Id;
	static const dsstring	bonesBuffer1Id;

	static const dsreal     transitionAnimationDurationSeconds;
	static const dsstring	transitionAnimationDurationId;

protected:

	std::set<AnimationEventHandler*>														m_evt_handlers;


    void run(EntityGraph::EntityNodeGraph* p_entitygraph);

	//recursive
	static void read_bones_hierarchy(const std::map<dsstring, DrawSpace::Aspect::AnimationsAspect::Node>& p_nodes,
		std::vector<DrawSpace::Aspect::AnimationsAspect::BoneOutput>& p_bones_output,
		const std::map<dsstring, int>& p_bones_mapping, DrawSpace::Aspect::AnimationsAspect::Node p_node,
		const DrawSpace::Utils::Matrix& p_parent_transform);


	void run_animations_pool(DrawSpace::Aspect::AnimationsAspect::AnimationsPool& p_animations_pool, DrawSpace::Aspect::AnimationsAspect* p_anims_aspect,
								DrawSpace::Aspect::TimeAspect* p_time_aspect, std::map<dsstring, DrawSpace::Aspect::AnimationsAspect::Node>& p_nodes);

	// heart of animation execution
	bool animation_step(const dsstring& p_animation_id, const DrawSpace::Aspect::AnimationsAspect::AnimationRoot& p_animation,
						DrawSpace::Aspect::AnimationsAspect* p_anims_aspect, std::map<dsstring, DrawSpace::Aspect::AnimationsAspect::Node>& p_nodes);

	void compute_node_animationresult_matrix(const DrawSpace::Aspect::AnimationsAspect::NodeAnimation& p_node, dsreal p_current_tick, Utils::Matrix& p_out_matrix) const;


	static void apply_animation_last_key(DrawSpace::Aspect::AnimationsAspect* p_anims_aspect);
	static void apply_forced_bones_pos(DrawSpace::Aspect::AnimationsAspect* p_anims_aspect);
	static void send_bones_to_shaders(DrawSpace::Aspect::AnimationsAspect* p_anims_aspect, DrawSpace::Aspect::RenderingAspect* p_rendering_aspect);
	void insert_transition_animation(DrawSpace::Aspect::AnimationsAspect* p_anims_aspect);

public:
    AnimationsSystem();
    ~AnimationsSystem();

    dsstring GetSystemId(void) const { return "AnimationsSystem"; };
    void VisitEntity(Core::Entity* p_parent, Core::Entity* p_entity);

	void RegisterAnimationEvtHandler(AnimationEventHandler* p_handler);
	void UnregisterAnimationEvtHandler(AnimationEventHandler* p_handler);


};
}
}
