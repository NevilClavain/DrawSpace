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

#include "scenegraph.h"
#include "planet_face.h"

namespace DrawSpace
{
namespace Planet
{
class Body : public Core::TransformNode
{
public:
/*
    static const int FrontPlanetFace    = 0;
    static const int RearPlanetFace     = 1;
    static const int LeftPlanetFace     = 2;
    static const int RightPlanetFace    = 3;
    static const int TopPlanetFace      = 4;
    static const int BottomPlanetFace   = 5;
    */
    static const int AllPlanetFaces     = 6;
    
protected:

    typedef struct
    {
        Face*        faces[6];

    } FacesSet;

    typedef Core::CallBack<Body, void, Core::RenderingNode*> RenderingNodeDrawCallback;

    std::map<dsstring, FacesSet>                m_passesnodes;
    std::vector<RenderingNodeDrawCallback*>     m_callbacks;
    Scenegraph*                                 m_scenegraph;

    void                                        on_renderingnode_draw( Core::RenderingNode* p_rendering_node );

public:

    Body( const dsstring& p_name );
    virtual ~Body( void );

    virtual void RegisterPassFaceSet( const dsstring p_passname );
    virtual Core::Fx* GetPassFaceFx( const dsstring& p_passname, int p_faceid );
    virtual void OnRegister( Scenegraph* p_scenegraph );
	virtual bool LoadAssets( void );
};
}
}
#endif