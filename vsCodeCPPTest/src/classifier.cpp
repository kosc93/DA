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
  res.lbound={1e-6};
  res.ubound={1};
  return res;
}
template<>
OptimizationBoundarys BinClassifier<svm_c_trainer<lin_kernel>,lin_kernel>::get_opt_boundariy()const{
  OptimizationBoundarys res;
  res.lbound={1};
  res.ubound={10000};
  return res;
}
