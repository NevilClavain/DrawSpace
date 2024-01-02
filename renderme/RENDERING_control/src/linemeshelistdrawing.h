
/* -*-LIC_BEGIN-*- */
/*
*
* renderMe grafx framework
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of renderMe.
*
*    renderMe is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    renderMe is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with renderMe.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#pragma once

#include <vector>
#include "primitivedrawing.h"

namespace renderMe
{
	namespace rendering
	{
		struct LineMesheListDrawing : public PrimitiveDrawing
		{
		public:
			
			LineMesheListDrawing() = default;
			LineMesheListDrawing(const std::vector<std::string>& p_linemesheNames);

			~LineMesheListDrawing() = default;

			void addName(const std::string& p_lmName);

			std::function<void(const std::function<void(const std::string&)>&)> getDrawingFunc() const;

		private:
			std::vector<std::string> m_linemesheNames;

		};
	}
}
