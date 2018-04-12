/*
 * data_gateway.h
 *
 *  Created on: Apr 4, 2018
 *      Author: kosc93
 */

#ifndef DATA_GATEWAY_H_
#define DATA_GATEWAY_H_
#include <dlib/server.h>
#include <iostream>
#include <vector>
#include <string>
#include "preprocessing.h"
#include "classifier.h"

using namespace dlib;
using namespace std;

template<class dec_func>
class DataGateway : public server_iostream{
public:
  DataGateway(Preprocessor preprocessor_,dec_func decision_function_):preprocessor(preprocessor_),decision_function(decision_function_){};
private:
  Preprocessor preprocessor;
  dec_func decision_function;
  void on_connect  (
          std::istream& in,
          std::ostream& out,
          const std::string& foreign_ip,
          const std::string& local_ip,
          unsigned short foreign_port,
          unsigned short local_port,
          uint64 connection_id
      ){
	  // The details of the connection are contained in the last few arguments to
	  // on_connect().  For more information, see the documentation for the
	  // server_iostream.  However, the main arguments of interest are the two streams.
	  // Here we also print the IP address of the remote machine.
	  //cout << "Got a connection from " << foreign_ip << endl;

	  // Loop until we hit the end of the stream.  This happens when the connection
	  // terminates.
	  while (in.good())
	  {
	      DataPoint dp;
	      double imag,real;
	      std::vector<double> features;
	      while (in>>real>>imag)
	      {
		 // now echo it back to them
		 //cout << std::complex<double>(real,imag)<<endl;
		 dp.features.push_back(real);
		 dp.features.push_back(imag);
	      }
	      //cout<<"received datapoint with "<<dp.features.size()<<endl;
		if(dp.features.size()==preprocessor.feature_size){
	      cout<<get_str_class(decision_function(preprocessor(dp)))<<endl;
	      }else{
		  cout<<"invalid read"<<endl;
	      }
	  }
	  //cout << "connection from " << foreign_ip <<" terminated"<< endl;
      };
  std::string get_str_class(double input){
    switch(int(input)){
      case 0:
	return "undefined";
      case 1:
	return ">a";
      case 2:
	return ">e";
      case 3:
	return ">i";
      case 4:
	return ">o";
      case 5:
	return ">u";
      case 6:
	return ">ä";
      case 7:
	return ">ö";
      case 8:
	return ">ü";
      case 9:
	return "<a";
      case 10:
	return "<i";
      case 11:
	return "<o";
      case 12:
	return "<u";
      case 13:
	return "<ä";
      case 14:
	return "<ö";
      case 15:
	return "<ü";
      case 16:
	return "b";
      case 17:
	return "d";
      case 18:
	return "g";
      case 19:
	return "l";
      case 20:
	return "r";
      case 21:
	return "f";
      case 22:
	return "s";
      case 23:
	return "sch";
      case 24:
	return "m";
      case 25:
	return "n";
      default:
	return "not found";
    }
  }
};




#endif /* INCLUDE_DATA_GATEWAY_H_ */
