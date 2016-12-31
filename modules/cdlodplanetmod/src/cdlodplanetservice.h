/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#ifndef _CDLODPLANETSERVICE_H_
#define _CDLODPLANETSERVICE_H_

#include "module_service.h"
#include "drawspace.h"


class CDLODPlanetService;

class SceneNodeGraphParam : public DrawSpace::Module::KeySink<DrawSpace::Core::SceneNodeGraph*>
{
protected:
    CDLODPlanetService* m_owner;

public:
    SceneNodeGraphParam( const dsstring& p_id, CDLODPlanetService* p_owner );
    virtual void OnUpdated( DrawSpace::Core::SceneNodeGraph* p_val );
};

class TexturePassParam : public DrawSpace::Module::KeySink<DrawSpace::IntermediatePass*>
{
protected:
    CDLODPlanetService* m_owner;

public:
    TexturePassParam( const dsstring& p_id, CDLODPlanetService* p_owner );
    virtual void OnUpdated( DrawSpace::IntermediatePass* p_val );
};


class SimpleColorBinder : public DrawSpace::SphericalLOD::Binder
{
protected:
public:

    SimpleColorBinder( void );

    virtual void Bind( void );
    virtual void Unbind( void );
};

///////////////////////////////////////////////////////////////////////////////////////////////

class CDLODPlanetService : public DrawSpace::Interface::Module::Service
{
protected:

    typedef struct
    {
        DrawSpace::SphericalLOD::Config                                 config;

        SimpleColorBinder*                                              simplebinder[6];
        DrawSpace::Core::Fx*                                            details_fx;

        DrawSpace::Core::Shader*                                        planet_vshader;
        DrawSpace::Core::Shader*                                        planet_pshader;

        DrawSpace::SphericalLOD::Root*                                  planet;
        DrawSpace::Core::SceneNode<DrawSpace::SphericalLOD::Root>*      planet_node;

    } PlanetEntry;

    DrawSpace::Interface::Renderer*                                     m_renderer;
    DrawSpace::Utils::TimeManager                                       m_tm;
    dsstring                                                            m_device;


    SceneNodeGraphParam*                                                m_scparam;
    TexturePassParam*                                                   m_texturepassparam;


    DrawSpace::Core::SceneNodeGraph*                                    m_scenenodegraph;
    DrawSpace::IntermediatePass*                                        m_texturepass;

    std::map<dsstring, PlanetEntry>                                     m_nodes;


public:
    CDLODPlanetService( const dsstring& p_id );
    ~CDLODPlanetService( void );

    virtual void                            GetKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys );
    virtual void                            Init( DrawSpace::Logger::Configuration* p_logconf, 
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousecircularmode_cb, 
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousevisible_cb, 
                                                    DrawSpace::Core::BaseCallback<void, int>* p_closeapp_cb );

    virtual void                            Run( void );
    virtual void                            Release( void );

    virtual DrawSpace::Core::BaseSceneNode* InstanciateSceneNode( const dsstring& p_sceneNodeName );
    virtual void                            RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph );
    virtual void                            ReleaseSceneNode( const dsstring& p_sceneNodeName );

    virtual void                            OnSceneNodeGraphUpdated( DrawSpace::Core::SceneNodeGraph* p_val );
    virtual void                            OnTexturePassUpdate( DrawSpace::IntermediatePass* p_val );
};

#endif