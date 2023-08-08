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

#include "lod_heightmapsubpass.h"
#include "lod_config.h"
#include "lod_drawing.h"
#include "csts.h"

#include "renderer.h"
#include "plugin.h"

#include "exceptions.h"

using namespace DrawSpace;
using namespace LOD;
using namespace DrawSpace::Core;


HeighmapSubPass::HeighmapSubPass(SubPassCreationHandler* p_subpasscreation_handler, LOD::Config* p_config, int p_orientation, int p_node_layer_index, Purpose p_purpose) :
m_purpose(p_purpose)
{
	m_heightmap_pass = create_heightmap_pass();

	// creation/preparation du node

	auto renderer{ SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface };
	const auto node{ _DRAWSPACE_NEW_(FaceDrawingNode, FaceDrawingNode(renderer, p_config, p_node_layer_index)) };

	node->SetMeshe(LOD::Body::m_patch_meshe);
	node->SetBinder(p_config->m_layers_descr[p_node_layer_index].heightmapGenerationBinder[p_orientation]);

	void* tx_data;
	if (false == renderer->CreateTexture(m_heightmap_pass->GetTargetTexture(), &tx_data))
	{
		_DSEXCEPTION("failed to create subpasstarget texture in renderer");
	}

	m_heightmap_pass->GetTargetTexture()->AllocTextureContent();

	
	m_subpass_node_list.push_back(node);
	m_subpass_list.push_back(m_heightmap_pass);

	const auto handler{ p_subpasscreation_handler };
	if (handler)
	{
		(*handler)(this, LOD::SubPass::Destination::IMMEDIATE_SINGLE_SUBPASS);

		m_heightmap_texture = m_heightmap_pass->GetTargetTexture();
		m_heightmap_content = m_heightmap_texture->GetTextureContentPtr();
	}	
}

HeighmapSubPass::~HeighmapSubPass(void)
{
	auto renderer{ SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface };

	for (const auto s : m_subpass_list)
	{
		renderer->DestroyTexture(s->GetTargetTexture()->GetRenderData());
	}

	for (auto n : m_subpass_node_list)
	{
		_DRAWSPACE_DELETE_(n);
	}

	for (auto s : m_subpass_list)
	{
		_DRAWSPACE_DELETE_(s);
	}
}

void HeighmapSubPass::RegisterSubpassDoneHandler(SubPassDoneHandler* p_handler)
{
	m_subpassdone_handler = p_handler;
}

void HeighmapSubPass::RegisterSubpassAbortedHandler(SubPassDoneHandler* p_handler)
{
	m_subpassaborted_handler = p_handler;
}

void HeighmapSubPass::DrawSubPass(void)
{
	SubPass::DrawSubPass();
}

void HeighmapSubPass::SubPassDone(void)
{
	if (m_subpassdone_handler)
	{
		(*m_subpassdone_handler)(this);
	}
}

void HeighmapSubPass::SubPassAborted(void)
{	
	if (m_subpassaborted_handler)
	{
		(*m_subpassaborted_handler)(this);
	}	
}

DrawSpace::Core::Texture* HeighmapSubPass::GetHMTexture(void) const
{
	return m_heightmap_texture;
}

void* HeighmapSubPass::GetHMTextureContent(void) const
{
	return m_heightmap_content;
}

DrawSpace::IntermediatePass* HeighmapSubPass::create_heightmap_pass(void)
{
	const auto thisname{ dsstring("layer_") + std::to_string((int)this) };
	const auto complete_name{ thisname + dsstring("_heightmap_pass") };

	const auto ipass{ _DRAWSPACE_NEW_(IntermediatePass, IntermediatePass(complete_name)) };

	ipass->SetTargetDimsFromRenderer(false);
	ipass->SetTargetDims(heightmapTextureSize, heightmapTextureSize);
	ipass->SetRenderPurpose(Texture::RENDERPURPOSE_FLOAT32);
	ipass->SetRenderTarget(Texture::RENDERTARGET_CPU);

	ipass->Initialize();
	ipass->GetRenderingQueue()->EnableDepthClearing(true);

	return ipass;
}


