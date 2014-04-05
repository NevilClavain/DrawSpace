/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#ifndef _PLANET_BODY_H_
#define _PLANET_BODY_H_

#include "drawable.h"
#include "scenegraph.h"
#include "task.h"
#include "property.h"
#include "mutex.h"
#include "planet_face.h"

namespace DrawSpace
{
namespace Planet
{

class FaceRenderingNode : public DrawSpace::Core::RenderingNode
{
    typedef DrawSpace::Core::CallBack2<FaceRenderingNode, void, int, Patch*>     PatchInstanciationCallback;
    typedef DrawSpace::Core::CallBack2<FaceRenderingNode, void, int, Patch*>     PatchDelCallback;
    typedef DrawSpace::Core::CallBack2<FaceRenderingNode, void, int, Patch*>     PatchSplitCallback;
    typedef DrawSpace::Core::CallBack2<FaceRenderingNode, void, int, Patch*>     PatchMergeCallback;
    
protected:

    DrawSpace::Interface::Renderer* m_renderer;
    std::map<dsstring, Patch*>      m_patchesleafs;
    std::map<dsstring, Patch*>      m_patches;
    PatchInstanciationCallback*     m_patchinstanciationcallback;
    PatchDelCallback*               m_patchdelcallback;
    PatchSplitCallback*             m_patchsplitcallback;
    PatchMergeCallback*             m_patchmergecallback;

    Face*                           m_face;

    void                            on_patchinstanciation( int p_orientation, Patch* p_patch );
    void                            on_patchdel( int p_orientation, Patch* p_patch );
    void                            on_patchsplit( int p_orientation, Patch* p_patch );
    void                            on_patchmerge( int p_orientation, Patch* p_patch );

    void                            draw_single_patch( Patch* p_patch, long p_nbv, long p_nbt, dsreal p_ray, const DrawSpace::Utils::Matrix& p_world, DrawSpace::Utils::Matrix& p_view );
    
public:
    FaceRenderingNode( Face* p_face, DrawSpace::Interface::Renderer* p_renderer );
    virtual ~FaceRenderingNode( void );

    virtual void Draw( long p_nbv, long p_nbt, dsreal p_ray, const DrawSpace::Utils::Matrix& p_world, DrawSpace::Utils::Matrix& p_view );    
    virtual Face::PatchInstanciationHandler* GetPatchInstanciationHandler( void );
    virtual Face::PatchDeletionHandler* GetPatchDelHandler( void );
    virtual Face::PatchSplitHandler* GetPatchSplitHandler( void );
    virtual Face::PatchMergeHandler* GetPatchMergeHandler( void );
    
};

class Body : public DrawSpace::Drawable
{	
protected:

    typedef struct
    {
        FaceRenderingNode*        nodes[6];

    } NodesSet;
       
    typedef DrawSpace::Core::CallBack<Body, void, DrawSpace::Core::RenderingNode*> RenderingNodeDrawCallback;
    typedef DrawSpace::Core::CallBack2<Body, void, int, Patch*>                    PatchInstanciationCallback;    

    std::map<dsstring, NodesSet>                                m_passesnodes;
    std::vector<RenderingNodeDrawCallback*>                     m_callbacks;
    DrawSpace::Scenegraph*                                      m_scenegraph;
    DrawSpace::Interface::Renderer*                             m_renderer;
    Face*                                                       m_faces[6];
    DrawSpace::Core::Meshe*                                     m_patchmeshe;

    DrawSpace::Core::Fx*                                        m_fx;

    //// properties
    DrawSpace::Core::TypedProperty<dsreal>                      m_diameter;
    DrawSpace::Core::TypedProperty<DrawSpace::Utils::Vector>    m_hotpoint;
    DrawSpace::Core::TypedProperty<DrawSpace::Utils::Vector>    m_relative_hotpoint;
    DrawSpace::Core::TypedProperty<dsreal>                      m_altitud;

    DrawSpace::Core::TypedProperty<dsstring>                    m_split;

    DrawSpace::Core::BaseCallback<void, const dsstring&>*       m_evt_handler;

    void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );
    void build_patch( void );


public:

    Body( void );
    virtual ~Body( void );

    virtual void GetDescr( dsstring& p_descr );
    virtual void SetRenderer( DrawSpace::Interface::Renderer * p_renderer );
    virtual void OnRegister( DrawSpace::Scenegraph* p_scenegraph );
    virtual DrawSpace::Core::Meshe* GetMeshe( const dsstring& p_mesheid );
    virtual void RegisterPassSlot( const dsstring p_passname );
    virtual DrawSpace::Core::RenderingNode* GetNodeFromPass( const dsstring& p_passname, const dsstring& p_nodeid );
    virtual void GetNodesIdsList( std::vector<dsstring>& p_ids );
    virtual void Compute( void );
    virtual void SetNodeFromPassSpecificFx( const dsstring& p_passname, const dsstring& p_nodeid, const dsstring& p_fxname );
    virtual void GetPropertiesList( std::vector<dsstring>& p_props );
    virtual DrawSpace::Core::Property* GetProperty( const dsstring& p_name );
    virtual void SetProperty( const dsstring& p_name, DrawSpace::Core::Property* p_prop );
    virtual void Initialize( void );
    virtual void RegisterEventHandler( DrawSpace::Core::BaseCallback<void, const dsstring&>* p_handler );
};
}
}
#endif