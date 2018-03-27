/*
 * vo_klass.cpp
 *
 *  Created on: Mar 5, 2018
 *      Author: kosc93
 */

#include "classifier.h"
#include "preprocessing.h"
#include "dlib/cmd_line_parser.h"
#include "measurement_data.h"
#include <string>
#include <vector>
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
  dlib::command_line_parser parser;
  parser.add_option("F","Filename of labeled HDF5 data",1);
  parser.add_option("I","Filename of import file",1);
  parser.add_option("E","Filename of export",1);
  parser.add_option("reps","flag to use repeated measurements");
  parser.add_option("u","use unformatted data(without any transformation)");
  parser.add_option("h","Display this help message.");
  parser.parse(argc,argv);
  const char* one_time_opts[] = {"F","u","h","reps"};
  parser.check_one_time_options(one_time_opts);
  //check for help option
  if (parser.option("h"))
  {
       parser.print_options();
       return 0;
  }

  string filename;
  Preprocessor preprocessor;
  //check if filename is validrbf_kernel
  if(parser.option("F")){
      std::vector<DataPoint> datapoints;
      filename=parser.option("F").argument();
      H5MeasurementFile file(filename);
        //make sure that there is data to b processed
        std::vector<Measurement> data = file.scan(parser.option("reps"),parser.option("u"));

        if(data.size()==0){
            cout<<"no measurements found"<<endl;
            return -1;
        }
        for(auto& measurement : data){
            if(measurement.label==0){
	      cout<<"not all measurements are labeled"<<endl;
	      return -1;
            }
            datapoints.push_back(measurement.getDatapoint(0,false));

        }
        cout<<"all measurements are labeled"<<endl;
        preprocessor.import_data(datapoints);
        preprocessor.rand_and_normalize();
        preprocessor.reduce_dim();
  }else{
      cout<<"Filename option ist mandatory "<<endl;
      return 0;
  }


  typedef rbf_kernel krnl;
  BinClassifier<svm_c_trainer<krnl>,krnl> classifier;
  if(parser.option("I")){
      classifier.import_params(parser.option("I").argument());
  }


  classifier.import_preprocessor(preprocessor);
  BinDecisionFunc<krnl> learned_func = classifier.train(preprocessor.samples,preprocessor.labels);
  if(parser.option("E")){
      classifier.export_params(parser.option("E").argument());
  }
      for(unsigned int i =0;i<preprocessor.samples.size();i++){
          cout<<preprocessor.labels[i]<<":";
          cout<<learned_func(preprocessor.samples[i])<<endl;
      }
  cout<<"start multiclass training"<<endl;
  MultClassifier<ovo_trainer,krnl> mult;
  mult.import_classifiers<svm_c_trainer<krnl>,krnl>(classifier);
  mult.train(preprocessor.samples,preprocessor.labels);

//  classifier.import_data(datapoints);
//
//  classifier.train();
//  classifier.export_params("test.param");

//  std::vector<SingleDecisionProblem> problems;
//  SingleDecisionProblem problem;
//  problem.trainer=classifier.get_trainer();
//  problem.class_one=0;
//  problem.class_two=0;
//  problems.push_back(problem);
//  MultiClassifier mult_class(problems);
//  mult_class.import_params("test.param");
//  mult_class.import_data(datapoints);
//mult_class.train();

  return 0;
}
