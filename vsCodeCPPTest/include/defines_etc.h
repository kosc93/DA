/*
 * defines_etc.h
 *
 *  Created on: Feb 6, 2018
 *      Author: kosc93
 */

#ifndef DEFINES_ETC_H_
#define DEFINES_ETC_H_

#define NUM_PENS 8
#include <stdexcept>
#include <vector>

class NotImplementedException : public std::logic_error
{
public:
    NotImplementedException():std::logic_error("not implemented"){};
};


const int ID_IMPORT =1;
const int ID_TRAIN =2;
const int ID_EXPORT =3;
const int ID_TRAINER_SELECTED = 4;

#endif /* INCLUDE_DEFINES_ETC_H_ */
