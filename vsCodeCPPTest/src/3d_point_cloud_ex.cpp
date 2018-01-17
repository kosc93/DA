// The contents of this file are in the public domain. See LICENSE_FOR_EXAMPLE_PROGRAMS.txt
/*

    This is an example illustrating the use of the perspective_window tool
    in the dlib C++ Library.  It is a simple tool for displaying 3D point 
    clouds on the screen.

*/

#include <dlib/gui_widgets.h>
#include <dlib/image_transforms.h>
#include <cmath>

using namespace dlib;
using namespace std;


// ----------------------------------------------------------------------------------------

int main()
{
    // Let's make a point cloud that looks like a 3D spiral.
    std::vector<perspective_window::overlay_dot> points;
    std::vector<perspective_window::overlay_dot> coord_system;

    rgb_pixel x_color = {255,0,0};
    rgb_pixel y_color = {0,255,0};
    rgb_pixel z_color = {0,0,255};
    dlib::rand rnd;
    for (double i = 0; i < 20; i+=0.001)
    {
        // Get a point on a spiral
        dlib::vector<double> val(cos(i*2),cos(i),i/4);

        // Now add some random noise to it
        dlib::vector<double> temp(rnd.get_random_gaussian(),
                                  rnd.get_random_gaussian(),
                                  rnd.get_random_gaussian());
        val += temp/20;

        // Pick a color based on how far we are along the spiral
        rgb_pixel color = colormap_jet(i,0,20);

        // And add the point to the list of points we will display
        points.push_back(perspective_window::overlay_dot(val, color));
    }

    for(double i =0; i<1; i+=0.001)
    {  
        dlib::vector<double> x_val(i,0,0);
        dlib::vector<double> y_val(0,i,0);
        dlib::vector<double> z_val(0,0,i);
        coord_system.push_back(perspective_window::overlay_dot(x_val,x_color));
        coord_system.push_back(perspective_window::overlay_dot(y_val,y_color));
        coord_system.push_back(perspective_window::overlay_dot(z_val,z_color));
    }

    // Now finally display the point cloud.
    perspective_window win;
    win.set_title("perspective_window 3D point cloud");
    win.add_overlay(points);
    win.add_overlay(coord_system);
    win.wait_until_closed();
}

//  ----------------------------------------------------------------------------


