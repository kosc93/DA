#ifndef MDOT_H_
#define MDOT_H_
#include <dlib/gui_widgets.h>
using namespace dlib;

class mdot : public label{
    //unsigned char red, green,blue;
    point x_y;
    double radius;
    public:
        mdot(drawable_window& w,
        double radius_,
        long x,
        long y);
        ~mdot();
        mdot(const mdot &obj);
        void sayHello();
    private:
        void draw(const canvas& c)const;
       
}; 
#endif