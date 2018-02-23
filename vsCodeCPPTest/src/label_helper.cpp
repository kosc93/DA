/*
 * label_helper.cpp
 *
 *  Created on: Feb 18, 2018
 *      Author: kosc93
 */


#include "dlib/cmd_line_parser.h"
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <complex>
#include "measurement_data.h"


using namespace std;

int main(int argc, char** argv){
  dlib::command_line_parser parser;
  parser.add_option("L","label to be applied",1);
  parser.add_option("F","file to be labeled",1);
  parser.add_option("m","file to appended",1);
  parser.add_option("h","Display this help message.");

  parser.parse(argc, argv);
  const char* one_time_opts[] = {"F", "L", "h"};
  parser.check_one_time_options(one_time_opts);
  if (parser.option("h"))
  {
      parser.print_options();
      return 0;
  }
  std::string filename;
  if(parser.option("F")){
      filename=parser.option("F").argument();
  }else{
      cout<<"Filename option ist mandatory"<<endl;
      return 0;
  }
  double label;
  if(parser.option("L")){
        label=get_option(parser,"L",0.0);
    }else{
        cout<<"Label option ist mandatory"<<endl;
        return 0;
    }
  H5MeasurementFile file(filename);
  std::vector<Measurement> data = file.scan();

  if(data.size()==0){
      cout<<"no measurements found!"<<endl;
      return -1;
  }
  file.label(label);
  std::string append_filename;
  if(parser.option("m")){
      append_filename = parser.option("m").argument();
      file.append(append_filename);
  }

  return 0;
}
