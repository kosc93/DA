#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

struct Measurement
{
  string filename;
  int session;
};

void
create_script (string script_name, vector<string> test_files,
	       vector<string> train_files)
{

  ofstream sbatch (script_name+"",ostream::out);
  sbatch<<"#!/bin/bash"<<endl;
sbatch<<"#SBATCH -p haswell"<<endl;
sbatch<<"#SBATCH --time=24:00:00   # walltime"<<endl;
sbatch<<"#SBATCH --nodes=1   # number of nodes"<<endl;
sbatch<<"#SBATCH --tasks-per-node=1"<<endl;
sbatch<<"#SBATCH --cpus-per-task=24"<<endl;
sbatch<<"#SBATCH --mem-per-cpu=400M   # memory per CPU core"<<endl;
sbatch<<"#SBATCH --mail-user=s0630137@mail.zih.tu-dresden.de   # email address"<<endl;
sbatch<<"#SBATCH --mail-type=BEGIN,END,FAIL,REQUEUE,TIME_LIMIT,TIME_LIMIT_90"<<endl;

sbatch<<"# Set the max number of threads to use for programs using OpenMP. Should be <= ppn. Does nothing if the program doesn't use OpenMP."<<endl;
sbatch<<"export OMP_NUM_THREADS=$SLURM_CPUS_ON_NODE"<<endl;
sbatch<<"OUTFILE=\"\""<<endl;

sbatch<<"/scratch/s0630137/cpp/"<<script_name<<".sh"<<endl;


  vector<string> kernels =
    { "r", "l" };
  vector<string> bins =
    { "c", "r", "k" };
  vector<string> mults =
    { "o", "a", "s" };
  ofstream file (script_name+".sh", ofstream::out);
  file << "#!/bin/bash" << endl;
  file << "start_time=$SECONDS" << endl;
  if (test_files.size () != train_files.size ())
    {
      throw;
    }
  for (string& kernel : kernels)
    {
      for (string& bin : bins)
	{
	  for (string mult : mults)
	    {
	      if (mult == "s" && bin != "c" && kernel != "l")
		continue;
	      for (unsigned int i = 0; i < test_files.size (); i++)
		{

		  file << "flags=\"vu\"" << endl;
		  file << "kernel=\"" << kernel << "\"" << endl;
		  file << "bin=\"" << bin << "\"" << endl;
		  file << "mult=\"" << mult << "\"" << endl;
		  file << "traces=\"Trc1 Trc2 Trc3 Trc4 Trc5 Trc6\"" << endl;
		  file << "freqs=\"1e9 16.5e9\"" << endl;
		  file << "train=\"" << train_files[i] << "\"" << endl;
		  file << "test=\"" << test_files[i] << "\"" << endl;
		  file
		      << "vo_klass --test $test --train $train -$flags --kernel $kernel --bin $bin --mult $mult --freqs \"$freqs\" >\"$kernel\"_\"$mult\"_\"$bin\"_\"${traces// /_}\"_\"${freqs// /_}\""
		      << script_name +".txt" << endl;
		  file << endl;
		}

	    }
	}
    }
  file << "elapsed_time=$(($SECONDS- $start_time))" << endl;
  file << "echo \"seconds: $elapsed_time \"" << endl;
  file.close ();
}

int
main ()
{
  ifstream file ("files.dat", ifstream::in);
  map<int, vector<Measurement>> types;
  string fn_buffer;
  int session_buffer;
  int type_buffer;
  while (file >> fn_buffer >> type_buffer >> session_buffer)
    {
      if (types.find (type_buffer) == types.end ())
	{
	  types[type_buffer] = vector<Measurement> ();
	}
      Measurement meas;
      meas.filename = fn_buffer;
      meas.session = session_buffer;
      types[type_buffer].push_back (meas);
    }
  //loop over all types --> type denotes a antenna position etc
  //for each type create leave one session out
  //do whole type with 0.25 testset
  //loop over all sessions
  //for each session --> create leave one measurement out
  //do whole session with 0.25 testset
  for (const auto& type : types)
    {
      map<int, vector<string>> sessions;
      vector<string> type_files;
      for (const auto& meas : type.second)
	{
	  type_files.push_back (meas.filename);
	  if (sessions.find (meas.session) == sessions.end ())
	    {
	      sessions[meas.session] = vector<string> ();
	    }
	  sessions[meas.session].push_back (meas.filename);
	}
      //leave measurement out
      vector<string> test_files;
      vector<string> train_files;
      for (const auto& item : sessions)
	{

	  vector<string> vec = item.second;

	  for (unsigned int i = 0; i < vec.size (); i++)
	    {
	      string train, test;
	      test = vec[i];
	      test_files.push_back (test);
	      for (unsigned int k = 0; k < vec.size (); k++)
		{
		  if (k == i)
		    continue;
		  train += vec[k] + " ";
		}
	      train.pop_back();
	      train_files.push_back (train);
	    }
	  create_script ("lmo_"+ to_string (type.first)+"_" + to_string (item.first)  , test_files,
			 train_files);

	}
      //leave session out
      test_files.clear ();
      train_files.clear ();
      for (unsigned int i = 0; i < sessions.size (); i++)
	{
	  string train, test;
	  for (const auto& str : sessions[i])
	    {
	      test += str + " ";
	    }
	  test.pop_back();
	  test_files.push_back (test);
	  for (unsigned int k = 0; k < sessions.size (); k++)
	    {
	      if (k == i)
		continue;
	      for (const auto& str : sessions[k])
		{
		  train += str + " ";
		}
	    }
	  train.pop_back();
	  train_files.push_back (train);
	}
      create_script ("lso_" + to_string (type.first), test_files,
		     train_files);
    }

  return 0;
}

