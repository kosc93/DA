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
	  this->data_visualizer = new DataVisualizer(this);
	  data_sizer->Add(data_visualizer,1,wxEXPAND);
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
  std::vector<DataPoint> datapoints;
  for(auto& measurement : data){
      datapoints.push_back(measurement.getDatapoint(0));
  }
  classifier.import_data(datapoints);
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
  file_dialog->Destroy();
  if(new_data.size()!=0){
      LabelHelper* label_helper= new LabelHelper();
      label_helper->show(new_data);
      data=std::move(new_data);
      data_visualizer->update(data);
  }


}



DataVisualizer::DataVisualizer (wxWindow* parent):wxNotebook(parent,-1)
{

  wxBoxSizer* nb_sizer = new wxBoxSizer(wxVERTICAL);
      wxPanel* empty_panel = new wxPanel(this,-1);
      wxStaticText* test_text = new wxStaticText(empty_panel,-1,wxT("test text"));
      nb_sizer->Add(test_text,1,wxEXPAND);
      empty_panel->SetSizer(nb_sizer);
  this->AddPage(empty_panel,wxT("empty page"));
}

void
DataVisualizer::update (std::vector<Measurement>& data)
{
  unsigned int num_pages = this->GetPageCount();
  for(unsigned int i=0;i<num_pages;i++){
      this->RemovePage(0);
  }
  for(const auto& measurement : data){
      wxBoxSizer* nb_sizer = new wxBoxSizer(wxVERTICAL);
            wxPanel* empty_panel = new wxPanel(this,-1);
            for(const auto& channel : measurement.devices[0].channels){
		  SParamPicture* s_param = new SParamPicture(empty_panel,channel);
		  nb_sizer->Add(s_param,1,wxEXPAND|wxALL,5);
		  s_param->Refresh();
            }
            empty_panel->SetSizer(nb_sizer);
            wxString wx_str = wxString(measurement.date.c_str(),wxConvUTF8);
      this->AddPage(empty_panel,wx_str);
  }


}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_IMPORT, MyFrame::OnImport)
    EVT_BUTTON(ID_TRAIN, MyFrame::OnTrain)
wxEND_EVENT_TABLE()

SParamPicture::SParamPicture (wxWindow* parent,const Channel& channel_):
BasicPicture(parent),
channel(channel_)
{
  ordinate_start = std::stod( channel_.traces[0].start_freq, nullptr);
  ordinate_end = std::stod( channel_.traces[0].stop_freq, nullptr);
  graph=Graph();
  graph.init(parent,50,30,80,500);
  graph.initAbscissa(PQ_FREQUENCY,0,100,ordinate_start,ordinate_start,ordinate_start,ordinate_end,ordinate_end,ordinate_end,3,3,false,false,true);
  graph.initLinearOrdinate(PQ_RATIO,0,0.2,0,0,0,1,1,1,10,2,false,false,true);
}

void
SParamPicture::draw (wxDC& dc)
{
  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();
  graph.paintAbscissa(dc);
  graph.paintOrdinate(dc);
  unsigned int pen_selector=0;
  for(const auto& trace : channel.traces){
    double delta_freq = (ordinate_end-ordinate_start)/trace.num_points;
    dc.SetPen(*pens[pen_selector%NUM_PENS]);
   for(unsigned int i = 1;i<trace.real.size();i++){
       double x1 = ordinate_start + (i-1)*delta_freq;
       double x2 = ordinate_start + (i)*delta_freq;
       double y1 = sqrt(trace.real[i-1]*trace.real[i-1]+trace.imag[i-1]*trace.imag[i-1]);
       double y2 = sqrt(trace.real[i]*trace.real[i]+trace.imag[i]*trace.imag[i]);
       int x_1 = graph.getXPos(x1);
       int x_2 = graph.getXPos(x2);
       int y_1 = graph.getYPos(y1);
       int y_2 = graph.getYPos(y2);
       dc.DrawLine(wxPoint(x_1,y_1),wxPoint(x_2,y_2));
   }
   pen_selector++;
  }

}
