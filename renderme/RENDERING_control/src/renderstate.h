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
#include <string>
#include <vector>

namespace renderMe
{
    namespace rendering
    {
        class RenderState
        {
        public:

            enum class Operation
            {
                NONE,
                SETCULLING,
                ENABLEZBUFFER,
                SETTEXTUREFILTERTYPE,
                SETVERTEXTEXTUREFILTERTYPE,
                SETFILLMODE,
                ALPHABLENDENABLE,
                ALPHABLENDOP,
                ALPHABLENDFUNC,
                ALPHABLENDDEST,
                ALPHABLENDSRC,
            };

            RenderState(void) = default;
            RenderState(Operation p_operation, const std::string& p_arg);
            ~RenderState() = default;

            void setOperation(Operation p_operation);
            void setArg(const std::string& p_arg);
            void setExtendedArgs(const std::vector<std::string>& p_args);

            void clearExtendedArgs(void);
            void pushExtendedArg(const std::string& p_arg);

            std::string toString(void) const;

            Operation getOperation(void) const;
            std::string getArg(void) const;
            std::vector<std::string> getExtendedArgs(void) const;

        private:
            std::string                             m_arg;         //argument operation renderstate, sous forme de chaine ascii
            std::vector<std::string>                m_extendedargs;

            Operation                               m_operation{ Operation::NONE };
        };
    }
}