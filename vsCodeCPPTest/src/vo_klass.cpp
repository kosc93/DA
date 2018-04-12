/*
 * vo_klass.cpp
 *
 *  Created on: Mar 5, 2018
 *      Author: kosc93
 */

#include "classifier.h"
#include "preprocessing.h"
#include "data_gateway.h"
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
  parser.add_option("G","open gateway at the end to accept more data to classify");
  parser.add_option("v","set verbose output");
  parser.add_option("reps","flag to use repeated measurements");
  parser.add_option("u","use unformatted data(without any transformation)");
  parser.add_option("traces","traces to display, please put a whitespace separated list in scores like 'Tr1 Tr3 Tr4'",1);
  parser.add_option("freqs","frequency range to display, please put a whitespace separated Hz range in scores like '2e9 1e10'",1);
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
  stringstream filter_hash("");
  //input traces for filtering
  std::vector<string> filt_traces;
  if(parser.option("traces")){
      std::stringstream traces(parser.option("traces").argument());
      std::string trace_str;
      while(traces>>trace_str){
	  filter_hash<<trace_str<<"_";
	  filt_traces.push_back(trace_str);
      }
      if(filt_traces.size()==0){
      cout<<"traces input not recognized"<<endl;
      return -1;
      }
  }
  // frequency span to be extracted
  FrequencySpan filt_freqs;
  if(parser.option("freqs")){
      std::stringstream freqs(parser.option("freqs").argument());
      double start_freq=0,stop_freq=0;
      while(freqs>>start_freq>>stop_freq){
	filt_freqs.start_freq=start_freq;
	filt_freqs.stop_freq=stop_freq;
	filter_hash<<start_freq<<"_"<<stop_freq;
      }
      if(start_freq==0&&stop_freq==0){
      cout<<"frequency input not recognized"<<endl;
      return -1;
      }
  }

  Preprocessor preprocessor;
  preprocessor<<filter_hash.str();
  //check if filename is validrbf_kernel
  if(parser.option("F")){
      string filename;
      std::vector<DataPoint> datapoints;
      filename=parser.option("F").argument();
      H5MeasurementFile file(filename);
        //make sure that there is data to b processed
        std::vector<Measurement> data = file.scan(parser.option("reps"),parser.option("u"),filt_traces,filt_freqs);

        if(data.size()==0){
            cout<<"no measurements found"<<endl;
            return -1;
        }
        for(auto& measurement : data){
            if(measurement.label==0){
	      cout<<"not all measurements are labeled"<<endl;
	      return -1;
            }
            datapoints.push_back(measurement.getDatapoint());

        }
        cout<<"all measurements are labeled"<<endl;

        //preprocess data
        preprocessor.import_data(datapoints);
        if(parser.option("I")){
              preprocessor.import_params(parser.option("I").argument());
	}
        //randomize datapoint order
	preprocessor.rand_and_normalize();
	//calculate lda
	preprocessor.reduce_dim();

        if(parser.option("E")){
              preprocessor.export_params(parser.option("E").argument());
	}
  }else{
      cout<<"Filename option ist mandatory "<<endl;
      return 0;
  }
  //define kernel and classifier
  typedef lin_kernel krnl;
  typedef svm_c_trainer<krnl> bin_class;
  typedef svm_multiclass_linear_trainer<krnl,double> mult_trainer;
  //typedef one_vs_all_decision_function<ova_trainer,decision_function<krnl>>  mult_func_type;
  typedef multiclass_linear_decision_function<krnl,double>  mult_func_type;
  MultDecisionFunc<mult_func_type> learned_func;
  learned_func<<filter_hash.str();
  //handle decision function import
  if(parser.option("I")){
      if(!learned_func.import_function(parser.option("I").argument())){
	  cout<<"start multiclass training"<<endl;
	  //create multiclass classifier and decision function objects
	  MultClassifier<mult_trainer,krnl> mult;
	  //mult.import_classifiers<bin_class,krnl>(BinClassifier<bin_class,krnl>(5));
	  learned_func = mult.train<mult_func_type>(preprocessor.samples,preprocessor.labels);
      }
  }else{
      cout<<"start multiclass training"<<endl;
      //create multiclass classifier and decision function objects
      MultClassifier<mult_trainer,krnl> mult;
      //mult.import_classifiers<bin_class,krnl>(BinClassifier<bin_class,krnl>(5));
      learned_func = mult.train<mult_func_type>(preprocessor.samples,preprocessor.labels);
  }

  if(parser.option("E")){
      learned_func<<filter_hash.str();
      learned_func.export_function(parser.option("E").argument());
  }


  //verbose mode prints confusion matrix
  if(parser.option("v")){
      cout<<test_multiclass_decision_function(learned_func.mult_decision_function,preprocessor.samples,preprocessor.labels)<<endl;
  }
  //open a gateway to feed new measurements via telnet and classify them with the learned decision function
  if(parser.option("G")){
    typedef decltype(learned_func) gateway_func_type;
    DataGateway<gateway_func_type> gateway(preprocessor,learned_func);
    gateway.set_listening_port(1234);
    gateway.start_async();
    cout << "Press enter to end this program" << endl;
    cin.get();
  }

  return 0;
}
