
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

namespace mage
{
	namespace core
	{
		struct teapotAspect // for test, debug
		{
			static constexpr int id{ 0x6000 };
		};

		struct renderingAspect
		{
			static constexpr int id{ 0x0001 };

			enum class renderingTarget
			{
				SCREEN_RENDERINGTARGET,
				BUFFER_RENDERINGTARGET
			};
		};

		struct timeAspect
		{
			static constexpr int id{ 0x0002 };
		};

		struct resourcesAspect
		{
			static constexpr int id{ 0x0003 };
		};

		struct cameraAspect
		{
			static constexpr int id{ 0x0004 };
		};

		struct worldAspect
		{
			static constexpr int id{ 0x0005 };
		};

		struct animationsAspect
		{
			static constexpr int id{ 0x0006 };
		};
	}
}
