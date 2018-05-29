/*
 * preprocessing.h
 *
 *  Created on: Mar 27, 2018
 *      Author: kosc93
 */

#ifndef PREPROCESSING_H_
#define PREPROCESSING_H_
#include "defines_etc.h"

using namespace std;
using namespace dlib;


struct Preprocessor{
  unsigned long feature_size=0;
  bool imported=false;
  bool norm=false;
  std::string filter_hash;
  void operator <<(std::string hash){
    filter_hash=hash;
  }
  std::vector<sample_type> samples;
  std::vector<double> labels;
  std::vector<sample_type> train_samples;
  std::vector<double> train_labels;
  std::vector<sample_type> test_samples;
  std::vector<double> test_labels;
  DimReduction dim_reduce;
  dlib::vector_normalizer<sample_type> normalizer;
  void import_data(const std::vector<DataPoint>& datapoints_);
  void import_train(const std::vector<DataPoint>& datapoints_);
  void import_test(const std::vector<DataPoint>& datapoints_);
  void normalize();
  void randomize();
  void reduce_dim();
  void split(double ratio_test=0.25);
  void import_params(std::string filename);
  void export_params(std::string filename);
  sample_type operator()(DataPoint& datapoint);
};



#endif /* INCLUDE_PREPROCESSING_H_ */
