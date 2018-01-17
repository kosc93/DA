#ifndef __BASIC_PICTURE_H__
#define __BASIC_PICTURE_H__

#include <wx/wx.h>
#include <wx/dcbuffer.h>

/****************************************************************************//**
 * \brief This class is a base class for bitmap based pictures.
 * 
 * This class is used as a base class for all other pictures. It provides
 * the basic functionality of handling a paint event that must be extended by
 * any derived classes for the specific use case.
 ****************************************************************************/
class BasicPicture : public wxPanel
{
  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  BasicPicture(wxWindow *parent);
  virtual void draw(wxDC &dc);
  wxBitmap *getBitmap();

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  /// Background bitmap of the picture for double-buffered painting
  wxBitmap *bitmap;    

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  void OnPaint(wxPaintEvent &event);
  void OnEraseBackground(wxEraseEvent &event);

  // ****************************************************************************
  // Declare the event table right at the end
  // ****************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif
