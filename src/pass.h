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

#ifndef _PASS_H_
#define _PASS_H_

#include "texture.h"
#include "transformqueue.h"
#include "renderingqueue.h"
#include "viewportquad.h"
#include "configurable.h"

namespace DrawSpace
{

class Pass : public Core::Configurable
{
protected:	
    Core::RenderingQueue*   m_renderingqueue;
    ViewportQuad*           m_viewportquad;

    dsstring                m_name;

    // pour empecher l'instanciation
    Pass( const dsstring& p_name );
public:
    virtual ~Pass( void );

    virtual Core::RenderingQueue* GetRenderingQueue( void );

    virtual void CreateViewportQuad( void );
    virtual void CreateViewportQuad( dsreal p_viewport_width, dsreal p_viewport_height );

    virtual ViewportQuad* GetViewportQuad( void );

    virtual void GetName( dsstring& p_name );
};

class FinalPass : public Pass
{
public:
    FinalPass( const dsstring& p_name );
    virtual ~FinalPass( void );

    virtual void Serialize( Utils::Archive& p_archive  );
    virtual void Unserialize( Utils::Archive& p_archive );

    virtual void ApplyConfiguration( void );

};

class IntermediatePass : public Pass
{
protected:
    Core::Texture*          m_targettexture;

public:
    IntermediatePass( const dsstring& p_name );
    IntermediatePass( const dsstring& p_name, long p_target_width, long p_target_height );
    virtual ~IntermediatePass( void );

    Core::Texture* GetTargetTexture( void );

    virtual void Serialize( Utils::Archive& p_archive  );
    virtual void Unserialize( Utils::Archive& p_archive );

    virtual void ApplyConfiguration( void );

};
}

#endif
