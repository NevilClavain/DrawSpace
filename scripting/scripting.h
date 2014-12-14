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

#include "drawspace_commons.h"
#include "callback.h"

#ifndef _SCRIPTING_H_
#define _SCRIPTING_H_

class Scripting
{
public:

    typedef DrawSpace::Core::BaseCallback<void, const dsstring&> ScriptErrorHandler;

    virtual bool Initialize( void ) = 0;
    virtual void Shutdown( void ) = 0;

    virtual void ExecChunk( const char* p_cmd ) = 0;
    virtual void ExecFile( const char* p_path ) = 0;


    virtual void RegisterScriptErrorHandler( ScriptErrorHandler* p_errorhandler ) = 0;


    //////////////////////////////////////////
    virtual void RegisterCB( DrawSpace::Core::BaseCallback<void, int>* p_handler ) = 0;
    virtual void TestCBCall( void ) = 0;

};


#endif