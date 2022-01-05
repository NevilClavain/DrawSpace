/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

#include <drawspace_commons.h>
#include <d3dx11.h>

void TranslateD3DD11Error( HRESULT p_hRes, dsstring &p_str )
{	
    switch( p_hRes )
    {
        case S_OK:
            p_str = "S_OK";
            break;

        case S_FALSE:
            p_str = "S_FALSE";
            break;

        case E_NOTIMPL:
            p_str = "E_NOTIMPL";
            break;

        case E_OUTOFMEMORY:
            p_str = "E_OUTOFMEMORY";
            break;

        case E_INVALIDARG:
            p_str = "E_INVALIDARG";
            break;

        case E_FAIL:
            p_str = "E_FAIL";
            break;

        case D3DERR_WASSTILLDRAWING :
            p_str = "D3DERR_WASSTILLDRAWING";
            break;

        case DXGI_ERROR_WAS_STILL_DRAWING:
            p_str = "DXGI_ERROR_WAS_STILL_DRAWING";
            break;

        case D3DERR_INVALIDCALL :
            p_str = "D3DERR_INVALIDCALL";
            break;

        case DXGI_ERROR_INVALID_CALL:
            p_str = "DXGI_ERROR_INVALID_CALL";
            break;

        case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD:
            p_str = "D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD";
            break;

        case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS:
            p_str = "D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS";
            break;

        case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
            p_str = "D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS";
            break;

        case D3D11_ERROR_FILE_NOT_FOUND:
            p_str = "D3D11_ERROR_FILE_NOT_FOUND";
            break;
       
        default:			
            char err[64];
            sprintf( err, "Unknown D3D11 error (%.8x)", p_hRes );

            p_str = err;
            break;
    }
}
