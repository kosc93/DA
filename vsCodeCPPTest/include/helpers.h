/*
 * helpers.h
 *
 *  Created on: Jan 18, 2018
 *      Author: kosc93
 */

#ifndef HELPERS_H_
#define HELPERS_H_
#include <stdexcept>


class NotImplementedException : public std::logic_error
{
public:
    NotImplementedException():std::logic_error("not implemented"){};
};


#endif /* INCLUDE_HELPERS_H_ */
