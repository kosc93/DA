/*
 * data_extractor.cpp
 *
 *  Created on: Feb 12, 2018
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
  parser.add_option("F","Filename",1);
  parser.add_option("L","label to be applied to all measurements",1);
  parser.add_option("l","label to be applied from the label file",1);
  parser.add_option("M","file to appended/married (all dates need to bee unique)",1);
  parser.add_option("c","check iff all measurements are labeled");
  parser.add_option("o","create a label and data txt files");
  parser.add_option("a","output a gif animation with one measurement per frame");
  parser.add_option("b","indicates the beginning of the measurement data number that we want to process",1);
  parser.add_option("e","indicates the end of the measurement data number that we want to process",1);
  parser.add_option("p","plot a figure with gnuplot");
  parser.add_option("n","create a new, empty file");
  parser.add_option("h","Display this help message.");

  parser.parse(argc, argv);
  const char* one_time_opts[] = {"F","L","M","c","l","n", "b", "e", "h", "a","p","o"};
  parser.check_one_time_options(one_time_opts);

  //check for help option
  if (parser.option("h"))
  {
      parser.print_options();
      return 0;
  }

  std::string filename;
  if(parser.option("n")){
      filename ="new_data.h5";
      H5MeasurementFile new_file(filename);
      cout<<"new file new_data.h5 was created"<<endl;
  }
  //check if filename is valid
  else if(parser.option("F")){
      filename=parser.option("F").argument();
  }else{
      cout<<"Filename option ist mandatory else use -n flag"<<endl;
      return 0;
  }
  H5MeasurementFile file(filename);
  //take care of file concatination
  if(parser.option("M")){
	std::string append_filename;
	append_filename = parser.option("M").argument();
	file.append(append_filename);
  }

  //make sure that there is data to b processed
  std::vector<Measurement> data = file.scan();
  if(data.size()==0){
        cout<<"no measurements found!"<<endl;
        return -1;
  }

  //take care of labeling
  if(parser.option("L")){
      double label;
      label=get_option(parser,"L",0.0);
      file.label(label);
  }
  if(parser.option("l")){
        string label_filename = parser.option("l").argument();
        file.label(label_filename);
    }
  //check if all measurements are labeled
  if(parser.option("c")){
            for(const auto& measurement : data){
                if(measurement.label==0){
            	cout<<"not all measurements are labeled"<<endl;
            	return -1;
                }
            }
            cout<<"all measurements are labeled"<<endl;
     }

  //create datamatrix for plotting and printing
  int num_tr=0;
  double f_min=0,f_max=0;
  int x_length = 1+ 2*(data.size());//f column + (abs+ arg)+# measurement
  int tr_length =data[0].devices[0].channels[0].traces[0].num_points; //points per trace
  int y_length = data[0].devices[0].channels.size()*data[0].devices[0].channels[0].traces.size()*tr_length;//#channels+#traces_per channel*#points_per_trace
  double formatted_data[x_length][y_length];
  int meas_num =1;
  for(unsigned long i =0;i<data.size();i++){
      Measurement measurement = data[i];
      Device dev = measurement.devices[0];
      int tr_point_num =0;
      num_tr=0;
      for(auto& channel : dev.channels){
	  for(auto& trace : channel.traces){
	      f_min=stod(trace.start_freq,nullptr);
	      f_max=stod(trace.stop_freq,nullptr);
	      double delta_freq = (f_max-f_min)/tr_length;
	      for(long int j =0;j<tr_length;j++){
		  complex<double> c_num(trace.real[j],trace.imag[j]);
		  formatted_data[0][tr_point_num]=f_min+(tr_point_num%tr_length)*delta_freq;
		  formatted_data[meas_num][tr_point_num]=abs(c_num);
		  formatted_data[meas_num+1][tr_point_num]=arg(c_num);
		  //formatted_data[meas_num][tr_point_num]=trace.real[j];
		  //formatted_data[meas_num+1][tr_point_num]=trace.imag[j];
		  tr_point_num++;
	      }
	      num_tr++;
	  }

      }
      meas_num++;
      meas_num++;
  }


  // handle output for mlpack et al
  if(parser.option("o")){
       ofstream label("label.txt",ofstream::out);
       for(const auto& measurement : data){
 	  label<<measurement.label<<" "<<endl;
       }
       label.close();
       ofstream mlpack ("mlpack.txt",ofstream::out);
         mlpack<<std::setprecision(10);
         for(int x =1;x<x_length;x=x+2){
             for(int y=0;y<y_length;y++){
       	  mlpack<<formatted_data[x][y]<<" \t"<<formatted_data[x+1][y]<<" \t";
             }
             mlpack<<endl;
         }
         mlpack.close();
   }

  //handle plotting flags
  if(parser.option("a")||parser.option("p")){
    //handle plotting selection
    unsigned long begin=get_option(parser,"b",0);
    unsigned long end=get_option(parser,"e",1000);//no more linestyles with default gnuplot
    if(end-begin>31&&parser.option("p")){
	cout<<"more than 31 Datasets will result in duplicating linestyles"<<endl;
    }
    if(end>data.size()){
	end=data.size();
    }
    //create data file for plotting
    ofstream out ("out.dat",ofstream::out);
    out<<std::setprecision(10);
    out<<"# f[Hz] ";
    meas_num =0;
    for(int i=1;i<x_length;i++){
	if(i%2==0){
      out<< "\t\t\tM" <<meas_num<< "abs" << " ";
	  meas_num++;
	}else{
      out<< "\t\t\tM" <<meas_num<< "arg" << " ";
	}
    }
    out<<endl;
    for(int y = 0; y<y_length;y++){
	if(y%tr_length==0&&y!=0){
    out<<endl;
    out<<endl;
	}
	for(int x=0;x<x_length;x++){
      out<<formatted_data[x][y]<<" \t";

	}
	out<<endl;
    }
    out.close();
    //create gnuplot file for animation
    ofstream plot ("plot.dat",ofstream::out);
    if(parser.option("a")){

      plot<<std::setprecision(1);
      plot<<"set term gif animate size 1920,900 delay 50"<<endl;
      plot<<"set output 'out.gif'"<<endl;
      plot<<"set tmargin 3"<<endl;
      plot<<"set lmargin 10"<<endl;
      plot<<"set rmargin 1"<<endl;
      plot<<"set xrange ["<<f_min<<":"<<f_max<<"]"<<endl;
      plot<<"set xtics "<<f_min<<","<<(f_max-f_min)/4<<","<<f_max<<endl;
      plot<<"do for [ii="<<begin+1<<":"<<end<<"]{"<<endl;
      plot<<"set multiplot layout 2, "<<num_tr<<" columnsfirst title 'Measurement nr '.ii"<<endl;
      for(int i=0;i<num_tr;i++){
	    plot<<"set title 'Arg Trace"<<i+1<<"'"<<endl<<"unset key"<<endl;
	    plot<<"plot 'out.dat' i "<<i<<" u 1:2*ii w lines lt 3 "<<endl;

	    plot<<"set title 'Phase Trace"<<i+1<<"'"<<endl;
	    plot<<"plot 'out.dat' i "<<i<<" u 1:2*ii+1 w lines lt 3 "<<endl;

	}
      plot<<"unset multiplot"<<endl;
      plot<<"}"<<endl;
      return system("gnuplot plot.dat");
    }
    //create gnuplot file for plotting
    if(parser.option("p")){
      plot<<std::setprecision(1);
      plot<<"set term qt size 1920,"<<900+(end-begin)*20<<endl;
      plot<<"set tmargin 3"<<endl<<"set lmargin 10"<<endl<<"set rmargin 1"<<endl;
      plot<<"set xrange ["<<f_min<<":"<<f_max<<"]"<<endl;
      plot<<"set xtics "<<f_min<<","<<(f_max-f_min)/4<<","<<f_max<<endl;
      plot<<"set multiplot layout 2, "<<num_tr<<" columnsfirst title 'test trace plot'"<<endl;
      for(int i=0;i<num_tr;i++){
	  plot<<"set title 'ABS Trace"<<i+1<<"'"<<endl<<"unset key"<<endl;
	  plot<<"plot ";
	  for(unsigned long selector=begin;selector<end;selector++){
	      plot<<"'out.dat' i "<<i<<"u 1:"<<2*selector+2<<" t 'T_{"<<i+1<<"} M_{"<<selector<<"}' w linespoints lt "<<selector%31+1<<" , ";
	  }
	  plot<<endl;
	  plot<<"set title 'ARG Trace"<<i+1<<"'"<<endl<<"set key out bot center horizontal box"<<endl;
	  plot<<"plot ";
	  for(unsigned long selector=begin;selector<end;selector++){
	      plot<<"'out.dat' i "<<i<<"u 1:"<<2*selector+3<<" t 'T_{"<<i+1<<"} M_{"<<selector<<"}' w linespoints lt "<<selector%31+1<<",";
	  }
	  plot<<endl;
      }

      plot<<"unset multiplot"<<endl;
      plot.close();
      return system("gnuplot plot.dat -persist");
    }
  }


  return 0;
}


