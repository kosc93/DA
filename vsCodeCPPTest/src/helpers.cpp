#include "helpers.h"

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
//    for(std::vector<wxTextCtrl*>::iterator it = texts.begin();it!=texts.end();it++){
//	delete (*it);
//    }
    Destroy();
}
/*
 * helpers.cpp
 *
 *  Created on: Feb 1, 2018
 *      Author: kosc93
 */




