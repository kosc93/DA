/*
 * preprocessing.cpp
 *
 *  Created on: Mar 27, 2018
 *      Author: kosc93
 */

#include "preprocessing.h"

void
Preprocessor::import_data (const std::vector<DataPoint>& datapoints_)
{
  for(const auto & datapoint: datapoints_){
            sample_type sample;
            feature_size = long(datapoint.features.size());
            sample.set_size(feature_size);
            for(unsigned int i=0;i<feature_size;i++){
                  sample(i)=datapoint.features[i];
              }
            samples.push_back(sample);
            //cout<<sample<<endl;
            labels.push_back(datapoint.original_class);
        };
}

void
Preprocessor::rand_and_normalize ()
{
  randomize_samples(samples,labels);
  normalizer.train(samples);
  for (auto& sample:samples){
    sample=normalizer(sample);
  }
}

void
Preprocessor::reduce_dim ()
{
  std::cout<<"reducing dimensions"<<std::endl;
  dlib::matrix<double>X;

  X.set_size(samples.size(),feature_size);
  for(unsigned int row=0;row<samples.size();row++){
  dlib::matrix<double> mat=samples[row];
  for(unsigned int col=0;col<feature_size;col++){
    X(row,col)=mat(col);
  }
  }
  dlib::matrix<double,0,1> M;
  std::vector<unsigned long> labs;
  for(auto& l : labels){
  labs.push_back(long(l));
  }

  dlib::compute_lda_transform(X,M,labs);
  dim_reduce.X_dim_reduce=X;
  dim_reduce.M_dim_reduce=M;

  std::cout<<"lda computed"<<std::endl;
  for(auto& sample_ : samples){
    dlib::matrix<double>buffer=dim_reduce.X_dim_reduce*sample_-dim_reduce.M_dim_reduce;
    sample_.set_size(buffer.size());
    sample_=buffer;
  }

}
