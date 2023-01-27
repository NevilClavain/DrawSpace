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

#include "passslot.h"
#include "memalloc.h"
#include "renderer.h"
#include "plugin.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

PassSlot::PassSlot(const dsstring& p_pass_name, PrimitiveType p_primitivetype) :
    m_primitivetype(p_primitivetype),
    m_pass_name(p_pass_name)
{
    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    m_rendering_node = _DRAWSPACE_NEW_(RenderingNode, RenderingNode);

    m_cb = _DRAWSPACE_NEW_(RenderingNodeDrawCallback, RenderingNodeDrawCallback(this, &PassSlot::on_renderingnode_draw));
    m_rendering_node->RegisterHandler(m_cb);

    m_world.Identity();

    m_view.Identity();
    m_proj.Identity();
}

PassSlot::~PassSlot(void)
{
    _DRAWSPACE_DELETE_(m_rendering_node);
    _DRAWSPACE_DELETE_(m_cb);
}

void PassSlot::on_renderingnode_draw(RenderingNode* p_rendering_node)
{
    if (PrimitiveType::TRIANGLE == m_primitivetype)
    {
        m_renderer->DrawMeshe(m_world, m_view, m_proj);
    }
    else
    {
        m_renderer->EnableLinesPrimitives();
        m_renderer->DrawLineMeshe(m_world, m_view, m_proj);
        m_renderer->EnableTrianglesPrimitives();;
    }    
}
