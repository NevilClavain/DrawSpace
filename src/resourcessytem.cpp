/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2019
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

#include "resourcessystem.h"
#include "resourcesaspect.h"
#include "texture.h"



#include "file.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::EntityGraph;
using namespace DrawSpace::Systems;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Utils;


dsstring ResourcesSystem::m_textures_rootpath = ".";


void ResourcesSystem::SetTexturesRootPath(const dsstring& p_path)
{
    m_textures_rootpath = p_path;
}

void ResourcesSystem::run(EntityGraph::EntityNodeGraph* p_entitygraph)
{
    p_entitygraph->AcceptSystemRootToLeaf( this );
}

void ResourcesSystem::VisitEntity(Entity* p_parent, Entity* p_entity)
{
    ResourcesAspect* resources_aspect = p_entity->GetAspect<ResourcesAspect>();
    if (resources_aspect)
    {
        ComponentList<std::tuple<Texture*, bool>> textures_assets;
        resources_aspect->GetComponentsByType<std::tuple<Texture*, bool>>(textures_assets);

        for(auto& e : textures_assets)
        {
            bool& loaded = std::get<1>( e->getPurpose() );
            if( !loaded )
            {
                dsstring asset_path;                
                std::get<0>( e->getPurpose() )->GetPath(asset_path);

                dsstring final_asset_path = compute_textures_final_path(asset_path);

                Texture* texture = std::get<0>( e->getPurpose() );
               
                if(m_texturesBloc.find(final_asset_path) == m_texturesBloc.end())
                {
                    Bloc bloc;
                    long size;
                    void* data;

                    // load it
                    data = Utils::File::LoadAndAllocBinaryFile(final_asset_path, &size);
                    if (!data)
                    {
                        _DSEXCEPTION("ResourcesSystem : failed to load " + final_asset_path);
                    }
                    bloc.data = data;
                    bloc.size = size;

                    m_texturesBloc[final_asset_path] = bloc;
                }

                // update texture
                texture->SetData(m_texturesBloc.at(final_asset_path).data, m_texturesBloc.at(final_asset_path).size);

                loaded = true;
            }
        }
    }
}

dsstring ResourcesSystem::compute_textures_final_path(const dsstring& p_path) const
{
    dsstring final_path = m_textures_rootpath + "/";
    final_path += p_path;
    return final_path;
}
