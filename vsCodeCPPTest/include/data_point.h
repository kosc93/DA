#ifndef DATA_POINT_H_
#define DATA_POINT_H_
#include <vector>
#include <wx/wx.h>

class DataPoint{
    public:
        DataPoint(double o_class,double x, double y);
//        DataPoint(const DataPoint& obj);
//        DataPoint(DataPoint&& obj);
        ~DataPoint()=default;
        const double original_class;
        double calculated_class;
        std::vector<double> features;
};

#endif
