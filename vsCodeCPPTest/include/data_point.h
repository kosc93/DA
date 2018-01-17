#ifndef DATA_POINT_H_
#define DATA_POINT_H_
#include <vector>
#include <wx/wx.h>

class DataPoint{
    public:
        DataPoint(int o_class,int x, int y);
//        DataPoint(const DataPoint& obj);
//        DataPoint(DataPoint&& obj);
        ~DataPoint()=default;
        const int original_class;
        int calculated_class;
        std::vector<double> features;
};

#endif
