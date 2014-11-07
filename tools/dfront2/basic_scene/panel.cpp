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
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listCtrl1 = new wxListCtrl( m_camerasPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_ICON );
	m_listCtrl1->SetMinSize( wxSize( 400,200 ) );
	
	bSizer5->Add( m_listCtrl1, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_deleteCameraButton = new wxButton( m_camerasPanel, wxID_ANY, wxT("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_deleteCameraButton, 0, wxALL, 5 );
	
	
	bSizer5->Add( bSizer6, 1, wxEXPAND, 5 );
	
	
	bSizer4->Add( bSizer5, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_camerasPanel, wxID_ANY, wxT("New camera") ), wxVERTICAL );
	
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 0, 2, 0, 0 );
	
	m_staticText1 = new wxStaticText( m_camerasPanel, wxID_ANY, wxT("Scene name :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	gSizer1->Add( m_staticText1, 0, wxALL, 5 );
	
	m_textCtrl1 = new wxTextCtrl( m_camerasPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_textCtrl1, 0, wxALL, 5 );
	
	m_staticText2 = new wxStaticText( m_camerasPanel, wxID_ANY, wxT("Movement :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	gSizer1->Add( m_staticText2, 0, wxALL, 5 );
	
	m_comboBox2 = new wxComboBox( m_camerasPanel, wxID_ANY, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	gSizer1->Add( m_comboBox2, 0, wxALL, 5 );
	
	m_staticText3 = new wxStaticText( m_camerasPanel, wxID_ANY, wxT("Longlat movement :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	gSizer1->Add( m_staticText3, 0, wxALL, 5 );
	
	m_comboBox3 = new wxComboBox( m_camerasPanel, wxID_ANY, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	gSizer1->Add( m_comboBox3, 0, wxALL, 5 );
	
	m_staticText4 = new wxStaticText( m_camerasPanel, wxID_ANY, wxT("Attached body :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	gSizer1->Add( m_staticText4, 0, wxALL, 5 );
	
	m_comboBox4 = new wxComboBox( m_camerasPanel, wxID_ANY, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	gSizer1->Add( m_comboBox4, 0, wxALL, 5 );
	
	m_staticText5 = new wxStaticText( m_camerasPanel, wxID_ANY, wxT("Lock on :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	gSizer1->Add( m_staticText5, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	m_radioBtn6 = new wxRadioButton( m_camerasPanel, wxID_ANY, wxT("Body"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_radioBtn6, 0, wxALL, 5 );
	
	m_radioBtn7 = new wxRadioButton( m_camerasPanel, wxID_ANY, wxT("Transform node"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_radioBtn7, 0, wxALL, 5 );
	
	
	bSizer8->Add( bSizer9, 1, wxEXPAND, 5 );
	
	m_comboBox6 = new wxComboBox( m_camerasPanel, wxID_ANY, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer8->Add( m_comboBox6, 0, wxALL, 5 );
	
	
	gSizer1->Add( bSizer8, 1, wxEXPAND, 5 );
	
	
	sbSizer1->Add( gSizer1, 1, wxEXPAND, 5 );
	
	m_button6 = new wxButton( m_camerasPanel, wxID_ANY, wxT("Create !"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_button6, 0, wxALL, 5 );
	
	
	bSizer4->Add( sbSizer1, 1, wxEXPAND, 5 );
	
	
	m_camerasPanel->SetSizer( bSizer4 );
	m_camerasPanel->Layout();
	bSizer4->Fit( m_camerasPanel );
	m_notebook1->AddPage( m_camerasPanel, wxT("Cameras"), true );
	m_passesPanel = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listBox2 = new wxListBox( m_passesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_listBox2->SetMinSize( wxSize( 300,200 ) );
	
	bSizer81->Add( m_listBox2, 0, wxALL, 5 );
	
	
	bSizer7->Add( bSizer81, 1, wxEXPAND, 5 );
	
	
	m_passesPanel->SetSizer( bSizer7 );
	m_passesPanel->Layout();
	bSizer7->Fit( m_passesPanel );
	m_notebook1->AddPage( m_passesPanel, wxT("Passes"), false );
	m_resourcesPanel = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebook1->AddPage( m_resourcesPanel, wxT("Resources"), false );
	m_scenePanel = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebook1->AddPage( m_scenePanel, wxT("Scene"), false );
	
	bSizer1->Add( m_notebook1, 1, wxEXPAND | wxALL, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

Panel::~Panel()
{
}
