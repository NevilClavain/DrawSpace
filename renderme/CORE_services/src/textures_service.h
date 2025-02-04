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
#include "singleton.h"
#include "eventsource.h"

namespace mage
{
	namespace core
	{
		namespace services
		{
			class TextureContentCopyService : public property::Singleton<TextureContentCopyService>, public property::EventSource<const std::string&, void**, size_t*>
			{
			public:
				TextureContentCopyService() = default;
				~TextureContentCopyService() = default;

				void readTextureContent(const std::string& p_textureId, void** p_data, size_t* p_dataSize) const;

			};
		}
	}
}