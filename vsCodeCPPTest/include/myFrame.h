#ifndef MYFRAME_H_
#define MYFRAME_H_
#include <wx/wx.h>
#include "draw_canvas.h"
class MyFrame : public wxFrame{
  DrawCanvas* dc;
  public:
        MyFrame(const wxString& title);
        ~MyFrame();
    private:
        void OnHello(wxCommandEvent& event);
        void OnTrain(wxCommandEvent& event);

    DECLARE_EVENT_TABLE();
};

const int ID_Hello =1;
const int ID_TRAIN =2;
#endif
