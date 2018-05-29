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

template<class dec_func>
  void
  print_score (const dec_func& multi_dec_func, const Preprocessor& preprocessor)
  {
    dlib::matrix<double> score_mat = test_multiclass_decision_function (
	multi_dec_func, preprocessor.test_samples, preprocessor.test_labels);
    cout << "confusion_matrix" << endl;
    for (const auto& label : multi_dec_func.get_labels ())
      {
	cout << label << " ";
      }
    cout << endl;
    cout << score_mat << endl;
    cout << "score:" << trace (score_mat) / preprocessor.test_labels.size ()
	<< endl;
  }

int
main (int argc, char **argv)
{
  dlib::command_line_parser parser;
  parser.add_option ("F", "Filename of labeled HDF5 data", 1);
  parser.add_option (
      "test",
      "Testfiles containing a labeled HDF5 data, like 'test0.h5 test1.h5'", 1);
  parser.add_option (
      "train",
      "Trainfiles containing a labeled HDF5 data, like 'train0.h5 train1.h5'",
      1);
  parser.add_option ("I", "Filename of import file", 1);
  parser.add_option ("E", "Filename of export", 1);
  parser.add_option ("G",
		     "open gateway at the end to accept more data to classify");
  parser.add_option ("v", "set verbose output");
  parser.add_option ("reps", "flag to use repeated measurements");
  parser.add_option ("u", "use unformatted data(without any transformation)");
  parser.add_option ("n", "normalize data");
  parser.add_option (
      "traces",
      "traces to display, please put a whitespace separated list in scores like 'Trc1 Trc3 Trc4'",
      1);
  parser.add_option (
      "freqs",
      "frequency range to display, please put a whitespace separated Hz range in scores like '2e9 1e10'",
      1);
  parser.add_option ("kernel",
		     "kernel to apply, choose r for rbf or l for linear kernel",
		     1);
  parser.add_option (
      "bin",
      "binary classifier for the ova or ovo trainer, choose n for svm-nu, c vor svm_c, k for krr or r for rvm",
      1);
  parser.add_option (
      "mult",
      "multiclass classifier to apply, choose o for ovo, a for ova  or s for multiclass svm",
      1);
  parser.add_option ("h", "Display this help message.");
  parser.parse (argc, argv);
  const char* one_time_opts[] =
    { "F", "u", "h", "reps" };
  parser.check_one_time_options (one_time_opts);
  //check for help option
  if (parser.option ("h"))
    {
      parser.print_options ();
      return 0;
    }
  stringstream filter_hash ("");
  //input traces for filtering
  std::vector<string> filt_traces;
  if (parser.option ("traces"))
    {
      std::stringstream traces (parser.option ("traces").argument ());
      std::string trace_str;
      while (traces >> trace_str)
	{
	  filter_hash << trace_str << "_";
	  filt_traces.push_back (trace_str);
	}
      if (filt_traces.size () == 0)
	{
	  cout << "traces input not recognized" << endl;
	  return -1;
	}
    }
  // frequency span to be extracted
  FrequencySpan filt_freqs;
  if (parser.option ("freqs"))
    {
      std::stringstream freqs (parser.option ("freqs").argument ());
      double start_freq = 0, stop_freq = 0;
      while (freqs >> start_freq >> stop_freq)
	{
	  filt_freqs.start_freq = start_freq;
	  filt_freqs.stop_freq = stop_freq;
	  filter_hash << start_freq << "_" << stop_freq;
	}
      if (start_freq == 0 && stop_freq == 0)
	{
	  cout << "frequency input not recognized" << endl;
	  return -1;
	}
    }

  Preprocessor preprocessor;
  preprocessor << filter_hash.str ();
  preprocessor.norm = parser.option ("n");
  //check if filename is validrbf_kernel
  if (parser.option ("F") || (parser.option ("test") && parser.option ("test")))
    {
      if (!parser.option ("test"))
	{
	  string filename;
	  std::vector<DataPoint> datapoints;
	  filename = parser.option ("F").argument ();
	  H5MeasurementFile file (filename);
	  //make sure that there is data to b processed
	  std::vector<Measurement> data = file.scan (parser.option ("reps"),
						     parser.option ("u"),
						     filt_traces, filt_freqs);

	  if (data.size () == 0)
	    {
	      cout << "no measurements found" << endl;
	      return -1;
	    }
	  for (auto& measurement : data)
	    {
	      if (measurement.label == 0)
		{
		  cout << "not all measurements are labeled" << endl;
		  return -1;
		}
	      datapoints.push_back (measurement.getDatapoint ());

	    }
	  cout << "all measurements are labeled" << endl;

	  //preprocess data
	  preprocessor.import_data (datapoints);
	  if (parser.option ("I"))
	    {
	      preprocessor.import_params (parser.option ("I").argument ());
	    }
	  //randomize data order
	  preprocessor.randomize ();
	  //normalize data
	  if (parser.option ("n"))
	    {
	      preprocessor.normalize ();
	    }
	  //split in test an training set

	  preprocessor.split ();
	}
      else
	{
	  string test_filenames, train_filenames, test_filename, train_filename;
	  test_filenames = parser.option ("test").argument ();
	  train_filenames = parser.option ("train").argument ();
	  stringstream test_stream (test_filenames);
	  stringstream train_stream (train_filenames);
	  std::vector<DataPoint> test_points, train_points;
	  while (train_stream >> train_filename)
	    {
	      H5MeasurementFile file (train_filename);
	      //make sure that there is data to b processed
	      std::vector<Measurement> data = file.scan (parser.option ("reps"),
							 parser.option ("u"),
							 filt_traces,
							 filt_freqs);

	      if (data.size () == 0)
		{
		  cout << "no measurements found" << endl;
		  return -1;
		}
	      for (auto& measurement : data)
		{
		  if (measurement.label == 0)
		    {
		      cout << "not all measurements are labeled" << endl;
		      return -1;
		    }
		  train_points.push_back (measurement.getDatapoint ());
		}
	    }
	  while (test_stream >> test_filename)
	    {
	      H5MeasurementFile file (test_filename);
	      //make sure that there is data to b processed
	      std::vector<Measurement> data = file.scan (parser.option ("reps"),
							 parser.option ("u"),
							 filt_traces,
							 filt_freqs);

	      if (data.size () == 0)
		{
		  cout << "no measurements found" << endl;
		  return -1;
		}
	      for (auto& measurement : data)
		{
		  if (measurement.label == 0)
		    {
		      cout << "not all measurements are labeled" << endl;
		      return -1;
		    }
		  test_points.push_back (measurement.getDatapoint ());
		}
	    }
	  preprocessor.import_train (train_points);
	  preprocessor.import_test (test_points);
	}

      //calculate lda
      preprocessor.reduce_dim ();

      if (parser.option ("E"))
	{
	  preprocessor.export_params (parser.option ("E").argument ());
	}
    }
  else
    {
      cout << "Filename option ist mandatory " << endl;
      return 0;
    }
  string kernel_arg = "";
  if (parser.option ("kernel"))
    {
      kernel_arg = parser.option ("kernel").argument ();
    }
  string trainer_arg = "";
  if (parser.option ("mult"))
    {
      trainer_arg = parser.option ("mult").argument ();
    }
  if (trainer_arg == "o")
    {
      string bin_arg = "";
      parser.option ("bin").argument ();
      if (parser.option ("bin"))
	{
	  bin_arg = parser.option ("bin").argument ();
	}
      if (kernel_arg == "r")
	{
	  if (bin_arg == "n")
	    {
	      typedef rbf_kernel krnl;
	      typedef svm_nu_trainer<krnl> bin_class;
	      //typedef svm_multiclass_linear_trainer<krnl,double> mult_trainer;
	      //typedef multiclass_linear_decision_function<krnl,double>  mult_func_type;
	      typedef ovo_trainer mult_trainer;
	      typedef one_vs_one_decision_function<ovo_trainer,
		  decision_function<krnl>> mult_func_type;
	      MultDecisionFunc<mult_func_type> learned_func;
	      learned_func << filter_hash.str ();
	      //handle decision function import
	      if (parser.option ("I"))
		{
		  if (!learned_func.import_function (
		      parser.option ("I").argument ()))
		    {
		      cout << "start ovo svm-nu rbf-kernel training" << endl;
		      //create multiclass classifier and decision function objects
		      MultClassifier<mult_trainer, krnl> mult;
		      mult.import_classifiers<bin_class, krnl> (
			  BinClassifier<bin_class, krnl> (4));
		      learned_func = mult.train<mult_func_type> (
			  preprocessor.train_samples,
			  preprocessor.train_labels);
		    }
		}
	      else
		{
		  cout << "start ovo svm-nu rbf-kernel training" << endl;
		  //create multiclass classifier and decision function objects
		  MultClassifier<mult_trainer, krnl> mult;
		  mult.import_classifiers<bin_class, krnl> (
		      BinClassifier<bin_class, krnl> (5));
		  learned_func = mult.train<mult_func_type> (
		      preprocessor.train_samples, preprocessor.train_labels);
		}

	      if (parser.option ("E"))
		{
		  learned_func << filter_hash.str ();
		  learned_func.export_function (
		      parser.option ("E").argument ());
		}

	      //verbose mode prints confusion matrix
	      if (parser.option ("v"))
		{
		  print_score (learned_func.mult_decision_function,
			       preprocessor);
		}
	      //open a gateway to feed new measurements via telnet and classify them with the learned decision function
	      if (parser.option ("G"))
		{
		  typedef decltype(learned_func) gateway_func_type;
		  DataGateway<gateway_func_type> gateway (preprocessor,
							  learned_func);
		  gateway.set_listening_port (1234);
		  gateway.start_async ();
		  cout << "Press enter to end this program" << endl;
		  cin.get ();
		}

	      return 0;
	    }
	  else if (bin_arg == "c")
	    {
	      typedef rbf_kernel krnl;
	      typedef svm_c_trainer<krnl> bin_class;
	      //typedef svm_multiclass_linear_trainer<krnl,double> mult_trainer;
	      //typedef multiclass_linear_decision_function<krnl,double>  mult_func_type;
	      typedef ovo_trainer mult_trainer;
	      typedef one_vs_one_decision_function<ovo_trainer,
		  decision_function<krnl>> mult_func_type;
	      MultDecisionFunc<mult_func_type> learned_func;
	      learned_func << filter_hash.str ();
	      //handle decision function import
	      if (parser.option ("I"))
		{
		  if (!learned_func.import_function (
		      parser.option ("I").argument ()))
		    {
		      cout << "start ovo svm-c rbf-kernel training" << endl;
		      //create multiclass classifier and decision function objects
		      MultClassifier<mult_trainer, krnl> mult;
		      mult.import_classifiers<bin_class, krnl> (
			  BinClassifier<bin_class, krnl> (4));
		      learned_func = mult.train<mult_func_type> (
			  preprocessor.train_samples,
			  preprocessor.train_labels);
		    }
		}
	      else
		{
		  cout << "start ovo svm-c rbf-kernel training" << endl;
		  //create multiclass classifier and decision function objects
		  MultClassifier<mult_trainer, krnl> mult;
		  mult.import_classifiers<bin_class, krnl> (
		      BinClassifier<bin_class, krnl> (5));
		  learned_func = mult.train<mult_func_type> (
		      preprocessor.train_samples, preprocessor.train_labels);
		}

	      if (parser.option ("E"))
		{
		  learned_func << filter_hash.str ();
		  learned_func.export_function (
		      parser.option ("E").argument ());
		}

	      //verbose mode prints confusion matrix
	      if (parser.option ("v"))
		{
		  print_score (learned_func.mult_decision_function,
			       preprocessor);
		}
	      //open a gateway to feed new measurements via telnet and classify them with the learned decision function
	      if (parser.option ("G"))
		{
		  typedef decltype(learned_func) gateway_func_type;
		  DataGateway<gateway_func_type> gateway (preprocessor,
							  learned_func);
		  gateway.set_listening_port (1234);
		  gateway.start_async ();
		  cout << "Press enter to end this program" << endl;
		  cin.get ();
		}

	      return 0;
	    }
	  else if (bin_arg == "r")
	    {
	      typedef rbf_kernel krnl;
	      typedef rvm_trainer<krnl> bin_class;
	      //typedef svm_multiclass_linear_trainer<krnl,double> mult_trainer;
	      //typedef multiclass_linear_decision_function<krnl,double>  mult_func_type;
	      typedef ovo_trainer mult_trainer;
	      typedef one_vs_one_decision_function<ovo_trainer,
		  decision_function<krnl>> mult_func_type;
	      MultDecisionFunc<mult_func_type> learned_func;
	      learned_func << filter_hash.str ();
	      //handle decision function import
	      if (parser.option ("I"))
		{
		  if (!learned_func.import_function (
		      parser.option ("I").argument ()))
		    {
		      cout << "start ovo rvm rbf-kernel training" << endl;
		      //create multiclass classifier and decision function objects
		      MultClassifier<mult_trainer, krnl> mult;
		      mult.import_classifiers<bin_class, krnl> (
			  BinClassifier<bin_class, krnl> (4));
		      learned_func = mult.train<mult_func_type> (
			  preprocessor.train_samples,
			  preprocessor.train_labels);
		    }
		}
	      else
		{
		  cout << "start ovo rvm rbf-kernel training" << endl;
		  //create multiclass classifier and decision function objects
		  MultClassifier<mult_trainer, krnl> mult;
		  mult.import_classifiers<bin_class, krnl> (
		      BinClassifier<bin_class, krnl> (5));
		  learned_func = mult.train<mult_func_type> (
		      preprocessor.train_samples, preprocessor.train_labels);
		}

	      if (parser.option ("E"))
		{
		  learned_func << filter_hash.str ();
		  learned_func.export_function (
		      parser.option ("E").argument ());
		}

	      //verbose mode prints confusion matrix
	      if (parser.option ("v"))
		{
		  print_score (learned_func.mult_decision_function,
			       preprocessor);
		}
	      //open a gateway to feed new measurements via telnet and classify them with the learned decision function
	      if (parser.option ("G"))
		{
		  typedef decltype(learned_func) gateway_func_type;
		  DataGateway<gateway_func_type> gateway (preprocessor,
							  learned_func);
		  gateway.set_listening_port (1234);
		  gateway.start_async ();
		  cout << "Press enter to end this program" << endl;
		  cin.get ();
		}

	      return 0;
	    }
	  else if (bin_arg == "k")
	    {
	      typedef rbf_kernel krnl;
	      typedef krr_trainer<krnl> bin_class;
	      //typedef svm_multiclass_linear_trainer<krnl,double> mult_trainer;
	      //typedef multiclass_linear_decision_function<krnl,double>  mult_func_type;
	      typedef ovo_trainer mult_trainer;
	      typedef one_vs_one_decision_function<ovo_trainer,
		  decision_function<krnl>> mult_func_type;
	      MultDecisionFunc<mult_func_type> learned_func;
	      learned_func << filter_hash.str ();
	      //handle decision function import
	      if (parser.option ("I"))
		{
		  if (!learned_func.import_function (
		      parser.option ("I").argument ()))
		    {
		      cout << "start ovo krr rbf-kernel training" << endl;
		      //create multiclass classifier and decision function objects
		      MultClassifier<mult_trainer, krnl> mult;
		      mult.import_classifiers<bin_class, krnl> (
			  BinClassifier<bin_class, krnl> (4));
		      learned_func = mult.train<mult_func_type> (
			  preprocessor.train_samples,
			  preprocessor.train_labels);
		    }
		}
	      else
		{
		  cout << "start ovo krr rbf-kernel training" << endl;
		  //create multiclass classifier and decision function objects
		  MultClassifier<mult_trainer, krnl> mult;
		  mult.import_classifiers<bin_class, krnl> (
		      BinClassifier<bin_class, krnl> (5));
		  learned_func = mult.train<mult_func_type> (
		      preprocessor.train_samples, preprocessor.train_labels);
		}

	      if (parser.option ("E"))
		{
		  learned_func << filter_hash.str ();
		  learned_func.export_function (
		      parser.option ("E").argument ());
		}

	      //verbose mode prints confusion matrix
	      if (parser.option ("v"))
		{
		  print_score (learned_func.mult_decision_function,
			       preprocessor);
		}
	      //open a gateway to feed new measurements via telnet and classify them with the learned decision function
	      if (parser.option ("G"))
		{
		  typedef decltype(learned_func) gateway_func_type;
		  DataGateway<gateway_func_type> gateway (preprocessor,
							  learned_func);
		  gateway.set_listening_port (1234);
		  gateway.start_async ();
		  cout << "Press enter to end this program" << endl;
		  cin.get ();
		}

	      return 0;
	    }
	}
      else
	{
	  if (bin_arg == "n")
	    {
	      typedef lin_kernel krnl;
	      typedef svm_nu_trainer<krnl> bin_class;
	      //typedef svm_multiclass_linear_trainer<krnl,double> mult_trainer;
	      //typedef multiclass_linear_decision_function<krnl,double>  mult_func_type;
	      typedef ovo_trainer mult_trainer;
	      typedef one_vs_one_decision_function<ovo_trainer,
		  decision_function<krnl>> mult_func_type;
	      MultDecisionFunc<mult_func_type> learned_func;
	      learned_func << filter_hash.str ();
	      //handle decision function import
	      if (parser.option ("I"))
		{
		  if (!learned_func.import_function (
		      parser.option ("I").argument ()))
		    {
		      cout << "start ovo svm-nu lin-kernel training" << endl;
		      //create multiclass classifier and decision function objects
		      MultClassifier<mult_trainer, krnl> mult;
		      mult.import_classifiers<bin_class, krnl> (
			  BinClassifier<bin_class, krnl> (4));
		      learned_func = mult.train<mult_func_type> (
			  preprocessor.train_samples,
			  preprocessor.train_labels);
		    }
		}
	      else
		{
		  cout << "start ovo svm-nu lin-kernel training" << endl;
		  //create multiclass classifier and decision function objects
		  MultClassifier<mult_trainer, krnl> mult;
		  mult.import_classifiers<bin_class, krnl> (
		      BinClassifier<bin_class, krnl> (5));
		  learned_func = mult.train<mult_func_type> (
		      preprocessor.train_samples, preprocessor.train_labels);
		}

	      if (parser.option ("E"))
		{
		  learned_func << filter_hash.str ();
		  learned_func.export_function (
		      parser.option ("E").argument ());
		}

	      //verbose mode prints confusion matrix
	      if (parser.option ("v"))
		{
		  print_score (learned_func.mult_decision_function,
			       preprocessor);
		}
	      //open a gateway to feed new measurements via telnet and classify them with the learned decision function
	      if (parser.option ("G"))
		{
		  typedef decltype(learned_func) gateway_func_type;
		  DataGateway<gateway_func_type> gateway (preprocessor,
							  learned_func);
		  gateway.set_listening_port (1234);
		  gateway.start_async ();
		  cout << "Press enter to end this program" << endl;
		  cin.get ();
		}

	      return 0;
	    }
	  else if (bin_arg == "c")
	    {
	      typedef lin_kernel krnl;
	      typedef svm_c_trainer<krnl> bin_class;
	      //typedef svm_multiclass_linear_trainer<krnl,double> mult_trainer;
	      //typedef multiclass_linear_decision_function<krnl,double>  mult_func_type;
	      typedef ovo_trainer mult_trainer;
	      typedef one_vs_one_decision_function<ovo_trainer,
		  decision_function<krnl>> mult_func_type;
	      MultDecisionFunc<mult_func_type> learned_func;
	      learned_func << filter_hash.str ();
	      //handle decision function import
	      if (parser.option ("I"))
		{
		  if (!learned_func.import_function (
		      parser.option ("I").argument ()))
		    {
		      cout << "start ovo svm-c lin-kernel training" << endl;
		      //create multiclass classifier and decision function objects
		      MultClassifier<mult_trainer, krnl> mult;
		      mult.import_classifiers<bin_class, krnl> (
			  BinClassifier<bin_class, krnl> (4));
		      learned_func = mult.train<mult_func_type> (
			  preprocessor.train_samples,
			  preprocessor.train_labels);
		    }
		}
	      else
		{
		  cout << "start ovo svm-c lin-kernel training" << endl;
		  //create multiclass classifier and decision function objects
		  MultClassifier<mult_trainer, krnl> mult;
		  mult.import_classifiers<bin_class, krnl> (
		      BinClassifier<bin_class, krnl> (5));
		  learned_func = mult.train<mult_func_type> (
		      preprocessor.train_samples, preprocessor.train_labels);
		}

	      if (parser.option ("E"))
		{
		  learned_func << filter_hash.str ();
		  learned_func.export_function (
		      parser.option ("E").argument ());
		}

	      //verbose mode prints confusion matrix
	      if (parser.option ("v"))
		{
		  print_score (learned_func.mult_decision_function,
			       preprocessor);
		}
	      //open a gateway to feed new measurements via telnet and classify them with the learned decision function
	      if (parser.option ("G"))
		{
		  typedef decltype(learned_func) gateway_func_type;
		  DataGateway<gateway_func_type> gateway (preprocessor,
							  learned_func);
		  gateway.set_listening_port (1234);
		  gateway.start_async ();
		  cout << "Press enter to end this program" << endl;
		  cin.get ();
		}

	      return 0;
	    }
	  else if (bin_arg == "r")
	    {
	      typedef lin_kernel krnl;
	      typedef rvm_trainer<krnl> bin_class;
	      //typedef svm_multiclass_linear_trainer<krnl,double> mult_trainer;
	      //typedef multiclass_linear_decision_function<krnl,double>  mult_func_type;
	      typedef ovo_trainer mult_trainer;
	      typedef one_vs_one_decision_function<ovo_trainer,
		  decision_function<krnl>> mult_func_type;
	      MultDecisionFunc<mult_func_type> learned_func;
	      learned_func << filter_hash.str ();
	      //handle decision function import
	      if (parser.option ("I"))
		{
		  if (!learned_func.import_function (
		      parser.option ("I").argument ()))
		    {
		      cout << "start ovo rvm lin-kernel training" << endl;
		      //create multiclass classifier and decision function objects
		      MultClassifier<mult_trainer, krnl> mult;
		      mult.import_classifiers<bin_class, krnl> (
			  BinClassifier<bin_class, krnl> (4));
		      learned_func = mult.train<mult_func_type> (
			  preprocessor.train_samples,
			  preprocessor.train_labels);
		    }
		}
	      else
		{
		  cout << "start ovo rvm lin-kernel training" << endl;
		  //create multiclass classifier and decision function objects
		  MultClassifier<mult_trainer, krnl> mult;
		  mult.import_classifiers<bin_class, krnl> (
		      BinClassifier<bin_class, krnl> (5));
		  learned_func = mult.train<mult_func_type> (
		      preprocessor.train_samples, preprocessor.train_labels);
		}

	      if (parser.option ("E"))
		{
		  learned_func << filter_hash.str ();
		  learned_func.export_function (
		      parser.option ("E").argument ());
		}

	      //verbose mode prints confusion matrix
	      if (parser.option ("v"))
		{
		  print_score (learned_func.mult_decision_function,
			       preprocessor);
		}
	      //open a gateway to feed new measurements via telnet and classify them with the learned decision function
	      if (parser.option ("G"))
		{
		  typedef decltype(learned_func) gateway_func_type;
		  DataGateway<gateway_func_type> gateway (preprocessor,
							  learned_func);
		  gateway.set_listening_port (1234);
		  gateway.start_async ();
		  cout << "Press enter to end this program" << endl;
		  cin.get ();
		}

	      return 0;
	    }
	  else if (bin_arg == "k")
	    {
	      typedef lin_kernel krnl;
	      typedef krr_trainer<krnl> bin_class;
	      //typedef svm_multiclass_linear_trainer<krnl,double> mult_trainer;
	      //typedef multiclass_linear_decision_function<krnl,double>  mult_func_type;
	      typedef ovo_trainer mult_trainer;
	      typedef one_vs_one_decision_function<ovo_trainer,
		  decision_function<krnl>> mult_func_type;
	      MultDecisionFunc<mult_func_type> learned_func;
	      learned_func << filter_hash.str ();
	      //handle decision function import
	      if (parser.option ("I"))
		{
		  if (!learned_func.import_function (
		      parser.option ("I").argument ()))
		    {
		      cout << "start ovo krr lin-kernel training" << endl;
		      //create multiclass classifier and decision function objects
		      MultClassifier<mult_trainer, krnl> mult;
		      mult.import_classifiers<bin_class, krnl> (
			  BinClassifier<bin_class, krnl> (4));
		      learned_func = mult.train<mult_func_type> (
			  preprocessor.train_samples,
			  preprocessor.train_labels);
		    }
		}
	      else
		{
		  cout << "start ovo krr lin-kernel training" << endl;
		  //create multiclass classifier and decision function objects
		  MultClassifier<mult_trainer, krnl> mult;
		  mult.import_classifiers<bin_class, krnl> (
		      BinClassifier<bin_class, krnl> (5));
		  learned_func = mult.train<mult_func_type> (
		      preprocessor.train_samples, preprocessor.train_labels);
		}

	      if (parser.option ("E"))
		{
		  learned_func << filter_hash.str ();
		  learned_func.export_function (
		      parser.option ("E").argument ());
		}

	      //verbose mode prints confusion matrix
	      if (parser.option ("v"))
		{
		  print_score (learned_func.mult_decision_function,
			       preprocessor);
		}
	      //open a gateway to feed new measurements via telnet and classify them with the learned decision function
	      if (parser.option ("G"))
		{
		  typedef decltype(learned_func) gateway_func_type;
		  DataGateway<gateway_func_type> gateway (preprocessor,
							  learned_func);
		  gateway.set_listening_port (1234);
		  gateway.start_async ();
		  cout << "Press enter to end this program" << endl;
		  cin.get ();
		}

	      return 0;
	    }
	}
    }
  else if (trainer_arg == "a")
    {
      string bin_arg = "";
      parser.option ("bin").argument ();
      if (parser.option ("bin"))
	{
	  bin_arg = parser.option ("bin").argument ();
	}
      if (kernel_arg == "r")
	{
	  if (bin_arg == "n")
	    {
	      typedef rbf_kernel krnl;
	      typedef svm_nu_trainer<krnl> bin_class;
	      //typedef svm_multiclass_linear_trainer<krnl,double> mult_trainer;
	      //typedef multiclass_linear_decision_function<krnl,double>  mult_func_type;
	      typedef ova_trainer mult_trainer;
	      typedef one_vs_all_decision_function<ova_trainer,
		  decision_function<krnl>> mult_func_type;
	      MultDecisionFunc<mult_func_type> learned_func;
	      learned_func << filter_hash.str ();
	      //handle decision function import
	      if (parser.option ("I"))
		{
		  if (!learned_func.import_function (
		      parser.option ("I").argument ()))
		    {
		      cout << "start ova svm-nu rbf-kernel training" << endl;
		      //create multiclass classifier and decision function objects
		      MultClassifier<mult_trainer, krnl> mult;
		      mult.import_classifiers<bin_class, krnl> (
			  BinClassifier<bin_class, krnl> (4));
		      learned_func = mult.train<mult_func_type> (
			  preprocessor.train_samples,
			  preprocessor.train_labels);
		    }
		}
	      else
		{
		  cout << "start ova svm-nu rbf-kernel training" << endl;
		  //create multiclass classifier and decision function objects
		  MultClassifier<mult_trainer, krnl> mult;
		  mult.import_classifiers<bin_class, krnl> (
		      BinClassifier<bin_class, krnl> (5));
		  learned_func = mult.train<mult_func_type> (
		      preprocessor.train_samples, preprocessor.train_labels);
		}

	      if (parser.option ("E"))
		{
		  learned_func << filter_hash.str ();
		  learned_func.export_function (
		      parser.option ("E").argument ());
		}

	      //verbose mode prints confusion matrix
	      if (parser.option ("v"))
		{
		  print_score (learned_func.mult_decision_function,
			       preprocessor);
		}
	      //open a gateway to feed new measurements via telnet and classify them with the learned decision function
	      if (parser.option ("G"))
		{
		  typedef decltype(learned_func) gateway_func_type;
		  DataGateway<gateway_func_type> gateway (preprocessor,
							  learned_func);
		  gateway.set_listening_port (1234);
		  gateway.start_async ();
		  cout << "Press enter to end this program" << endl;
		  cin.get ();
		}

	      return 0;
	    }
	  else if (bin_arg == "c")
	    {
	      typedef rbf_kernel krnl;
	      typedef svm_c_trainer<krnl> bin_class;
	      //typedef svm_multiclass_linear_trainer<krnl,double> mult_trainer;
	      //typedef multiclass_linear_decision_function<krnl,double>  mult_func_type;
	      typedef ova_trainer mult_trainer;
	      typedef one_vs_all_decision_function<ova_trainer,
		  decision_function<krnl>> mult_func_type;
	      MultDecisionFunc<mult_func_type> learned_func;
	      learned_func << filter_hash.str ();
	      //handle decision function import
	      if (parser.option ("I"))
		{
		  if (!learned_func.import_function (
		      parser.option ("I").argument ()))
		    {
		      cout << "start ova svm-c rbf-kernel training" << endl;
		      //create multiclass classifier and decision function objects
		      MultClassifier<mult_trainer, krnl> mult;
		      mult.import_classifiers<bin_class, krnl> (
			  BinClassifier<bin_class, krnl> (4));
		      learned_func = mult.train<mult_func_type> (
			  preprocessor.train_samples,
			  preprocessor.train_labels);
		    }
		}
	      else
		{
		  cout << "start ova svm-c rbf-kernel training" << endl;
		  //create multiclass classifier and decision function objects
		  MultClassifier<mult_trainer, krnl> mult;
		  mult.import_classifiers<bin_class, krnl> (
		      BinClassifier<bin_class, krnl> (5));
		  learned_func = mult.train<mult_func_type> (
		      preprocessor.train_samples, preprocessor.train_labels);
		}

	      if (parser.option ("E"))
		{
		  learned_func << filter_hash.str ();
		  learned_func.export_function (
		      parser.option ("E").argument ());
		}

	      //verbose mode prints confusion matrix
	      if (parser.option ("v"))
		{
		  print_score (learned_func.mult_decision_function,
			       preprocessor);
		}
	      //open a gateway to feed new measurements via telnet and classify them with the learned decision function
	      if (parser.option ("G"))
		{
		  typedef decltype(learned_func) gateway_func_type;
		  DataGateway<gateway_func_type> gateway (preprocessor,
							  learned_func);
		  gateway.set_listening_port (1234);
		  gateway.start_async ();
		  cout << "Press enter to end this program" << endl;
		  cin.get ();
		}

	      return 0;
	    }
	  else if (bin_arg == "r")
	    {
	      typedef rbf_kernel krnl;
	      typedef rvm_trainer<krnl> bin_class;
	      //typedef svm_multiclass_linear_trainer<krnl,double> mult_trainer;
	      //typedef multiclass_linear_decision_function<krnl,double>  mult_func_type;
	      typedef ova_trainer mult_trainer;
	      typedef one_vs_all_decision_function<ova_trainer,
		  decision_function<krnl>> mult_func_type;
	      MultDecisionFunc<mult_func_type> learned_func;
	      learned_func << filter_hash.str ();
	      //handle decision function import
	      if (parser.option ("I"))
		{
		  if (!learned_func.import_function (
		      parser.option ("I").argument ()))
		    {
		      cout << "start ova rvm rbf-kernel training" << endl;
		      //create multiclass classifier and decision function objects
		      MultClassifier<mult_trainer, krnl> mult;
		      mult.import_classifiers<bin_class, krnl> (
			  BinClassifier<bin_class, krnl> (4));
		      learned_func = mult.train<mult_func_type> (
			  preprocessor.train_samples,
			  preprocessor.train_labels);
		    }
		}
	      else
		{
		  cout << "start ova rvm rbf-kernel training" << endl;
		  //create multiclass classifier and decision function objects
		  MultClassifier<mult_trainer, krnl> mult;
		  mult.import_classifiers<bin_class, krnl> (
		      BinClassifier<bin_class, krnl> (5));
		  learned_func = mult.train<mult_func_type> (
		      preprocessor.train_samples, preprocessor.train_labels);
		}

	      if (parser.option ("E"))
		{
		  learned_func << filter_hash.str ();
		  learned_func.export_function (
		      parser.option ("E").argument ());
		}

	      //verbose mode prints confusion matrix
	      if (parser.option ("v"))
		{
		  print_score (learned_func.mult_decision_function,
			       preprocessor);
		}
	      //open a gateway to feed new measurements via telnet and classify them with the learned decision function
	      if (parser.option ("G"))
		{
		  typedef decltype(learned_func) gateway_func_type;
		  DataGateway<gateway_func_type> gateway (preprocessor,
							  learned_func);
		  gateway.set_listening_port (1234);
		  gateway.start_async ();
		  cout << "Press enter to end this program" << endl;
		  cin.get ();
		}

	      return 0;
	    }
	  else if (bin_arg == "k")
	    {
	      typedef rbf_kernel krnl;
	      typedef krr_trainer<krnl> bin_class;
	      //typedef svm_multiclass_linear_trainer<krnl,double> mult_trainer;
	      //typedef multiclass_linear_decision_function<krnl,double>  mult_func_type;
	      typedef ova_trainer mult_trainer;
	      typedef one_vs_all_decision_function<ova_trainer,
		  decision_function<krnl>> mult_func_type;
	      MultDecisionFunc<mult_func_type> learned_func;
	      learned_func << filter_hash.str ();
	      //handle decision function import
	      if (parser.option ("I"))
		{
		  if (!learned_func.import_function (
		      parser.option ("I").argument ()))
		    {
		      cout << "start ova krr rbf-kernel training" << endl;
		      //create multiclass classifier and decision function objects
		      MultClassifier<mult_trainer, krnl> mult;
		      mult.import_classifiers<bin_class, krnl> (
			  BinClassifier<bin_class, krnl> (4));
		      learned_func = mult.train<mult_func_type> (
			  preprocessor.train_samples,
			  preprocessor.train_labels);
		    }
		}
	      else
		{
		  cout << "start ova krr rbf-kernel training" << endl;
		  //create multiclass classifier and decision function objects
		  MultClassifier<mult_trainer, krnl> mult;
		  mult.import_classifiers<bin_class, krnl> (
		      BinClassifier<bin_class, krnl> (5));
		  learned_func = mult.train<mult_func_type> (
		      preprocessor.train_samples, preprocessor.train_labels);
		}

	      if (parser.option ("E"))
		{
		  learned_func << filter_hash.str ();
		  learned_func.export_function (
		      parser.option ("E").argument ());
		}

	      //verbose mode prints confusion matrix
	      if (parser.option ("v"))
		{
		  print_score (learned_func.mult_decision_function,
			       preprocessor);
		}
	      //open a gateway to feed new measurements via telnet and classify them with the learned decision function
	      if (parser.option ("G"))
		{
		  typedef decltype(learned_func) gateway_func_type;
		  DataGateway<gateway_func_type> gateway (preprocessor,
							  learned_func);
		  gateway.set_listening_port (1234);
		  gateway.start_async ();
		  cout << "Press enter to end this program" << endl;
		  cin.get ();
		}

	      return 0;
	    }
	}
      else
	{
	  if (bin_arg == "n")
	    {
	      typedef lin_kernel krnl;
	      typedef svm_nu_trainer<krnl> bin_class;
	      //typedef svm_multiclass_linear_trainer<krnl,double> mult_trainer;
	      //typedef multiclass_linear_decision_function<krnl,double>  mult_func_type;
	      typedef ova_trainer mult_trainer;
	      typedef one_vs_all_decision_function<ova_trainer,
		  decision_function<krnl>> mult_func_type;
	      MultDecisionFunc<mult_func_type> learned_func;
	      learned_func << filter_hash.str ();
	      //handle decision function import
	      if (parser.option ("I"))
		{
		  if (!learned_func.import_function (
		      parser.option ("I").argument ()))
		    {
		      cout << "start ova svm-nu lin-kernel training" << endl;
		      //create multiclass classifier and decision function objects
		      MultClassifier<mult_trainer, krnl> mult;
		      mult.import_classifiers<bin_class, krnl> (
			  BinClassifier<bin_class, krnl> (4));
		      learned_func = mult.train<mult_func_type> (
			  preprocessor.train_samples,
			  preprocessor.train_labels);
		    }
		}
	      else
		{
		  cout << "start ova svm-nu lin-kernel training" << endl;
		  //create multiclass classifier and decision function objects
		  MultClassifier<mult_trainer, krnl> mult;
		  mult.import_classifiers<bin_class, krnl> (
		      BinClassifier<bin_class, krnl> (5));
		  learned_func = mult.train<mult_func_type> (
		      preprocessor.train_samples, preprocessor.train_labels);
		}

	      if (parser.option ("E"))
		{
		  learned_func << filter_hash.str ();
		  learned_func.export_function (
		      parser.option ("E").argument ());
		}

	      //verbose mode prints confusion matrix
	      if (parser.option ("v"))
		{
		  print_score (learned_func.mult_decision_function,
			       preprocessor);
		}
	      //open a gateway to feed new measurements via telnet and classify them with the learned decision function
	      if (parser.option ("G"))
		{
		  typedef decltype(learned_func) gateway_func_type;
		  DataGateway<gateway_func_type> gateway (preprocessor,
							  learned_func);
		  gateway.set_listening_port (1234);
		  gateway.start_async ();
		  cout << "Press enter to end this program" << endl;
		  cin.get ();
		}

	      return 0;
	    }
	  else if (bin_arg == "c")
	    {
	      typedef lin_kernel krnl;
	      typedef svm_c_trainer<krnl> bin_class;
	      //typedef svm_multiclass_linear_trainer<krnl,double> mult_trainer;
	      //typedef multiclass_linear_decision_function<krnl,double>  mult_func_type;
	      typedef ova_trainer mult_trainer;
	      typedef one_vs_all_decision_function<ova_trainer,
		  decision_function<krnl>> mult_func_type;
	      MultDecisionFunc<mult_func_type> learned_func;
	      learned_func << filter_hash.str ();
	      //handle decision function import
	      if (parser.option ("I"))
		{
		  if (!learned_func.import_function (
		      parser.option ("I").argument ()))
		    {
		      cout << "start ova svm-c lin-kernel training" << endl;
		      //create multiclass classifier and decision function objects
		      MultClassifier<mult_trainer, krnl> mult;
		      mult.import_classifiers<bin_class, krnl> (
			  BinClassifier<bin_class, krnl> (4));
		      learned_func = mult.train<mult_func_type> (
			  preprocessor.train_samples,
			  preprocessor.train_labels);
		    }
		}
	      else
		{
		  cout << "start ova svm-c lin-kernel training" << endl;
		  //create multiclass classifier and decision function objects
		  MultClassifier<mult_trainer, krnl> mult;
		  mult.import_classifiers<bin_class, krnl> (
		      BinClassifier<bin_class, krnl> (5));
		  learned_func = mult.train<mult_func_type> (
		      preprocessor.train_samples, preprocessor.train_labels);
		}

	      if (parser.option ("E"))
		{
		  learned_func << filter_hash.str ();
		  learned_func.export_function (
		      parser.option ("E").argument ());
		}

	      //verbose mode prints confusion matrix
	      if (parser.option ("v"))
		{
		  print_score (learned_func.mult_decision_function,
			       preprocessor);
		}
	      //open a gateway to feed new measurements via telnet and classify them with the learned decision function
	      if (parser.option ("G"))
		{
		  typedef decltype(learned_func) gateway_func_type;
		  DataGateway<gateway_func_type> gateway (preprocessor,
							  learned_func);
		  gateway.set_listening_port (1234);
		  gateway.start_async ();
		  cout << "Press enter to end this program" << endl;
		  cin.get ();
		}

	      return 0;
	    }
	  else if (bin_arg == "r")
	    {
	      typedef lin_kernel krnl;
	      typedef rvm_trainer<krnl> bin_class;
	      //typedef svm_multiclass_linear_trainer<krnl,double> mult_trainer;
	      //typedef multiclass_linear_decision_function<krnl,double>  mult_func_type;
	      typedef ova_trainer mult_trainer;
	      typedef one_vs_all_decision_function<ova_trainer,
		  decision_function<krnl>> mult_func_type;
	      MultDecisionFunc<mult_func_type> learned_func;
	      learned_func << filter_hash.str ();
	      //handle decision function import
	      if (parser.option ("I"))
		{
		  if (!learned_func.import_function (
		      parser.option ("I").argument ()))
		    {
		      cout << "start ova rvm lin-kernel training" << endl;
		      //create multiclass classifier and decision function objects
		      MultClassifier<mult_trainer, krnl> mult;
		      mult.import_classifiers<bin_class, krnl> (
			  BinClassifier<bin_class, krnl> (4));
		      learned_func = mult.train<mult_func_type> (
			  preprocessor.train_samples,
			  preprocessor.train_labels);
		    }
		}
	      else
		{
		  cout << "start ova rvm lin-kernel training" << endl;
		  //create multiclass classifier and decision function objects
		  MultClassifier<mult_trainer, krnl> mult;
		  mult.import_classifiers<bin_class, krnl> (
		      BinClassifier<bin_class, krnl> (5));
		  learned_func = mult.train<mult_func_type> (
		      preprocessor.train_samples, preprocessor.train_labels);
		}

	      if (parser.option ("E"))
		{
		  learned_func << filter_hash.str ();
		  learned_func.export_function (
		      parser.option ("E").argument ());
		}

	      //verbose mode prints confusion matrix
	      if (parser.option ("v"))
		{
		  print_score (learned_func.mult_decision_function,
			       preprocessor);
		}
	      //open a gateway to feed new measurements via telnet and classify them with the learned decision function
	      if (parser.option ("G"))
		{
		  typedef decltype(learned_func) gateway_func_type;
		  DataGateway<gateway_func_type> gateway (preprocessor,
							  learned_func);
		  gateway.set_listening_port (1234);
		  gateway.start_async ();
		  cout << "Press enter to end this program" << endl;
		  cin.get ();
		}

	      return 0;
	    }
	  else if (bin_arg == "k")
	    {
	      typedef lin_kernel krnl;
	      typedef krr_trainer<krnl> bin_class;
	      //typedef svm_multiclass_linear_trainer<krnl,double> mult_trainer;
	      //typedef multiclass_linear_decision_function<krnl,double>  mult_func_type;
	      typedef ova_trainer mult_trainer;
	      typedef one_vs_all_decision_function<ova_trainer,
		  decision_function<krnl>> mult_func_type;
	      MultDecisionFunc<mult_func_type> learned_func;
	      learned_func << filter_hash.str ();
	      //handle decision function import
	      if (parser.option ("I"))
		{
		  if (!learned_func.import_function (
		      parser.option ("I").argument ()))
		    {
		      cout << "start ova krr lin-kernel training" << endl;
		      //create multiclass classifier and decision function objects
		      MultClassifier<mult_trainer, krnl> mult;
		      mult.import_classifiers<bin_class, krnl> (
			  BinClassifier<bin_class, krnl> (4));
		      learned_func = mult.train<mult_func_type> (
			  preprocessor.train_samples,
			  preprocessor.train_labels);
		    }
		}
	      else
		{
		  cout << "start ova krr lin-kernel training" << endl;
		  //create multiclass classifier and decision function objects
		  MultClassifier<mult_trainer, krnl> mult;
		  mult.import_classifiers<bin_class, krnl> (
		      BinClassifier<bin_class, krnl> (5));
		  learned_func = mult.train<mult_func_type> (
		      preprocessor.train_samples, preprocessor.train_labels);
		}

	      if (parser.option ("E"))
		{
		  learned_func << filter_hash.str ();
		  learned_func.export_function (
		      parser.option ("E").argument ());
		}

	      //verbose mode prints confusion matrix
	      if (parser.option ("v"))
		{
		  print_score (learned_func.mult_decision_function,
			       preprocessor);
		}
	      //open a gateway to feed new measurements via telnet and classify them with the learned decision function
	      if (parser.option ("G"))
		{
		  typedef decltype(learned_func) gateway_func_type;
		  DataGateway<gateway_func_type> gateway (preprocessor,
							  learned_func);
		  gateway.set_listening_port (1234);
		  gateway.start_async ();
		  cout << "Press enter to end this program" << endl;
		  cin.get ();
		}

	      return 0;
	    }
	}
    }
  else
    {
      typedef lin_kernel krnl;
      typedef svm_multiclass_linear_trainer<krnl, double> mult_trainer;
      typedef multiclass_linear_decision_function<krnl, double> mult_func_type;
      MultDecisionFunc<mult_func_type> learned_func;
      learned_func << filter_hash.str ();
      //handle decision function import
      if (parser.option ("I"))
	{
	  if (!learned_func.import_function (parser.option ("I").argument ()))
	    {
	      cout << "start multiclass-lin-svm training" << endl;
	      //create multiclass classifier and decision function objects
	      MultClassifier<mult_trainer, krnl> mult;
	      learned_func = mult.train<mult_func_type> (
		  preprocessor.train_samples, preprocessor.train_labels);
	    }
	}
      else
	{
	  cout << "start multiclass-lin-svm training" << endl;
	  //create multiclass classifier and decision function objects
	  MultClassifier<mult_trainer, krnl> mult;
	  learned_func = mult.train<mult_func_type> (preprocessor.train_samples,
						     preprocessor.train_labels);
	}

      if (parser.option ("E"))
	{
	  learned_func << filter_hash.str ();
	  learned_func.export_function (parser.option ("E").argument ());
	}

      //verbose mode prints confusion matrix
      if (parser.option ("v"))
	{
	  print_score (learned_func.mult_decision_function, preprocessor);
	}
      //open a gateway to feed new measurements via telnet and classify them with the learned decision function
      if (parser.option ("G"))
	{
	  typedef decltype(learned_func) gateway_func_type;
	  DataGateway<gateway_func_type> gateway (preprocessor, learned_func);
	  gateway.set_listening_port (1234);
	  gateway.start_async ();
	  cout << "Press enter to end this program" << endl;
	  cin.get ();
	}
      return 0;
    }
  return 0;
}
