/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

#include "componentcontainer.h"
#include "tracedefs.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

int ComponentContainer::m_uid_count;

DrawSpace::Logger::Sink aspect_logger("ComponentContainer", DrawSpace::Logger::Configuration::GetInstance());

void ComponentContainer::logComponent(const dsstring& p_comment, const dsstring& p_id, BaseComponent* p_instance) const
{
    _DSDEBUG(aspect_logger, p_comment.c_str() << dsstring(" id :") << p_id.c_str() << dsstring( " " ) << p_instance << dsstring( " " ) << p_instance->m_uid)
}
