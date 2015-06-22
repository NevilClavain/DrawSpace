/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#ifndef _COLLIDER_H_
#define _COLLIDER_H_

#include "body.h"
#include "orbiter.h"
#include "movement.h"
#include "scenenode.h"

namespace DrawSpace
{
namespace Dynamics
{
class Collider : public Body
{
protected:

    btRigidBody*                    m_rigidBody;
    btCollisionShape*               m_collisionShape;
    btTriangleMesh*                 m_meshe_data;
    btDefaultMotionState*           m_motionState;

    DrawSpace::Core::TransformNode* m_drawable;

    Orbiter*                        m_orbiter;

    DrawSpace::Core::Movement*      m_movement;

    DrawSpace::Utils::Matrix        m_lastlocalworldtrans;

    DrawSpace::Utils::Matrix        m_basetransformation;
    DrawSpace::Utils::Matrix        m_finaltransformation;


    DrawSpace::Core::BaseSceneNode* m_owner;

    bool                            m_parameters_set;
    Parameters                      m_parameters;

    // NE PAS SUPPRIMER
    Orbiter*                        m_referent_orbiter;

public:

    Collider( DrawSpace::Core::TransformNode* p_drawable );
    virtual ~Collider( void );

    virtual void Update( Utils::TimeManager& p_timemanager, const DrawSpace::Utils::Matrix& p_mat );

    virtual void Update( Utils::TimeManager& p_timemanager ) {};
    virtual void Update2( Utils::TimeManager& p_timemanager );

    virtual void SetRootOrbiter( Orbiter* p_orbiter );
    virtual void RegisterMovement( DrawSpace::Core::Movement* p_movement );


    virtual void SetKinematic( const Body::Parameters& p_parameters );
    virtual void UnsetKinematic( void );

    virtual void AddToWorld( World* p_world );
    virtual void RemoveFromWorld( void );

    virtual btRigidBody* GetRigidBody( void );

    virtual void GetLastLocalWorldTrans( DrawSpace::Utils::Matrix& p_mat );

    void SetBaseTransform( const DrawSpace::Utils::Matrix& p_mat );

    void GetBaseTransform( DrawSpace::Utils::Matrix& p_mat );
    
    void SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat );

    void OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node );

    bool GetParameters( Parameters& p_parameters );

    // NE PAS SUPPRIMER
    virtual void SetReferentOrbiter( DrawSpace::Dynamics::Orbiter* p_reforbiter );
    virtual DrawSpace::Dynamics::Orbiter* GetReferentOrbiter( void );


};
}
}

#endif
