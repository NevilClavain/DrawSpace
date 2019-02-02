/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2019
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

#include "lod_patch.h"

namespace LOD
{
struct Config;
class Body;

class Layer
{
public:

    using SubPassCreationHandler = DrawSpace::Core::BaseCallback2<SubPass::EntryInfos, SubPass*, int>;

protected:

    Config*                            m_config;
    Body*                              m_body;
    Layer::SubPassCreationHandler*     m_handler;

public:
    Layer( Config* p_config, Body* p_body, Layer::SubPassCreationHandler* p_handler, int p_index );

    Body* GetBody(void) const;
    Layer::SubPassCreationHandler* GetSubPassCreationHandler(void) const;

};
}