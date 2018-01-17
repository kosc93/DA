#include "color_box.h"

color_box::color_box(drawable_window& w,
        rectangle area,
        unsigned char red_,
        unsigned char green_,
        unsigned char blue_ ):
        draggable(w),
        red(red_),
        green(green_),
        blue(blue_)
        {
        rect = area;
        set_draggable_area(rectangle(10,10,400,400));
        enable_events();
}

 color_box::~color_box(){
     disable_events();
     parent.invalidate_rectangle(rect);
}

void color_box::draw(const canvas& c) const {
    rectangle area = c.intersect(rect);
        if (area.is_empty() == true)
            return;
        fill_rect(c,rect,rgb_pixel(red,green,blue));
}
