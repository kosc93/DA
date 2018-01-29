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
#include "hdf5/serial/H5Cpp.h"

using namespace H5;

using namespace std;


struct Trace{
  vector<double> real;
  vector<double> imag;
  unsigned int name;
  unsigned int num_points;
};

struct Channel{
  vector<Trace> traces;
  unsigned int name;
};

struct Device{
  vector<Channel> channels;
  string manufacturer;
  string model;
  string serial;
};

class Measurement{
  public:
    Measurement(string date_):date(date_){};
    vector<Device> devices;
    const string date;
};

class H5MeasurementFile{
  public:
    H5MeasurementFile(string& filename_):filename(filename_){};
    vector<Measurement> scan();
    vector<Measurement> measurements;
  private:
    string filename;

};


#endif /* INCLUDE_MEASUREMENT_DATA_H_ */
