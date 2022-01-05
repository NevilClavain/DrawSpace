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

#include "loadshaderfiletask.h"
#include "memalloc.h"
#include <regex>

using namespace DrawSpace;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Systems;

LoadShaderFileTask::LoadShaderFileTask() : ITask("", "")
{
}

LoadShaderFileTask::~LoadShaderFileTask()
{
    release_blocs();
}

void LoadShaderFileTask::Execute(void)
{
    dsstring shader_source;
    recurs_browse_shader_source(m_final_asset_path, shader_source);

    m_size = shader_source.size();
    m_data = (void*)_DRAWSPACE_NEW_EXPLICIT_SIZE_WITH_COMMENT(unsigned char, unsigned char[m_size], m_size, m_final_asset_path);
    
    memcpy(m_data, shader_source.c_str(), m_size);
}

void LoadShaderFileTask::split_source(dsstring& p_source, std::list<dsstring>& p_decomposed_source, std::list<dsstring>& p_decomposed_source_whitecars)
{
    std::regex regex("\\s");

    std::list<std::string> decomposed_source(
        std::sregex_token_iterator(p_source.begin(), p_source.end(), regex, -1),
        std::sregex_token_iterator()
    );

    std::list<std::string> decomposed_source_whitechars(
        std::sregex_token_iterator(p_source.begin(), p_source.end(), regex, 0),
        std::sregex_token_iterator()
    );

    p_decomposed_source = decomposed_source;
    p_decomposed_source_whitecars = decomposed_source_whitechars;

}

void LoadShaderFileTask::release_blocs(void)
{
    for (auto& e : m_blocs)
    {
        _DRAWSPACE_DELETE_N_(e);
    }
}

void LoadShaderFileTask::recurs_browse_shader_source(const dsstring& p_filepath, dsstring& p_updated_source)
{
    // 1/ load filepath source text

    // 2/ foreach #include "..." 

    //    -> recursive call -> retrieve p_updated_source argument content
    //    -> replace current '#include "..."' by p_updated_source argument content 
    
    // 3/ return completed source text in 'p_updated_source' arg for parent caller

    /////////////////////////////////////////////////////////////

 
    long size;
    void* data = Utils::File::LoadAndAllocBinaryFile(p_filepath, &size);
    if (data)
    {
        m_blocs.push_back(data);

        dsstring shader_source((const char*)data, size);

        std::list<dsstring> decomposed_source;
        std::list<dsstring> decomposed_source_whitecars;
        split_source(shader_source, decomposed_source, decomposed_source_whitecars);

        for (auto it = decomposed_source.begin(); it != decomposed_source.end(); ++it)
        {
            dsstring word{ *it }; 
            
            if (word == "#include")
            {
                //to_remove.push_back(it);

                auto it_next = it;
                dsstring included_hlsl = *(++it_next);
                
                // remove double quotes : first and last char
                included_hlsl.erase(included_hlsl.length() - 1, 1);
                included_hlsl.erase(0, 1);

                // concat to obtain final path of included file
                dsstring included_hlsl_complete_path = m_final_asset_dir + included_hlsl;
                
                dsstring included_hlsl_content;
                recurs_browse_shader_source(included_hlsl_complete_path, included_hlsl_content);

                if (m_failure)
                {
                    // something went wrong, cancel all...
                    break;
                }

                //substitute...
                *(it) = "";
                *(it_next) = included_hlsl_content;

            }
        }

        //////////////// re-build final source text

        std::vector<dsstring> out_shader_source_concat;

        auto it1{ decomposed_source.cbegin() };
        auto it2{ decomposed_source_whitecars.cbegin() };


        dsstring out_shader_source;
        bool merge = true;

        do
        {
            dsstring str1 = *it1;
            dsstring str2 = *it2;

            //if (str1 != "")
            {
                out_shader_source += str1;
            }

            //if (str2 != "")
            {
                out_shader_source += str2;
            }

            it1++;
            if (it1 == decomposed_source.end())
            {
                merge = false;
            }
            it2++;
            if (it2 == decomposed_source_whitecars.end())
            {
                merge = false;
            }

        } while (merge);
       
        ////////////////////////////////////////////
        
        p_updated_source = out_shader_source;        
    }
    else
    {
        m_failure = true;
    }
}

void LoadShaderFileTask::SetFinalAssetPath(const dsstring& p_final_asset_path)
{
    m_final_asset_path = p_final_asset_path;
}

void LoadShaderFileTask::SetFinalAssetDir(const dsstring& p_final_asset_dir)
{
    m_final_asset_dir = p_final_asset_dir;
}

bool LoadShaderFileTask::Failed(void) const
{
    return m_failure;
}

long LoadShaderFileTask::GetSize(void) const
{
    return m_size;
}

void* LoadShaderFileTask::GetData(void) const
{
    return m_data;
}
