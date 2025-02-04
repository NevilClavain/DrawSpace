
/* -*-LIC_BEGIN-*- */
/*
*
* MaGE rendering framework
* Emmanuel Chaumont Copyright (c) 2013-2025
*
* This file is part of MaGE.
*
*    MaGE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    MaGE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with MaGE.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>

#include "primitives.h"
#include "animationbone.h"
#include "scenenode.h"
#include "animations.h"

namespace mage
{
	//fwd decl
	class ResourceSystem;

	class TriangleMeshe
	{
	public:

		enum class Source
		{
			CONTENT_FROM_FILE,
			CONTENT_DYNAMIC_INIT
		};

		enum class State
		{
			INIT,
			BLOBLOADING,
			BLOBLOADED,
			RENDERERLOADING,
			RENDERERLOADED,
		};

		enum class NormalesGenerationMode
		{
			NORMALES_DISCARDED,
			NORMALES_AUTO,
			NORMALES_AUTO_SMOOTH,
			NORMALES_FROMFILE,
			NORMALES_FROMFILE_SMOOTH,
			NORMALES_COMPUTED
		};

		enum class TangentBinormalesGenerationMode
		{
			TB_DISCARDED,
			TB_AUTO,
			TB_FROMFILE,
			TB_COMPUTED
		};

		TriangleMeshe() = default;		
		TriangleMeshe(const TriangleMeshe& p_other);

		TriangleMeshe& operator=(const TriangleMeshe& p_other)
		{
			m_source = p_other.m_source;
			m_source_id = p_other.m_source_id;
			m_resource_uid = p_other.m_resource_uid;

			m_vertices = p_other.m_vertices;
			m_triangles = p_other.m_triangles;
			m_triangles_for_vertex = p_other.m_triangles_for_vertex;

			m_normales_transformation = p_other.m_normales_transformation;
			m_n_gen_mode = p_other.m_n_gen_mode;
			m_tb_gen_mode = p_other.m_tb_gen_mode;

			m_animation_bones = p_other.m_animation_bones;
			m_animation_bones_names_mapping = p_other.m_animation_bones_names_mapping;

			m_scene_nodes = p_other.m_scene_nodes;
			m_scene_root_node_id = p_other.m_scene_root_node_id;

			m_animations_keys = p_other.m_animations_keys;

			m_state_mutex.lock();
			p_other.m_state_mutex.lock();
			m_state = p_other.m_state;
			p_other.m_state_mutex.unlock();
			m_state_mutex.unlock();

			return *this;
		}

		~TriangleMeshe() = default;

		

		std::vector<mage::Vertex>							getVertices(void) const;
		size_t													getVerticesListSize() const;
		
		std::vector<TrianglePrimitive<unsigned int>>			getTriangles(void) const;
		size_t													getTrianglesListSize() const;

		void													setNGenerationMode(NormalesGenerationMode p_mode);
		void													setTBGenerationMode(TangentBinormalesGenerationMode p_mode);

		NormalesGenerationMode									getNGenerationMode(void) const;
		TangentBinormalesGenerationMode							getTBGenerationMode(void) const;

		core::maths::Matrix										getNormalesTransf(void) const;
		void													setNormalesTransf(const core::maths::Matrix& p_transf);


		void													clearVertices(void);
		void													clearTriangles(void);
		void													clearAnimationBones(void);

		void													push(const TrianglePrimitive<unsigned int>& p_triangle);
		void													push(const Vertex& p_vertex);
		void													push(const AnimationBone& p_bone, const std::string& p_boneId);
		void													push(AnimationKeys p_animation_keys);

		void													setSceneNodes(const std::map<std::string, SceneNode>& p_scene_nodes, const std::string& p_scene_root_node_id);

		Vertex													getVertex(unsigned int p_index);
		void													update(unsigned int p_index, const Vertex& p_vertex);

		void													computeNormales();
		void													computeTB();

		State													getState() const;
		void													setState(State p_state);

		std::string												getResourceUID() const;

		std::string												getSourceID() const;

		void													setSourceID(const std::string& p_source_id);
		void													setSource(Source p_source);
		
		void													computeResourceUID();

		std::vector<AnimationBone>&								animationBonesAccess();
		const std::unordered_map<std::string, int>&				getAnimationBonesNamesMapping() const;

		std::string												getSceneRootNodeId() const;
		const std::map<std::string, SceneNode>&					getSceneNodes() const;
		std::map<std::string, SceneNode>&						sceneNodesAccess();


		const std::unordered_map<std::string, AnimationKeys>&	getAnimationsKeys() const;

		//AnimationKeys&											transitionAnimationAccess();

		std::string												getPreviousAnimation() const;
		void													setPreviousAnimation(const std::string& p_previous_animation);

	private:

		std::string																m_resource_uid;       // meshe content source unique identifier

		Source																	m_source{ Source::CONTENT_FROM_FILE };

		std::string																m_source_id;

		std::vector<Vertex>														m_vertices;
		std::vector<TrianglePrimitive<unsigned int>>							m_triangles;

		// list of triangles for each vertex
		std::unordered_map<long, std::vector<TrianglePrimitive<unsigned int>>>	m_triangles_for_vertex;

		NormalesGenerationMode													m_n_gen_mode{ NormalesGenerationMode::NORMALES_COMPUTED };
		TangentBinormalesGenerationMode											m_tb_gen_mode{ TangentBinormalesGenerationMode::TB_DISCARDED };

		core::maths::Matrix														m_normales_transformation;

		mutable std::mutex														m_state_mutex;
		State																	m_state{ State::INIT };

		///////// keep those animation structures as TriangleMeshe members because it also get protection benefits from the mutex when loaded in the resource system threads
		std::vector<AnimationBone>												m_animation_bones;
		std::unordered_map<std::string, int>									m_animation_bones_names_mapping;

		std::map<std::string, SceneNode>										m_scene_nodes;  // note : no need to include it in md5 hash computing
		std::string																m_scene_root_node_id;

		std::unordered_map<std::string, AnimationKeys>							m_animations_keys;

		//AnimationKeys															m_transition_animation;
		std::string																m_previous_animation;

		// IF NEW MEMBERS HERE :
		// UPDATE COPY CTOR AND OPERATOR !!!!!!

		void compute_TBN(const Vertex& p_v1, const Vertex& p_v2, const Vertex& p_v3, int p_stage,
							core::maths::Real4Vector& p_T, core::maths::Real4Vector& p_B, core::maths::Real4Vector& p_N);	

		friend class mage::ResourceSystem;

	};
}
