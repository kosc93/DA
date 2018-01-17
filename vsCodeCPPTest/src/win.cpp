#include "win.h"
#include <sstream>
#include <string>

using namespace std;

win::win(): c(*this),
            cb(*this,rectangle(100,100,200,200),0,0,255),
            b(*this),            
            mbar(*this)            
            {
                //dots = {};
                b.set_pos(10,60);
                b.set_name("button");
                c.set_pos(b.left(),b.bottom()+5);
                b.set_click_handler(*this,&win::on_button_clicked);
                mbar.set_number_of_menus(1);
                mbar.set_menu_name(0,"Menu",'M');
                mbar.menu(0).add_menu_item(menu_item_text("Click Button!",*this,&win::on_button_clicked,'C'));
                mbar.menu(0).add_menu_item(menu_item_separator());
                mbar.menu(0).add_menu_item(menu_item_text("About",*this,&win::show_about,'A'));
                set_size(430,380);
                
                counter = 0;

                

                set_title("dlib gui example");
                show();
}

win::~win(){
    close_window();
}

void win::on_button_clicked(){
    ++counter;
        ostringstream sout;
        sout << "counter: " << counter;
        c.set_text(sout.str());
}

void win::show_about(){
    message_box("About","This is a dlib gui example program");
}

void win::on_mouse_up( unsigned long btn,
            unsigned long state,
            long x,
            long y){
                mdot d = mdot(*this,5.0,x,y);
                //d.sayHello();
                dots.push_back(d);                                    
                rectangle r = d.get_rect();
                invalidate_rectangle(d.get_rect());                      
            }

