#include "data_point.h"

DataPoint::DataPoint(double o_class, double x, double y):
  original_class(o_class),
  calculated_class(0)
{
  features.push_back(double(x));
  features.push_back(double(y));
}

//DataPoint::DataPoint(DataPoint&& obj):
//    original_class(obj.original_class),
//    calculated_class(0)
//{
//  features=obj.features;
//}
//
//DataPoint::DataPoint(const DataPoint& obj):
//    original_class(obj.original_class),
//    calculated_class(0)
//{
//
//  features=obj.features;
//}
//
//DataPoint::~DataPoint(){
//
//}


