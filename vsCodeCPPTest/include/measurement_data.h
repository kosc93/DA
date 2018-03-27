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
#include <algorithm>
#include <dlib/statistics.h>
#include "hdf5/serial/H5Cpp.h"
#include "classifier.h"
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace H5;


struct ClassifierParam{
  TrainerType type;
  bool normalize;
  long cc_manifold;
  bool optimize;
  std::vector<double> params;
  std::string getHash(){
    std::ostringstream result;;
    result <<std::fixed<<std::setprecision(3);
    result<<static_cast<int>(type)<<"_"<<normalize<<"_"<<cc_manifold<<"_"<<optimize<<"_";
    for(const auto& param : params){
	result<<param<<"_";
    }
    return result.str();
  }
};

struct Trace{
  std::vector<double> real;
  std::vector<double> imag;
  std::vector<double> mag;
  std::vector<double> phase;
  std::vector<double> processed_real;
  std::vector<double> processed_imag;
  void normalize();
  std::string name;
  bool isProcessed = false;
  unsigned int num_points;
  std::string start_freq;
  std::string stop_freq;
};

struct Channel{
  std::vector<Trace> traces;
  std::string name;
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
    int num_features;
    std::vector<Device> devices;
    std::string date;
    DataPoint getDatapoint(unsigned int device_index,bool only_mag=false);
    void rename(const std::string newName){date = newName;};
    double label;
    std::string h5_name;
//    void serialize();
//    void deserialize();
//    implement serialization for subclasses
};

class H5MeasurementFile{
  public:
    H5MeasurementFile(std::string& filename_):filename(filename_){H5File file(filename, H5F_ACC_CREAT | H5F_ACC_RDWR);file.close();};
    std::vector<Measurement> scan(bool use_reps=false, bool unformatted=true);// change to void  to adopt for either new data or exported files
    void label(double& label);
    void label(string label_filename);
    void export_data(std::vector<Measurement>& data_);
    void export_data(std::vector<Measurement>& data_, ClassifierParam& param_);
    void append(string append_filename);
    std::vector<Measurement> measurements;
  private:
    std::string filename;

};


#endif /* INCLUDE_MEASUREMENT_DATA_H_ */
