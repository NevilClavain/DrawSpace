#pragma once

//#include <d3dx9.h>
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
	bool									m_fullscreen;


	//std::vector<adapter_infos>				m_adapters_infos;

    virtual void OnMainDialogClose( wxCloseEvent& event );
    virtual void OnSaveButtonClick( wxCommandEvent& event );

public:
    DConfMainDialog( wxWindow* parent, const wxString& title );
    virtual ~DConfMainDialog( void );
};

