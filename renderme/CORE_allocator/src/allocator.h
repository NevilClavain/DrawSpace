
/* -*-LIC_BEGIN-*- */
/*
*
* MaGE rendering framework
* Emmanuel Chaumont Copyright (c) 2013-2025
*
* This file is part of MaGE.
*
*    MaGE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    MaGE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with MaGE.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#pragma once

#include "memorychunks.h"

#define _NEW_CHUNK_( __type__, __item__ ) mage::core::MemoryChunks::getInstance()->registerChunk<__type__>( new __item__, sizeof( __type__ ), #__item__, __FUNCTION__, __LINE__, __FILE__ )
#define _NEW_CHUNK_WITH_COMMENT( __type__, __item__, __comment__ ) mage::core::MemoryChunks::getInstance()->registerChunk<__type__>( new __item__, sizeof( __type__ ), #__item__, __FUNCTION__, __LINE__, __FILE__, __comment__ )

#define _NEW_CHUNK_EXPLICIT_SIZE_( __type__, __item__, __size__ ) mage::core::MemoryChunks::getInstance()->registerChunk<__type__>( new __item__, __size__, #__item__, __FUNCTION__, __LINE__, __FILE__ )
#define _NEW_CHUNK_EXPLICIT_SIZE_WITH_COMMENT( __type__, __item__, __size__, __comment__ ) mage::core::MemoryChunks::getInstance()->registerChunk<__type__>( new __item__, __size__, #__item__, __FUNCTION__, __LINE__, __FILE__, __comment__ )

#define _DELETE_CHUNK_( __ptr__ ) delete __ptr__; mage::core::MemoryChunks::getInstance()->unregisterChunk( __ptr__ ); __ptr__ = nullptr
#define _DELETE_CHUNK_N_( __ptr__ ) delete[] __ptr__; mage::core::MemoryChunks::getInstance()->unregisterChunk( __ptr__ ); __ptr__ = nullptr
