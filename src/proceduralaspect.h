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

#ifndef _PROCEDURALASPECT_H_
#define _PROCEDURALASPECT_H_

#include "aspect.h"
#include "entity.h"

#define PROCEDURALBLOCID( __type__) typeid(__type__).hash_code()

namespace DrawSpace
{
namespace Aspect
{
class ProceduralAspect : public Core::Aspect 
{
public:



    typedef enum
    {
        NOARG,
        ARG0,
        ARG1,
        ARG2,
        ARG3,
        ARG4
    
    } Args;

    class ProceduralBloc
    {
    public:
        std::vector<ProceduralBloc*> m_children;

    public:
        virtual void Evaluate( void ) = 0;
    };

    class RootProceduralBloc : public ProceduralBloc
    {
    public:
        virtual void Evaluate( void );
    };

    class PublishProceduralBloc : public ProceduralBloc
    {
    public:
        virtual void Evaluate( void );   
    };

    class RepeatProceduralBloc : public ProceduralBloc
    {
    public:
        ProceduralBloc* m_nbIteration;
        ProceduralBloc* m_action;

    public:
        virtual void Evaluate( void );   

    };

    class RandomProceduralBloc : public ProceduralBloc
    {
    public:

    public:
        virtual void Evaluate( void );
        
    };

    ////////////////////////////


protected:
    
    bool m_to_update;
    
    void update( void );

public:
    ProceduralAspect( void );

    void SetToUpdate( bool p_state );
    bool GetToUpdate( void ) const;

    void Run( Core::Entity* p_parent, Core::Entity* p_entity  );
};



}
}

#endif