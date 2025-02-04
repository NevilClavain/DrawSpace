
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

#include <functional>
#include <unordered_map>

namespace mage
{
	namespace core
	{
		class ComponentContainer;
	}

	namespace transform
	{
		// fwd decl
		struct WorldPosition;

		using AnimatorFunc = std::function<void(const core::ComponentContainer&, const core::ComponentContainer&, const WorldPosition&, const std::unordered_map<std::string, std::string>& )>;

		struct Animator
		{
		public:
			Animator(std::unordered_map<std::string, std::string> p_component_keys, const AnimatorFunc& p_func) :
			func(p_func),
			component_keys(p_component_keys)
			{}

			Animator() = delete;

			AnimatorFunc									func;
			std::unordered_map<std::string, std::string>	component_keys;
		};
	}
}