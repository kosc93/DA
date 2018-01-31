#include "myFrame.h"

using namespace std;

MyFrame::MyFrame(const wxString& title):

wxFrame(NULL, wxID_ANY, title, wxDefaultPosition,wxSize(800,1000)){
    wxMenu* menuFile= new wxMenu();
    menuFile->Append(ID_IMPORT, "&Import");
    menuFile->Append(ID_EXPORT, "&Export");
    wxMenuBar* menuBar= new wxMenuBar();
    menuBar->Append(menuFile, "&File" );
    SetMenuBar( menuBar );
//    implement a signaling layer in processing classes to display progress
//    CreateStatusBar();
//    SetStatusText( "Welcome to wxWidgets!" );
//    dc = new DrawCanvas(this);
    wxBoxSizer* main_sizer = new wxBoxSizer(wxHORIZONTAL);
      wxBoxSizer* control_sizer = new wxBoxSizer(wxVERTICAL);
	  wxButton* train_button = new wxButton(this,ID_TRAIN,wxT("Train"));
          control_sizer->Add(train_button,1,wxEXPAND);

      wxBoxSizer* data_sizer = new wxBoxSizer(wxVERTICAL);
	  wxNotebook* nb = new wxNotebook(this,-1);
	  wxBoxSizer* nb_sizer = new wxBoxSizer(wxVERTICAL);
	      wxPanel* empty_panel = new wxPanel(nb,-1);
	      wxStaticText* test_text = new wxStaticText(empty_panel,-1,wxT("test text"));
	      nb_sizer->Add(test_text,1,wxEXPAND);
	      empty_panel->SetSizer(nb_sizer);
	  nb->AddPage(empty_panel,wxT("empty page"));
	  data_sizer->Add(nb,1,wxEXPAND);
     main_sizer->Add(control_sizer,1,wxEXPAND);
     main_sizer->Add(data_sizer,1,wxEXPAND);

    this->SetSizer(main_sizer);
    this->SetAutoLayout(true);
}

MyFrame::~MyFrame(){
}

void MyFrame::OnHello(wxCommandEvent& event){
}

void MyFrame::OnTrain(wxCommandEvent& event){
  RVMClassifier classifier;
  //classifier.import_data(dc->datapoints);
  classifier.train();
}

void
MyFrame::OnImport (wxCommandEvent& event)
{
  std::vector<Measurement> new_data;
  wxFileDialog* file_dialog = new wxFileDialog(this);
  if(file_dialog->ShowModal()==wxID_OK){
      wxString  wx_path = file_dialog->GetPath();
      std::string path = std::string(wx_path.mb_str());
      H5MeasurementFile hdf5_file(path);
      new_data=hdf5_file.scan();
  }
  if(new_data.size()!=0){
      //implement labelhelper
  }

}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_IMPORT, MyFrame::OnImport)
    EVT_BUTTON(ID_TRAIN, MyFrame::OnTrain)
wxEND_EVENT_TABLE()
