#ifndef MYFRAME_H_
#define MYFRAME_H_
#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/stattext.h>
#include <string>
#include <memory>
#include <vector>
#include "bin_classifier.h"
#include "draw_canvas.h"
#include "measurement_data.h"
#include "helpers.h"
#include "Graph.h"
#include <utility>
#include <math.h>
#define NUM_PENS 8

using namespace std;

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
  const wxPen* pens[8]={wxBLACK_PEN,wxBLUE_PEN,wxCYAN_PEN,wxGREEN_PEN,wxYELLOW_PEN,wxGREY_PEN,wxRED_PEN,wxLIGHT_GREY_PEN};
  const Channel channel;
  Graph graph;
  double ordinate_start;
  double ordinate_end;
};

class MyFrame : public wxFrame{
  //DrawCanvas* dc;

  public:
        MyFrame(const wxString& title);
        ~MyFrame();
    private:
        DataVisualizer* data_visualizer;
        std::vector<Measurement> data;
        void OnHello(wxCommandEvent& event);
        void OnTrain(wxCommandEvent& event);
        void OnImport(wxCommandEvent& event);
	//unique_ptr<RVMClassifier> classifier;
    DECLARE_EVENT_TABLE();
};


const int ID_IMPORT =1;
const int ID_TRAIN =2;
const int ID_EXPORT =3;

#endif
