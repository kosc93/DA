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

using namespace std;
using namespace dlib;


enum class KernelType {RBF,LINEAR};
enum class TrainerType {KRR,SVM_NU,SVM_C};

class BinClassifier{
  KernelType k_type;
  TrainerType t_type;
  std::vector<double> params;
  /*
   * params(all):
   * 0 -> num features
   * 1 -> normalize 1: true
   * 2 -> gamma
   * 3 -> nu
   * 4 -> Crossvalidation manifold
   *
   *
   */
  public:
    BinClassifier(KernelType k_type_, TrainerType t_type_, std::vector<double> params_);
    void train(std::vector<std::vector<double>> samples, std::vector<double> labels);
    //double classify(std::vector<double> sample);
  private:
    void optimize_model_param();
    void create_trainer();
};


#endif /* INCLUDE_BIN_CLASSIFIER_H_ */
