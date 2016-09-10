/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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
#pragma warning( disable : 4005 )

#include <drawspace_commons.h>
#include <d3dx9.h>

void TranslateD3DD9Error( HRESULT p_hRes, dsstring &p_str )
{	
    switch( p_hRes )
    {
        case D3D_OK:
            p_str = "D3D_OK";
            break;
        
        case D3DERR_CONFLICTINGRENDERSTATE:
            p_str = "D3DERR_CONFLICTINGRENDERSTATE";
            break;

        case D3DERR_CONFLICTINGTEXTUREFILTER:
            p_str = "D3DERR_CONFLICTINGTEXTUREFILTER";
            break;

        case D3DERR_CONFLICTINGTEXTUREPALETTE:
            p_str = "D3DERR_CONFLICTINGTEXTUREPALETTE";
            break;

        case D3DERR_DEVICELOST:
            p_str = "D3DERR_DEVICELOST";
            break;

        case D3DERR_DEVICENOTRESET:
            p_str = "D3DERR_DEVICENOTRESET";
            break;

        case D3DERR_DRIVERINTERNALERROR:
            p_str = "D3DERR_DRIVERINTERNALERROR";
            break;

        case D3DERR_DRIVERINVALIDCALL:
            p_str = "D3DERR_DRIVERINVALIDCALL";
            break;

        case D3DERR_INVALIDCALL:
            p_str = "D3DERR_INVALIDCALL";
            break;

        case D3DERR_INVALIDDEVICE:
            p_str = "D3DERR_INVALIDDEVICE";
            break;

        case D3DERR_MOREDATA:
            p_str = "D3DERR_MOREDATA";
            break;

        case D3DERR_NOTAVAILABLE:
            p_str = "D3DERR_NOTAVAILABLE";
            break;

        case D3DERR_NOTFOUND:
            p_str = "D3DERR_NOTFOUND";
            break;

        case D3DERR_OUTOFVIDEOMEMORY:
            p_str = "D3DERR_OUTOFVIDEOMEMORY ";
            break;

        case D3DERR_TOOMANYOPERATIONS:
            p_str = "D3DERR_TOOMANYOPERATIONS";
            break;

        case D3DERR_UNSUPPORTEDALPHAARG:
            p_str = "D3DERR_UNSUPPORTEDALPHAARG";
            break;

        case D3DERR_UNSUPPORTEDALPHAOPERATION:
            p_str = "D3DERR_UNSUPPORTEDALPHAOPERATION";
            break;

        case D3DERR_UNSUPPORTEDCOLORARG:
            p_str = "D3DERR_UNSUPPORTEDCOLORARG";
            break;

        case D3DERR_UNSUPPORTEDCOLOROPERATION:
            p_str = "D3DERR_UNSUPPORTEDCOLOROPERATION";
            break;

        case D3DERR_UNSUPPORTEDFACTORVALUE:
            p_str = "D3DERR_UNSUPPORTEDFACTORVALUE";
            break;

        case D3DERR_UNSUPPORTEDTEXTUREFILTER:
            p_str = "D3DERR_UNSUPPORTEDTEXTUREFILTER";
            break;

        case D3DERR_WASSTILLDRAWING:
            p_str = "D3DERR_WASSTILLDRAWING";
            break;

        case D3DERR_WRONGTEXTUREFORMAT:
            p_str = "D3DERR_WRONGTEXTUREFORMAT";
            break;

        case E_FAIL :
            p_str = "E_FAIL ";
            break;

        case E_INVALIDARG:
            p_str = "E_INVALIDARG";
            break;

        case E_NOINTERFACE:
            p_str = "E_INVALIDARG";
            break;

        case E_OUTOFMEMORY:
            p_str = "E_OUTOFMEMORY";
            break;

        default:			
            char err[64];
            sprintf( err, "Unknown D3D9 error (%.8x)", p_hRes );

            p_str = err;

            break;
    }
}
