///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  5 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "panel.h"

///////////////////////////////////////////////////////////////////////////

ObjectsListDialog::ObjectsListDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	m_listCtrl = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	bSizer9->Add( m_listCtrl, 5, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button4 = new wxButton( this, wxID_ANY, wxT("MyButton"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_button4, 0, wxALL, 5 );
	
	
	bSizer9->Add( bSizer11, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer9 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

ObjectsListDialog::~ObjectsListDialog()
{
}

ObjectPropertiesDialog::ObjectPropertiesDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );
	
	m_propertyGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE);
	bSizer12->Add( m_propertyGrid, 5, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );
	
	m_close_button = new wxButton( this, wxID_ANY, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_close_button, 0, wxALL, 5 );
	
	m_apply_button = new wxButton( this, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_apply_button, 0, wxALL, 5 );
	
	
	bSizer12->Add( bSizer13, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer12 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_close_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjectPropertiesDialog::OnCloseButtonClicked ), NULL, this );
	m_apply_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjectPropertiesDialog::OnApplyButtonClicked ), NULL, this );
}

ObjectPropertiesDialog::~ObjectPropertiesDialog()
{
	// Disconnect Events
	m_close_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjectPropertiesDialog::OnCloseButtonClicked ), NULL, this );
	m_apply_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjectPropertiesDialog::OnApplyButtonClicked ), NULL, this );
	
}

MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer8->Add( 0, 0, 3, wxEXPAND, 5 );
	
	m_notebook2 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_camerasPanel = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer51;
	sbSizer51 = new wxStaticBoxSizer( new wxStaticBox( m_camerasPanel, wxID_ANY, wxT("CameraPoints") ), wxVERTICAL );
	
	m_cameras_listCtrl = new wxListCtrl( m_camerasPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	sbSizer51->Add( m_cameras_listCtrl, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	m_createcamera_button = new wxButton( m_camerasPanel, wxID_ANY, wxT("Create camera"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_createcamera_button, 0, wxALL, 5 );
	
	
	sbSizer51->Add( bSizer10, 1, wxEXPAND, 5 );
	
	
	bSizer81->Add( sbSizer51, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer6;
	sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( m_camerasPanel, wxID_ANY, wxT("Movements") ), wxVERTICAL );
	
	m_mvts_listCtrl = new wxListCtrl( m_camerasPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	sbSizer6->Add( m_mvts_listCtrl, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer( wxHORIZONTAL );
	
	m_createmvt_button = new wxButton( m_camerasPanel, wxID_ANY, wxT("Create movement"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer91->Add( m_createmvt_button, 0, wxALL, 5 );
	
	m_mvttype_comboBox = new wxComboBox( m_camerasPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
	m_mvttype_comboBox->Append( wxT("Linear") );
	m_mvttype_comboBox->Append( wxT("Circular") );
	m_mvttype_comboBox->Append( wxT("FPS") );
	m_mvttype_comboBox->Append( wxT("Free") );
	m_mvttype_comboBox->Append( wxT("Head") );
	m_mvttype_comboBox->Append( wxT("Spectator") );
	m_mvttype_comboBox->Append( wxT("LongLat") );
	m_mvttype_comboBox->Append( wxEmptyString );
	m_mvttype_comboBox->Append( wxEmptyString );
	m_mvttype_comboBox->SetSelection( 0 );
	bSizer91->Add( m_mvttype_comboBox, 0, wxALL, 5 );
	
	
	sbSizer6->Add( bSizer91, 1, wxEXPAND, 5 );
	
	
	bSizer81->Add( sbSizer6, 1, wxEXPAND, 5 );
	
	
	m_camerasPanel->SetSizer( bSizer81 );
	m_camerasPanel->Layout();
	bSizer81->Fit( m_camerasPanel );
	m_notebook2->AddPage( m_camerasPanel, wxT("Cameras"), true );
	m_passesPanel = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer31;
	sbSizer31 = new wxStaticBoxSizer( new wxStaticBox( m_passesPanel, wxID_ANY, wxT("Passes") ), wxVERTICAL );
	
	m_passes_listCtrl = new wxListCtrl( m_passesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	sbSizer31->Add( m_passes_listCtrl, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer7->Add( sbSizer31, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_passesPanel, wxID_ANY, wxT("Shaders parameters") ), wxVERTICAL );
	
	m_shadersparams_listCtrl = new wxListCtrl( m_passesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	sbSizer4->Add( m_shadersparams_listCtrl, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer7->Add( sbSizer4, 1, wxEXPAND, 5 );
	
	
	m_passesPanel->SetSizer( bSizer7 );
	m_passesPanel->Layout();
	bSizer7->Fit( m_passesPanel );
	m_notebook2->AddPage( m_passesPanel, wxT("Passes"), false );
	m_resourcesPanel = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_resourcesPanel, wxID_ANY, wxT("Assets") ), wxVERTICAL );
	
	m_assets_listCtrl = new wxListCtrl( m_resourcesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	sbSizer3->Add( m_assets_listCtrl, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer9->Add( sbSizer3, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( m_resourcesPanel, wxID_ANY, wxT("Configurables") ), wxVERTICAL );
	
	m_configs_listCtrl = new wxListCtrl( m_resourcesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	sbSizer5->Add( m_configs_listCtrl, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer9->Add( sbSizer5, 1, wxEXPAND, 5 );
	
	
	m_resourcesPanel->SetSizer( bSizer9 );
	m_resourcesPanel->Layout();
	bSizer9->Fit( m_resourcesPanel );
	m_notebook2->AddPage( m_resourcesPanel, wxT("Resources"), false );
	m_scenePanel = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebook2->AddPage( m_scenePanel, wxT("Scene"), false );
	
	bSizer8->Add( m_notebook2, 1, wxEXPAND | wxALL, 5 );
	
	
	this->SetSizer( bSizer8 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrame::OnClose ) );
	this->Connect( wxEVT_IDLE, wxIdleEventHandler( MainFrame::OnIdle ) );
	m_createcamera_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnCreateCameraButtonClicked ), NULL, this );
	m_mvts_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnMvtsListItemActivated ), NULL, this );
	m_createmvt_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnCreateMvtButtonClicked ), NULL, this );
	m_passes_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnPassesListItemActivated ), NULL, this );
	m_passes_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( MainFrame::OnPassesListItemSelected ), NULL, this );
	m_shadersparams_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnShadersListItemActivated ), NULL, this );
	m_assets_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnAssetsListItemActivated ), NULL, this );
	m_configs_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnConfigsListItemActivated ), NULL, this );
}

MainFrame::~MainFrame()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrame::OnClose ) );
	this->Disconnect( wxEVT_IDLE, wxIdleEventHandler( MainFrame::OnIdle ) );
	m_createcamera_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnCreateCameraButtonClicked ), NULL, this );
	m_mvts_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnMvtsListItemActivated ), NULL, this );
	m_createmvt_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnCreateMvtButtonClicked ), NULL, this );
	m_passes_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnPassesListItemActivated ), NULL, this );
	m_passes_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( MainFrame::OnPassesListItemSelected ), NULL, this );
	m_shadersparams_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnShadersListItemActivated ), NULL, this );
	m_assets_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnAssetsListItemActivated ), NULL, this );
	m_configs_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnConfigsListItemActivated ), NULL, this );
	
}
