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

#include "drawspace_commons.h"
#include "callback.h"
#include "mediator.h"

#ifndef _SCRIPTING_H_
#define _SCRIPTING_H_

class Scripting
{
public:

    typedef DrawSpace::Core::BaseCallback<void, const dsstring&>                    ErrorHandler;
    typedef DrawSpace::Core::BaseCallback<void, DrawSpace::Core::PropertyPool&>     ScriptCallsHandler;

    virtual bool Initialize( void ) = 0;
    virtual void Shutdown( void ) = 0;

    virtual bool ExecChunk( const char* p_cmd ) = 0;
    virtual bool ExecFile( const char* p_path ) = 0;
    virtual void GetLastError( dsstring& p_str ) = 0;

    virtual void RegisterScriptErrorHandler( ErrorHandler* p_errorhandler ) = 0;
    virtual void RegisterScriptCallsHandler( ScriptCallsHandler* p_handler ) = 0;
};


#endif
