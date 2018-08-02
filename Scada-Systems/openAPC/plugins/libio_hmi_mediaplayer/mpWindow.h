#ifndef MPWINDOW_H
#define MPWINDOW_H

#include <wx/wx.h>
#include <wx/mediactrl.h>

#include "oapc_libio.h"
#include "libio_hmi_mediaplayer.h"

extern lib_oapc_io_callback m_oapc_io_callback;

class mpWindow : public wxMediaCtrl
{
public:
   mpWindow(wxWindow *parent,wxSize size,struct instData *data);

   void load(struct instData *data,char *path);

private:
   void OnMediaPlaying(wxMediaEvent& evt);
   void OnMediaLoaded(wxMediaEvent& evt);

   struct instData *m_data;
};

#endif //MPWINDOW_H

