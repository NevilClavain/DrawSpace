
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

#include "linemeshelistdrawing.h"

using namespace renderMe::rendering;

LineMesheListDrawing::LineMesheListDrawing(const std::vector<std::string>& p_linemesheNames) :
m_linemesheNames(p_linemesheNames)
{
}

void LineMesheListDrawing::addName(const std::string& p_lmName)
{
	m_linemesheNames.push_back(p_lmName);
}

std::function<void(const std::function<void(const std::string& p_lmName)>&)> LineMesheListDrawing::getDrawingFunc() const
{
	const auto drawingFunc
	{
		[this](const std::function<void(const std::string& p_lmName)>& p_lineMeshePresentationFunc)
		{
			// simply present linemeshes id one by one to the renderer (D3D11...)
			for (auto e : m_linemesheNames)
			{
				p_lineMeshePresentationFunc(e);
			}
		}
	};

	return drawingFunc;
}
