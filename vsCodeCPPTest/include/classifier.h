/*
 * bin_classifier.h
 *
 *  Created on: Jan 17, 2018
 *      Author: kosc93
 */

#ifndef CLASSIFIER_H_
#define CLASSIFIER_H_
#include <dlib/svm.h>
#include <dlib/svm_threaded.h>
#include <dlib/threads.h>
#include <vector>
#include <stdexcept>
#include <dlib/global_optimization.h>
#include <dlib/optimization.h>
#include "defines_etc.h"
#include "preprocessing.h"

using namespace std;
using namespace dlib;



struct OptimizationBoundarys{
  matrix<double,0,1> lbound;
  matrix<double,0,1> ubound;
};

template<class kernel>
struct BinDecisionFunc{
  typedef decision_function<kernel > dec_funct_type;
  typedef normalized_function<dec_funct_type> funct_type;
  DimReduction dim_reduce;
  funct_type renormalized_decision_function;
  vector_normalizer<sample_type> normalizer;
  bool imported=false;
  double operator()(const sample_type& sample_)const{
//    //datapoint to dlib::matrix conversion
        sample_type sample;
        sample.set_size(sample_.size());
        sample=sample_;
//        //normalization
//        sample=normalizer(sample);
//        //dimension reduction
//        dlib::matrix<double>sample_dim_reduced =dim_reduce.X_dim_reduce*sample-dim_reduce.M_dim_reduce;
//        sample.set_size(sample_dim_reduced.size());
//        sample=sample_dim_reduced;
//    	//cout<<sample;

        //result of the decision function
        return renormalized_decision_function.function(sample);
  }
  double operator()(const DataPoint& sample_)const{
    //datapoint to dlib::matrix conversion
        sample_type sample;
        sample.set_size(sample_.features.size());
        for(unsigned int i=0;i<sample_.features.size();i++){
    	sample(i)=sample_.features[i];
        }
        //normalization
        sample=normalizer(sample);
        //dimension reduction
        dlib::matrix<double>sample_dim_reduced =dim_reduce.X_dim_reduce*sample-dim_reduce.M_dim_reduce;
        sample.set_size(sample_dim_reduced.size());
        sample=sample_dim_reduced;
    	//cout<<sample;

        //result of the decision function
        return renormalized_decision_function.function(sample);
  }
};

template<class classifier,class kernel=rbf_kernel>
class BinClassifier{
  public:
    BinClassifier(const long& cross_validation_manifold_=3):
      feature_size(0),
      cross_validation_manifold(cross_validation_manifold_)
      {};
       void import_preprocessor(const Preprocessor& preprocessor);
       void import_params(std::string filename);
       void export_params(std::string filename);
       BinDecisionFunc<kernel> train(const std::vector<sample_type>& samples_, const std::vector<double>& labels_)const;
  protected:
    classifier set_params(const double& gamma, const double& second_param)const;
    classifier optimize_model_param(const std::vector<sample_type>& samples_={}, const std::vector<double>& labels_={})const;
    OptimizationBoundarys get_opt_boundariy()const;
    unsigned long feature_size;
    long cross_validation_manifold;
    BinDecisionFunc<kernel> bin_dec_func;
};

template<class classifier,class kernel>
void BinClassifier<classifier,kernel>::import_preprocessor(const Preprocessor& preprocessor){
  bin_dec_func.normalizer=preprocessor.normalizer;
  bin_dec_func.dim_reduce=preprocessor.dim_reduce;
}


template<class classifier,class kernel>
void BinClassifier<classifier,kernel>::import_params (std::string filename)
{
  ifstream in_stream(filename,ifstream::in);
  if(!in_stream.good()){
      cout<<"file does not exists"<<endl;
      in_stream.close();
      return;
  }
  try {
      dlib::deserialize(bin_dec_func.dim_reduce.X_dim_reduce,in_stream);
      dlib::deserialize(bin_dec_func.dim_reduce.M_dim_reduce,in_stream);
      dlib::deserialize(bin_dec_func.normalizer,in_stream);
      dlib::deserialize(bin_dec_func.renormalized_decision_function,in_stream);
      bin_dec_func.imported=true;
      cout<<"file sucessfully loaded"<<endl;
  } catch (dlib::serialization_error) {
      cout<<"file could not be loaded"<<endl;
  }
  in_stream.close();
}
template<class classifier,class kernel>
void BinClassifier<classifier,kernel>::export_params (std::string filename)
{
  ofstream out_stream(filename,ofstream::out);
  try {
      dlib::serialize(bin_dec_func.dim_reduce.X_dim_reduce,out_stream);
      dlib::serialize(bin_dec_func.dim_reduce.M_dim_reduce,out_stream);
      dlib::serialize(bin_dec_func.normalizer,out_stream);
      dlib::serialize(bin_dec_func.renormalized_decision_function,out_stream);
        cout<<"file sucessfully saved"<<endl;
    } catch (dlib::serialization_error) {
        cout<<"file does not exists"<<endl;
    }
    out_stream.close();
}
template<class classifier,class kernel>
BinDecisionFunc<kernel> BinClassifier<classifier,kernel>::train (const std::vector<sample_type>& samples_, const std::vector<double>& labels_)const
{
    BinDecisionFunc<kernel> res;
    std::vector<sample_type> samples__=samples_;
    std::vector<double> labels__=labels_;

//    vector_normalizer<sample_type> normalizer=normalize_input(samples__);
    res.normalizer=bin_dec_func.normalizer;
    res.renormalized_decision_function.normalizer=bin_dec_func.normalizer;


    if(!bin_dec_func.imported){
	//handle dimension reduction
	res.dim_reduce=bin_dec_func.dim_reduce;
	//take care of converting a multiclass into a binary classificatopn
	if(!is_binary_classification_problem(samples__,labels__)){
	  for(auto& label:labels__){
	      if(label>8){
		  label=-1;
	      }else{
		  label=1;
	      }
	  }
	}
	//optimize hyper parameter
	classifier trainer=optimize_model_param(samples__,labels__);
	//train model
	res.renormalized_decision_function.function = trainer.train(samples__,labels__);
	return res;
    }
    return bin_dec_func;
}

template<class classifier,class kernel>
classifier BinClassifier<classifier,kernel>::optimize_model_param (const std::vector<sample_type>& samples_, const std::vector<double>& labels_)const
{

  classifier trainer;
  cout<<"opt startet"<<endl;
  auto cross_validation_score = [&](const double gamma,const double second_param){
      trainer = set_params(gamma,second_param);
      //asset that datapoints are randomized
      matrix<double> opt_result = cross_validate_trainer(trainer,samples_,labels_,cross_validation_manifold);
      //cout<<"optimization step taken"<<endl;
      return 2*prod(opt_result)/sum(opt_result);
    };
    OptimizationBoundarys boundarys=get_opt_boundariy();
    auto result = find_max_global(cross_validation_score,
  				boundarys.lbound,
				boundarys.ubound,
				max_function_calls(30));
    double best_gamma = result.x(0);
    double best_second_param = result.x(1);
    trainer = set_params(best_gamma,best_second_param);
    //asset that datapoints are randomized
    cout<<cross_validate_trainer(trainer,samples_,labels_,cross_validation_manifold)<<endl;
    cout<<best_gamma<<" : "<<best_second_param<<endl;
    return trainer;
}

template<class classifier,class kernel>
struct MultDecisionFunc{
  typedef decision_function<kernel> dec_funct_type;
  typedef normalized_function<dec_funct_type> funct_type;
  DimReduction dim_reduce;
  funct_type renormalized_decision_function;
  vector_normalizer<sample_type> normalizer;
  bool imported=false;
  double operator()(const DataPoint& sample_)const{
    //datapoint to dlib::matrix conversion
    sample_type sample;
    sample.set_size(sample_.features.size());
    for(unsigned int i=0;i<sample_.features.size();i++){
      sample(i)=sample_.features[i];
    }
    //normalization
    sample=normalizer(sample);
    //dimension reduction
    dlib::matrix<double>sample_dim_reduced =dim_reduce.X_dim_reduce*sample-dim_reduce.M_dim_reduce;
    sample.set_size(sample_dim_reduced.size());
    sample=sample_dim_reduced;
      //cout<<sample;

    //result of the decision function
    return renormalized_decision_function.function(sample);
  }
};



template<class classifier, class kernel=sample_type>
class MultClassifier : public BinClassifier<any_trainer<kernel>>{
public:
  MultClassifier(const long& cross_validation_manifold_=3):BinClassifier<any_trainer<kernel>>(cross_validation_manifold_),trainer(classifier()){};
  void import_data(const std::vector<DataPoint>& datapoints_);
  template<class bin_classifier,class bin_kernel=rbf_kernel>
  void import_classifiers(BinClassifier<bin_classifier,bin_kernel> new_trainer){trainer.set_trainer(new_trainer);};
  template<class bin_classifier,class bin_kernel=rbf_kernel>
  void import_classifiers(BinClassifier<bin_classifier,bin_kernel> new_trainer,double problem_1,double problem_2){trainer.set_trainer(new_trainer,problem_1,problem_2);};
  void train(std::vector<sample_type> samples,std::vector<double> labels){
    cout << "cross validation: \n" << cross_validate_multiclass_trainer(trainer, samples, labels, this->cross_validation_manifold) << endl;
  };
private:
  classifier trainer;
};


#endif /* INCLUDE_BIN_CLASSIFIER_H_ */
