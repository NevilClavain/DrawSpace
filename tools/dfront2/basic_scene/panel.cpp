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
	
	m_specific_button_0 = new wxButton( this, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_specific_button_0, 0, wxALL, 5 );
	
	m_specific_button_1 = new wxButton( this, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_specific_button_1, 0, wxALL, 5 );
	
	
	bSizer12->Add( bSizer13, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer12 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_close_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjectPropertiesDialog::OnCloseButtonClicked ), NULL, this );
	m_apply_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjectPropertiesDialog::OnApplyButtonClicked ), NULL, this );
	m_specific_button_0->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjectPropertiesDialog::OnSpecificButton0Clicked ), NULL, this );
	m_specific_button_1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjectPropertiesDialog::OnSpecificButton1Clicked ), NULL, this );
}

ObjectPropertiesDialog::~ObjectPropertiesDialog()
{
	// Disconnect Events
	m_close_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjectPropertiesDialog::OnCloseButtonClicked ), NULL, this );
	m_apply_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjectPropertiesDialog::OnApplyButtonClicked ), NULL, this );
	m_specific_button_0->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjectPropertiesDialog::OnSpecificButton0Clicked ), NULL, this );
	m_specific_button_1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjectPropertiesDialog::OnSpecificButton1Clicked ), NULL, this );
	
}

MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer25;
	bSizer25 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer28->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxHORIZONTAL );
	
	m_consoleinput_textCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 760,120 ), wxTE_MULTILINE );
	m_consoleinput_textCtrl->SetFont( wxFont( 10, 74, 90, 92, false, wxT("System") ) );
	m_consoleinput_textCtrl->SetForegroundColour( wxColour( 255, 255, 0 ) );
	m_consoleinput_textCtrl->SetBackgroundColour( wxColour( 0, 0, 255 ) );
	
	bSizer31->Add( m_consoleinput_textCtrl, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxVERTICAL );
	
	m_consoleinsend_button = new wxButton( this, wxID_ANY, wxT("Send"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer32->Add( m_consoleinsend_button, 0, wxALL, 5 );
	
	m_consoleinclear_button = new wxButton( this, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer32->Add( m_consoleinclear_button, 0, wxALL, 5 );
	
	m_consoleinload_button = new wxButton( this, wxID_ANY, wxT("Load"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer32->Add( m_consoleinload_button, 0, wxALL, 5 );
	
	
	bSizer31->Add( bSizer32, 1, wxEXPAND, 5 );
	
	
	bSizer28->Add( bSizer31, 0, wxEXPAND, 5 );
	
	
	bSizer25->Add( bSizer28, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook2 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_camerasPanel = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer51;
	sbSizer51 = new wxStaticBoxSizer( new wxStaticBox( m_camerasPanel, wxID_ANY, wxT("CameraPoints") ), wxVERTICAL );
	
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxHORIZONTAL );
	
	m_cameras_listCtrl = new wxListCtrl( m_camerasPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	bSizer20->Add( m_cameras_listCtrl, 1, wxALL|wxEXPAND, 5 );
	
	m_cameraedit_button = new wxButton( m_camerasPanel, wxID_ANY, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( m_cameraedit_button, 0, wxALL, 5 );
	
	
	sbSizer51->Add( bSizer20, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	m_createcamera_button = new wxButton( m_camerasPanel, wxID_ANY, wxT("Create camera"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_createcamera_button, 0, wxALL, 5 );
	
	
	sbSizer51->Add( bSizer10, 0, wxEXPAND, 5 );
	
	
	bSizer81->Add( sbSizer51, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer6;
	sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( m_camerasPanel, wxID_ANY, wxT("Movements") ), wxVERTICAL );
	
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxHORIZONTAL );
	
	m_mvts_listCtrl = new wxListCtrl( m_camerasPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	bSizer16->Add( m_mvts_listCtrl, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );
	
	m_control_button = new wxButton( m_camerasPanel, wxID_ANY, wxT("Control"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer17->Add( m_control_button, 0, wxALL, 5 );
	
	
	bSizer16->Add( bSizer17, 0, wxEXPAND, 5 );
	
	
	sbSizer6->Add( bSizer16, 1, wxEXPAND, 5 );
	
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
	m_mvttype_comboBox->SetSelection( 0 );
	bSizer91->Add( m_mvttype_comboBox, 0, wxALL, 5 );
	
	m_staticText1 = new wxStaticText( m_camerasPanel, wxID_ANY, wxT("controlled :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer91->Add( m_staticText1, 0, wxALL, 5 );
	
	m_mousekeyboardoutput_comboBox = new wxComboBox( m_camerasPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	bSizer91->Add( m_mousekeyboardoutput_comboBox, 0, wxALL, 5 );
	
	
	sbSizer6->Add( bSizer91, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer29->Add( bSizer30, 1, wxEXPAND, 5 );
	
	
	sbSizer6->Add( bSizer29, 1, wxEXPAND, 5 );
	
	
	bSizer81->Add( sbSizer6, 1, wxEXPAND, 5 );
	
	
	m_camerasPanel->SetSizer( bSizer81 );
	m_camerasPanel->Layout();
	bSizer81->Fit( m_camerasPanel );
	m_notebook2->AddPage( m_camerasPanel, wxT("Cameras && Movements"), false );
	m_passesPanel = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer31;
	sbSizer31 = new wxStaticBoxSizer( new wxStaticBox( m_passesPanel, wxID_ANY, wxT("Passes") ), wxVERTICAL );
	
	m_passes_listCtrl = new wxListCtrl( m_passesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	sbSizer31->Add( m_passes_listCtrl, 1, wxALL|wxEXPAND, 5 );
	
	
	bSizer7->Add( sbSizer31, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_passesPanel, wxID_ANY, wxT("Shaders parameters") ), wxVERTICAL );
	
	m_shadersparams_listCtrl = new wxListCtrl( m_passesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	sbSizer4->Add( m_shadersparams_listCtrl, 1, wxALL|wxEXPAND, 5 );
	
	
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
	
	m_assets_listCtrl = new wxListCtrl( m_resourcesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	sbSizer3->Add( m_assets_listCtrl, 1, wxALL|wxEXPAND, 5 );
	
	
	bSizer9->Add( sbSizer3, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( m_resourcesPanel, wxID_ANY, wxT("Configurables") ), wxVERTICAL );
	
	m_configs_listCtrl = new wxListCtrl( m_resourcesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	sbSizer5->Add( m_configs_listCtrl, 1, wxALL|wxEXPAND, 5 );
	
	
	bSizer9->Add( sbSizer5, 1, wxEXPAND, 5 );
	
	
	m_resourcesPanel->SetSizer( bSizer9 );
	m_resourcesPanel->Layout();
	bSizer9->Fit( m_resourcesPanel );
	m_notebook2->AddPage( m_resourcesPanel, wxT("Resources"), false );
	m_scenePanel = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxHORIZONTAL );
	
	m_scenegraph_listCtrl = new wxListCtrl( m_scenePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	bSizer14->Add( m_scenegraph_listCtrl, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	m_transftype_button = new wxButton( m_scenePanel, wxID_ANY, wxT("Transfo source"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_transftype_button, 0, wxALL, 5 );
	
	m_transfoedit_button = new wxButton( m_scenePanel, wxID_ANY, wxT("Transfo edit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_transfoedit_button, 0, wxALL, 5 );
	
	
	bSizer14->Add( bSizer15, 0, wxEXPAND, 5 );
	
	
	bSizer11->Add( bSizer14, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText2 = new wxStaticText( m_scenePanel, wxID_ANY, wxT("Current camera :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer12->Add( m_staticText2, 0, wxALL, 5 );
	
	m_cameraslist_comboBox = new wxComboBox( m_scenePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	bSizer12->Add( m_cameraslist_comboBox, 0, wxALL, 5 );
	
	
	bSizer11->Add( bSizer12, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );
	
	m_create_drawable_button = new wxButton( m_scenePanel, wxID_ANY, wxT("Create Drawable"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_create_drawable_button, 0, wxALL, 5 );
	
	m_drawabletypes_comboBox = new wxComboBox( m_scenePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_drawabletypes_comboBox->Append( wxT("SpaceBox") );
	m_drawabletypes_comboBox->Append( wxT("Chunk") );
	m_drawabletypes_comboBox->SetSelection( 0 );
	bSizer13->Add( m_drawabletypes_comboBox, 0, wxALL, 5 );
	
	
	bSizer11->Add( bSizer13, 1, wxEXPAND, 5 );
	
	m_treeCtrl1 = new wxTreeCtrl( m_scenePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE );
	bSizer11->Add( m_treeCtrl1, 1, wxALL|wxEXPAND, 5 );
	
	
	m_scenePanel->SetSizer( bSizer11 );
	m_scenePanel->Layout();
	bSizer11->Fit( m_scenePanel );
	m_notebook2->AddPage( m_scenePanel, wxT("Scene"), true );
	m_registersPanel = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxHORIZONTAL );
	
	m_registers_listCtrl = new wxListCtrl( m_registersPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	bSizer21->Add( m_registers_listCtrl, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxVERTICAL );
	
	m_modreg_button = new wxButton( m_registersPanel, wxID_ANY, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer22->Add( m_modreg_button, 0, wxALL, 5 );
	
	m_resetreg_button = new wxButton( m_registersPanel, wxID_ANY, wxT("Reset"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer22->Add( m_resetreg_button, 0, wxALL, 5 );
	
	m_regon_button = new wxButton( m_registersPanel, wxID_ANY, wxT("On"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer22->Add( m_regon_button, 0, wxALL, 5 );
	
	m_regoff_button = new wxButton( m_registersPanel, wxID_ANY, wxT("Off"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer22->Add( m_regoff_button, 0, wxALL, 5 );
	
	
	bSizer21->Add( bSizer22, 0, wxEXPAND, 5 );
	
	
	bSizer19->Add( bSizer21, 1, wxEXPAND, 5 );
	
	m_createreg_button = new wxButton( m_registersPanel, wxID_ANY, wxT("Create register"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer19->Add( m_createreg_button, 0, wxALL, 5 );
	
	
	m_registersPanel->SetSizer( bSizer19 );
	m_registersPanel->Layout();
	bSizer19->Fit( m_registersPanel );
	m_notebook2->AddPage( m_registersPanel, wxT("Registers"), false );
	
	bSizer33->Add( m_notebook2, 1, wxEXPAND, 0 );
	
	m_consoleoutput_textCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 300,170 ), wxTE_MULTILINE|wxTE_READONLY );
	m_consoleoutput_textCtrl->SetFont( wxFont( 6, 75, 90, 90, false, wxT("Terminal") ) );
	
	bSizer33->Add( m_consoleoutput_textCtrl, 0, wxALL|wxEXPAND, 0 );
	
	
	bSizer25->Add( bSizer33, 0, wxEXPAND, 5 );
	
	
	bSizer8->Add( bSizer25, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer8 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrame::OnClose ) );
	this->Connect( wxEVT_IDLE, wxIdleEventHandler( MainFrame::OnIdle ) );
	this->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrame::OnKeyDown ) );
	this->Connect( wxEVT_KEY_UP, wxKeyEventHandler( MainFrame::OnKeyUp ) );
	this->Connect( wxEVT_MOTION, wxMouseEventHandler( MainFrame::OnMouseMotion ) );
	m_consoleinsend_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnConsoleInSendButtonClicked ), NULL, this );
	m_consoleinclear_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnConsoleInClearButtonClicked ), NULL, this );
	m_consoleinload_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnConsoleInLoadButtonClicked ), NULL, this );
	m_cameras_listCtrl->Connect( wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, wxListEventHandler( MainFrame::OnCamerasListDeleteAllItems ), NULL, this );
	m_cameras_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnCamerasListItemActivated ), NULL, this );
	m_cameras_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( MainFrame::OnCamerasListItemSelected ), NULL, this );
	m_cameraedit_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnCameraEditButtonClicked ), NULL, this );
	m_createcamera_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnCreateCameraButtonClicked ), NULL, this );
	m_mvts_listCtrl->Connect( wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, wxListEventHandler( MainFrame::OnMvtsListDeleteAllItems ), NULL, this );
	m_mvts_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnMvtsListItemActivated ), NULL, this );
	m_mvts_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( MainFrame::OnMvtsListItemSelected ), NULL, this );
	m_control_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnControlButtonClicked ), NULL, this );
	m_createmvt_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnCreateMvtButtonClicked ), NULL, this );
	m_mousekeyboardoutput_comboBox->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( MainFrame::OnMouseKeyboardOutputCombobox ), NULL, this );
	m_passes_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnPassesListItemActivated ), NULL, this );
	m_passes_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( MainFrame::OnPassesListItemSelected ), NULL, this );
	m_shadersparams_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnShadersListItemActivated ), NULL, this );
	m_assets_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnAssetsListItemActivated ), NULL, this );
	m_configs_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnConfigsListItemActivated ), NULL, this );
	m_scenegraph_listCtrl->Connect( wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, wxListEventHandler( MainFrame::OnScenegraphListDeleteAllItems ), NULL, this );
	m_scenegraph_listCtrl->Connect( wxEVT_COMMAND_LIST_DELETE_ITEM, wxListEventHandler( MainFrame::OnScenegraphListDeleteItem ), NULL, this );
	m_scenegraph_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnScenegraphItemActivated ), NULL, this );
	m_scenegraph_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( MainFrame::OnScenegraphListItemDeselected ), NULL, this );
	m_scenegraph_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( MainFrame::OnScenegraphListRightClick ), NULL, this );
	m_scenegraph_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( MainFrame::OnScenegraphListItemSelected ), NULL, this );
	m_transftype_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnTransfTypeButtonClicked ), NULL, this );
	m_transfoedit_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnTransfoEditButtonClicked ), NULL, this );
	m_create_drawable_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnCreateDrawableButtonClicked ), NULL, this );
	m_treeCtrl1->Connect( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler( MainFrame::OnSceneNodeGraphsListRightClick ), NULL, this );
	m_registers_listCtrl->Connect( wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, wxListEventHandler( MainFrame::OnRegsListDeleteAllItems ), NULL, this );
	m_registers_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnRegistersListItemActivated ), NULL, this );
	m_registers_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( MainFrame::OnRegsListItemSelected ), NULL, this );
	m_modreg_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnModRegButtonClicked ), NULL, this );
	m_resetreg_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnResetRegButtonClicked ), NULL, this );
	m_regon_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnRegOnButtonClicked ), NULL, this );
	m_regoff_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnRegOffButtonClicked ), NULL, this );
	m_createreg_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnCreateRegButtonClicked ), NULL, this );
}

MainFrame::~MainFrame()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrame::OnClose ) );
	this->Disconnect( wxEVT_IDLE, wxIdleEventHandler( MainFrame::OnIdle ) );
	this->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrame::OnKeyDown ) );
	this->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( MainFrame::OnKeyUp ) );
	this->Disconnect( wxEVT_MOTION, wxMouseEventHandler( MainFrame::OnMouseMotion ) );
	m_consoleinsend_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnConsoleInSendButtonClicked ), NULL, this );
	m_consoleinclear_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnConsoleInClearButtonClicked ), NULL, this );
	m_consoleinload_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnConsoleInLoadButtonClicked ), NULL, this );
	m_cameras_listCtrl->Disconnect( wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, wxListEventHandler( MainFrame::OnCamerasListDeleteAllItems ), NULL, this );
	m_cameras_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnCamerasListItemActivated ), NULL, this );
	m_cameras_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( MainFrame::OnCamerasListItemSelected ), NULL, this );
	m_cameraedit_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnCameraEditButtonClicked ), NULL, this );
	m_createcamera_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnCreateCameraButtonClicked ), NULL, this );
	m_mvts_listCtrl->Disconnect( wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, wxListEventHandler( MainFrame::OnMvtsListDeleteAllItems ), NULL, this );
	m_mvts_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnMvtsListItemActivated ), NULL, this );
	m_mvts_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( MainFrame::OnMvtsListItemSelected ), NULL, this );
	m_control_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnControlButtonClicked ), NULL, this );
	m_createmvt_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnCreateMvtButtonClicked ), NULL, this );
	m_mousekeyboardoutput_comboBox->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( MainFrame::OnMouseKeyboardOutputCombobox ), NULL, this );
	m_passes_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnPassesListItemActivated ), NULL, this );
	m_passes_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( MainFrame::OnPassesListItemSelected ), NULL, this );
	m_shadersparams_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnShadersListItemActivated ), NULL, this );
	m_assets_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnAssetsListItemActivated ), NULL, this );
	m_configs_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnConfigsListItemActivated ), NULL, this );
	m_scenegraph_listCtrl->Disconnect( wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, wxListEventHandler( MainFrame::OnScenegraphListDeleteAllItems ), NULL, this );
	m_scenegraph_listCtrl->Disconnect( wxEVT_COMMAND_LIST_DELETE_ITEM, wxListEventHandler( MainFrame::OnScenegraphListDeleteItem ), NULL, this );
	m_scenegraph_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnScenegraphItemActivated ), NULL, this );
	m_scenegraph_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( MainFrame::OnScenegraphListItemDeselected ), NULL, this );
	m_scenegraph_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( MainFrame::OnScenegraphListRightClick ), NULL, this );
	m_scenegraph_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( MainFrame::OnScenegraphListItemSelected ), NULL, this );
	m_transftype_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnTransfTypeButtonClicked ), NULL, this );
	m_transfoedit_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnTransfoEditButtonClicked ), NULL, this );
	m_create_drawable_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnCreateDrawableButtonClicked ), NULL, this );
	m_treeCtrl1->Disconnect( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler( MainFrame::OnSceneNodeGraphsListRightClick ), NULL, this );
	m_registers_listCtrl->Disconnect( wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, wxListEventHandler( MainFrame::OnRegsListDeleteAllItems ), NULL, this );
	m_registers_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( MainFrame::OnRegistersListItemActivated ), NULL, this );
	m_registers_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( MainFrame::OnRegsListItemSelected ), NULL, this );
	m_modreg_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnModRegButtonClicked ), NULL, this );
	m_resetreg_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnResetRegButtonClicked ), NULL, this );
	m_regon_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnRegOnButtonClicked ), NULL, this );
	m_regoff_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnRegOffButtonClicked ), NULL, this );
	m_createreg_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnCreateRegButtonClicked ), NULL, this );
	
}
