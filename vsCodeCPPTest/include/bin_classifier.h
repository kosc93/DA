/*
 * bin_classifier.h
 *
 *  Created on: Jan 17, 2018
 *      Author: kosc93
 */

#ifndef BIN_CLASSIFIER_H_
#define BIN_CLASSIFIER_H_
#include <dlib/svm.h>
#include <vector>
#include <stdexcept>
#include "data_point.h"

using namespace std;
using namespace dlib;

typedef matrix<double,0,1> sample_type;
typedef radial_basis_kernel<sample_type> rbf_kernel;
typedef linear_kernel<sample_type> linear_kernel;
typedef decision_function<rbf_kernel > dec_funct_type;
typedef normalized_function<dec_funct_type> funct_type;

enum class KernelType {RBF,LINEAR};
enum class TrainerType {RVM,SVM_NU,SVM_C};

class BinClassifier{
  public:
    BinClassifier(const bool& normalize_=true,const long& cross_validation_manifold_=3);
    virtual ~BinClassifier(){};
    double classify(DataPoint& sample_);
  protected:
    virtual any_trainer<sample_type> get_trainer()=0;//for multiclass
    virtual void train(const bool& optimize=true,const std::vector<double>& parameter_={})=0;
    void normalize_input();
    void import_data(const std::vector<DataPoint>& datapoints_);
    std::vector<sample_type> samples;
    std::vector<double> labels;
    dec_funct_type decision_function;
    funct_type renormalized_decision_function;
    const bool normalize;
    long cross_validation_manifold;

};

class RVMClassifier : public BinClassifier{

  public:
    RVMClassifier(const bool& normalize_=true, const long& cross_validation_manifold_=3):BinClassifier(normalize_,cross_validation_manifold_){};
    ~RVMClassifier()=default;
    any_trainer<sample_type> get_trainer() override;
    void train(const bool& optimize=true,const std::vector<double>& parameter_={}) override;
  private:
    void optimize_model_param(rvm_trainer<rbf_kernel>& trainer);
};

class SVMNuClassifier : public BinClassifier{
  public:
    SVMNuClassifier(const bool& normalize_=true, const long& cross_validation_manifold_=3):BinClassifier(normalize_,cross_validation_manifold_){};
    ~SVMNuClassifier()=default;
    any_trainer<sample_type> get_trainer() override;
    void train(const bool& optimize=true, const std::vector<double>& parameter_={}) override;
  private:
    void optimize_model_param(svm_nu_trainer<rbf_kernel>& trainer);
};

class SVMCClassifier : public BinClassifier{
  public:
    SVMCClassifier(const bool& normalize_=true, const long& cross_validation_manifold_=3):BinClassifier(normalize_,cross_validation_manifold_){};
    ~SVMCClassifier()=default;
    any_trainer<sample_type> get_trainer() override;
    void train(const bool& optimize=true, const std::vector<double>& parameter_={}) override;
  private:
    void optimize_model_param(svm_c_trainer<rbf_kernel>& trainer);
};

#endif /* INCLUDE_BIN_CLASSIFIER_H_ */
