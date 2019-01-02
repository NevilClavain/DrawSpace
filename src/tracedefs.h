/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

#ifndef _TRACESDEFS_H_
#define _TRACESDEFS_H_

#include "drawspace_commons.h"
/*
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;
*/

#include "logconf.h"

#ifdef _DRAWSPACE_ENABLE_TRACES_

#define _DSTRACE( _logger, _message ) _logger.LogIt( DrawSpace::Logger::Sink::LEVEL_TRACE, __FUNCTION__ << dsstring( " " ) << _message );          //LOG4CXX_TRACE( _logger, __FUNCTION__ << " " << _message )
#define _DSDEBUG( _logger, _message ) _logger.LogIt( DrawSpace::Logger::Sink::LEVEL_DEBUG, __FUNCTION__ << dsstring( " " ) << _message );          //LOG4CXX_DEBUG( _logger, __FUNCTION__ << " " << _message )
#define _DSWARN( _logger, _message )  _logger.LogIt( DrawSpace::Logger::Sink::LEVEL_WARN, __FUNCTION__ << dsstring( " " ) << _message );          //LOG4CXX_WARN( _logger, __FUNCTION__ << " " << _message )
#define _DSERROR( _logger, _message ) _logger.LogIt( DrawSpace::Logger::Sink::LEVEL_ERROR, __FUNCTION__ << dsstring( " " ) << _message );          //LOG4CXX_ERROR( _logger, __FUNCTION__ << " " << _message )
#define _DSFATAL( _logger, _message ) _logger.LogIt( DrawSpace::Logger::Sink::LEVEL_FATAL, __FUNCTION__ << dsstring( " " ) << _message );          //LOG4CXX_FATAL( _logger, __FUNCTION__ << " " << _message )

#define _DECLARE_DS_LOGGER( _logger, _name, _confptr ) static DrawSpace::Logger::Sink _logger( _name, _confptr ); //static LoggerPtr _logger(Logger::getLogger( _name ));

#define _INIT_LOGGER( _file_ ) DrawSpace::Logger::Configuration::GetInstance()->Run( _file_, "   " ); //PropertyConfigurator::configure( _file_ );  

#else

#define _DSTRACE( _logger, _message )
#define _DSDEBUG( _logger, _message )
#define _DSINFO( _logger, _message )
#define _DSWARN( _logger, _message )
#define _DSERROR( _logger, _message )
#define _DSFATAL( _logger, _message )

#define _DECLARE_DS_LOGGER( _logger, _name )

#define _INIT_LOGGER( _file_ )

#endif

#endif
