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

#include <unordered_map>
#include "renderstate.h"

using namespace renderMe;

RenderState::RenderState(Operation p_operation, const std::string& p_arg) :
m_operation(p_operation),
m_arg(p_arg)
{
}

void RenderState::setOperation(RenderState::Operation p_operation)
{
    m_operation = p_operation;
}

void RenderState::setArg(const std::string& p_arg)
{
    m_arg = p_arg;
}

void RenderState::setExtendedArgs(const std::vector<std::string>& p_args)
{
    m_extendedargs = p_args;
}

void RenderState::clearExtendedArgs(void)
{
    m_extendedargs.clear();
}

void RenderState::pushExtendedArg(const std::string& p_arg)
{
    m_extendedargs.push_back(p_arg);
}

RenderState::Operation RenderState::getOperation(void) const
{
    return m_operation;
}

std::string RenderState::getArg(void) const
{
    return m_arg;
}

std::vector<std::string> RenderState::getExtendedArgs(void) const
{
    return m_extendedargs;
}

std::string RenderState::toString(void) const
{
    static const std::unordered_map<Operation, std::string> translate =
    {
        { Operation::NONE, "NONE" },
        { Operation::SETCULLING, "SETCULLING" },
        { Operation::ENABLEZBUFFER, "ENABLEZBUFFER" },
        { Operation::SETTEXTUREFILTERTYPE, "SETTEXTUREFILTERTYPE" },
        { Operation::SETVERTEXTEXTUREFILTERTYPE, "SETVERTEXTEXTUREFILTERTYPE" },
        { Operation::SETFILLMODE, "SETFILLMODE" },
        { Operation::ALPHABLENDENABLE, "ALPHABLENDENABLE" },
        { Operation::ALPHABLENDOP, "ALPHABLENDOP" },
        { Operation::ALPHABLENDFUNC, "ALPHABLENDFUNC" },
        { Operation::ALPHABLENDDEST, "ALPHABLENDDEST" },
        { Operation::ALPHABLENDSRC, "ALPHABLENDSRC" }
    };

    const std::string result{ translate.at(m_operation) + " = " + m_arg };

    return result;
}