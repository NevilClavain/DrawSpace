///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  5 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "panel.h"

///////////////////////////////////////////////////////////////////////////

Panel::Panel( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_camerasPanel = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebook1->AddPage( m_camerasPanel, wxT("Cameras"), false );
	m_passesPanel = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebook1->AddPage( m_passesPanel, wxT("Passes"), false );
	m_resourcesPanel = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_resourcesPanel, wxID_ANY, wxT("Assets") ), wxVERTICAL );
	
	m_assets_listCtrl = new wxListCtrl( m_resourcesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	sbSizer4->Add( m_assets_listCtrl, 2, wxEXPAND, 5 );
	
	
	bSizer15->Add( sbSizer4, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer6;
	sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( m_resourcesPanel, wxID_ANY, wxT("Configurables") ), wxVERTICAL );
	
	m_configs_listCtrl = new wxListCtrl( m_resourcesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	sbSizer6->Add( m_configs_listCtrl, 2, wxEXPAND, 5 );
	
	
	bSizer15->Add( sbSizer6, 0, wxEXPAND, 5 );
	
	
	bSizer14->Add( bSizer15, 1, 0, 5 );
	
	
	m_resourcesPanel->SetSizer( bSizer14 );
	m_resourcesPanel->Layout();
	bSizer14->Fit( m_resourcesPanel );
	m_notebook1->AddPage( m_resourcesPanel, wxT("Resources"), true );
	m_scenePanel = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebook1->AddPage( m_scenePanel, wxT("Scene"), false );
	
	bSizer1->Add( m_notebook1, 1, wxEXPAND | wxALL, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_assets_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( Panel::OnAssetsListItemActivated ), NULL, this );
}

Panel::~Panel()
{
	// Disconnect Events
	m_assets_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( Panel::OnAssetsListItemActivated ), NULL, this );
	
}

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
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	m_close_button = new wxButton( this, wxID_ANY, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_close_button, 0, wxALL, 5 );
	
	
	bSizer12->Add( bSizer13, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer12 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_close_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjectPropertiesDialog::OnCloseButtonClicked ), NULL, this );
}

ObjectPropertiesDialog::~ObjectPropertiesDialog()
{
	// Disconnect Events
	m_close_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjectPropertiesDialog::OnCloseButtonClicked ), NULL, this );
	
}

MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer8->Add( 0, 0, 3, wxEXPAND, 5 );
	
	m_notebook2 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_camerasPanel = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebook2->AddPage( m_camerasPanel, wxT("Cameras"), false );
	m_passesPanel = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
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
	
	m_listCtrl5 = new wxListCtrl( m_resourcesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	sbSizer5->Add( m_listCtrl5, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer9->Add( sbSizer5, 1, wxEXPAND, 5 );
	
	
	m_resourcesPanel->SetSizer( bSizer9 );
	m_resourcesPanel->Layout();
	bSizer9->Fit( m_resourcesPanel );
	m_notebook2->AddPage( m_resourcesPanel, wxT("Resources"), true );
	m_scenePanel = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebook2->AddPage( m_scenePanel, wxT("Scene"), false );
	
	bSizer8->Add( m_notebook2, 1, wxEXPAND | wxALL, 5 );
	
	
	this->SetSizer( bSizer8 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrame::OnClose ) );
	this->Connect( wxEVT_IDLE, wxIdleEventHandler( MainFrame::OnIdle ) );
	m_assets_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnAssetsListItemActivated ), NULL, this );
}

MainFrame::~MainFrame()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrame::OnClose ) );
	this->Disconnect( wxEVT_IDLE, wxIdleEventHandler( MainFrame::OnIdle ) );
	m_assets_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnAssetsListItemActivated ), NULL, this );
	
}
