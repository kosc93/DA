#include <myApp.h>
#include "myFrame.h"
#include <memory>
#include <typeinfo>
#include "dlib/svm.h"

using namespace std;
using namespace dlib;

bool MyApp::OnInit(){
    MyFrame* app =new MyFrame(wxT("MyApp"));
    app->Show(true);
    return true;
}

IMPLEMENT_APP(MyApp);

//int
//MyApp::OnExit ()
//{
//  return 0;
//}
