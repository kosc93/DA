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

enum class KernelType {RBF,LINEAR};
enum class TrainerType {KRR,SVM_NU,SVM_C};

class BinClassifier{
  std::vector<sample_type> samples;
  std::vector<double> lables;
  const bool normalize;
  long cross_validation_manifold;
  public:
    BinClassifier(const bool& normalize_=true,const long& cross_validation_manifold_=3);
    virtual ~BinClassifier(){};
    void import_data(std::vector<DataPoint>& datapoints_);
  protected:
    virtual any_trainer<sample_type> get_trainer()=0;//for multiclass
    virtual double classify(DataPoint& sample_)=0;
    virtual void train(std::vector<double>& parameter_)=0;
    virtual void optimize_model_param()=0;


};

class KRRClassifier : public BinClassifier{
  public:
    KRRClassifier(const bool& normalize_=true, const long& cross_validation_manifold_=3):BinClassifier(normalize_,cross_validation_manifold_){};
    ~KRRClassifier()=default;
    any_trainer<sample_type> get_trainer() override;
    double classify(DataPoint& sample_) override;
    void train(std::vector<double>& parameter_) override;
  private:
    void optimize_model_param() override;
};


#endif /* INCLUDE_BIN_CLASSIFIER_H_ */
