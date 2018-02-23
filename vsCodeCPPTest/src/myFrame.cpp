#include "myFrame.h"

using namespace std;

MyFrame::MyFrame(const wxString& title):

wxFrame(NULL, wxID_ANY, title, wxDefaultPosition,wxSize(800,1000)){
    classifier= nullptr;
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
	  settings_control = new SettingsControl(this);
	  control_sizer->Add(settings_control,8,wxEXPAND);
      wxBoxSizer* data_sizer = new wxBoxSizer(wxVERTICAL);
	  this->data_visualizer = new DataVisualizer(this);
	  data_sizer->Add(data_visualizer,1,wxEXPAND);
     main_sizer->Add(control_sizer,1,wxEXPAND);
     main_sizer->Add(data_sizer,3,wxEXPAND);

    this->SetSizer(main_sizer);
    this->SetAutoLayout(true);
}

MyFrame::~MyFrame(){

}

void MyFrame::OnHello(wxCommandEvent& event){
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
  file_dialog->Destroy();
  if(new_data.size()!=0){
      LabelHelper* label_helper= new LabelHelper();
      label_helper->show(new_data);
      //todo implement a conditional append to data vector
      //a.insert(std::end(a), std::begin(b), std::end(b));
      for(const auto& new_measurement:new_data){
	  bool is_new = true;
	  for(const auto& measuerment:data){
	      if(new_measurement.date==measuerment.date){
		  is_new=false;
		  continue;
	      }
	  }
	  if(is_new){
	      data.push_back(new_measurement);
	  }
      }
      data_visualizer->update(data);//maybe substitute for a custom event

      this->Refresh();
  }


}

void
MyFrame::OnExport (wxCommandEvent& event)
{
  ClassifierParam param;

  wxFileDialog* dialog = new wxFileDialog(this,wxEmptyString,wxEmptyString,wxT("export.hdf5"),wxEmptyString,wxFD_SAVE);
  if(dialog->ShowModal()==wxID_OK){
        wxString  wx_path = dialog->GetPath();
        std::string path = std::string(wx_path.mb_str());
        H5MeasurementFile hdf5_file(path);

        if(!settings_control->get_classifier_Param(param)){
           hdf5_file.export_data(data);
          }else{
        hdf5_file.export_data(data,param);
          }
    }
    dialog->Destroy();
}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_EXPORT, MyFrame::OnExport)
    EVT_MENU(ID_IMPORT, MyFrame::OnImport)
wxEND_EVENT_TABLE()


