#include "bin_classifier.h"
#include <dlib/global_optimization.h>
#include <dlib/optimization.h>
#include "helpers.h"


BinClassifier::BinClassifier(const bool& normalize_, const long& cross_validation_manifold_):
  normalize(normalize_),
  cross_validation_manifold(cross_validation_manifold_)
{
}

void BinClassifier::import_data(std::vector<DataPoint>& datapoints_){
    for(const auto & datapoint: datapoints_){
      sample_type sample;
      const long vector_size = long(datapoint.features.size());
      sample.set_size(vector_size);
      for(long i=0;i<vector_size;i++){
	  sample(i)=datapoint.features[i];
      }
      samples.push_back(sample);
      labels.push_back(datapoint.original_class);
  }
}

double
BinClassifier::classify (DataPoint& sample_)
{
  sample_type sample;
  const long vector_size = sample_.features.size();
  sample.set_size(vector_size);
  for(long i =0;i<vector_size;i++)
    sample(i)=sample_.features[i];
  if(normalize)
    return renormalized_decision_function(sample);
  return decision_function(sample);
}

any_trainer<sample_type>
RVMClassifier::get_trainer ()
{
  throw NotImplementedException();
}




void
RVMClassifier::train (bool& optmize, std::vector<double>& parameter_)
{
  vector_normalizer<sample_type> normalizer;
  if(normalize){
      normalizer.train(samples);
      for (unsigned long i = 0; i < samples.size(); ++i)
              samples[i] = normalizer(samples[i]);
      renormalized_decision_function.normalizer=normalizer;
  }
  randomize_samples(samples,labels);
  rvm_trainer<rbf_kernel> trainer;
  if(optmize)
    optimize_model_param(trainer);
  if(normalize){
      renormalized_decision_function.function = trainer.train(samples,labels);
  }else{
      decision_function = trainer.train(samples,labels);
  }
}

void
RVMClassifier::optimize_model_param (rvm_trainer<rbf_kernel>& trainer)
{
  auto cross_validation_score = [&](const double gamma, const double eps){
    trainer.set_epsilon(eps);
    trainer.set_kernel(rbf_kernel(gamma));
    trainer.set_max_iterations(2000);
    //asset that datapoints are randomized
    matrix<double> opt_result = cross_validate_trainer(trainer,samples,labels,cross_validation_manifold);

    return 2*prod(opt_result)/sum(opt_result);
  };
  auto result = find_max_global(cross_validation_score,
				{1e-6,1e-4},
				{1.0,0.01},
				max_function_calls(50));
  double best_gamma = result.x(0);
  double best_eps = result.x(1);
  trainer.set_epsilon(best_eps);
  trainer.set_kernel(rbf_kernel(best_gamma));
}

any_trainer<sample_type>
SVMNuClassifier::get_trainer ()
{
  throw NotImplementedException();
}


void
SVMNuClassifier::train (bool& optmize, std::vector<double>& parameter_)
{
  throw NotImplementedException();
}

void
SVMNuClassifier::optimize_model_param ()
{
  throw NotImplementedException();
}

any_trainer<sample_type>
SVMCClassifier::get_trainer ()
{
  throw NotImplementedException();
}

void
SVMCClassifier::train (bool& optmize, std::vector<double>& parameter_)
{
  throw NotImplementedException();
}

void
SVMCClassifier::optimize_model_param ()
{
  throw NotImplementedException();
}


