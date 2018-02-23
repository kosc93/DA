#ifndef MYAPP_H_
#define MYAPP_H_
#include <wx/wx.h>


class MyApp : public wxApp{
public:
    virtual bool OnInit() override;
    //virtual int OnExit() override;
};

#endif
