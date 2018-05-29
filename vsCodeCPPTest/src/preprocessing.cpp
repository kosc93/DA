/*
 * preprocessing.cpp
 *
 *  Created on: Mar 27, 2018
 *      Author: kosc93
 */

#include "preprocessing.h"

void
Preprocessor::import_data (const std::vector<DataPoint>& datapoints_)
{
  for (const auto & datapoint : datapoints_)
    {
      sample_type sample;
      feature_size = long (datapoint.features.size ());
      sample.set_size (feature_size);
      for (unsigned int i = 0; i < feature_size; i++)
	{
	  sample (i) = datapoint.features[i];

	}
      samples.push_back (sample);
      //cout<<sample<<endl;
      labels.push_back (datapoint.original_class);
    };
}

void
Preprocessor::normalize ()
{
  if (!norm)
    {
      return;
    }
  normalizer.train (samples);
  for (auto& sample : samples)
    {
      sample = normalizer (sample);
    }
}
void
Preprocessor::randomize ()
{
  randomize_samples (samples, labels);
}

void
Preprocessor::reduce_dim ()
{
  if (!imported)
    {
      std::cout << "reducing dimensions from " << feature_size << std::endl;
      dlib::matrix<double> X;

      X.set_size (train_samples.size (), feature_size);
      for (unsigned int row = 0; row < train_samples.size (); row++)
	{
	  dlib::matrix<double> matrix = train_samples[row];

	  for (unsigned int col = 0; col < feature_size; col++)
	    {
	      X (row, col) = matrix (col);
	    }
	}
      dlib::matrix<double, 0, 1> M;
      std::vector<unsigned long> labs;
      for (auto& l : train_labels)
	{
	  labs.push_back (long (l));
	}

      //dlib::compute_lda_transform (X, M, labs, 3);
      dlib::compute_lda_transform (X, M, labs);
      dim_reduce.X_dim_reduce = X;
      dim_reduce.M_dim_reduce = M;
      cout << "to " << M.size () << endl;

    }

  std::cout << "lda computed" << std::endl;
  for (auto& sample_ : test_samples)
    {
      dlib::matrix<double> buffer = dim_reduce.X_dim_reduce * sample_
	  - dim_reduce.M_dim_reduce;
      sample_.set_size (buffer.size ());
      sample_ = buffer;

    }

  for (auto& sample_ : train_samples)
    {
      dlib::matrix<double> buffer = dim_reduce.X_dim_reduce * sample_
	  - dim_reduce.M_dim_reduce;
      sample_.set_size (buffer.size ());
      sample_ = buffer;

    }

}

void
Preprocessor::import_params (std::string filename)
{
  ifstream in_stream (filename + ".preprocessing", ifstream::in);
  if (!in_stream.good ())
    {
      cout << "file does not exists" << endl;
      in_stream.close ();
      return;
    }
  try
    {
      unsigned long imported_feature_size;
      dlib::deserialize (imported_feature_size, in_stream);
      if (feature_size != imported_feature_size)
	{
	  cout << "feature size in imported file does not match!" << endl;
	  throw dlib::serialization_error ("feature size not matching");
	}
      std::string imported_filter_hash;
      dlib::deserialize (imported_filter_hash, in_stream);
      if (filter_hash != imported_filter_hash)
	{
	  cout << "filters in imported file does not match!" << endl;
	  throw dlib::serialization_error ("filters not matching");
	}

      dlib::deserialize (dim_reduce.X_dim_reduce, in_stream);
      dlib::deserialize (dim_reduce.M_dim_reduce, in_stream);
      dlib::deserialize (normalizer, in_stream);
      imported = true;
      cout << "file sucessfully loaded" << endl;
    }
  catch (dlib::serialization_error)
    {
      cout << "file could not be loaded" << endl;
    }
  in_stream.close ();
}

void
Preprocessor::export_params (std::string filename)
{
  ofstream out_stream (filename + ".preprocessing", ofstream::out);
  try
    {
      dlib::serialize (feature_size, out_stream);
      dlib::serialize (filter_hash, out_stream);
      dlib::serialize (dim_reduce.X_dim_reduce, out_stream);
      dlib::serialize (dim_reduce.M_dim_reduce, out_stream);
      dlib::serialize (normalizer, out_stream);
      cout << "file sucessfully saved" << endl;
    }
  catch (dlib::serialization_error)
    {
      cout << "file could not be saved" << endl;
    }
  out_stream.close ();
}

void
Preprocessor::split (double ratio_test)
{
  std::map<double, double> label_percentages;
  for (auto& label : labels)
    {
      label_percentages[label] += 1;
    }
  std::map<double, unsigned int> test_dist;
  for (auto& percetage : label_percentages)
    {
      //num_samples*ratio_of_class_in_samples*ratio_test
      int test = int (round (percetage.second * ratio_test));
      test_dist[percetage.first] = test;
    }
  for (unsigned int i = 0; i < samples.size (); i++)
    {
      if (test_dist[labels[i]] != 0)
	{
	  test_samples.push_back (samples[i]);
	  test_labels.push_back (labels[i]);
	  test_dist[labels[i]]--;
	}
      else
	{
	  train_samples.push_back (samples[i]);
	  train_labels.push_back (labels[i]);
	}
    }

  //make vector with the percetages of each class
  //convert to unsigned integer vector reflecting the correnponding test_set sizes
  //loop over all samples(randomized) and decide wether they have enough in the test_set else put in training set
  //make sure to take same amount from every class

//  for(;counter<num_test;counter++){
//      test_samples.push_back(samples[counter]);
//      test_labels.push_back(labels[counter]);
//  }
//  for(;counter<samples.size();counter++){
//      train_samples.push_back(samples[counter]);
//      train_labels.push_back(labels[counter]);
//  }
}

sample_type
Preprocessor::operator () (DataPoint& datapoint)
{
  sample_type sample;
  sample.set_size (datapoint.features.size ());
  for (unsigned int i = 0; i < datapoint.features.size (); i++)
    {
      sample (i) = datapoint.features[i];
    }
  //normalization
  if (norm)
    {
      sample = normalizer (sample);
    }
  //dimension reduction
  dlib::matrix<double> sample_dim_reduced = dim_reduce.X_dim_reduce * sample
      - dim_reduce.M_dim_reduce;
  sample.set_size (sample_dim_reduced.size ());
  sample = sample_dim_reduced;
  //cout<<sample;
  return sample;
}

void
Preprocessor::import_test (const std::vector<DataPoint>& datapoints_)
{
  for (const auto & datapoint : datapoints_)
    {
      sample_type sample;
      feature_size = long (datapoint.features.size ());
      sample.set_size (feature_size);
      for (unsigned int i = 0; i < feature_size; i++)
	{
	  sample (i) = datapoint.features[i];
	}
      test_samples.push_back (sample);
      //cout<<sample<<endl;
      test_labels.push_back (datapoint.original_class);
    };
  randomize_samples (test_samples, test_labels);
  if (norm)
    {
      for (auto& sample : test_samples)
	{
	  sample = normalizer (sample);
	}
    }
}

void
Preprocessor::import_train (const std::vector<DataPoint>& datapoints_)
{
  for (const auto & datapoint : datapoints_)
    {
      sample_type sample;
      feature_size = long (datapoint.features.size ());
      sample.set_size (feature_size);
      for (unsigned int i = 0; i < feature_size; i++)
	{
	  sample (i) = datapoint.features[i];
	}
      train_samples.push_back (sample);
      //cout<<sample<<endl;
      train_labels.push_back (datapoint.original_class);
    };
  randomize_samples (train_samples, train_labels);
  if (norm)
    {
      normalizer.train (train_samples);
      for (auto& sample : train_samples)
	{
	  sample = normalizer (sample);
	}
    }
}


