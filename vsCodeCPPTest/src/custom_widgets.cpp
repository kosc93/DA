/*
 * custom_widgets.cpp
 *
 *  Created on: Feb 6, 2018
 *      Author: kosc93
 */
#include "custom_widgets.h"

LabelHelper::LabelHelper ():
wxDialog(NULL,-1,wxT("please label the data"),wxDefaultPosition, wxSize(250, 230))
{
}

void
LabelHelper::show (std::vector<Measurement>& measurements)
{
  // implement autoload of already labeled datasets
  std::vector<wxTextCtrl*> texts;
  wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    for(const auto& measurement : measurements){
        wxBoxSizer* measurement_sizer = new wxBoxSizer(wxHORIZONTAL);
        wxString str_name(measurement.date.c_str(),wxConvUTF8);
        wxStaticText* measurement_name = new wxStaticText(this,-1,str_name);
        wxTextCtrl* measuremtn_label = new wxTextCtrl(this,-1,wxT("0"));
        texts.push_back(measuremtn_label);
        measurement_sizer->Add(measurement_name,1,wxEXPAND);
        measurement_sizer->Add(measuremtn_label);
        main_sizer->Add(measurement_sizer,1,wxEXPAND);
    }
    wxButton* ok_button = new wxButton(this,wxID_OK,wxT("OK"));
    main_sizer->Add(ok_button,1,wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);
    SetSizer(main_sizer);
    Centre();
  //wxRegEx class_regex=wxT("^\d+$");
  if(ShowModal()==wxID_OK){
        if(measurements.size()!=texts.size()){
  	  throw std::invalid_argument("something went terribly wrong");
        }
        for(unsigned int i=0;i<texts.size();i++){
  	  wxString new_label = texts[i]->GetValue();
  	  if(!new_label.ToDouble(& measurements[i].label)){
  	      throw std::invalid_argument("wrong class entry format");
  	  }

        }
    }

    Destroy();
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
            wxString wx_class_str;
            wx_class_str<<"Label: "<<measurement.label;
            wxStaticText* class_text = new wxStaticText(empty_panel,-1,wx_class_str);
            nb_sizer->Add(class_text,0,wxEXPAND);
            for(const auto& channel : measurement.devices[0].channels){
		  SParamPicture* s_param = new SParamPicture(empty_panel,channel);
		  nb_sizer->Add(s_param,1,wxEXPAND|wxALL,5);
            }
            nb_sizer->Layout();
            empty_panel->SetSizer(nb_sizer);
            wxString wx_str = wxString(measurement.date.c_str(),wxConvUTF8);
      this->AddPage(empty_panel,wx_str);
  }


}


SParamPicture::SParamPicture (wxWindow* parent,const Channel& channel_):
BasicPicture(parent),
p(parent),
channel(channel_)
{
  ordinate_start = std::stod( channel_.traces[0].start_freq, nullptr);
  ordinate_end = std::stod( channel_.traces[0].stop_freq, nullptr);
  graph=Graph();
  graph.init(p,50,30,80,30);
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

SettingsControl::SettingsControl (wxWindow* parent):
    wxPanel(parent),
    trainer_type(TrainerType::NONE)
{

    main_sizer = new wxBoxSizer(wxVERTICAL);
    selected_trainer = new wxComboBox(this,ID_TRAINER_SELECTED,wxT("select Trainer"));
    selected_trainer->Append(wxT("RVM"));
    selected_trainer->Append(wxT("SVM-Nu"));
    selected_trainer->Append(wxT("SVM-C"));
    main_sizer->Add(selected_trainer,0,wxEXPAND);
    normalize_cb = new wxCheckBox(this,-1,wxT("normalize"));
    optimize_cb = new wxCheckBox(this,-1,wxT("optimize"));
    cc_manifold = new wxSpinCtrl(this,-1,wxT("select cross validation manifold"));
    param_panel = new wxPanel(this,-1);
    main_sizer->Add(normalize_cb,0,wxEXPAND);
    main_sizer->Add(optimize_cb,0,wxEXPAND);
    main_sizer->Add(cc_manifold,0,wxEXPAND);
    main_sizer->Add(param_panel);
    this->SetSizer(main_sizer);
    //checkboxes
    //textentrys for params
}

void
SettingsControl::OnTrainerSelected (wxCommandEvent& event)
{
  wxBoxSizer* param_sizer = new wxBoxSizer(wxVERTICAL);
  classifier_params.clear();
  if(selected_trainer->GetValue().IsSameAs(wxT("RVM"))){
      trainer_type = TrainerType::RVM;
      wxTextCtrl* epsilon = new wxTextCtrl(param_panel,-1,wxT("epsilon"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,wxT("eps"));
      classifier_params.push_back(epsilon);
      wxTextCtrl* gamma = new wxTextCtrl(param_panel,-1,wxT("gamma"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,wxT("gam"));
      classifier_params.push_back(gamma);
      wxTextCtrl* max_iter = new wxTextCtrl(param_panel,-1,wxT("max iteration"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,wxT("mit"));
      classifier_params.push_back(max_iter);
  }else if  (selected_trainer->GetValue().IsSameAs(wxT("SVM-Nu"))){
      trainer_type = TrainerType::SVM_NU;
      wxTextCtrl* nu = new wxTextCtrl(param_panel,-1,wxT("nu"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,wxT("nu"));
      classifier_params.push_back(nu);
      wxTextCtrl* gamma = new wxTextCtrl(param_panel,-1,wxT("gamma"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,wxT("gam"));
      classifier_params.push_back(gamma);
  }else if (selected_trainer->GetValue().IsSameAs(wxT("SVM-C"))){
      trainer_type = TrainerType::SVM_C;
      wxTextCtrl* c = new wxTextCtrl(param_panel,-1,wxT("c"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,wxT("c"));
      classifier_params.push_back(c);
      wxTextCtrl* gamma = new wxTextCtrl(param_panel,-1,wxT("gamma"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,wxT("gam"));
      classifier_params.push_back(gamma);
  }else{
      trainer_type = TrainerType::NONE;
      param_panel->SetSizer(param_sizer);
      main_sizer->Layout();
      return;
  }

  for(const auto& param:classifier_params){
      param_sizer->Add(param,0,wxEXPAND);
  }
//  wxButton* save_button  = new wxButton(param_panel,ID_TRAIN,wxT("Save"));
//  param_sizer->Add(save_button,0,wxEXPAND);
  param_panel->SetSizer(param_sizer);
  main_sizer->Layout();
}

template<class T>
void train_classifier(T& classifier){
  classifier.train();
}


bool
SettingsControl::get_classifier_Param (ClassifierParam& param_)
{

  std::vector<double> params;
  bool param_fault=false;
      for(const auto& param:classifier_params){
  	double value;
  	if(param->GetValue().ToDouble(&value)){
  	    param->SetBackgroundColour(wxColour(255,255,255));
  	    params.push_back(value);
  	}else{
  	    param->SetBackgroundColour(wxColour(255,0,0));
  	    param_fault=true;
  	}
      }
  if(param_fault||classifier_params.size()==0)
    return false;
  param_.type = trainer_type;
  param_.normalize = normalize_cb->GetValue();
  param_.cc_manifold = long(cc_manifold->GetValue());
  param_.optimize = optimize_cb->GetValue();
  param_.params=params;
  return true;

}

wxBEGIN_EVENT_TABLE(SettingsControl, wxPanel)
    //EVT_BUTTON(ID_TRAIN, SettingsControl::OnTrain)
    EVT_COMBOBOX(ID_TRAINER_SELECTED, SettingsControl::OnTrainerSelected)
wxEND_EVENT_TABLE()


