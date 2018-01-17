#ifndef WIN_H_
#define WIN_H_
#include <dlib/gui_widgets.h>
#include "color_box.h"
#include <dlib/image_transforms.h>
#include "mdot.h"
using namespace dlib;


class win: public drawable_window{
    public:
        win();
        ~win();
    private:
        unsigned long counter;
        label c;
        button b;
        std::vector<mdot> dots;
        color_box cb;
        menu_bar mbar;
        
        void on_mouse_up( unsigned long btn,
            unsigned long state,
            long x,
            long y);
        void on_button_clicked();
        void show_about();
        //void paint(const canvas& c);

};

#endif