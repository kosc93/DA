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
  DimReduction dim_reduce;
  dec_funct_type bin_decision_function;
  bool imported=false;
  double operator()(const sample_type& sample_)const{
        sample_type sample;
        sample.set_size(sample_.size());
        sample=sample_;

        return bin_decision_function(sample);
  }
};

template<class classifier,class kernel=rbf_kernel>
class BinClassifier{
  public:
    BinClassifier(const long& cross_validation_manifold_=3):
      feature_size(0),
      cross_validation_manifold(cross_validation_manifold_)
      {};
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
BinDecisionFunc<kernel> BinClassifier<classifier,kernel>::train (const std::vector<sample_type>& samples_, const std::vector<double>& labels_)const
{
    BinDecisionFunc<kernel> res;
    std::vector<sample_type> samples__=samples_;
    std::vector<double> labels__=labels_;


    if(!bin_dec_func.imported){
	if(!is_binary_classification_problem(samples__,labels__)){
	  cout<<"not a binary decision problem use multiclass classifier!"<<endl;
	  throw;
	}
	//optimize hyper parameter
	classifier trainer=optimize_model_param(samples__,labels__);
	//train model
	res.bin_decision_function = trainer.train(samples__,labels__);
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
      //return sqrt(opt_result(0)*opt_result(0)+opt_result(1)*opt_result(1));
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
    //cout<<best_gamma<<" : "<<best_second_param<<endl;
    return trainer;
}

template<class dec_func_type=decision_function<sample_type>>
struct MultDecisionFunc{
  dec_func_type mult_decision_function;
  std::string filter_hash;
  void operator <<(std::string hash){
    filter_hash=hash;
  }
  double operator()(const sample_type& sample_)const{
    sample_type sample;
    sample.set_size(sample_.size());
    sample=sample_;

    //result of the decision function
    return mult_decision_function(sample);
  }
  bool import_function(std::string filename){
    bool res=false;
    ifstream in_stream(filename+".dec",ifstream::in);
    if(!in_stream.good()){
	cout<<"file does not exists"<<endl;
	in_stream.close();
	return false;
    }
    try {
	std::string imported_filter_hash;
	dlib::deserialize(imported_filter_hash,in_stream);
	if(filter_hash!=imported_filter_hash){
	    cout<<"filters in imported file does not match!"<<endl;
	    throw dlib::serialization_error("filters not matching");
	}

	dlib::deserialize(mult_decision_function,in_stream);
	//dlib::deserialize(filename+".dec")>>mult_decision_function;
	cout<<"file sucessfully loaded"<<endl;
	res=true;
    } catch (dlib::serialization_error) {
	cout<<"file could not be loaded"<<endl;
	res=false;
    }
    in_stream.close();
    return res;
  }
  bool export_function(std::string filename){
    bool res=false;
      ofstream out_stream(filename+".dec",ofstream::out);
      try {
	  dlib::serialize(filter_hash,out_stream);
	  dlib::serialize(mult_decision_function,out_stream);
	  //dlib::serialize(filename+".dec")<<mult_decision_function;
	  cout<<"file sucessfully saved"<<endl;
	  res=true;
      } catch (dlib::serialization_error e) {
	  cout<<e.what()<<endl;
	  cout<<"file could not be saved"<<endl;
	  res=false;
      }
      out_stream.close();
      return res;
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
  template<class mult_dec_func_type>
  MultDecisionFunc<mult_dec_func_type> train(const std::vector<sample_type> samples,const std::vector<double> labels);
private:
  classifier trainer;
};


#endif /* INCLUDE_BIN_CLASSIFIER_H_ */
