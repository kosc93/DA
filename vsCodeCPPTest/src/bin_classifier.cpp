#include "bin_classifier.h"

BinClassifier::BinClassifier(const bool& normalize_, const int& cross_validation_manifold_):
  normalize(normalize_),
  cross_validation_manifold(cross_validation_manifold_)
{
}

void BinClassifier::import_data(std::vector<DataPoint>& datapoints_){
    for(const auto & datapoint: datapoints_){
      sample_type sample;
      long vector_size = long(datapoint.features.size());
      sample.set_size(vector_size);
      for(long i=0;i<vector_size;i++){
	  sample(i)=datapoint.features[i];
      }
      samples.push_back(sample);
      lables.push_back(datapoint.original_class);
  }
}
