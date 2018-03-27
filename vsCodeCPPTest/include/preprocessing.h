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
  long feature_size=0;
  std::vector<sample_type> samples;
  std::vector<double> labels;
  DimReduction dim_reduce;
  dlib::vector_normalizer<sample_type> normalizer;
  void import_data(const std::vector<DataPoint>& datapoints_);
  void rand_and_normalize();
  void reduce_dim();
};



#endif /* INCLUDE_PREPROCESSING_H_ */
