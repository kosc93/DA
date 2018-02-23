#ifndef DATA_POINT_H_
#define DATA_POINT_H_
#include <vector>


class DataPoint{
    public:
	DataPoint():original_class(0),calculated_class(0){};
        DataPoint(double o_class,double x, double y);
//        DataPoint(const DataPoint& obj);
//        DataPoint(DataPoint&& obj);
        ~DataPoint()=default;
        const double original_class;
        double calculated_class;
        std::vector<double> features;
};

#endif
