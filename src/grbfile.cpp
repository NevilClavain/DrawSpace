/***************************************************************************
*                                                                          *
* DrawSpace Rendering engine                                               *
* Emmanuel Chaumont Copyright (c) 2013-2014                                *
*                                                                          *
* This file is part of DrawSpace.                                          *
*                                                                          *
*    DrawSpace is free software: you can redistribute it and/or modify     *
*    it under the terms of the GNU General Public License as published by  *
*    the Free Software Foundation, either version 3 of the License, or     *
*    (at your option) any later version.                                   *
*                                                                          *
*    DrawSpace is distributed in the hope that it will be useful,          *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*    GNU General Public License for more details.                          *
*                                                                          *
*    You should have received a copy of the GNU General Public License     *
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.    *
*                                                                          *
***************************************************************************/

#include "grbfile.h"
#include "asset.h"
#include "file.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

GRBFile::GRBFile( dsstring& p_path ) : m_path( p_path )
{

}

GRBFile::~GRBFile( void )
{


}

bool GRBFile::Load( void )
{

	return true;
}

bool GRBFile::Save( Factory& p_factory )
{
	Archive archive;
	p_factory.SerializeAll( archive );
	File file( m_path, DrawSpace::Utils::File::CREATENEW );
	file.SaveArchive( archive );
	return true;
}
