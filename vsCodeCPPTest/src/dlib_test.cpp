// The contents of this file are in the public domain. See LICENSE_FOR_EXAMPLE_PROGRAMS.txt
/*

    This is an example illustrating the use of the server_iostream object from
    the dlib C++ Library.

    This is a simple echo server.  It listens on port 1234 for incoming
    connections and just echos back any text it receives, but in upper case.  So
    basically it is the same as the sockets_ex.cpp example program  except it
    uses iostreams.

    To test it out you can just open a command prompt and type:
    telnet localhost 1234

    Then you can type away.

*/




#include <dlib/server.h>
#include "defines_etc.h"
#include <iostream>
#include <vector>

using namespace dlib;
using namespace std;


int main()
{

	std::vector<DataPoint> dp;
	for(int i=0;i<6;i++){
	    dp.push_back(DataPoint());
	}
	for(int i=0;i<6;i++){
	    dp[0].features.push_back(0.5);
	    dp[1].features.push_back(1);
	    dp[2].features.push_back(2);
	    dp[3].features.push_back(double((i+1)%2));
	    dp[4].features.push_back(double(i+1));
	    dp[5].features.push_back(double((i+1)*((i+1)%2)));
	  }
	for(int i =0;i<6;i++){
	    for(int j=0;j<6;j++){
		std::cout<<dp[j].cos_sim(dp[i])<<" ";
	    }
	    std::cout<<endl;
	}

}



