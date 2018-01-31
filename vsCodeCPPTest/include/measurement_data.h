/*
 * measurement_data.h
 *
 *  Created on: Jan 29, 2018
 *      Author: kosc93
 */

#ifndef MEASUREMENT_DATA_H_
#define MEASUREMENT_DATA_H_
#include <vector>
#include <string>
#include <dlib/serialize.h>
#include "hdf5/serial/H5Cpp.h"
#include "data_point.h"

using namespace H5;




struct Trace{
  std::vector<double> real;
  std::vector<double> imag;
  unsigned int name;
  unsigned int num_points;
  std::string start_freq;
  std::string stop_freq;
};

struct Channel{
  std::vector<Trace> traces;
  unsigned int name;
};

struct Device{
  std::vector<Channel> channels;
  std::string manufacturer;
  std::string model;
  std::string serial;
};

class Measurement{
  public:
    Measurement(std::string date_):date(date_),label(0){};
    std::vector<Device> devices;
    std::string date;
    DataPoint getDatapoint(unsigned int device_index);
    void rename(const std::string newName){date = newName;};
    double label;
//    void serialize();
//    void deserialize();
//    implement serialization for subclasses
};

class H5MeasurementFile{
  public:
    H5MeasurementFile(std::string& filename_):filename(filename_){};
    std::vector<Measurement> scan();
    std::vector<Measurement> measurements;
  private:
    std::string filename;

};


#endif /* INCLUDE_MEASUREMENT_DATA_H_ */
