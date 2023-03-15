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
#include "lod_subpass.h"

namespace LOD
{
// fwd declaration
class Layer;
struct Config;

class HeighmapSubPass : public LOD::SubPass
{
private:

    bool                            m_enable{ true };
    Layer*                          m_layer{ nullptr };
    DrawSpace::Core::Texture*       m_heightmap_texture{ nullptr };
    void*                           m_heightmap_content{ nullptr };

    DrawSpace::IntermediatePass*    m_heightmap_pass{ nullptr };

    DrawSpace::IntermediatePass*    create_heightmap_pass(void);

public:

    static const int heightmapTextureSize = 1024;


    HeighmapSubPass(Layer* p_owner, LOD::Config* p_config, int p_orientation, int p_node_layer_index);
    virtual ~HeighmapSubPass(void);

    virtual void                        DrawSubPass(void);
    virtual void                        SubPassDone(void);
    virtual void                        Enable(void);
    virtual void                        Disable(void);
    virtual DrawSpace::Core::Texture*   GetHMTexture(void) const;
    virtual void*                       GetHMTextureContent(void) const;

};
}
