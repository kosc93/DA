// ****************************************************************************
// This file is part of VocalTractLab.
// Copyright (C) 2008, Peter Birkholz, Hamburg, Germany
// www.vocaltractlab.de
// author: Peter Birkholz
// ****************************************************************************

#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <wx/wx.h>

// ****************************************************************************
/// Represents a physical quantity 
struct PhysicalQuantity
{
	/// name of the quantity
  char name[64];
	/// symbol used in equations/formulae
  char symbol[64];
	/// unit in the MKS system (meter, kilogram, second)
  char mksUnit[64];
	/// unit in the CGS system (centimeter, gram, second)
  char cgsUnit[64];
	/// factor to convert from MKS to CGS system
  double mksToCgsFactor;
};

/// kind of the physical quantities in \ref Graph::physicalQuantity[] by index
enum PhysicalQuantityIndex 
{ 
  PQ_LENGTH = 0,            PQ_MASS = 1,          PQ_TIME = 2,
  PQ_AREA = 3,              PQ_FORCE = 4,         PQ_MOMENTUM = 5,
  PQ_FREQUENCY = 6,         PQ_ANGLE = 7,         PQ_ANGULAR_VELOCITY = 8,
  PQ_PRESSURE = 9,          PQ_MASSFLOW = 10,     PQ_DENSITY = 11,
  PQ_VOLUME_VELOCITY = 12,  PQ_TEMPERATURE = 13,  PQ_RATIO = 14,
  PQ_VELOCITY = 15  ,		PQ_SEMITONE = 16    

};
/// total number of physical quantities in \ref Graph::physicalQuantity[]
const int NUM_PHYSICAL_QUANTITIES = 17;

/// common phsyical quantities
const PhysicalQuantity physicalQuantity[NUM_PHYSICAL_QUANTITIES] = 
{
  "length",           "l",      "m",      "cm",         100.0,
  "mass",             "m",      "kg",     "g",          1000.0,
  "time",             "t",      "s",      "s",          1.0,
  "area",             "A",      "m^2",    "cm^2",       10000.0,
  "force",            "F",      "N",      "dyne",       100000.0,
  "momentum",         "(mv)",   "N-s",    "dyne-s",     100000.0,
  "frequency",        "f",      "Hz",     "Hz",         1.0,
  "angle",            "phi",    "rad",    "rad",        1.0,
  "angular velocity", "omega",  "rad/s",  "rad/s",      10.0,
  "pressure",         "P",      "N/m^2",  "dyne/cm^2",  10.0,
  "mass flow",        "dm/dt",  "kg/s",   "g/s",        1000.0,
  "density",          "rho",    "kg/m^3", "g/cm^3",     0.001,
  "volume velocity",  "dV/dt",  "m^3/s",  "cm^3/s",     1000000.0,
  "temperature",      "T",      "K",      "-",          1.0,
  "ratio",            "",       "",       "",           1.0,
  "velocity",         "v",      "m/s",    "cm/s",       100.0,
  "semitones",		  "st",		"st",	  "c",		    100.0
};

// ****************************************************************************
/// \brief This class is used to display graphs.
///
/// This class can draw an axes, display a graph of some kind of physical
/// quantity and offers numerous functions for analysis of the graph.
// ****************************************************************************
class Graph
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:
	/// Axes properties in the linear domain
  struct LinearDomain
  {
		/// displayed physical quantity
    PhysicalQuantityIndex quantity;
		/// reference value
    double reference;
		/// interval of scale division
    double scaleDivision;
  
		/// minimum negative limit
    double negativeLimitMin;
		/// maximum negative limit
    double negativeLimitMax;
		/// negative limit
    double negativeLimit;

		/// minimum positive limit
    double positiveLimitMin;
		/// maximum positive limit
    double positiveLimitMax;
		/// positive limit
    double positiveLimit;

		/// number of possible zoom steps
    int    numZoomSteps;
		/// number of decimal places to display
    int    postDecimalPositions; 
		/// scale labels need to be converted to CGS system
    bool   useCgsUnit;                  
		/// labels are relative to the reference value
    bool   useRelativeInscription;
		/// show gray grid lines
    bool   showGrayLines;
  } 
  abscissa, linearOrdinate;
  
	/// Ordinate properties in the logarithmic domain
  struct LogDomain
  {
		/// reference value
    double reference;
		/// interval of scale division
    double scaleDivision;
		/// minimum lower level
    double lowerLevelMin;
		/// maximum lower level
    double lowerLevelMax;
		/// lower level
    double lowerLevel;

		/// minimum upper level
    double upperLevelMin;
		/// maximum upper level
    double upperLevelMax;
		/// upper level
    double upperLevel;
		/// show gray grid lines
    bool   showGrayLines;
		/// number of zoom steps in dB
    double zoomStep;
  } logOrdinate;

  // Optionen *****************************************************
	/// ordinate is linearly scaled (otherwise logarithmic)
  bool isLinearOrdinate;
	/// abscissa is at the bottom of the graph (otherwise at the top)
  bool abscissaAtBottom;                    
	/// ordinate is at the left side of the graph (otherwise at the right side)
  bool ordinateAtLeftSide;                  

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  Graph();

  void init(wxWindow *ctrl, int leftMargin, int rightMargin, int topMargin, int bottomMargin);
  void getDimensions(int& x, int& y, int& w, int& h);
  void getMargins(int& left, int& right, int& top, int& bottom);

  void initAbscissa(PhysicalQuantityIndex quantity, double reference, double scaleDivision,
                    double negativeLimitMin, double negativeLimitMax, double negativeLimit,
                    double positiveLimitMin, double positiveLimitMax, double positiveLimit,
                    int numZoomSteps, int postDecimalPositions, 
                    bool useCgsUnit, bool useRelativeInscription, bool showGrayLines);
  
  void initLinearOrdinate(PhysicalQuantityIndex quantity, double reference, double scaleDivision,
                    double negativeLimitMin, double negativeLimitMax, double negativeLimit,
                    double positiveLimitMin, double positiveLimitMax, double positiveLimit,
                    int numZoomSteps, int postDecimalPositions, 
                    bool useCgsUnit, bool useRelativeInscription, bool showGrayLines);

  void initLogOrdinate(double reference, double scaleDivision,
                      double lowerLevelMin, double lowerLevelMax, double lowerLevel,
                      double upperLevelMin, double upperLevelMax, double upperLevel,
                      bool showGrayLines, double zoomStep);

  void paintAbscissa(wxDC &dc);
  void paintOrdinate(wxDC &dc);

  void zoomInAbscissa(bool negativeLimit, bool positiveLimit);
  void zoomOutAbscissa(bool negativeLimit, bool positiveLimit);
  void zoomInOrdinate(bool negativeLimit, bool positiveLimit);
  void zoomOutOrdinate(bool negativeLimit, bool positiveLimit);

  // Transformationen phys. Größe <-> Pixelzeile/-spalte **********

  int getXPos(double absXValue);
  int getYPos(double absYValue);
  double getAbsXValue(int xPos);
  double getAbsYValue(int yPos);

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  static const double EPSILON;
  
  /// Margins of the graph to the edge of the picture
  int leftMargin, rightMargin, bottomMargin, topMargin;
  wxWindow *control;

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  void getZoomFactors(LinearDomain *domain, double& positiveZoomFactor, double& negativeZoomFactor);
};

#endif