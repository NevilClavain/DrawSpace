#pragma once

#include <d3dx11.h>
#include <vector>
#include "panel.h"



class DConfMainDialog : public MainDialog
{
protected:
    /*
	typedef struct
	{
		UINT width; 
		UINT height;
		UINT refresh_rate;
		D3DFORMAT format;
		std::string comment;

	} adapter_mode;

	typedef struct
	{
		D3DADAPTER_IDENTIFIER9		infos;
		std::vector<adapter_mode>	modes;

	} adapter_infos;

    LPDIRECT3D9								m_lpd3d;
    */

	typedef struct
	{
		UINT            width; 
		UINT            height;
		UINT            refresh_rate;
		DXGI_FORMAT     format;
		std::string     comment;

	} adapter_mode;

	typedef struct
	{
		DXGI_ADAPTER_DESC 		    infos;
		std::vector<adapter_mode>	modes;

	} adapter_infos;

	bool									m_fullscreen;


	std::vector<adapter_infos>				m_adapters_infos;

    virtual void OnMainDialogClose( wxCloseEvent& event );
    virtual void OnSaveButtonClick( wxCommandEvent& event );

public:
    DConfMainDialog( wxWindow* parent, const wxString& title );
    virtual ~DConfMainDialog( void );
};

