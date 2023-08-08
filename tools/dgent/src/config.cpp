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

#include "config.h"
#include "exceptions.h"

std::string Config::extract_token(int p_tkindex) const
{
    dsstring token_text;
    DrawSpace::JSONParser::getTokenString(p_tkindex, token_text);
    return token_text;
}

void Config::ParseFromFile(const dsstring& p_filepath)
{
	DrawSpace::JSONParser::parseFromFile(p_filepath);

    if (JSMN_OBJECT == DrawSpace::JSONParser::getTokenType(0))
    {
        const int token_size{ DrawSpace::JSONParser::getTokenSize(0) };
        for (int i = 0; i < token_size; i++)
        {
            const int tkindex{ (2 * i) + 1 };
            const auto token_key{ extract_token(tkindex) };
            const auto token_value{ extract_token(tkindex + 1) };

            if ("template" == token_key)
            {
                m_template_path = token_value;
            }
            else if ("destination" == token_key)
            {
                m_destination_path = token_value;
            }
            else
            {
                m_substitution_table[token_key] = token_value;
            }
        }
    }
    else
    {
        _DSEXCEPTION("JSON parse : unexpected type for token 0");
    }
}

std::string Config::GetTemplatePath(void) const
{
    return m_template_path;
}

std::string Config::GetDestinationPath(void) const
{
    return m_destination_path;
}

SubstitutionTable Config::GetSubstitutionTable(void) const
{
    return m_substitution_table;
}
