#ifndef MYFRAME_H_
#define MYFRAME_H_
#include <wx/wx.h>
#include <memory>
#include "bin_classifier.h"
#include "draw_canvas.h"

using namespace std;

class MyFrame : public wxFrame{
  DrawCanvas* dc;
  public:
        MyFrame(const wxString& title);
        ~MyFrame();
    private:
        void OnHello(wxCommandEvent& event);
        void OnTrain(wxCommandEvent& event);
	//unique_ptr<RVMClassifier> classifier;
    DECLARE_EVENT_TABLE();
};

const int ID_Hello =1;
const int ID_TRAIN =2;
#endif
