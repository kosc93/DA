/*
 *  Copyright (c), 2017, Adrien Devresse
 *
 *  Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#include <iostream>
#include <string>
#include <vector>

#include <highfive/H5Attribute.hpp>
#include <highfive/H5File.hpp>
#include <highfive/H5DataSet.hpp>
#include <highfive/H5DataSpace.hpp>

using namespace HighFive;

const std::string FILE_NAME("test.h5");
const std::string DATASET_NAME("my_dataset");

const std::string ATTRIBUTE_NAME_NOTE("note");
const std::string ATTRIBUTE_NAME_VERSION("version_string");

// create a dataset from a vector of string
// read it back and print it
template<class T>
size_t getNumObj(T parent){
  return parent.getNumberObjects();
}
template<class T>
Group getChildGroup(T parent, int i){
  std::cout<<parent.getObjectName(i)<<std::endl;
  return parent.getGroup(parent.getObjectName(i));
}

int main(void) {

    try {
        // Create a new file using the default property lists.
        File file(FILE_NAME, File::ReadWrite);

        // Create a dummy dataset of one single integer
//        DataSet dataset =
//            file.createDataSet(DATASET_NAME, DataSpace(1), AtomicType<int>());

	size_t num_messungen = getNumObj(file);
	for(unsigned int i = 0;i<num_messungen;i++){
	     Group messung=getChildGroup(file,i);
	     size_t num_geraete= getNumObj(messung);
		 for(unsigned int j =0;j<num_geraete;j++){
		     Group geraet=getChildGroup(messung,j);
		     messung.getObjectName(0);
		     size_t num_channels=getNumObj(geraet);
		     for(unsigned int k =0;k<num_channels;k++){
			 Group channel = getChildGroup(geraet,k);
			 size_t num_traces = getNumObj(channel);
			 for(unsigned int l =0;l<num_traces;l++){
			     Group trace = getChildGroup(channel,l);
			     DataSet s_param=trace.getDataSet(std::string("unformatted_data"));
			     DataType s_type = s_param.getDataType();
			     DataSpace s_space= s_param.getSpace();

			     std::vector<unsigned long> s_dim = s_space.getDimensions();
			     std::vector<double> s_data;
			     s_param.read(s_data);
			 }

		     }

		 }

	}

        std::vector<std::string> objectNames=file.listObjectNames();

//        // Now let's add a attribute on this dataset
//        // This attribute will be named "note"
//        // and have the following content
        std::string string_list("very important Dataset !");
//
//        Attribute a = dataset.createAttribute<std::string>(
//            ATTRIBUTE_NAME_NOTE, DataSpace::From(string_list));
//        a.write(string_list);
//
//        // We also add a "version" attribute
//        // that will be an array 1x2 of integer
//        std::vector<int> version;
//        version.push_back(1);
//        version.push_back(0); // version 1.0
//
//        Attribute v = dataset.createAttribute<int>(ATTRIBUTE_NAME_VERSION,
//                                                   DataSpace::From(version));
//        v.write(version);
//
//        // Ok all attributes are now written
//
//        // lets list the keys of all attributes now
//        std::vector<std::string> all_attributes_keys =
//            dataset.listAttributeNames();
//        for (std::vector<std::string>::const_iterator it =
//                 all_attributes_keys.begin();
//             it < all_attributes_keys.end(); ++it) {
//            std::cout << "attribute: " << *it << std::endl;
//        }

    } catch (Exception& err) {
        // catch and print any HDF5 error
        std::cerr << err.what() << std::endl;
    }

    return 0; // successfully terminated
}
