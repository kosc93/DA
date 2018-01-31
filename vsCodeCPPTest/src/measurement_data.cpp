#include "measurement_data.h"
#include <iostream>

template <class T>
std::string getObjectName(const T& parent,size_t index){
    const ssize_t maxLength = 1023;
    char buffer[maxLength + 1];
    ssize_t length =
        H5Lget_name_by_idx(parent.getId(), ".",
                           H5_INDEX_NAME, H5_ITER_INC, index,
                           buffer, maxLength, H5P_DEFAULT);
    if (length < 0)
        return "";
    if (length <= maxLength)
        return std::string(buffer, length);
    std::vector<char> bigBuffer(length + 1, 0);
    H5Lget_name_by_idx(parent.getId(), ".",
                       H5_INDEX_NAME, H5_ITER_INC, index,
                       bigBuffer.data(), length, H5P_DEFAULT);
    return std::string(bigBuffer.data(), length);
}

template<class T>
Group getChildGroup(T& parent, int i){
  std::cout<<getObjectName(parent,i)<<std::endl;
  return parent.openGroup(getObjectName(parent,i));
}

template<class T>
size_t getNumObj(T& parent){
  return parent.getNumObjs();
}
template<class T>
std::string getAttr(T parent, std::string attr_name){
  std::string result;
  Attribute att = parent.openAttribute(attr_name);
  DataType type = att.getDataType();
  att.read(type,result);
  return result;
}


std::vector<Measurement> H5MeasurementFile::scan ()
{
  const H5std_string REAL( "r" );
  const H5std_string IMAG( "i" );
  struct MeasurementDataCompType {
      double r;
      double i;
     };
     std::vector<Measurement> result;
     try {
	 Exception::dontPrint();
	 CompType data_type(sizeof(MeasurementDataCompType));
	 data_type.insertMember(REAL,HOFFSET(MeasurementDataCompType,r), PredType::NATIVE_DOUBLE);
	 data_type.insertMember(IMAG,HOFFSET(MeasurementDataCompType,i), PredType::NATIVE_DOUBLE);
	 H5File file = H5File(filename, H5F_ACC_RDONLY);
	 size_t num_messungen = getNumObj(file);
	 	for(unsigned int i = 0;i<num_messungen;i++){
		     Group messung=getChildGroup(file,i);
		     Measurement measurement(getAttr(messung,"time"));
		     size_t num_geraete= getNumObj(messung);
		     for(unsigned int j =0;j<num_geraete;j++){
			 Device device;
			 Group geraet=getChildGroup(messung,j);
			 device.manufacturer = getAttr(geraet,"manufacturer");
			 device.model = getAttr(geraet,"model");
			 device.serial = getAttr(geraet,"serial");
			 size_t num_channels=getNumObj(geraet);
			 for(unsigned int k =0;k<num_channels;k++){
			     Channel channel_;
			     channel_.name=k;
			     Group channel = getChildGroup(geraet,k);
			     size_t num_traces = getNumObj(channel);
			     for(unsigned int l =0;l<num_traces;l++){
				 Trace trace_;
				 trace_.name=l;
				 Group trace = getChildGroup(channel,l);
				 Group metaData = trace.openGroup("metaData");
				 DataSet f_start = metaData.openDataSet("start_freq");
				 StrType f_start_type = f_start.getStrType();
				 f_start.read(trace_.start_freq,f_start_type);
				 DataSet f_stop = metaData.openDataSet("stop_freq");
				 StrType f_stop_type = f_stop.getStrType();
				 f_stop.read(trace_.stop_freq,f_stop_type);
				 //trace_.start_freq = getAttr(metaData,"start_freq");
				 //trace_.stop_freq = getAttr(metaData,"stop_freq");
				 DataSet s_param=trace.openDataSet(std::string("unformatted_data"));
				 const unsigned int num_elements = s_param.getStorageSize()/sizeof(data_type);
				 MeasurementDataCompType data[num_elements];
				 trace_.num_points=num_elements;
				 s_param.read(data,data_type);
				 for(unsigned int n =0; n<num_elements;n++){
				     trace_.real.push_back(data[n].r);
				     trace_.imag.push_back(data[n].i);
				 }
				 channel_.traces.push_back(trace_);
			     }
			     device.channels.push_back(channel_);
			 }
			 measurement.devices.push_back(device);
		     }
		     result.push_back(measurement);

	 	}
	 	file.close();

     } catch (Exception e) {
	 e.printError();
     }
     return result;
}

DataPoint
Measurement::getDatapoint(unsigned int device_index)
{
  if(label==0){
      throw std::invalid_argument("measurement not labeled");
  }
  if(device_index>=devices.size()){
      throw std::invalid_argument("invalid device index");
  }
  Device device = devices[device_index];
  DataPoint dp;
  dp.calculated_class=label;
  for(const auto& channel: device.channels){
      for(const auto& trace : channel.traces){
	  for(unsigned int i=0;i<trace.num_points;i++){
	      dp.features.push_back(trace.real[i]);
	      dp.features.push_back(trace.imag[i]);
	  }
      }
  }
  return dp;
}

//
//void
//Measurement::serialize ()
//{
//  try {
//      std::string filename = date+".dat";
//      dlib::serialize(filename)<<date<<devices;
//  } catch (Exception e) {
//      e.printError();
//  }
//}
//
//void
//Measurement::deserialize ()
//{
//  try {
//      std::string filename = date+".dat";
//      std::string date_;
//      std::vector<Device> devices_;
//      dlib::deserialize(filename)>>date_>>devices;
//      date=date_;
//      devices=devices_;
//  } catch (Exception e) {
//      e.printError();
//  }
//
//}
/*
 * measurement_data.cpp
 *
 *  Created on: Jan 29, 2018
 *      Author: kosc93
 */




