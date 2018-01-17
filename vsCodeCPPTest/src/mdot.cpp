#include "mdot.h"
#include <iostream>

mdot::mdot(drawable_window& w,
        double radius_,
        long x,
        long y):
        label(w)   
        {
            radius = radius_;
            set_text("test");
            x_y = point(x,y);
            long lradius = (long) radius;
            rect = rectangle(x-lradius,y+lradius,x+lradius,y-lradius);
            enable_events();
        }

mdot::~mdot(){    
    disable_events();
    parent.invalidate_rectangle(rect);
}

mdot::mdot(const mdot &obj):label(obj.parent){
    rect=obj.rect;
    radius = obj.radius;
    x_y = obj.x_y;
}

void mdot::draw(const canvas& c)const {
        rectangle area = c.intersect(rect);
        if (area.is_empty() == true)
            return;
        draw_solid_circle(c,x_y,radius);
        
}

void mdot::sayHello(){
    std::cout<<"Hello"<<std::endl;
}