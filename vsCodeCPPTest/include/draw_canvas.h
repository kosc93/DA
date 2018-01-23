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
        std::vector<DataPoint> datapoints;
        //wxWindow* parent;
    private:
        void OnLeftClick(wxMouseEvent& event);
        void OnMidClick(wxMouseEvent& event);
        void OnRightClick(wxMouseEvent& event);
        DECLARE_EVENT_TABLE();
};

#endif
