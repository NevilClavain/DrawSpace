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

#include <vector>
#include "parser.h"
#include "file.h"


using namespace DrawSpace;
using namespace DrawSpace::Utils;

static std::vector<std::string> split(const std::string& s, const std::string& seperators)
{
    std::vector<std::string> output;
    std::string::size_type prev_pos{ 0 };
    std::string::size_type pos{ 0 };

    while ((pos = s.find(seperators, pos)) != std::string::npos)
    {
        std::string substring(s.substr(prev_pos, pos - prev_pos));

        output.push_back(substring);
        prev_pos = ++pos;
    }

    output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word
    return output;
}

void Parser::run(const dsstring& p_filepath, const dsstring& p_separators, const ParserCallback& p_callback)
{
    long line_count{ 0 };

    File file(p_filepath, File::OPENEXISTINGTEXT);
    constexpr int lineMaxSize{ 1024 };

    char line[lineMaxSize];

    while (file.Gets(line, lineMaxSize))
    {
        line_count++;
        // supprimer le retour chariot en fin de ligne
        if (0x0a != line[0])
        {
            const auto len{ strlen(line) };

            if (0x0a == line[len - 1])
            {
                line[len - 1] = 0x00;
            }

            const dsstring current_line(line);
            if (current_line != "")
            {
                const auto words{ split(current_line, p_separators)};

                p_callback(current_line, line_count, words);
            }
        }
    }
}