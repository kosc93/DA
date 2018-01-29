/*
 * test_hdf5.cpp
 *
 *  Created on: Jan 26, 2018
 *      Author: kosc93
 */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the root of the source code       *
 * distribution tree, or in https://support.hdfgroup.org/ftp/HDF5/releases.  *
 * If you do not have access to either file, you may request a copy from     *
 * help@hdfgroup.org.                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
 * This example shows how to create a compound datatype,
 * write an array which has the compound datatype to the file,
 * and read back fields' subsets.
 */
#ifdef OLD_HEADER_FILENAME
#include <iostream.h>
#else
#include <iostream>
#include <vector>
#endif
using std::cout;
using std::endl;
#include <string>
#include "H5Cpp.h"
using namespace H5;
const H5std_string FILE_NAME( "test.h5" );
const H5std_string DATASET_NAME( "/0/test_manufacturer_test_model_test_serial/Ch_0/Trc_0/unformatted_data" );
const H5std_string MEMBER1( "r" );
const H5std_string MEMBER2( "i" );
const H5std_string MEMBER3( "c_name" );
const H5std_string GROUP_NAME ("0");
const int   LENGTH = 10;
const int   RANK = 1;


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

int main(void)
{
   /* First structure  and dataset*/
   typedef struct s1_t {
    double r;
    double i;
   } s1_t;
   /* Second structure (subset of s1_t)  and dataset*/

   // Try block to detect exceptions raised by any of the calls inside it
   try
   {
      /*
       * Initialize the data
       */


       Exception::dontPrint();
      CompType mtype2( sizeof(s1_t) );
      mtype2.insertMember( MEMBER1, HOFFSET(s1_t, r), PredType::NATIVE_DOUBLE);
      mtype2.insertMember( MEMBER2, HOFFSET(s1_t, i), PredType::NATIVE_DOUBLE);
      int len = 10;

      std::vector<s1_t> s1;

      H5File file = H5File( FILE_NAME, H5F_ACC_RDONLY );
      file.getNumObjs();
      Group g=getChildGroup(file,0);
      std::string s = getObjectName(file,0);
      size_t num_messungen = getNumObj(file);
	for(unsigned int i = 0;i<num_messungen;i++){
	     Group messung=getChildGroup(file,i);
	     size_t num_geraete= getNumObj(messung);
		 for(unsigned int j =0;j<num_geraete;j++){
		     Group geraet=getChildGroup(messung,j);
		     size_t num_channels=getNumObj(geraet);
		     for(unsigned int k =0;k<num_channels;k++){
			 Group channel = getChildGroup(geraet,k);
			 size_t num_traces = getNumObj(channel);
			 for(unsigned int l =0;l<num_traces;l++){
			     Group trace = getChildGroup(channel,l);
			     DataSet s_param=trace.openDataSet(std::string("unformatted_data"));
			     const int num_elements = s_param.getStorageSize()/sizeof(mtype2);
			     s1_t s2[num_elements];
			     s_param.read(s2,mtype2);

			     double r= s2[0].r;
			     double i =s2[0].i;
			 }

		     }

		 }

	}

      //DataSet*  dataset = new DataSet (file->openDataSet( DATASET_NAME ));

      /*
       * Create a datatype for s2
       */

      /*
       * Read two fields c and a from s1 dataset. Fields in the file
       * are found by their names "c_name" and "a_name".
       */
      // s1_t s2[LENGTH];
      //dataset->read( s2, mtype2 );
      /*
       * Display the fields
       */

  //      delete dataset;
  //      delete file;
   }  // end of try block
   // catch failure caused by the H5File operations
   catch( FileIException error )
   {
      error.printError();
      return -1;
   }
   // catch failure caused by the DataSet operations
   catch( DataSetIException error )
   {
      error.printError();
      return -1;
   }
   // catch failure caused by the DataSpace operations
   catch( DataSpaceIException error )
   {
      error.printError();
      return -1;
   }
   // catch failure caused by the DataSpace operations
   catch( DataTypeIException error )
   {
      error.printError();
      return -1;
   }
   return 0;
}


