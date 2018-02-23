#include "measurement_data.h"

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
  //std::cout<<getObjectName(parent,i)<<std::endl;
  return parent.openGroup(getObjectName(parent,i));
}



template<class T>
Group assertGroup(T& parent, std::string group_name){
  Group result;
  try{
      result = parent.openGroup(group_name);
  }catch(...){
      result = parent.createGroup(group_name,group_name.size());
  }
  return result;
}

template<class T>
Attribute assertAttribute(T& parent, std::string attr_name, PredType type){
  Attribute result;
  hsize_t one[1] ={1};
  	if(parent.attrExists(attr_name)){
  	  result = parent.openAttribute(attr_name);
  	}else{
  	  result = parent.createAttribute(attr_name,type,DataSpace(1,one));
  	}
  return result;
}

template<class T>
bool groupExists(T& parent, std::string group_name){
  try{
        parent.openGroup(group_name);
        return true;
    }catch(...){
        return false;
    }
}

template<class T>
DataSet datasetOverride(T& parent, std::string dataset_name,std::vector<double> data_){
  DataSet result;
  try{
        DataSet ds= parent.openDataSet(dataset_name);
        ds.close();
        parent.unlink(dataset_name);
    }catch(...){

    }
    hsize_t dim_dp[1]={data_.size()};
    return parent.createDataSet("features",PredType::NATIVE_DOUBLE,DataSpace(1,dim_dp));

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
		     std::string date=getAttr(messung,"time");
		     double label =0;
		     if(messung.attrExists("label")){
			 Attribute label_attr = messung.openAttribute("label");
			 label_attr.read(PredType::NATIVE_DOUBLE,&label);
		     }
		     Measurement measurement(date);
		     measurement.label=label;
		     size_t num_geraete= getNumObj(messung);
		     for(unsigned int j =0;j<num_geraete;j++){
			 Device device;
			 Group geraet=getChildGroup(messung,j);
			 if(geraet.getNumAttrs()==0){
			     continue;
			 }
			 //device.manufacturer = getAttr(geraet,"manufacturer");
			 //device.model = getAttr(geraet,"model");
			 //device.serial = getAttr(geraet,"serial");
			 size_t num_channels=getNumObj(geraet);
			 for(unsigned int k =0;k<num_channels;k++){
			     Channel channel_;
			     Group channel = getChildGroup(geraet,k);
			     channel_.name=getObjectName(geraet,k);
			     size_t num_traces = getNumObj(channel);
			     for(unsigned int l =0;l<num_traces;l++){
				 Trace trace_;

				 Group trace = getChildGroup(channel,l);
				 Group metaData = trace.openGroup("metaData");
				 DataSet t_name = metaData.openDataSet("trace");
				 StrType t_name_type = t_name.getStrType();
				 t_name.read(trace_.name,t_name_type);
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
	 cout<<"scanning error:"<<endl;
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

void
H5MeasurementFile::export_data (std::vector<Measurement>& data_)
{
  Exception::dontPrint();
  H5File file;
  try {
	file = H5File(filename, H5F_ACC_CREAT | H5F_ACC_RDWR);

	Attribute marker = assertAttribute(file,"export_DA",PredType::NATIVE_INT);
	marker.write(PredType::NATIVE_INT,"0");//arbitrary data, only for checking if exported file

  	Group data = assertGroup(file,"data");
  	string name = data.getObjName();
  	for(auto& measurement:data_){
  	    Group data_point = assertGroup(data,measurement.date);
	    Attribute marker = assertAttribute(data_point,"label",PredType::NATIVE_DOUBLE);
	    marker.write(PredType::NATIVE_DOUBLE,&measurement.label);
	    DataPoint dp = measurement.getDatapoint(0);
	    DataSet features = datasetOverride(data_point,"features",dp.features);
	    features.write(&dp.features[0],PredType::NATIVE_DOUBLE);

  	}

  	file.close();

      } catch (Exception e) {
	  file.close();
	  e.printError();
      }
}

void
H5MeasurementFile::export_data (std::vector<Measurement>& data_,
				ClassifierParam& param_)
{
  Exception::dontPrint();
  export_data(data_);
  H5File file;
    try {
  	file = H5File(filename, H5F_ACC_CREAT | H5F_ACC_RDWR);
    	Group all_classifier = assertGroup(file,"classifier");
    	std::string class_name =param_.getHash();
    	if(groupExists(file,class_name)){
    	    throw GroupIException("export_data","classifier already exists");
    	}
    	Group classifier = assertGroup(all_classifier,class_name);
	Attribute class_type = assertAttribute(classifier,"type",PredType::NATIVE_INT);
	int attr_type=static_cast<int>(param_.type);
	class_type.write(PredType::NATIVE_INT,&attr_type);
	Attribute normalize = assertAttribute(classifier,"normalize",PredType::NATIVE_HBOOL);
	normalize.write(PredType::NATIVE_HBOOL,&param_.normalize);
	Attribute optimize = assertAttribute(classifier,"optimize",PredType::NATIVE_HBOOL);
	optimize.write(PredType::NATIVE_HBOOL,&param_.optimize);
	Attribute cc_manifold = assertAttribute(classifier,"cc_manifold",PredType::NATIVE_INT);
	cc_manifold.write(PredType::NATIVE_INT,&param_.cc_manifold);

	DataSet params = datasetOverride(classifier,"params",param_.params);
	params.write(&param_.params[0],PredType::NATIVE_DOUBLE);

    	file.close();

        } catch (Exception e) {
  	  file.close();
  	  e.printError();
        }
}

void
H5MeasurementFile::label (double& label)
{
  try {
  	 Exception::dontPrint();
	  double l = label;
  	 H5File file = H5File(filename, H5F_ACC_RDWR);
  	 size_t num_messungen = getNumObj(file);
  	 	for(unsigned int i = 0;i<num_messungen;i++){
  		     Group messung=getChildGroup(file,i);
  		     Attribute label = assertAttribute(messung,"label",PredType::NATIVE_DOUBLE);
  		     label.write(PredType::NATIVE_DOUBLE,&l);
  	 	}
  	 	file.close();

       } catch (Exception e) {
	   cout<<"label error:"<<endl;
  	 e.printError();
       }
}

void
H5MeasurementFile::label (string label_filename)
{
  try {
    	 Exception::dontPrint();
    	 std::vector<double> labels;
    	 std::ifstream label_stream(label_filename,std::ifstream::in);
    	 if(!label_stream.good()){
    	     cout<<"ca not find file!"<<endl;
    	     throw;
    	 }
    	 double single_measurement=0.0;
    	 while(label_stream>>single_measurement){
    	     labels.push_back(single_measurement);
    	 }
    	 H5File file = H5File(filename, H5F_ACC_RDWR);
    	 size_t num_messungen = getNumObj(file);
    	 if(num_messungen>labels.size()){
    	     cout<<"number of labels in file does not match number of measurements"<<endl;
    	     throw;
    	 }
    	 	for(unsigned int i = 0;i<num_messungen;i++){
    		     Group messung=getChildGroup(file,i);
    		     Attribute label = assertAttribute(messung,"label",PredType::NATIVE_DOUBLE);
    		     label.write(PredType::NATIVE_DOUBLE,&labels[i]);
    	 	}
    	 	file.close();

         } catch (Exception e) {
  	   cout<<"label error:"<<endl;
    	 e.printError();
         }
}

void
H5MeasurementFile::append (string append_filename)
{
  try {
    	 Exception::dontPrint();
  	 H5MeasurementFile file(filename);
  	 H5MeasurementFile append_file(append_filename);
  	 //scan files, compare group attr and write groups
  	 std::vector<Measurement> append_data = append_file.scan();
  	 std::vector<Measurement> data = file.scan();
  	 long start_index=data.size();
  	 for(unsigned int measurement_count=0;measurement_count<append_data.size();measurement_count++){
  	     bool is_new=true;
  	     for(auto& measurement:data){
  		 if(append_data[measurement_count].date==measurement.date){
  		     cout<<"measurement with date "<<measurement.date<<" already exists! Single measurement not copied!"<<endl;
  		     is_new=false;
  		     break;
  		 }
  	     }
  	     if(is_new){
  		 ostringstream command;
  		 command<<"h5copy -i '"<<append_filename<<"' -s '/"<<measurement_count;
  		 command<<"' -o '"<<filename<<"' -d '/"<<start_index<<"'";
  		 start_index++;
  		 if(system(command.str().c_str())){
  		   cout<<"something went wrong with the copying"<<endl;
  		 }else{
  		     cout<<"sucessfully copied"<<endl;
  		 }

  	     }

  	 }
         } catch (Exception e) {
             cout<<"appending error:"<<endl;
    	 e.printError();
         }
}


