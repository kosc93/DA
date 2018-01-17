#include <myApp.h>
#include "myFrame.h"
#include <memory>
#include <typeinfo>
#include "dlib/svm.h"

using namespace std;
using namespace dlib;
bool MyApp::OnInit(){
    if(typeid(matrix<double>)==typeid(matrix<double,1,3>))
      int a =1;
    MyFrame* app =new MyFrame(wxT("MyApp"));
    app->Show(true);
    return true;
}

IMPLEMENT_APP(MyApp);
