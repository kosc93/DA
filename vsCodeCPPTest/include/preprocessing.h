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


class Preprocessor{
public:
  unsigned long feature_size=0;
  bool imported=false;
  std::vector<sample_type> samples;
  std::vector<double> labels;
  DimReduction dim_reduce;
  dlib::vector_normalizer<sample_type> normalizer;
  void import_data(const std::vector<DataPoint>& datapoints_);
  void rand_and_normalize();
  void reduce_dim();
  void import_params(std::string filename);
  void export_params(std::string filename);
  sample_type operator()(DataPoint& datapoint);
};



#endif /* INCLUDE_PREPROCESSING_H_ */
