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

#pragma once
#include "matrix.h"
#include "callback.h"

namespace DrawSpace
{
// fwd decl
namespace Interface
{
class Renderer;
}
namespace Core
{
class RenderingNode;


struct PassSlot
{
private:
    using RenderingNodeDrawCallback = DrawSpace::Core::CallBack<PassSlot, void, DrawSpace::Core::RenderingNode*>;

    dsstring                                m_pass_name;
    DrawSpace::Core::RenderingNode*         m_rendering_node     = { nullptr };
    RenderingNodeDrawCallback*              m_cb                 = { nullptr };
    DrawSpace::Interface::Renderer*         m_renderer           = { nullptr };

    virtual void on_renderingnode_draw(DrawSpace::Core::RenderingNode* p_rendering_node);

public:

    DrawSpace::Utils::Matrix                           m_world;
    DrawSpace::Utils::Matrix                           m_view;
    DrawSpace::Utils::Matrix                           m_proj;

    PassSlot(const dsstring& p_pass_name);
    ~PassSlot(void);

    inline DrawSpace::Core::RenderingNode* GetRenderingNode(void) const { return m_rendering_node; };
    inline dsstring                         GetPassName(void) const { return m_pass_name; };
};
}
}

