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

using namespace std;

class MyFrame : public wxFrame{
  //DrawCanvas* dc;
  public:
        MyFrame(const wxString& title);
        ~MyFrame();
    private:
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
