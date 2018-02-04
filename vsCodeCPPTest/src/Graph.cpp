// ****************************************************************************
// This file is part of VocalTractLab.
// Copyright (C) 2008, Peter Birkholz, Hamburg, Germany
// www.vocaltractlab.de
// author: Peter Birkholz
// ****************************************************************************

#include "Graph.h"
#include <cmath>

const double Graph::EPSILON = 0.000001;

// ****************************************************************************
/// \brief Constructor.
///
/// Constructs a graph object and initializes it with default values.
// ****************************************************************************

Graph::Graph()
{
  // Default graph dimensions ***********************

  leftMargin = 0;
  rightMargin = 0;
  bottomMargin = 0;
  topMargin = 0;

  control = NULL;

  // Default values **************************

  abscissa.quantity           = PQ_RATIO;
  abscissa.reference          = 0.0;
  abscissa.scaleDivision      = 0.1;
  abscissa.negativeLimitMin   = 0.0;
  abscissa.negativeLimitMax   = 0.0;
  abscissa.negativeLimit      = 0.0;
  abscissa.positiveLimitMin   = 1.0;
  abscissa.positiveLimitMax   = 1.0;
  abscissa.positiveLimit      = 1.0;
  abscissa.numZoomSteps       = 10;
  abscissa.useCgsUnit         = false;
  abscissa.postDecimalPositions   = 2;
  abscissa.useRelativeInscription = true;
  abscissa.showGrayLines      = true;

  linearOrdinate.quantity         = PQ_RATIO;
  linearOrdinate.reference        = 0.0;
  linearOrdinate.scaleDivision    = 0.1;
  linearOrdinate.negativeLimitMin = 0.0;
  linearOrdinate.negativeLimitMax = 0.0;
  linearOrdinate.negativeLimit    = 0.0;
  linearOrdinate.positiveLimitMin = 1.0;
  linearOrdinate.positiveLimitMax = 1.0;
  linearOrdinate.positiveLimit    = 1.0;
  linearOrdinate.numZoomSteps     = 10;
  linearOrdinate.useCgsUnit       = false;
  linearOrdinate.postDecimalPositions   = 2;
  linearOrdinate.useRelativeInscription = true;
  linearOrdinate.showGrayLines    = true;

  logOrdinate.reference     = 1.0;
  logOrdinate.scaleDivision = 1.0;      // dB
  logOrdinate.lowerLevelMin = -20.0;
  logOrdinate.lowerLevelMax = -20.0;
  logOrdinate.lowerLevel    = -20.0;
  logOrdinate.upperLevelMin = 20.0;
  logOrdinate.upperLevelMax = 20.0;
  logOrdinate.upperLevel    = 20.0;
  logOrdinate.zoomStep      = 10.0;      // dB
  logOrdinate.showGrayLines = true;

  // Options *******************************************************

  isLinearOrdinate   = true;
  abscissaAtBottom   = true;                
  ordinateAtLeftSide = true;                
}

// ****************************************************************************
/// \brief Initializes the margins.
///
/// Initializes the margins on all four sides of the graph.
/// \param ctrl parent window
/// \param leftMargin margin to the left of the graph
/// \param rightMargin margin to the right of the graph
/// \param topMargin margin at the top of the graph
/// \param bottomMargin margin at the bottom of the graph
// ****************************************************************************

void Graph::init(wxWindow *ctrl, int leftMargin, int rightMargin, int topMargin, int bottomMargin)
{
  control = ctrl;

  this->leftMargin   = leftMargin;
  this->rightMargin  = rightMargin;
  this->topMargin    = topMargin;
  this->bottomMargin = bottomMargin;
}

// ****************************************************************************
/// \brief Returns the dimensions of the graph.
///
/// Returns the coordinates and dimensions of the graph.
/// \param [out] x x-coordinate of the graph
/// \param [out] y y-coordinate of the graph
/// \param [out] w width of the graph
/// \param [out] h height of the graph
// ****************************************************************************

void Graph::getDimensions(int& x, int& y, int& w, int& h)
{
  if (control == NULL)
  {
    x = y = 0;
    w = h = 1;
    return;
  }

  int width, height;
  control->GetSize(&width, &height);
  
  x = leftMargin;
  y = topMargin;
  w = width - leftMargin - rightMargin;
  h = height - topMargin - bottomMargin;
}

// ****************************************************************************
/// \brief Return the margins of the graph.
///
/// Returns all four margins of the graph.
/// \param [out] left left margin
/// \param [out] right right margin
/// \param [out] top top margin
/// \param [out] bottom bottom margin
// ****************************************************************************

void Graph::getMargins(int& left, int& right, int& top, int& bottom)
{
  left   = leftMargin;
  right  = rightMargin;
  top    = topMargin;
  bottom = bottomMargin;
}

// ****************************************************************************
/// \brief Inits the scale of the abscissa.
/// 
/// Initializes the abscissa with all necessary values.
/// \param quantity physical quantity represented by the abscissa
/// \param reference reference value for the abscissa values
/// \param scaleDivision division interval of the abscissa
/// \param negativeLimitMin the minimum negative limit
/// \param negativeLimitMax the maximum negative limit
/// \param negativeLimit the negative limit
/// \param positiveLimitMin the minimum positive limit
/// \param positiveLimitMax the maximum positive limit
/// \param positiveLimit the positive limit
/// \param numZoomSteps the number of possible zoom steps
/// \param postDecimalPositions the number of displayed decimal places
/// \param useCgsUnit use CGS unit system (or MKS)
/// \param useRelativeInscription values are relative to \ref reference
/// \param showGrayLines use gray minor grid lines
// ****************************************************************************

void Graph::initAbscissa(PhysicalQuantityIndex quantity, double reference, double scaleDivision,
                         double negativeLimitMin, double negativeLimitMax, double negativeLimit,
                         double positiveLimitMin, double positiveLimitMax, double positiveLimit,
                         int numZoomSteps, int postDecimalPositions, 
                         bool useCgsUnit, bool useRelativeInscription, bool showGrayLines)
{
  abscissa.quantity               = quantity;
  abscissa.reference              = reference;
  abscissa.scaleDivision          = scaleDivision;
  abscissa.negativeLimitMin       = negativeLimitMin;
  abscissa.negativeLimitMax       = negativeLimitMax;
  abscissa.negativeLimit          = negativeLimit;
  abscissa.positiveLimitMin       = positiveLimitMin;
  abscissa.positiveLimitMax       = positiveLimitMax;
  abscissa.positiveLimit          = positiveLimit;
  abscissa.numZoomSteps           = numZoomSteps;
  abscissa.postDecimalPositions   = postDecimalPositions;
  abscissa.useCgsUnit             = useCgsUnit;
  abscissa.useRelativeInscription = useRelativeInscription;
  abscissa.showGrayLines          = showGrayLines;
}
    
// ****************************************************************************
/// \brief Inits the linear scale of the ordinate.
/// 
/// Initializes the ordinate with all necessary values for a linear 
/// representation.
/// \param quantity physical quantity represented by the ordinate
/// \param reference reference value for the ordinate values
/// \param scaleDivision division interval of the ordinate
/// \param negativeLimitMin the minimum negative limit
/// \param negativeLimitMax the maximum negative limit
/// \param negativeLimit the negative limit
/// \param positiveLimitMin the minimum positive limit
/// \param positiveLimitMax the maximum positive limit
/// \param positiveLimit the positive limit
/// \param numZoomSteps the number of possible zoom steps
/// \param postDecimalPositions the number of displayed decimal places
/// \param useCgsUnit use CGS unit system (or MKS)
/// \param useRelativeInscription values are relative to \ref reference
/// \param showGrayLines use gray minor grid lines
// ****************************************************************************

void Graph::initLinearOrdinate(PhysicalQuantityIndex quantity, double reference, double scaleDivision,
                               double negativeLimitMin, double negativeLimitMax, double negativeLimit,
                               double positiveLimitMin, double positiveLimitMax, double positiveLimit,
                               int numZoomSteps, int postDecimalPositions, 
                               bool useCgsUnit, bool useRelativeInscription, bool showGrayLines)
{
  linearOrdinate.quantity               = quantity;
  linearOrdinate.reference              = reference;
  linearOrdinate.scaleDivision          = scaleDivision;
  linearOrdinate.negativeLimitMin       = negativeLimitMin;
  linearOrdinate.negativeLimitMax       = negativeLimitMax;
  linearOrdinate.negativeLimit          = negativeLimit;
  linearOrdinate.positiveLimitMin       = positiveLimitMin;
  linearOrdinate.positiveLimitMax       = positiveLimitMax;
  linearOrdinate.positiveLimit          = positiveLimit;
  linearOrdinate.numZoomSteps           = numZoomSteps;
  linearOrdinate.postDecimalPositions   = postDecimalPositions;
  linearOrdinate.useCgsUnit             = useCgsUnit;
  linearOrdinate.useRelativeInscription = useRelativeInscription;
  linearOrdinate.showGrayLines          = showGrayLines;
}

// ****************************************************************************
/// \brief Inits the logarithmic scale of the ordinate.
/// 
/// Initializes the ordinate with all necessary values for a logarithmic 
/// representation.
/// \param quantity physical quantity represented by the ordinate
/// \param reference reference value for the ordinate values
/// \param negativeLevelMin the minimum negative level
/// \param negativeLevelMax the maximum negative level
/// \param negativeLevel the negative level
/// \param positiveLevelMin the minimum positive level
/// \param positiveLevelMax the maximum positive level
/// \param positiveLevel the positive level
/// \param showGrayLines use gray minor grid lines
/// \param numZoomSteps the number of possible zoom steps
// ****************************************************************************

void Graph::initLogOrdinate(double reference, double scaleDivision,
                            double lowerLevelMin, double lowerLevelMax, double lowerLevel,
                            double upperLevelMin, double upperLevelMax, double upperLevel,
                            bool showGrayLines, double zoomStep)
{
  logOrdinate.reference     = reference;
  logOrdinate.scaleDivision = scaleDivision;
  logOrdinate.lowerLevelMin = lowerLevelMin;
  logOrdinate.lowerLevelMax = lowerLevelMax;
  logOrdinate.lowerLevel    = lowerLevel;
  logOrdinate.upperLevelMin = upperLevelMin;
  logOrdinate.upperLevelMax = upperLevelMax;
  logOrdinate.upperLevel    = upperLevel;
  logOrdinate.showGrayLines = showGrayLines;
  logOrdinate.zoomStep      = zoomStep;
}


// ****************************************************************************
/// \brief Draw the abscissa scale.
///
/// Draw the abscissa to the specified device context.
/// \param dc device context for painting (see wxWidgets 2.8 documentation)
// ****************************************************************************

void Graph::paintAbscissa(wxDC &dc)
{
  int i;
  int xPos, yPos;
  double d;
  wxString st;
  wxString unitString;
  int charWidth = dc.GetCharWidth();
  int charHeight = dc.GetCharHeight();
  int w, h;

  int graphX, graphY, graphW, graphH;
  getDimensions(graphX, graphY, graphW, graphH);

  // ****************************************************************
  // Paint the coordinate axis.
  // ****************************************************************

  dc.SetPen(*wxBLACK_PEN);
  if (abscissaAtBottom)
  { 
    dc.DrawLine(graphX, graphY+graphH, graphX+graphW-1, graphY+graphH);
  }
  else
  { 
    dc.DrawLine(graphX, graphY-1, graphX+graphW-1, graphY-1);
  }

  // ****************************************************************
  // Consider scaling from mks to cgs system.
  // ****************************************************************

  double unitConversionFactor = 1.0;
  if (abscissa.useCgsUnit) 
  { 
    unitConversionFactor = physicalQuantity[abscissa.quantity].mksToCgsFactor; 
  }

  // ****************************************************************
  // Print the physical unit at the right.
  // ****************************************************************
    
  if (abscissa.useCgsUnit)
  { 
    unitString = wxString(physicalQuantity[abscissa.quantity].cgsUnit, wxConvUTF8); 
  }
  else
  { 
    unitString = wxString(physicalQuantity[abscissa.quantity].mksUnit, wxConvUTF8); 
  }

  dc.SetPen(*wxBLACK_PEN);
  dc.SetBackgroundMode(wxTRANSPARENT);

  dc.GetTextExtent(unitString, &w, &h);
  int unitLengthInPixels = w + 10;

  if (abscissaAtBottom)
  { 
    dc.DrawText(unitString, graphX + graphW - 1 - w, graphY + graphH + 5); 
  }
  else
  { 
    dc.DrawText(unitString, graphX + graphW - 1 - w, graphY - 5 - h); 
  }

  // ****************************************************************
  // Calc. the length of the longest number on the scale in pixels*1.5
  // ****************************************************************
  
  int inscriptionLengthInPixels = 1;
  int numLeftChars;
  int numRightChars;
  

  if (abscissa.useRelativeInscription)
  {
    st.Printf(wxT("%d"), (int)abscissa.negativeLimit);
    numLeftChars = (int)st.length() + 1 + abscissa.postDecimalPositions;  // + Komma + Nachkommastellen
    st.Printf(wxT("%d"), (int)abscissa.positiveLimit);
    numRightChars = (int)st.length() + 1 + abscissa.postDecimalPositions;
  }
  else
  {
    st.Printf(wxT("%d"), (int)(abscissa.reference + abscissa.negativeLimit));
    numLeftChars = (int)st.length() + 1 + abscissa.postDecimalPositions;
    st.Printf(wxT("%d"), (int)(abscissa.reference + abscissa.positiveLimit));
    numRightChars = (int)st.length() + 1 + abscissa.postDecimalPositions;
  }
  
  if (numLeftChars > numRightChars) 
  { 
    i = numLeftChars; 
  } 
  else 
  { 
    i = numRightChars; 
  }
  inscriptionLengthInPixels = (int)(1.5*i*charWidth);

  // ****************************************************************
  // Calc. the size of one division to display.
  // ****************************************************************

  double division;

  if (inscriptionLengthInPixels < 1) { inscriptionLengthInPixels = 1; }
  if (abscissa.scaleDivision < EPSILON) { abscissa.scaleDivision = EPSILON; }

  int maxAllowedDivisions  = (int)((double)graphW / (double)inscriptionLengthInPixels);
  int numStandardDivisions = (int)((abscissa.positiveLimit - abscissa.negativeLimit) / abscissa.scaleDivision);
  if (maxAllowedDivisions < 1) { maxAllowedDivisions = 1; }

  if (numStandardDivisions > maxAllowedDivisions)
  {
    int minFactor = (numStandardDivisions / maxAllowedDivisions) + 1;
    int factor = minFactor;
      
    if (2 >= minFactor) { factor = 2; } else
    if (5 >= minFactor) { factor = 5; } else
    if (10 >= minFactor) { factor = 10; } else
    if (20 >= minFactor) { factor = 20; } else
    if (50 >= minFactor) { factor = 50; } else
    if (100 >= minFactor) { factor = 100; } else 
    if (200 >= minFactor) { factor = 200; } else 
    if (500 >= minFactor) { factor = 500; } else 
    if (1000 >= minFactor) { factor = 1000; }

    division = abscissa.scaleDivision*factor;
  }
  else
  {
    division = abscissa.scaleDivision;
  }

  // ****************************************************************
  // Calc. the index of the first and the last division.
  // ****************************************************************

  int firstDivisionIndex;
  int lastDivisionIndex;

  if (abscissa.useRelativeInscription)
  {
    firstDivisionIndex = (int)(abscissa.negativeLimit / division);
    lastDivisionIndex  = (int)(abscissa.positiveLimit / division) + 1;
  }
  else
  {
    firstDivisionIndex = (int)((abscissa.reference + abscissa.negativeLimit) / division);
    lastDivisionIndex  = (int)((abscissa.reference + abscissa.positiveLimit) / division) + 1;
  }
    
  if (firstDivisionIndex > lastDivisionIndex) { firstDivisionIndex = lastDivisionIndex; }

  // ****************************************************************
  // Paint the divisions.
  // ****************************************************************

  int stringWidth;
 
  for (i=firstDivisionIndex; i <= lastDivisionIndex; i++)
  {
    if (abscissa.useRelativeInscription)
    { 
      xPos = getXPos((double)i*division + abscissa.reference); 
    }
    else
    {
      xPos = getXPos((double)i*division);
    }

    // einen kurzen Skalenstrich zeichnen *****************

    if ((xPos >= graphX) && (xPos < graphX + graphW))
    {
      dc.SetPen(*wxBLACK_PEN);

      if (abscissaAtBottom)
      { 
        dc.DrawLine(xPos, graphY + graphH, xPos, graphY + graphH + 4); 
      }
      else
      { 
        dc.DrawLine(xPos, graphY - 5, xPos, graphY - 1); 
      }

      if (abscissa.showGrayLines) 
      { 
        dc.SetPen( wxPen(wxColor(220, 220, 220)) );
        dc.DrawLine(xPos, graphY, xPos, graphY + graphH - 1); 
      }

      // noch Text hinschreiben ? *************************

      if (xPos < graphX + graphW - unitLengthInPixels)
      {
        d = (double)i*division*unitConversionFactor;
        if (abscissa.postDecimalPositions <  1) { st.Printf(wxT("%2.0f"), d); } else
          if (abscissa.postDecimalPositions == 1) { st.Printf(wxT("%2.1f"), d); } else
            if (abscissa.postDecimalPositions == 2) { st.Printf(wxT("%2.2f"), d); } else
              if (abscissa.postDecimalPositions == 3) { st.Printf(wxT("%2.3f"), d); } else
                if (abscissa.postDecimalPositions >  3) { st.Printf(wxT("%2.4f"), d); }

        stringWidth = (int)st.length()*charWidth;
        xPos-= stringWidth / 2;
        if (xPos < graphX) { xPos = graphX; }

        if (xPos+stringWidth/2 < graphX+graphW-1-unitLengthInPixels-8) 
        { 
          if (abscissaAtBottom) 
          { 
            yPos = graphY+graphH+8; 
          } 
          else 
          { 
            yPos = graphY-8-charHeight; 
          }

          dc.SetPen(*wxBLACK_PEN);
          dc.DrawText(st, xPos, yPos);
        }

      }
    }
  }

}


// ****************************************************************************
/// \brief Draw the ordinate scale.
///
/// Draw the ordinate to the specified device context.
/// \param dc device context for painting (see wxWidgets 2.8 documentation)
// ****************************************************************************

void Graph::paintOrdinate(wxDC &dc)
{
  int i;
  int yPos;
  double d;
  int firstDivisionIndex;
  int lastDivisionIndex;
  wxString st;
  int charWidth = dc.GetCharWidth();
  int charHeight = dc.GetCharHeight();
  int w, h;

  int graphX, graphY, graphW, graphH;
  getDimensions(graphX, graphY, graphW, graphH);

  // ****************************************************************

  dc.SetPen(*wxBLACK_PEN);
  if (ordinateAtLeftSide)
  {
    dc.DrawLine(graphX-1, graphY, graphX-1, graphY+graphH-1);
  }
  else
  {
    dc.DrawLine(graphX+graphW, graphY, graphX+graphW, graphY+graphH-1);
  }

  // ****************************************************************
  // It's a linear scale.
  // ****************************************************************

  if (isLinearOrdinate)
  {
    // Consider the conversion mks->cgs

    double unitConversionFactor = 1.0;
    if (linearOrdinate.useCgsUnit) 
    { 
      unitConversionFactor = physicalQuantity[linearOrdinate.quantity].mksToCgsFactor; 
    }

    // **************************************************************
    // Output the physical unit.
    // **************************************************************
    
    if (linearOrdinate.useCgsUnit)
    { 
      st = wxString(physicalQuantity[linearOrdinate.quantity].cgsUnit, wxConvUTF8); 
    }
    else
    { 
      st = wxString(physicalQuantity[linearOrdinate.quantity].mksUnit, wxConvUTF8); 
    }

    dc.SetPen(*wxBLACK_PEN);

    if (ordinateAtLeftSide)
    { 
      dc.GetTextExtent(st, &w, &h);
      dc.DrawText(st, graphX - 8 - w, graphY); 
    }
    else
    { 
      dc.DrawText(st, graphX + graphW + 8, graphY); 
    }

    // **************************************************************
    // Determine the size of a visible scale part (division).
    // **************************************************************

    double division;

    if (linearOrdinate.scaleDivision < EPSILON) { linearOrdinate.scaleDivision = EPSILON; }
    if (charHeight < 1) { charHeight = 1; }

    int maxAllowedDivisions  = (int)((double)graphH / (1.5*(double)charHeight));
    int numStandardDivisions = (int)((linearOrdinate.positiveLimit - linearOrdinate.negativeLimit) / linearOrdinate.scaleDivision);
    if (maxAllowedDivisions < 1) { maxAllowedDivisions = 1; }

    if (numStandardDivisions > maxAllowedDivisions)
    {
      int minFactor = (numStandardDivisions / maxAllowedDivisions) + 1;
      int factor = minFactor;
      
      if (2 >= minFactor) { factor = 2; } else
      if (5 >= minFactor) { factor = 5; } else
      if (10 >= minFactor) { factor = 10; } else
      if (20 >= minFactor) { factor = 20; } else
      if (50 >= minFactor) { factor = 50; } else
      if (100 >= minFactor) { factor = 100; } else 
      if (200 >= minFactor) { factor = 200; } else 
      if (500 >= minFactor) { factor = 500; } else 
      if (1000 >= minFactor) { factor = 1000; }

      division = linearOrdinate.scaleDivision*factor;
    }
    else
    {
      division = linearOrdinate.scaleDivision;
    }

    // **************************************************************
    // Index of the first and the last scale part.
    // **************************************************************

    if (linearOrdinate.useRelativeInscription)
    {
      firstDivisionIndex = (int)(linearOrdinate.negativeLimit / division);
      lastDivisionIndex  = (int)(linearOrdinate.positiveLimit / division) + 1;
    }
    else
    {
      firstDivisionIndex = (int)((linearOrdinate.reference + linearOrdinate.negativeLimit) / division);
      lastDivisionIndex  = (int)((linearOrdinate.reference + linearOrdinate.positiveLimit) / division) + 1;
    }
    if (firstDivisionIndex > lastDivisionIndex) { firstDivisionIndex = lastDivisionIndex; }

    // **************************************************************
    // Inscription for the scale parts.
    // **************************************************************

    for (i=firstDivisionIndex; i <= lastDivisionIndex; i++)
    {
      if (linearOrdinate.useRelativeInscription)
      {
        yPos = getYPos((double)i*division + linearOrdinate.reference);
      }
      else
      {
        yPos = getYPos((double)i*division);
      }

      // Draw a short scaling line **********************************

      if ((yPos >= graphY) && (yPos < graphY + graphH))
      {
        dc.SetPen(*wxBLACK_PEN);
        if (ordinateAtLeftSide)
        { 
          dc.DrawLine(graphX-5, yPos, graphX-1, yPos); 
        }
        else
        { 
          dc.DrawLine(graphX+graphW, yPos, graphX+graphW+4, yPos); 
        }
            
        if (linearOrdinate.showGrayLines) 
        { 
          dc.SetPen( wxPen(wxColor(220, 220, 220)) );
          dc.DrawLine(graphX, yPos, graphX+graphW-1, yPos); 
        }

        // Write down text ? ****************************************

        if (yPos >= graphY + charHeight)
        {
          d = (double)i*division*unitConversionFactor;

          if (linearOrdinate.postDecimalPositions <  1) { st.Printf(wxT("%2.0f"), d); } else
            if (linearOrdinate.postDecimalPositions == 1) { st.Printf(wxT("%2.1f"), d); } else
              if (linearOrdinate.postDecimalPositions == 2) { st.Printf(wxT("%2.2f"), d); } else
                if (linearOrdinate.postDecimalPositions == 3) { st.Printf(wxT("%2.3f"), d); } else
                  if (linearOrdinate.postDecimalPositions >  3) { st.Printf(wxT("%2.4f"), d); }

          yPos-= charHeight/2;
          if (yPos < graphY + charHeight) { yPos = graphY + charHeight; }
          if (yPos + charHeight > graphY + graphH - 1) { yPos = graphY + graphH - 1 - charHeight; }

          dc.SetPen(*wxBLACK_PEN);

          if (ordinateAtLeftSide)
          { 
            dc.GetTextExtent(st, &w, &h);
            dc.DrawText(st, graphX-8 - w, yPos); 
          }
          else
          { 
            dc.DrawText(st, graphX + graphW + 8, yPos); 
          }
        }
      }
    }

  }
  else

  // ****************************************************************
  // It's a logarithmic scale.
  // ****************************************************************

  {
    // **************************************************************
    // Output the physical quantity on top.
    // **************************************************************
    
    dc.SetPen(*wxBLACK_PEN);

    if (ordinateAtLeftSide)
    { 
      dc.GetTextExtent(wxT("dB"), &w, &h);
      dc.DrawText(wxT("dB"), graphX - 8 - w, graphY); 
    }
    else
    { 
      dc.DrawText(wxT("dB"), graphX + graphW + 8, graphY); 
    }

    // **************************************************************
    // Determine the size of a visible scale part (division).
    // **************************************************************

    const double standardLogDivision = logOrdinate.scaleDivision; // dB
    double division;

    if (charHeight < 1) { charHeight = 1; }

    int maxAllowedDivisions  = (int)((double)graphH / (1.5*(double)charHeight));
    int numStandardDivisions = (int)((logOrdinate.upperLevel - logOrdinate.lowerLevel) / standardLogDivision);
    if (maxAllowedDivisions < 1) { maxAllowedDivisions = 1; }

    if (numStandardDivisions > maxAllowedDivisions)
    {
      int minFactor = (numStandardDivisions / maxAllowedDivisions) + 1;
      int factor = minFactor;
      
      if (2 >= minFactor) { factor = 2; } else
      if (5 >= minFactor) { factor = 5; } else
      if (10 >= minFactor) { factor = 10; } else
      if (20 >= minFactor) { factor = 20; } else
      if (50 >= minFactor) { factor = 50; } else
      if (100 >= minFactor) { factor = 100; } else 
      if (200 >= minFactor) { factor = 200; } else 
      if (500 >= minFactor) { factor = 500; } else 
      if (1000 >= minFactor) { factor = 1000; }

      division = standardLogDivision*factor;
    }
    else
    {
      division = standardLogDivision;
    }

    // Index des ersten und letzten Skalenteils bestimmen ***********

    firstDivisionIndex = (int)(logOrdinate.lowerLevel / division);
    lastDivisionIndex  = (int)(logOrdinate.upperLevel / division) + 1;
    if (firstDivisionIndex > lastDivisionIndex) { firstDivisionIndex = lastDivisionIndex; }

    // Beschriftung der Skalenteile *********************************

    d = logOrdinate.upperLevel - logOrdinate.lowerLevel;
    if (d < EPSILON) { d = EPSILON; }
    double vertPixelsPerDB = (double)graphH / d;

    for (i=firstDivisionIndex; i <= lastDivisionIndex; i++)
    {
      yPos = graphY + graphH - 1 - (int)(((double)i*division - logOrdinate.lowerLevel)*vertPixelsPerDB);

      // einen kurzen Skalenstrich zeichnen

      if ((yPos >= graphY) && (yPos < graphY + graphH))
      {
        dc.SetPen(*wxBLACK_PEN);

        if (ordinateAtLeftSide)
        { 
          dc.DrawLine(graphX-5, yPos, graphX-1, yPos); 
        }
        else
        { 
          dc.DrawLine(graphX+graphW, yPos, graphX+graphW+4, yPos); 
        }

        if (logOrdinate.showGrayLines) 
        {
          dc.SetPen( wxPen(wxColor(220, 220, 220)) );
          dc.DrawLine(graphX, yPos, graphX+graphW-1, yPos);
        }

        // Output some text? ****************************************

        if (yPos >= graphY + charHeight)
        {
          d = (double)i*division;
          st.Printf(wxT("%d"), (int)d);

          yPos-= charHeight/2;
          if (yPos < graphY + charHeight) { yPos = graphY + charHeight; }
          if (yPos + charHeight > graphY + graphH - 1) { yPos = graphY + graphH - 1 - charHeight; }
          
          if (ordinateAtLeftSide)
          { 
            dc.GetTextExtent(st, &w, &h);
            dc.DrawText(st, graphX - 8 - w, yPos); 
          }
          else
          { 
            dc.DrawText(st, graphX + graphW + 8, yPos); 
          }
        }
      }
    }     // Durchlaufen der Skalenteile
  }     // log. Skala

}

// ****************************************************************************
/// \brief Zoom into the abscissa.
///
/// Zooms into the abscissa by changing the current negative and/or positive 
/// limit by a zoom factor.
/// \param negativeLimit negative limit shall be affected (or not)
/// \param positiveLimit positive limit shall be affected (or not)
// ****************************************************************************
 
void Graph::zoomInAbscissa(bool negativeLimit, bool positiveLimit)
{
  double positiveFactor, negativeFactor;
  getZoomFactors(&abscissa, positiveFactor, negativeFactor);

  // positive and negative limit shall both be changed *************

  if ((positiveLimit) && (negativeLimit))
  {
    if ((abscissa.negativeLimit/negativeFactor <= abscissa.negativeLimitMax) &&
        (abscissa.positiveLimit/positiveFactor >= abscissa.positiveLimitMin))
    {
      abscissa.negativeLimit/= negativeFactor;
      abscissa.positiveLimit/= positiveFactor;
    }
  }
  else

  // only the negative limit shall be changed **********************************

  if (negativeLimit)
  {
    if (abscissa.negativeLimit/negativeFactor <= abscissa.negativeLimitMax)
    {
      abscissa.negativeLimit/= negativeFactor;
    }
  }
  else

  // only the positive limit shall be changed **********************************

  if (positiveLimit)
  {
    if (abscissa.positiveLimit/positiveFactor >= abscissa.positiveLimitMin)
    {
      abscissa.positiveLimit/= positiveFactor;
    }
  }
}

// ****************************************************************************
/// \brief Zoom out of the abscissa.
///
/// Zooms out of the abscissa by changing the current negative and/or positive 
/// limit by a zoom factor.
/// \param negativeLimit negative limit shall be affected (or not)
/// \param positiveLimit positive limit shall be affected (or not)
// ****************************************************************************

void Graph::zoomOutAbscissa(bool negativeLimit, bool positiveLimit)
{
  double positiveFactor, negativeFactor;
  getZoomFactors(&abscissa, positiveFactor, negativeFactor);

	// positive and negative limit shall both be changed *************

  if ((positiveLimit) && (negativeLimit))
  {
    if ((abscissa.negativeLimit*negativeFactor >= abscissa.negativeLimitMin) &&
        (abscissa.positiveLimit*positiveFactor <= abscissa.positiveLimitMax))
    {
      abscissa.negativeLimit*= negativeFactor;
      abscissa.positiveLimit*= positiveFactor;
    }
  }
  else

		// only the negative limit shall be changed **********************************

  if (negativeLimit)
  {
    if (abscissa.negativeLimit*negativeFactor >= abscissa.negativeLimitMin)
    {
      abscissa.negativeLimit*= negativeFactor;
    }
  }
  else

		// only the positive limit shall be changed **********************************

  if (positiveLimit)
  {
    if (abscissa.positiveLimit*positiveFactor <= abscissa.positiveLimitMax)
    {
      abscissa.positiveLimit*= positiveFactor;
    }
  }
}

// ****************************************************************************
/// \brief Zoom into the ordinate.
///
/// Zooms into the ordinate by changing the current negative and/or positive 
/// limit by a zoom factor.
/// \param negativeLimit negative limit shall be affected (or not)
/// \param positiveLimit positive limit shall be affected (or not)
// ****************************************************************************

void Graph::zoomInOrdinate(bool negativeLimit, bool positiveLimit)
{
  double positiveFactor, negativeFactor;

  if (isLinearOrdinate)
  {
    getZoomFactors(&linearOrdinate, positiveFactor, negativeFactor);

    // change positive and negative limit ***********

    if ((negativeLimit) && (positiveLimit))
    {
      if ((linearOrdinate.negativeLimit/negativeFactor <= linearOrdinate.negativeLimitMax) &&
          (linearOrdinate.positiveLimit/positiveFactor >= linearOrdinate.positiveLimitMin))
      {
        linearOrdinate.negativeLimit/= negativeFactor; 
        linearOrdinate.positiveLimit/= positiveFactor;
      }
    }
    else

    // change only negative limit ********************************

    if (negativeLimit) 
    {
      if (linearOrdinate.negativeLimit/negativeFactor <= linearOrdinate.negativeLimitMax)
      { 
        linearOrdinate.negativeLimit/= negativeFactor; 
      }
    }
    else

    // change only positive limit ********************************

    if (positiveLimit)
    {
      if (linearOrdinate.positiveLimit/positiveFactor >= linearOrdinate.positiveLimitMin)
      {
        linearOrdinate.positiveLimit/= positiveFactor;
      }
    }
  }
  else
  {
    if ((negativeLimit) && (logOrdinate.lowerLevel+logOrdinate.zoomStep <= logOrdinate.lowerLevelMax))
    {
      logOrdinate.lowerLevel+= logOrdinate.zoomStep;
    }

    if ((positiveLimit) && (logOrdinate.upperLevel-logOrdinate.zoomStep >= logOrdinate.upperLevelMin))
    {
      logOrdinate.upperLevel-= logOrdinate.zoomStep;
    }
  }
}

// ****************************************************************************
/// \brief Zoom out of the ordinate.
///
/// Zooms out of the ordinate by changing the current negative and/or positive 
/// limit by a zoom factor.
/// \param negativeLimit negative limit shall be affected (or not)
/// \param positiveLimit positive limit shall be affected (or not)
// ****************************************************************************

void Graph::zoomOutOrdinate(bool negativeLimit, bool positiveLimit)
{
  double positiveFactor, negativeFactor;

  if (isLinearOrdinate)
  {
    getZoomFactors(&linearOrdinate, positiveFactor, negativeFactor);

    // change positive and negative limit ***********

    if ((negativeLimit) && (positiveLimit))
    {
      if ((linearOrdinate.negativeLimit*negativeFactor >= linearOrdinate.negativeLimitMin) &&
          (linearOrdinate.positiveLimit*positiveFactor <= linearOrdinate.positiveLimitMax))
      {
        linearOrdinate.negativeLimit*= negativeFactor;
        linearOrdinate.positiveLimit*= positiveFactor;
      }
    }
    else

    // change only negative limit ********************************

    if (negativeLimit)
    {
      if (linearOrdinate.negativeLimit*negativeFactor >= linearOrdinate.negativeLimitMin)
      {
        linearOrdinate.negativeLimit*= negativeFactor;
      }
    }
    else

    // change only positive limit ********************************

    if (positiveLimit)
    {
      if (linearOrdinate.positiveLimit*positiveFactor <= linearOrdinate.positiveLimitMax)
      {
        linearOrdinate.positiveLimit*= positiveFactor;
      }
    }
  }
  else
  {
    if ((negativeLimit) && (logOrdinate.lowerLevel-logOrdinate.zoomStep >= logOrdinate.lowerLevelMin))
    {
      logOrdinate.lowerLevel-= logOrdinate.zoomStep;
    }

    if ((positiveLimit) && (logOrdinate.upperLevel+logOrdinate.zoomStep <= logOrdinate.upperLevelMax))
    {
      logOrdinate.upperLevel+= logOrdinate.zoomStep;
    }
  }
}

// ****************************************************************************
/// \brief Return the x-position in pixels for the given physical quantity.
///
/// Returns the x-position in the graph that corresponds with the specified
/// physical quantity.
/// \param absXValue value of a physical quantity
/// \return x-position in graph corresponding to value
// ****************************************************************************

int Graph::getXPos(double absXValue)
{
  int graphX, graphY, graphW, graphH;
  int xPos;

  getDimensions(graphX, graphY, graphW, graphH);

  double d = abscissa.positiveLimit - abscissa.negativeLimit;
  if (d < EPSILON) { d = EPSILON; }
  xPos = graphX + (int)(((double)graphW*(absXValue - abscissa.reference - abscissa.negativeLimit)) / d);

  return xPos;
}

// ****************************************************************************
/// \brief Return the y-position in pixels for the given physical quantity.
///
/// Returns the y-position in the graph that corresponds with the specified
/// physical quantity.
/// \param absYValue value of a physical quantity
/// \return y-position in graph corresponding to value
// ****************************************************************************

int Graph::getYPos(double absYValue)
{
  int graphX, graphY, graphW, graphH;
  int yPos;
  double d, ref;

  getDimensions(graphX, graphY, graphW, graphH);

  if (isLinearOrdinate)
  {
    d = linearOrdinate.positiveLimit - linearOrdinate.negativeLimit;
    if (d < EPSILON) { d = EPSILON; }
    yPos = graphY + graphH - 1 - (int)(((double)graphH*(absYValue - linearOrdinate.reference - linearOrdinate.negativeLimit)) / d);
  }
  else
  {
    d = logOrdinate.upperLevel - logOrdinate.lowerLevel;
    if (d < EPSILON) { d = EPSILON; }
    ref = logOrdinate.reference;
    if (ref < EPSILON) { ref = EPSILON; }
    double dBValue = 20.0*log10(absYValue/ref);
    
	yPos = graphY + graphH - 1 - (int)(((dBValue - logOrdinate.lowerLevel)*(double)graphH) / d);
  }

  return yPos;
}

// ****************************************************************************
/// \brief Return the physical quantity at a specified x-position in pixels.
///
/// Returns value of a physical quantity in the graph that corresponds 
/// to the specified x-coordinate.
/// \param xPos x-position in graph
/// \return value of a physical quantity at specified position
// ****************************************************************************

double Graph::getAbsXValue(int xPos)
{
  int graphX, graphY, graphW, graphH;
  double absXValue;

  getDimensions(graphX, graphY, graphW, graphH);

  double d = graphW;
  if (d < EPSILON) { d = EPSILON; }
  absXValue = abscissa.reference + abscissa.negativeLimit + 
    ((double)(xPos - graphX)*(abscissa.positiveLimit - abscissa.negativeLimit)) / d;

  return absXValue;
}

// ****************************************************************************
/// \brief Return the physical quantity at a specified y-position in pixels.
///
/// Returns value of a physical quantity in the graph that corresponds 
/// to the specified y-coordinate.
/// \param yPos y-position in graph
/// \return value of a physical quantity at specified position
// ****************************************************************************

double Graph::getAbsYValue(int yPos)
{
  int graphX, graphY, graphW, graphH;
  double absYValue;
  double d;

  getDimensions(graphX, graphY, graphW, graphH);

  if (isLinearOrdinate)
  {
    d = graphH;
    if (d < EPSILON) { d = EPSILON; }
    absYValue = linearOrdinate.reference + linearOrdinate.negativeLimit + 
      ((double)(graphY + graphH - 1 - yPos)*(linearOrdinate.positiveLimit - linearOrdinate.negativeLimit)) / d;
  }
  else
  {
    const double ln10 = log(10.0);
    d = graphH;
    if (d < EPSILON) { d = EPSILON; }

    double dBLevel = logOrdinate.lowerLevel + 
      ((double)(graphY + graphH - 1 - yPos)*(logOrdinate.upperLevel - logOrdinate.lowerLevel)) / d;

    absYValue = logOrdinate.reference*exp((ln10*dBLevel) / 20.0);
  }

  return absYValue;
}

// ****************************************************************************
/// \brief Calculate the zoom factors.
/// 
/// Calculates the factors by which the negative and/or positive limits of the
/// axes are divided or multiplied when zooming.
/// \param domain the current domain (can be abscissa or ordinate)
/// \param [out] positiveZoomFactor zoom factor for positive limit
/// \param [out] negativeZoomfactor zoom factor for negative limit
// ****************************************************************************

void Graph::getZoomFactors(LinearDomain *domain, double& positiveZoomFactor, double& negativeZoomFactor)
{
  if (domain == NULL) 
  { 
    return; 
  }

  if (domain->negativeLimitMax < 0.0)
  {
    negativeZoomFactor = pow(domain->negativeLimitMin/domain->negativeLimitMax, 1.0 / (double)domain->numZoomSteps);
  }
  else
  {
    negativeZoomFactor = 1.0;
  }

  if (domain->positiveLimitMin > 0.0)
  {
    positiveZoomFactor = pow(domain->positiveLimitMax/domain->positiveLimitMin, 1.0 / (double)domain->numZoomSteps);
  }
  else
  {
    positiveZoomFactor = 1.0;
  }
}

// ****************************************************************************
