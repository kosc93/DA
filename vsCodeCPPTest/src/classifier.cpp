#include "classifier.h"


template<>
rvm_trainer<rbf_kernel> BinClassifier<rvm_trainer<rbf_kernel>,rbf_kernel>::set_params(const double& gamma, const double& second_param)const{
  rvm_trainer<rbf_kernel> trainer;
  trainer.set_kernel(rbf_kernel(gamma));
  trainer.set_epsilon(second_param);
  return trainer;
}

template<>
svm_nu_trainer<rbf_kernel> BinClassifier<svm_nu_trainer<rbf_kernel>,rbf_kernel>::set_params(const double& gamma,const  double& second_param)const{
  svm_nu_trainer<rbf_kernel> trainer;
  trainer.set_kernel(rbf_kernel(gamma));
  trainer.set_nu(second_param);
  return trainer;
}

template<>
svm_c_trainer<rbf_kernel> BinClassifier<svm_c_trainer<rbf_kernel>,rbf_kernel>::set_params(const double& gamma, const double& second_param)const{
  svm_c_trainer<rbf_kernel> trainer;
  trainer.set_kernel(rbf_kernel(gamma));
  trainer.set_c(second_param);
  return trainer;
}
template<>
rvm_trainer<lin_kernel> BinClassifier<rvm_trainer<lin_kernel>,lin_kernel>::set_params(const double& gamma, const double& second_param)const{
  return rvm_trainer<lin_kernel>();
}

template<>
svm_nu_trainer<lin_kernel> BinClassifier<svm_nu_trainer<lin_kernel>,lin_kernel>::set_params(const double& gamma,const  double& second_param)const{
  svm_nu_trainer<lin_kernel> trainer;
  trainer.set_nu(second_param);
  return trainer;
}

template<>
svm_c_trainer<lin_kernel> BinClassifier<svm_c_trainer<lin_kernel>,lin_kernel>::set_params(const double& gamma, const double& second_param)const{
  svm_c_trainer<lin_kernel> trainer;
  trainer.set_c(second_param);
  return trainer;
}

template<>
OptimizationBoundarys BinClassifier<rvm_trainer<rbf_kernel>,rbf_kernel>::get_opt_boundariy()const{
  OptimizationBoundarys res;
  res.lbound={1e-6,9e-2};
  res.ubound={1,11e-2};
  return res;

}
template<>
OptimizationBoundarys BinClassifier<svm_nu_trainer<rbf_kernel>,rbf_kernel>::get_opt_boundariy()const{
  OptimizationBoundarys res;
  res.lbound={1e-6,1e-6};
  res.ubound={1,1};
  return res;
}
template<>
OptimizationBoundarys BinClassifier<svm_c_trainer<rbf_kernel>,rbf_kernel>::get_opt_boundariy()const{
  OptimizationBoundarys res;
  res.lbound={1e-6,1};
  res.ubound={1,10000};
  return res;
}

template<>
OptimizationBoundarys BinClassifier<rvm_trainer<lin_kernel>,lin_kernel>::get_opt_boundariy()const{
  OptimizationBoundarys res;
  res.lbound={1e-6,9e-2};
  res.ubound={1,11e-2};
  return res;

}
template<>
rvm_trainer<lin_kernel> BinClassifier<rvm_trainer<lin_kernel>,lin_kernel>::optimize_model_param (const std::vector<sample_type>& samples_, const std::vector<double>& labels_)const{
  return rvm_trainer<lin_kernel>();
}
template<>
OptimizationBoundarys BinClassifier<svm_nu_trainer<lin_kernel>,lin_kernel>::get_opt_boundariy()const{
  OptimizationBoundarys res;
  res.lbound={1e-6,0};
  res.ubound={1,1};
  return res;
}
template<>
OptimizationBoundarys BinClassifier<svm_c_trainer<lin_kernel>,lin_kernel>::get_opt_boundariy()const{
  OptimizationBoundarys res;
  res.lbound={1,0};
  res.ubound={10000,1};
  return res;
}

template<>
template<>
MultDecisionFunc<one_vs_one_decision_function<ovo_trainer,decision_function<rbf_kernel>>> MultClassifier<ovo_trainer,rbf_kernel>::train(const std::vector<sample_type> samples,const std::vector<double> labels){
  MultDecisionFunc<one_vs_one_decision_function<ovo_trainer,decision_function<rbf_kernel>>> func;
  func.mult_decision_function=trainer.train(samples,labels);
  return func;
}

template<>
template<>
MultDecisionFunc<one_vs_one_decision_function<ovo_trainer,decision_function<lin_kernel>>> MultClassifier<ovo_trainer,lin_kernel>::train(const std::vector<sample_type> samples,const std::vector<double> labels){
  MultDecisionFunc<one_vs_one_decision_function<ovo_trainer,decision_function<lin_kernel>>> func;
  func.mult_decision_function=trainer.train(samples,labels);
  return func;
}

template<>
template<>
MultDecisionFunc<one_vs_all_decision_function<ova_trainer,decision_function<rbf_kernel>>> MultClassifier<ova_trainer,rbf_kernel>::train(const std::vector<sample_type> samples,const std::vector<double> labels){
  MultDecisionFunc<one_vs_all_decision_function<ova_trainer,decision_function<rbf_kernel>>> func;
  func.mult_decision_function=trainer.train(samples,labels);
  return func;
}

template<>
template<>
MultDecisionFunc<one_vs_all_decision_function<ova_trainer,decision_function<lin_kernel>>> MultClassifier<ova_trainer,lin_kernel>::train(const std::vector<sample_type> samples,const std::vector<double> labels){
  MultDecisionFunc<one_vs_all_decision_function<ova_trainer,decision_function<lin_kernel>>> func;
  func.mult_decision_function=trainer.train(samples,labels);
  return func;
}

template<>
template<>
MultDecisionFunc<multiclass_linear_decision_function<lin_kernel,double>> MultClassifier<svm_multiclass_linear_trainer<lin_kernel>,lin_kernel>::train(const std::vector<sample_type> samples,const std::vector<double> labels){
  //optimize c and
  svm_multiclass_linear_trainer<lin_kernel> trainer;
  cout<<"opt startet"<<endl;
  auto cross_validation_score = [&](const double eps,const double second_param){
    trainer.set_c(second_param);
    trainer.set_epsilon(eps);
    //asset that datapoints are randomized
    matrix<double> opt_result = cross_validate_multiclass_trainer(trainer,samples,labels,cross_validation_manifold);
    //cout<<"optimization step taken"<<endl;
    return trace(opt_result);
    //return sqrt(opt_result(0)*opt_result(0)+opt_result(1)*opt_result(1));
  };
  auto result = find_max_global(cross_validation_score,
				{9e-2,1},
				{11e-2,10000},
			    max_function_calls(30));
  double best_eps = result.x(0);
  double best_second_param = result.x(1);
  trainer.set_c(best_second_param);
  trainer.set_epsilon(best_eps);
  //asset that datapoints are randomized
  //cout<<cross_validate_trainer(trainer,samples_,labels_,cross_validation_manifold)<<endl;
  //cout<<best_gamma<<" : "<<best_second_param<<endl;

  //training
  MultDecisionFunc<multiclass_linear_decision_function<lin_kernel,double>> func;
  func.mult_decision_function=trainer.train(samples,labels);
  return func;
}
