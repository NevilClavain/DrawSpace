/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

#ifndef _RENDERINGNODE_H_
#define _RENDERINGNODE_H_

#include "drawspace_commons.h"
#include "matrix.h"
#include "fx.h"
#include "texture.h"
#include "meshe.h"
#include "linemeshe.h"
#include "callback.h"

namespace DrawSpace
{
namespace Core
{
struct RenderingNode
{
public:
    static constexpr int NeutralOrder = 10000;
    static constexpr int NbMaxTextures = 32;

    using ShadersParams = struct
    {
        long						shader_index;
        long						param_register;

        bool						vector; // si true, "param_values" est valide, sinon c'est "mat"
        Utils::Vector				param_values;
        Utils::Matrix				mat;
    };

	using ShadersArrayParam = struct
	{
		long						shader_index;
		long						begin_register;
		std::vector<Utils::Vector>	array;
	};

    dsstring									m_debug_id;

public:
    RenderingNode( void );
    virtual ~RenderingNode( void );

    virtual void SetTexture( Texture* p_texture, long p_stage );
    virtual void SetVertexTexture( Texture* p_texture, long p_stage );

    virtual void SetMeshe( Meshe* p_meshe )
    {
        m_meshe = p_meshe;
    }

    virtual Meshe* GetMeshe( void ) const
    {
        return m_meshe;
    }

    virtual void SetLineMeshe(LineMeshe* p_linemeshe)
    {
        m_linemeshe = p_linemeshe;
    }

    virtual LineMeshe* GetLineMeshe(void) const
    {
        return m_linemeshe;
    }

    virtual Fx* GetFx( void ) const
    {
        return m_fx;
    }

    virtual void SetFx( Fx* p_fx )
    {
        m_fx = p_fx;
    }


    static  long        GetTextureListSize( void );
    virtual Texture*    GetTexture( long p_index ) const;
    virtual Texture*    GetVertexTexture( long p_index ) const;

    virtual void OnDraw( void );
    virtual void RegisterHandler( BaseCallback<void, RenderingNode*>* p_handler );
    virtual long GetOrderNumber( void ) const;
    virtual void SetOrderNumber( long p_order );

    virtual void AddShaderParameter( long p_shader_index, const dsstring& p_id, long p_register );
    virtual void SetShaderReal( const dsstring& p_id, dsreal p_value );
    virtual void SetShaderRealVector( const dsstring& p_id, const Utils::Vector& p_value );
	virtual void SetShaderRealInVector(const dsstring& p_id, int p_index_in_vector, dsreal p_value );
    virtual void SetShaderRealMatrix( const dsstring& p_id, const Utils::Matrix& p_value );
    virtual void SetShaderBool( const dsstring& p_id, bool p_value );
    virtual void CleanupShaderParams( void );

	virtual void AddShaderArrayParameter(long p_shader_index, const dsstring& p_id, long p_begin_register);
	virtual void SetShaderArrayParameter(const dsstring& p_id, const std::vector<Utils::Vector>& p_array);
	virtual void CleanupShaderArrayParams(void);

    virtual void UpdateShaderParams( const dsstring& p_id, ShadersParams& p_params );

    virtual void GetShadersParams( std::map<dsstring, ShadersParams*>& p_outlist ) const;

	virtual void GetShadersArrayParams(std::map<dsstring, ShadersArrayParam*>& p_outlist) const;

    virtual void SetDrawingState( bool p_drawing );

    friend class RenderingQueue;

protected:
    Meshe*                                      m_meshe{ nullptr };

private:

    Fx*                                         m_fx{ nullptr };
    Texture*                                    m_textures[NbMaxTextures]; // 32 textures stages max
    Texture*                                    m_vertextextures[NbMaxTextures];

    
    LineMeshe*                                  m_linemeshe{ nullptr };

    std::map<dsstring, ShadersParams*>			m_shader_params;

    std::map<dsstring, ShadersArrayParam*>		m_shaders_array_params;

    long										m_order{ 10000 };

    BaseCallback<void, RenderingNode*>*         m_handler{ nullptr };

    bool										m_drawing_enabled{ TRUE };
};
}
}

#endif
