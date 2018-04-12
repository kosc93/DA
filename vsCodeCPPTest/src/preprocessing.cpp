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
  if(!imported){
    std::cout<<"reducing dimensions from "<<feature_size<<std::endl;
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
    cout<<"to "<<M.size()<<endl;
  }
  std::cout<<"lda computed"<<std::endl;
  for(auto& sample_ : samples){
    dlib::matrix<double>buffer=dim_reduce.X_dim_reduce*sample_-dim_reduce.M_dim_reduce;
    sample_.set_size(buffer.size());
    sample_=buffer;
  }

}

void
Preprocessor::import_params (std::string filename)
{
  ifstream in_stream(filename+".preprocessing",ifstream::in);
  if(!in_stream.good()){
      cout<<"file does not exists"<<endl;
      in_stream.close();
      return;
  }
  try {
      unsigned long imported_feature_size;
      dlib::deserialize(imported_feature_size,in_stream);
      if(feature_size!=imported_feature_size){
	  cout<<"feature size in imported file does not match!"<<endl;
	  throw dlib::serialization_error("feature size not matching");
      }
      std::string imported_filter_hash;
      dlib::deserialize(imported_filter_hash,in_stream);
      if(filter_hash!=imported_filter_hash){
	  cout<<"filters in imported file does not match!"<<endl;
	  throw dlib::serialization_error("filters not matching");
      }

      dlib::deserialize(dim_reduce.X_dim_reduce,in_stream);
      dlib::deserialize(dim_reduce.M_dim_reduce,in_stream);
      dlib::deserialize(normalizer,in_stream);
      imported=true;
      cout<<"file sucessfully loaded"<<endl;
  } catch (dlib::serialization_error) {
      cout<<"file could not be loaded"<<endl;
  }
  in_stream.close();
}

void
Preprocessor::export_params (std::string filename)
{
  ofstream out_stream(filename+".preprocessing",ofstream::out);
  try {
     dlib::serialize(feature_size,out_stream);
     dlib::serialize(filter_hash,out_stream);
     dlib::serialize(dim_reduce.X_dim_reduce,out_stream);
     dlib::serialize(dim_reduce.M_dim_reduce,out_stream);
     dlib::serialize(normalizer,out_stream);
       cout<<"file sucessfully saved"<<endl;
   } catch (dlib::serialization_error) {
       cout<<"file could not be saved"<<endl;
   }
   out_stream.close();
}

sample_type Preprocessor::operator () (DataPoint& datapoint)
{
   sample_type sample;
   sample.set_size(datapoint.features.size());
   for(unsigned int i=0;i<datapoint.features.size();i++){
       sample(i)=datapoint.features[i];
   }
   //normalization
   sample=normalizer(sample);
   //dimension reduction
   dlib::matrix<double>sample_dim_reduced =dim_reduce.X_dim_reduce*sample-dim_reduce.M_dim_reduce;
   sample.set_size(sample_dim_reduced.size());
   sample=sample_dim_reduced;
  //cout<<sample;
  return sample;
}
