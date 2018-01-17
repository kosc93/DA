#ifndef COLOR_BOX_H_
#define COLOR_BOX_H_
#include <dlib/gui_widgets.h>
using namespace dlib;

class color_box : public draggable{
    unsigned char red, green,blue;
    public:
        color_box(drawable_window& w,
        rectangle area,
        unsigned char red_,
        unsigned char green_,
        unsigned char blue_ );
        ~color_box();
    private:
        void draw(const canvas& c) const;
};


#endif
