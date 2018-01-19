#include "draw_canvas.h"

DrawCanvas::DrawCanvas(wxWindow* parent_): BasicPicture(parent_){
    parent = parent_;
}

//DrawCanvas::DrawCanvas(const DrawCanvas& obj):
//    parent(obj.parent),
//    class1(obj.class1),
//    datapoints(obj.datapoints){
//
//}
//
//DrawCanvas::~DrawCanvas(){
//    //delete parent;
//}

void DrawCanvas::draw(wxDC& dc){
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();

    for(auto const& datapoint: datapoints){
        switch(int(datapoint.original_class)){
          case 1:
            dc.SetBrush(*wxBLUE_BRUSH);
            break;
          case 2:
            dc.SetBrush(*wxRED_BRUSH);
            break;
          case 3:
            dc.SetBrush(*wxGREEN_BRUSH);
            break;
          default:
            dc.SetBrush(*wxWHITE_BRUSH);
            break;
        }
        wxPoint point(int(datapoint.features[0]),int(datapoint.features[1]));
	dc.DrawCircle(point,3);
	wxString text;
	text<<datapoint.original_class;
	text<<':';
	text<<datapoint.calculated_class;
        dc.DrawText(text,point);

    }
}

void DrawCanvas::OnLeftClick(wxMouseEvent& event){
  DataPoint p(1.0,event.GetPosition().x,event.GetPosition().y);
  datapoints.push_back(p);
  wxWindow::Refresh();
}
void DrawCanvas::OnMidClick(wxMouseEvent& event){
    DataPoint p(2.0,event.GetPosition().x,event.GetPosition().y);
    datapoints.push_back(p);
    wxWindow::Refresh();
}
void DrawCanvas::OnRightClick(wxMouseEvent& event){
    DataPoint p(3.0,event.GetPosition().x,event.GetPosition().y);
    datapoints.push_back(p);
    wxWindow::Refresh();
}
BEGIN_EVENT_TABLE(DrawCanvas,BasicPicture)
EVT_LEFT_UP(DrawCanvas::OnLeftClick)
EVT_MIDDLE_UP(DrawCanvas::OnMidClick)
EVT_RIGHT_UP(DrawCanvas::OnRightClick)
END_EVENT_TABLE()
