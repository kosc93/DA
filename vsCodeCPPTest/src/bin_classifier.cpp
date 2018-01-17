#include "bin_classifier.h"

BinClassifier::BinClassifier(KernelType k_type_, TrainerType t_type_, std::vector<double> params_):
  k_type(k_type_),
  t_type(t_type_),
  params(params_)
{
}

void BinClassifier::train(std::vector<std::vector<double>> samples_, std::vector<double> labels){
  typedef matrix<double,0,1> sample_type;
  switch(k_type){
    case KernelType::RBF:{

      break;}
    case KernelType::LINEAR:{
      typedef linear_kernel<sample_type> kernel_type;
      break;}
    default:{
      typedef int kernel_type;
      throw std::invalid_argument("invalid kerneltype");
    }
  }

  std::vector<sample_type> samples;
  for(const auto & datapoint: samples_){
      sample_type sample;
      long vector_size = long(datapoint.size());
      sample.set_size(vector_size);
      for(unsigned long i=0;i<vector_size;i++){
	  sample(i)=datapoint[i];
      }
      samples.push_back(sample);
  }
  if(params[1]==1.0){
      vector_normalizer<sample_type> normalizer;
      normalizer.train(samples);
      for (unsigned long i = 0; i < samples.size(); ++i)
	  samples[i] = normalizer(samples[i]);
      //TODO
      //implement normalized decision function
  }
  switch(t_type){
    case TrainerType::KRR:{
      typedef radial_basis_kernel<sample_type> kernel_type;
      krr_trainer<kernel_type> trainer;

    }

  }
  trainer
}
