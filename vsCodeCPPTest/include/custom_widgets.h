/*
 * custom_widgets.h
 *
 *  Created on: Feb 6, 2018
 *      Author: kosc93
 */

#ifndef CUSTOM_WIDGETS_H_
#define CUSTOM_WIDGETS_H_

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <string>
#include <memory>
#include <vector>
#include "bin_classifier.h"
#include "measurement_data.h"
#include "BasicPicture.h"
#include "Graph.h"
#include <utility>
#include <math.h>
#include "defines_etc.h"



class LabelHelper : public wxDialog{
  public:
    LabelHelper();
    void show(std::vector<Measurement>& measurements);

};


class DataVisualizer : public wxNotebook{
  public:
    DataVisualizer(wxWindow* parent);
    void update(std::vector<Measurement>& data);
};

class SParamPicture : public BasicPicture{
public:
  SParamPicture(wxWindow* parent,const Channel& channel_);
  void draw(wxDC& dc) override;
private:
  wxWindow* p;
  const wxPen* pens[8]={wxBLACK_PEN,wxBLUE_PEN,wxCYAN_PEN,wxGREEN_PEN,wxYELLOW_PEN,wxGREY_PEN,wxRED_PEN,wxLIGHT_GREY_PEN};
  const Channel channel;
  Graph graph;
  double ordinate_start;
  double ordinate_end;
};

class SettingsControl : public wxPanel{
public:
  SettingsControl(wxWindow* parent);
  bool get_classifier_Param(ClassifierParam& param_);
private:
  TrainerType trainer_type;
  void OnTrainerSelected(wxCommandEvent& event);
  //void OnTrain(wxCommandEvent& event);
  wxComboBox* selected_trainer;
  wxCheckBox* normalize_cb;
  wxCheckBox* optimize_cb;
  wxSpinCtrl* cc_manifold;
  wxBoxSizer* main_sizer;
  wxPanel* param_panel;
  std::vector<wxTextCtrl*> classifier_params;
  //std::unique_ptr<BinClassifier> classifier;
  DECLARE_EVENT_TABLE();
};

#endif /* INCLUDE_CUSTOM_WIDGETS_H_ */
