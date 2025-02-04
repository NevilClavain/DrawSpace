
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

#include <md5.h>
#include "trianglemeshe.h"


using namespace mage;
using namespace mage::core::maths;

TriangleMeshe::TriangleMeshe(const TriangleMeshe& p_other)
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
}


std::vector<mage::Vertex>TriangleMeshe::getVertices(void) const
{
	return m_vertices;
}

size_t TriangleMeshe::getVerticesListSize() const
{
	return m_vertices.size();
}


std::vector<TrianglePrimitive<unsigned int>> TriangleMeshe::getTriangles(void) const
{
	return m_triangles;
}

size_t TriangleMeshe::getTrianglesListSize() const
{
	return m_triangles.size();
}


void TriangleMeshe::setNGenerationMode(NormalesGenerationMode p_mode)
{
	m_n_gen_mode = p_mode;
}

void TriangleMeshe::setTBGenerationMode(TangentBinormalesGenerationMode p_mode)
{
	m_tb_gen_mode = p_mode;
}

TriangleMeshe::NormalesGenerationMode TriangleMeshe::getNGenerationMode(void) const
{
	return m_n_gen_mode;
}

TriangleMeshe::TangentBinormalesGenerationMode TriangleMeshe::getTBGenerationMode(void) const
{
	return m_tb_gen_mode;
}

core::maths::Matrix	TriangleMeshe::getNormalesTransf(void) const
{
	return m_normales_transformation;
}

void TriangleMeshe::setNormalesTransf(const core::maths::Matrix& p_transf)
{
	m_normales_transformation = p_transf;
}

void TriangleMeshe::clearVertices(void)
{
	m_vertices.clear();
}

void TriangleMeshe::clearTriangles(void)
{
	m_triangles.clear();
	m_triangles_for_vertex.clear();
}

void TriangleMeshe::clearAnimationBones(void)
{
	m_animation_bones.clear();
	m_animation_bones_names_mapping.clear();
}

void TriangleMeshe::push(const Vertex& p_vertex)
{
	m_vertices.push_back(p_vertex);
}

void TriangleMeshe::update(unsigned int p_index, const Vertex& p_vertex)
{
	m_vertices.at(p_index) = p_vertex;
}

Vertex TriangleMeshe::getVertex(unsigned int p_index)
{
	return m_vertices.at(p_index);

}

void TriangleMeshe::push(const TrianglePrimitive<unsigned int>& p_triangle)
{
	m_triangles.push_back(p_triangle);

	m_triangles_for_vertex[p_triangle[0]].push_back(p_triangle);
	m_triangles_for_vertex[p_triangle[1]].push_back(p_triangle);
	m_triangles_for_vertex[p_triangle[2]].push_back(p_triangle);
}

void TriangleMeshe::push(const AnimationBone& p_bone, const std::string& p_boneId)
{
	int last_object_index = m_animation_bones.size();
	m_animation_bones.push_back(p_bone);
	m_animation_bones_names_mapping[p_boneId] = last_object_index;
}

void TriangleMeshe::push(AnimationKeys p_animation_keys)
{
	m_animations_keys.emplace(p_animation_keys.name, p_animation_keys);
}

void TriangleMeshe::computeNormales()
{
	for (auto it = m_triangles_for_vertex.begin(); it != m_triangles_for_vertex.end(); ++it)
	{
		Real4Vector normales_sum;
		const auto triangles_list{ it->second };

		for (size_t i = 0; i < triangles_list.size(); i++)
		{
			const auto triangle{ triangles_list.at(i) };
			const Vertex v1 { m_vertices.at(triangle[0]) };
			const Vertex v2 { m_vertices.at(triangle[1]) };
			const Vertex v3 { m_vertices.at(triangle[2]) };

			const Vector d1(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z, 1.0);
			const Vector d2(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z, 1.0);

			Vector res { Real4Vector::crossProduct(d1, d2) };
			res.normalize();

			normales_sum = normales_sum + res;
		}
		normales_sum.scale(1.0 / triangles_list.size());
		normales_sum.normalize();

		m_vertices[it->first].nx = normales_sum[0];
		m_vertices[it->first].ny = normales_sum[1];
		m_vertices[it->first].nz = normales_sum[2];
	}
}

void TriangleMeshe::compute_TBN(const Vertex& p_v1, const Vertex& p_v2, const Vertex& p_v3, int p_stage,
	core::maths::Real4Vector& p_T, core::maths::Real4Vector& p_B, core::maths::Real4Vector& p_N)
{
	const Real4Vector v2v1(p_v2.x - p_v1.x, p_v2.y - p_v1.y, p_v2.z - p_v1.z, 1.0);
	const Real4Vector v3v1(p_v3.x - p_v1.x, p_v3.y - p_v1.y, p_v3.z - p_v1.z, 1.0);

	const double c2c1t { p_v2.tu[p_stage] - p_v1.tu[p_stage] };
	const double c2c1b { p_v2.tv[p_stage] - p_v1.tv[p_stage] };

	const double c3c1t { p_v3.tu[p_stage] - p_v1.tu[p_stage] };
	const double c3c1b { p_v3.tv[p_stage] - p_v1.tv[p_stage] };

	const double det { (c2c1t * c3c1b) - (c3c1t * c2c1b) };

	Vector t, b, n;

	t[0] = ((c3c1b * v2v1[0]) - (c2c1b * v3v1[0])) / det;
	t[1] = ((c3c1b * v2v1[1]) - (c2c1b * v3v1[1])) / det;
	t[2] = ((c3c1b * v2v1[2]) - (c2c1b * v3v1[2])) / det;
	t[3] = 1.0;

	b[0] = ((-c3c1t * v2v1[0]) + (c2c1t * v3v1[0])) / det;
	b[1] = ((-c3c1t * v2v1[1]) + (c2c1t * v3v1[1])) / det;
	b[2] = ((-c3c1t * v2v1[2]) + (c2c1t * v3v1[2])) / det;
	b[3] = 1.0;

	n = Real4Vector::crossProduct(b, t);

	t.normalize();
	b.normalize();
	n.normalize();

	p_T = t;
	p_B = b;
	p_N = n;
}

void TriangleMeshe::computeTB()
{
	for (auto it = m_triangles_for_vertex.begin(); it != m_triangles_for_vertex.end(); ++it)
	{
		Real4Vector tangents_sum;
		Real4Vector binormales_sum;
		Real4Vector normales_sum;

		const auto triangles_list{ it->second };

		for (size_t i = 0; i < triangles_list.size(); i++)
		{
			const auto triangle{ triangles_list.at(i) };
			const Vertex v1 { m_vertices.at(triangle[0]) };
			const Vertex v2 { m_vertices.at(triangle[1]) };
			const Vertex v3 { m_vertices.at(triangle[2]) };

			Real4Vector t, b, n;
			compute_TBN(v1, v2, v3, 0, t, b, n);

			normales_sum = normales_sum + n;
			binormales_sum = binormales_sum + b;
			tangents_sum = tangents_sum + t;
		}

		normales_sum.scale(1.0 / triangles_list.size());
		normales_sum.normalize();

		binormales_sum.scale(1.0 / triangles_list.size());
		binormales_sum.normalize();

		tangents_sum.scale(1.0 / triangles_list.size());
		tangents_sum.normalize();

		m_vertices[it->first].bx = binormales_sum[0];
		m_vertices[it->first].by = binormales_sum[1];
		m_vertices[it->first].bz = binormales_sum[2];

		m_vertices[it->first].tx = tangents_sum[0];
		m_vertices[it->first].ty = tangents_sum[1];
		m_vertices[it->first].tz = tangents_sum[2];
	}
}


TriangleMeshe::State TriangleMeshe::getState() const
{
	m_state_mutex.lock();
	const auto state{ m_state };
	m_state_mutex.unlock();
	return state;
}

void TriangleMeshe::setState(TriangleMeshe::State p_state)
{
	m_state_mutex.lock();
	m_state = p_state;
	m_state_mutex.unlock();
}

void TriangleMeshe::computeResourceUID()
{
	MD5 md5;

	const auto vbuff{ new Vertex[m_vertices.size()] };
	auto curr{ vbuff };
	for (size_t i = 0; i < m_vertices.size(); i++)
	{
		*curr = m_vertices[i];
		curr++;
	}
	const std::string hash_v{ md5.digestMemory((BYTE*)vbuff, (int)(m_vertices.size() * sizeof(Vertex))) };

	auto tbuff{ new TrianglePrimitive<unsigned int>[m_triangles.size()] };
	TrianglePrimitive<unsigned int>* curr2{ tbuff };

	for (size_t i = 0; i < m_triangles.size(); i++)
	{
		const TrianglePrimitive<unsigned int> triangle{ m_triangles.at(i) };
		*curr2 = triangle;
		curr2++;
	}
	const std::string hash_t{ md5.digestMemory((BYTE*)tbuff, (int)(m_triangles.size() * sizeof(TrianglePrimitive<unsigned int>))) };

	delete[] vbuff;
	delete[] tbuff;

	const std::string hash_n_gen{ md5.digestMemory((BYTE*)&m_n_gen_mode, (int)(sizeof(m_n_gen_mode))) };
	const std::string hash_tb_gen{ md5.digestMemory((BYTE*)&m_tb_gen_mode, (int)(sizeof(m_tb_gen_mode))) };

	std::string hash_bones;
	for (int i = 0; i < m_animation_bones.size(); i++)
	{
		const double* content_array{ m_animation_bones.at(i).offset_matrix.getArray() };
		const std::string hash_bone{ md5.digestMemory((BYTE*)&content_array, (int)(16 * sizeof(double))) };

		hash_bones += hash_bone;
	}

	std::string hash{ hash_v + hash_t + hash_n_gen + hash_tb_gen + hash_bones };

	m_resource_uid = hash;
}

std::string TriangleMeshe::getResourceUID() const
{
	return m_resource_uid;
}

std::string TriangleMeshe::getSourceID() const
{
	return m_source_id;
}

void TriangleMeshe::setSourceID(const std::string& p_source_id)
{
	m_source_id = p_source_id;
}

void TriangleMeshe::setSource(TriangleMeshe::Source p_source)
{
	m_source = p_source;
}

void TriangleMeshe::setSceneNodes(const std::map<std::string, SceneNode>& p_scene_nodes, const std::string& p_scene_root_node_id)
{
	m_scene_nodes = p_scene_nodes;
	m_scene_root_node_id = p_scene_root_node_id;
}

std::string	TriangleMeshe::getSceneRootNodeId() const
{
	return m_scene_root_node_id;
}

const std::map<std::string, SceneNode>& TriangleMeshe::getSceneNodes() const
{
	return m_scene_nodes;
}

std::map<std::string, SceneNode>& TriangleMeshe::sceneNodesAccess()
{
	return m_scene_nodes;
}

std::vector<AnimationBone>& TriangleMeshe::animationBonesAccess()
{
	return m_animation_bones;
}

const std::unordered_map<std::string, int>& TriangleMeshe::getAnimationBonesNamesMapping() const
{
	return m_animation_bones_names_mapping;
}

const std::unordered_map<std::string, AnimationKeys>& TriangleMeshe::getAnimationsKeys() const
{
	return m_animations_keys;
}

/*
AnimationKeys& TriangleMeshe::transitionAnimationAccess()
{
	return m_transition_animation;
}
*/

std::string	TriangleMeshe::getPreviousAnimation() const
{
	return m_previous_animation;
}

void TriangleMeshe::setPreviousAnimation(const std::string& p_previous_animation)
{
	m_previous_animation = p_previous_animation;
}