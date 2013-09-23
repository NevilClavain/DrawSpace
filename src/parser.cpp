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


#include "parser.h"

using namespace DrawSpace::Utils;

Parser::Parser( void )
{

}

Parser::~Parser( void )
{


}

void Parser::split_line( const dsstring& p_line, const dsstring& p_separators, std::vector<dsstring>& p_words )
{
	char* pch;
    char line[256];
    strcpy( line, p_line.c_str() );
    pch = strtok( line, p_separators.c_str() );
	if( pch )
	{
        p_words.push_back( pch );
	}
	while( pch != NULL )
	{
        pch = strtok( NULL, p_separators.c_str() );
		if( pch )
		{
			p_words.push_back( pch );
		}
	}
}

bool Parser::Run( const dsstring& p_filepath, const dsstring& p_separators )
{
    bool status = true;
    long line_count = 0;

    FILE* fp = fopen( p_filepath.c_str(), "r" );
	if( fp )
	{
		char line[256];

		while( fgets( line, 256, fp ) )
		{
			line_count++;
			// supprimer le retour chariot en fin de ligne
			if( 0x0a != line[0] )
			{
				size_t len = strlen( line );
				if( 0x0a == line[len - 1 ] )
				{
					line[len - 1] = 0x00;
				}

                std::vector<dsstring> words;
                split_line( line, p_separators, words );

                if( !on_new_line( line, line_count, words ) )
                {
                    break;
                }
			}
		}
		fclose( fp );
	}
    else
    {
        status = false;
    }

    return status;
}
