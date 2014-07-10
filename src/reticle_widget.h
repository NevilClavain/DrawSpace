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

#ifndef _RETICLE_WIDGET_H_
#define _RETICLE_WIDGET_H_

#include "widget.h"
#include "body.h"
#include "scenegraph.h"

namespace DrawSpace
{
namespace Gui
{
class ReticleWidget : public Widget
{
public:

    typedef enum
    {
        NO_CLIPPING,
        CLIPPING_CUT,
        CLIPPING_HOLD,

    } ClippingPolicy;


    typedef struct
    {
        ClippingPolicy clipping_policy;

        dsreal xmin, ymin;
        dsreal xmax, ymax;

    } ClippingParams;

protected:

    DrawSpace::Dynamics::Body*                      m_locked_body;
    DrawSpace::Core::TransformNode*                 m_locked_node;
    DrawSpace::Scenegraph*                          m_scenegraph;

    ClippingParams                                  m_clipping_params;

    dsreal                                          m_zdepth;


public:
    ReticleWidget( const dsstring& p_name, long p_virtual_width, long p_virtual_height, DrawSpace::Scenegraph* p_scenegraph, Widget* p_parentwidget );
    virtual ~ReticleWidget( void );

    virtual void LockOnBody( DrawSpace::Dynamics::Body* p_locked_body );
    virtual void LockOnTransformNode( DrawSpace::Core::TransformNode* p_locked_node );
    virtual void SetTranslation( dsreal p_x, dsreal p_y );
    virtual void SetClippingParams( const ClippingParams& p_params );
    virtual void Transform( void );
    virtual dsreal GetLastZDepth( void );
};
}
}

#endif