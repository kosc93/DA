#ifndef MYFRAME_H_
#define MYFRAME_H_
#include <wx/wx.h>
#include <string>
#include <vector>
#include "measurement_data.h"
#include <utility>
#include <math.h>
#include <thread>
#include "custom_widgets.h"
#include "defines_etc.h"

using namespace std;

class MyFrame : public wxFrame{
  BinClassifier* classifier;
  std::vector<Measurement> data;
  public:
        MyFrame(const wxString& title);
        ~MyFrame();
  private:
        DataVisualizer* data_visualizer;
        SettingsControl* settings_control;

        void OnHello(wxCommandEvent& event);
        void OnImport(wxCommandEvent& event);
        void OnExport(wxCommandEvent& event);
    DECLARE_EVENT_TABLE();
};







#endif
