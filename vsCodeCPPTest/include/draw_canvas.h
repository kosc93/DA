#ifndef DRAW_CANVAS_H_
#define _DRAW_CANVAS_
#include "BasicPicture.h"
#include "data_point.h"
#include <vector>
using namespace std;

class DrawCanvas : public BasicPicture{
    public:
        DrawCanvas(wxWindow* parent_);
        //DrawCanvas(const DrawCanvas& obj);
        //~DrawCanvas();
        void draw(wxDC& dc);
        wxWindow* parent;
    private:
        void OnLeftClick(wxMouseEvent& event);
        void OnMidClick(wxMouseEvent& event);
        void OnRightClick(wxMouseEvent& event);
        vector<wxPoint> class1;
        vector<wxPoint> class2;
        vector<wxPoint> class3;
        vector<wxPoint> support_vectors1;
        vector<wxPoint> support_vectors2;
        vector<wxPoint> support_vectors3;
        vector<DataPoint> datapoints;
        DECLARE_EVENT_TABLE();
};

#endif
