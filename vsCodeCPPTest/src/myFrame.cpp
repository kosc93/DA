#include "myFrame.h"


#include <memory>

using namespace std;

MyFrame::MyFrame(const wxString& title):

wxFrame(NULL, wxID_ANY, title, wxDefaultPosition,wxSize(800,1000)){
    wxMenu* menuFile= new wxMenu();
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "&Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu* menuHelp = new wxMenu();
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar* menuBar= new wxMenuBar();
    menuBar->Append(menuFile, "&File" );
    menuBar->Append(menuHelp, "&Help" );
    SetMenuBar( menuBar );
    CreateStatusBar();
    SetStatusText( "Welcome to wxWidgets!" );  
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    dc = new DrawCanvas(this);
    sizer->Add(dc,20,wxEXPAND);
    wxButton* train_button = new wxButton(this,ID_TRAIN,wxT("Train"));

    sizer->Add(train_button,1,wxEXPAND);
    this->SetSizer(sizer);
    this->SetAutoLayout(true);
}

MyFrame::~MyFrame(){
}

void MyFrame::OnHello(wxCommandEvent& event){
}

void MyFrame::OnTrain(wxCommandEvent& event){
  RVMClassifier classifier;
  classifier.import_data(dc->datapoints);
  classifier.train();
}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_Hello,   MyFrame::OnHello)
    EVT_BUTTON(ID_TRAIN, MyFrame::OnTrain)
wxEND_EVENT_TABLE()
