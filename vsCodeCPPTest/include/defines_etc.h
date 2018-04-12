/*
 * defines_etc.h
 *
 *  Created on: Feb 6, 2018
 *      Author: kosc93
 */

#ifndef DEFINES_ETC_H_
#define DEFINES_ETC_H_

#define NUM_PENS 8
#include <stdexcept>
#include <vector>
#include <dlib/matrix.h>
#ifndef _NUM_FEATURES
#define NUM_FEATURES 360
#else
#define NUM_FEATURES _NUM_FEATURES
#endif
#include <dlib/svm.h>
#include <dlib/svm_threaded.h>
#include <dlib/threads.h>
#include <stdio.h>
using namespace dlib;
class NotImplementedException : public std::logic_error
{
public:
    NotImplementedException():std::logic_error("not implemented"){};
};

typedef matrix<double,0,1> sample_type;
typedef radial_basis_kernel<sample_type> rbf_kernel;
typedef linear_kernel<sample_type> lin_kernel;
typedef one_vs_one_trainer<any_trainer<sample_type>> ovo_trainer;
typedef one_vs_all_trainer<any_trainer<sample_type>> ova_trainer;



struct DataPoint{
	DataPoint(double original_class_=0):original_class(original_class_),calculated_class(0){};
        ~DataPoint()=default;
        const double original_class;
        double calculated_class;
        std::vector<double> features;
        double cos_sim(const DataPoint& ref)const{
          if(features.size()!=ref.features.size()){
              std::cout<<"feature size must  be equal"<<std::endl;
              throw;
          }
          matrix<double,0,1> A;
          matrix<double,0,1> B;
	  long feature_size = long(features.size());
	  A.set_size(feature_size);
	  B.set_size(feature_size);
	  for(unsigned int i=0;i<feature_size;i++){
	      A(i)=features[i];
	      B(i)=ref.features[i];
	  }
          //A dot B / sqrt(sum(A pointwise A)) sqrt(sum(B pointwise B))
	  double dotprod=dot(A,B);
	  double nenner =(sqrt(sum(squared(A)))*sqrt(sum(squared(B))));
          return dotprod/nenner;
        }
};

struct DimReduction{
  dlib::matrix<double> X_dim_reduce;
  dlib::matrix<double> M_dim_reduce;
};

const int ID_IMPORT =1;
const int ID_TRAIN =2;
const int ID_EXPORT =3;
const int ID_TRAINER_SELECTED = 4;



enum class KernelType {RBF,LINEAR};
enum class TrainerType {NONE,RVM,SVM_NU,SVM_C};

#endif /* INCLUDE_DEFINES_ETC_H_ */
