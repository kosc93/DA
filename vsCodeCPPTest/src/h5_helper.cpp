/*
 * data_extractor.cpp
 *
 *  Created on: Feb 12, 2018
 *      Author: kosc93
 */
#include "dlib/cmd_line_parser.h"
#include "dlib/statistics.h"
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <complex>
#include <map>
#include "measurement_data.h"

using namespace std;

int main(int argc, char** argv){
  dlib::command_line_parser parser;
  parser.add_option("F","Filename",1);
  parser.add_option("L","label to be applied to all measurements",1);
  parser.add_option("l","label to be applied from the label file",1);
  parser.add_option("M","file to appended/married (all dates need to bee unique)",1);
  parser.add_option("f","fixes plot size, so that it may print above display resolution");
  parser.add_option("c","check iff all measurements are labeled");
  parser.add_option("o","create a label and mlpack formatted files");
  parser.add_option("a","output a gif animation with one measurement per frame");
  parser.add_option("b","indicates the beginning of the measurement data number that we want to process",1);
  parser.add_option("e","indicates the end of the measurement data number that we want to process",1);
  parser.add_option("p","plot a figure with gnuplot");
  parser.add_option("r","plot a errorbars with gnuplot");
  parser.add_option("d","plot a figure with gnuplot with magnitude in dB");
  parser.add_option("n","create a new, empty file");
  parser.add_option("t","create a touchstone file",1);
  parser.add_option("u","use formatted data(without any transformation)");
  parser.add_option("reps","flag to use repeated measurements");
  parser.add_option("normalize","flag to normalize errorplot");
  parser.add_option("h","Display this help message.");


  parser.parse(argc, argv);
  const char* one_time_opts[] = {"F","L","M","c","l","n", "b", "e", "h", "a","p","o","d","r","u"};
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
      return 0;
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
	return 0;
  }

  //make sure that there is data to b processed
  static std::vector<Measurement> data = file.scan(parser.option("reps"),parser.option("u"));
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
  bool dB= false;
  if(parser.option("d")){
      dB=true;
  }
  int num_tr=0;
  double f_min=0,f_max=0;
  int x_length = 1+ 2*(data.size());//f column + (abs+ arg)+# measurement
  int tr_length =data[0].devices[0].channels[0].traces[0].num_points; //points per trace
  int y_length = data[0].devices[0].channels.size()*data[0].devices[0].channels[0].traces.size()*tr_length;//#channels+#traces_per channel*#points_per_trace
  double formatted_data[x_length][y_length];
  int meas_num =1;
  for(auto& measurement:data){
      Device dev = measurement.devices[0];
      int tr_point_num =0;
      num_tr=0;
      for(auto& channel : dev.channels){
	  for(auto& trace : channel.traces){
	      if(parser.option("normalize")){
		  trace.normalize();
	      }
	      f_min=stod(trace.start_freq,nullptr);
	      f_max=stod(trace.stop_freq,nullptr);
	      double delta_freq = (f_max-f_min)/tr_length;
	      for(long int j =0;j<tr_length;j++){
		  formatted_data[0][tr_point_num]=f_min+(tr_point_num%tr_length)*delta_freq;
		  if(dB){
		      formatted_data[meas_num][tr_point_num]=20*std::log10(trace.mag[j]);
		  }else{
		      formatted_data[meas_num][tr_point_num]=abs(trace.mag[j]);
		  }
		  formatted_data[meas_num+1][tr_point_num]=trace.phase[j];
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
 	  label<<measurement.h5_name<<" "<<measurement.label<<" "<<endl;
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
  //handle touchstone creation
  if(parser.option("t")){
      Channel c =data[0].devices[0].channels[0];
      if(c.traces.size()!=6){
	  cout<<"only data with Saa Sbb Scc Sab Sac Sbc format is valid"<<endl;
	  return -1;
      }
      unsigned long measurement_index = get_option(parser,"t",0);
      if(measurement_index>=data.size()){
	  measurement_index=data.size()-1;
      }
      double delta_freq = (f_max-f_min)/tr_length;
      ofstream ts_stream(std::to_string(measurement_index)+".s3p");
      ts_stream<<"# HZ S RI R 50.0"<<endl;
      //f t0 t3 t4
      //\t\t\t t3 t1 t5
      //\t\t\t t4 t5 t2
      for(int i =0;i<tr_length;i++){
	  ts_stream<<f_min+i*delta_freq<<" "<<c.traces[0].real[i]<<" "<<c.traces[0].imag[i]<<" "<<c.traces[3].real[i]<<" "<<c.traces[3].imag[i]<<" "<<c.traces[4].real[i]<<" "<<c.traces[4].imag[i]<<endl;
	  ts_stream<<"\t\t\t"               <<c.traces[3].real[i]<<" "<<c.traces[3].imag[i]<<" "<<c.traces[1].real[i]<<" "<<c.traces[1].imag[i]<<" "<<c.traces[5].real[i]<<" "<<c.traces[5].imag[i]<<endl;
	  ts_stream<<"\t\t\t"               <<c.traces[4].real[i]<<" "<<c.traces[4].imag[i]<<" "<<c.traces[5].real[i]<<" "<<c.traces[5].imag[i]<<" "<<c.traces[2].real[i]<<" "<<c.traces[2].imag[i]<<endl;
	  ts_stream<<endl;
      }
      ts_stream.close();
  }

  //handle plotting flags
  if(parser.option("a")||parser.option("p")||parser.option("r")){
    //handle plotting selection
    unsigned long begin=get_option(parser,"b",0);
    unsigned long end=get_option(parser,"e",1000);//no more linestyles with default gnuplot
    if(end>=data.size()){
	end=data.size();
    }
    if(end-begin>31&&parser.option("p")){
    	cout<<"more than 31 Datasets will result in duplicating linestyles"<<endl;
    }
    //create data file for plotting
    ofstream out ("out.dat",ofstream::out);
    out<<std::setprecision(10);
    out<<"# f[Hz] ";
    meas_num =0;
    for(int i=1;i<x_length;i++){
	if(i%2==0){
	    out<< "\t\t\tM" <<meas_num<< "abs";
	    if(dB){
		out<<"(dB)";
	    }
	    out<< " ";
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
	    plot<<"set title 'ABS";
	    if(dB){
		plot<<"(dB)";
	    }
	    plot<<"Trace"<<i+1<<"'"<<endl<<"unset key"<<endl;
	    plot<<"plot 'out.dat' i "<<i<<" u 1:2*ii w lines lt 3 "<<endl;

	    plot<<"set title 'ARG Trace"<<i+1<<"'"<<endl;
	    plot<<"plot 'out.dat' i "<<i<<" u 1:2*ii+1 w lines lt 3 "<<endl;

	}
      plot<<"unset multiplot"<<endl;
      plot<<"}"<<endl;
      return system("gnuplot plot.dat");
    }
    //create gnuplot file for plotting
    if(parser.option("p")){
      plot<<std::setprecision(3);
      if(parser.option("f")){
	  plot<<"set term qt size "<< 900*num_tr<<","<<900+(end-begin)*20<<endl;
      }else{
	  plot<<"set term qt size 1920,"<<900+(end-begin)*20<<endl;
      }
      plot<<"set tmargin 3"<<endl<<"set lmargin 10"<<endl<<"set rmargin 1"<<endl;
      plot<<"set xrange ["<<f_min<<":"<<f_max<<"]"<<endl;
      plot<<"set xtics "<<f_min<<","<<(f_max-f_min)/4<<","<<f_max<<endl;
      plot<<"set multiplot layout 2, "<<num_tr<<" columnsfirst title 'test trace plot'"<<endl;
      for(int i=0;i<num_tr;i++){
	  plot<<"set title 'ABS";
	  if(dB){
	      plot<<"(dB)";
	  }
	  plot<<"Trace"<<i+1<<"'"<<endl<<"unset key"<<endl;
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

    if(parser.option("r")){
	struct error_info{
	  std::vector<double> mean;
	  std::vector<double> variance;
	  string name;
	  int size=0;
	};//error info for a single class
	// each of the vectors points represent a feature
	std::map<std::string,std::vector<Measurement>> data_map;//maps classes to measurements
	for(auto& measurement:data){
	    string label = std::to_string(measurement.label);
	    if(data_map.find(label)==data_map.end()){
		data_map[label]=std::vector<Measurement>();
	    }
	    data_map[label].push_back(measurement);
	}
	std::vector<error_info> infos;//error_info for all classes
	for(auto& class_:data_map){
	    error_info e_info;
	    e_info.name=class_.first;
	    std::vector<DataPoint> datapoints;//datapoints of the class
	    for(auto& measurement:class_.second){
		datapoints.push_back(measurement.getDatapoint(0,true));
	    }
	    int feature_size=datapoints[0].features.size();
	    e_info.size=feature_size;
	    for(int i=0;i<feature_size;i++){
		dlib::running_stats<double> rs;
		for(const auto& datapoint:datapoints){
		    rs.add(datapoint.features[i]);
		}
		e_info.mean.push_back(rs.mean());
		if(dB){
		    e_info.variance.push_back(rs.variance());
		    //e_info.variance.push_back(-0.01*std::log10(rs.variance()));
		}else{
		    e_info.variance.push_back(rs.variance());
		}
	    }
	    infos.push_back(e_info);
	}

	//print info data to file
	ofstream error_stream("error_dat.dat",std::ofstream::out);
	error_stream<<std::setprecision(8);

	plot<<"set term qt size 1920,1080"<<endl;
	plot<<"set xtics "<<0<<","<<tr_length<<","<<num_tr*tr_length<<endl;
	plot<<"set grid xtics"<<endl;
	if(dB){
	    plot<<"set logscale y"<<endl;
	}
	plot<<"plot ";
	int class_counter=0;
	for(const auto& class_:infos){
	    for(int i=0;i<class_.size;i++){
		error_stream<<i<<" \t\t"<<class_.mean[i]<<" \t\t"<<class_.variance[i]<< endl;
	    }
	    error_stream<<endl;
	    error_stream<<endl;

	    //plotting
	    plot<<"'error_dat.dat' i "<<class_counter<<" u 1:2:3 w yerrorbars t 'var class "<<class_.name<<"' lt "<<class_counter+1<<", '' i "<<class_counter<<" u 1:2 w lines t 'mean class "<<class_.name<<"' lt "<<class_counter+1<<" dt 3 ,";
	    class_counter++;
	}
	plot<<endl;
	error_stream.close();
	plot.close();

	return system("gnuplot plot.dat -persist");

    }
  }
  return 0;
}


