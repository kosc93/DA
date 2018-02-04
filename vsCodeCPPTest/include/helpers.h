/*
 * helpers.h
 *
 *  Created on: Jan 18, 2018
 *      Author: kosc93
 */

#ifndef HELPERS_H_
#define HELPERS_H_
#include <stdexcept>
#include <vector>
#include "measurement_data.h"
#include <wx/wx.h>
#include <wx/stattext.h>
#include <wx/regex.h>

class NotImplementedException : public std::logic_error
{
public:
    NotImplementedException():std::logic_error("not implemented"){};
};

class LabelHelper : public wxDialog{
  public:
    LabelHelper();
    void show(std::vector<Measurement>& measurements);

};

#endif /* INCLUDE_HELPERS_H_ */
