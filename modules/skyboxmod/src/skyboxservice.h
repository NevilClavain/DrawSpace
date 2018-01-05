/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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

/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#ifndef _SKYBOXSERVICE_H_
#define _SKYBOXSERVICE_H_

#include "module_service.h"
#include "spacebox.h"
#include "transformation.h"

class SkyboxService;


class TexturePassParam : public DrawSpace::Module::KeySink<DrawSpace::IntermediatePass*>
{
protected:
    SkyboxService* m_owner;

public:
    TexturePassParam( const dsstring& p_id, SkyboxService* p_owner );
    virtual void OnUpdated( DrawSpace::IntermediatePass* p_val );
};

class TextureMirrorPassParam : public DrawSpace::Module::KeySink<DrawSpace::IntermediatePass*>
{
protected:
    SkyboxService* m_owner;

public:
    TextureMirrorPassParam( const dsstring& p_id, SkyboxService* p_owner );
    virtual void OnUpdated( DrawSpace::IntermediatePass* p_val );
};

class ReflectorNormaleParam : public DrawSpace::Module::KeySink<DrawSpace::Utils::Vector>
{
protected:
    SkyboxService* m_owner;

public:
    ReflectorNormaleParam( const dsstring& p_id, SkyboxService* p_owner );
    virtual void OnUpdated( DrawSpace::Utils::Vector p_val );
};

class TexturesBankPathParam : public DrawSpace::Module::KeySink<dsstring>
{
protected:
    SkyboxService* m_owner;

public:
    TexturesBankPathParam( const dsstring& p_id, SkyboxService* p_owner );
    virtual void OnUpdated( dsstring p_val );
};

class TexturesBankVirtualFSPathParam : public DrawSpace::Module::KeySink<dsstring>
{
protected:
    SkyboxService* m_owner;

public:
    TexturesBankVirtualFSPathParam( const dsstring& p_id, SkyboxService* p_owner );
    virtual void OnUpdated( dsstring p_val );
};

class TexturesNamesParam : public DrawSpace::Module::KeySink<std::vector<dsstring>>
{
protected:
    SkyboxService* m_owner;

public:
    TexturesNamesParam( const dsstring& p_id, SkyboxService* p_owner );
    virtual void OnUpdated( std::vector<dsstring> p_val );
};


///////////////////////////////////////////////////////////////////////////////////////////////

class SkyboxService : public DrawSpace::Interface::Module::Service
{
protected:

    typedef struct
    {
        DrawSpace::Spacebox*                                            skybox;
        DrawSpace::Core::SceneNode<DrawSpace::Spacebox>*                skybox_node;
    
    } SkyboxNodeEntry;

    DrawSpace::Interface::Renderer*                                     m_renderer;
    DrawSpace::Utils::TimeManager                                       m_tm;
    dsstring                                                            m_device;

    TexturePassParam*                                                   m_texturepassparam;
    TextureMirrorPassParam*                                             m_texturemirrorpassparam;
    ReflectorNormaleParam*                                              m_reflectornormaleparam;
    TexturesBankPathParam*                                              m_texturebankpathparam;
    TexturesBankVirtualFSPathParam*                                     m_texturebankvirtualfspathparam;
    TexturesNamesParam*                                                 m_texturesnamesparam;

    DrawSpace::Core::SceneNodeGraph*                                    m_scenenodegraph;


    DrawSpace::IntermediatePass*                                        m_texturepass;
    DrawSpace::IntermediatePass*                                        m_texturemirrorpass;


    std::map<dsstring, SkyboxNodeEntry>                                 m_nodes;

    DrawSpace::Utils::Vector                                            m_reflector_normale;


    dsstring                                                            m_texturevirtualfspath;

    std::vector<dsstring>                                               m_textures_names;


public:
    SkyboxService( const dsstring& p_id );
    ~SkyboxService( void );

    virtual void                            GetKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys );
    virtual void                            Init( DrawSpace::Logger::Configuration* p_logconf, 
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousecircularmode_cb, 
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousevisible_cb, 
                                                    DrawSpace::Core::BaseCallback<void, int>* p_closeapp_cb );

    virtual void                            Run( void );
    virtual void                            Release( void );

    virtual DrawSpace::Core::BaseSceneNode* InstanciateSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar, LODDependantNodeInfoStateHandler* p_handler );
    virtual void                            RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph );
    virtual void                            UnregisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph );
    virtual void                            ReleaseSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar );


    virtual void                            OnSceneNodeGraphUpdated( DrawSpace::Core::SceneNodeGraph* p_val );
    virtual void                            OnTexturePassUpdate( DrawSpace::IntermediatePass* p_val );
    virtual void                            OnTextureMirrorPassUpdate( DrawSpace::IntermediatePass* p_val );
    virtual void                            OnReflectorNormaleUpdate( const DrawSpace::Utils::Vector& p_normale );
    virtual void                            OnTextureVirtualFSPathUpdate( const dsstring& p_path );
    virtual void                            OnTexturesNamesUpdate( const std::vector<dsstring> p_textures );

};

#endif
