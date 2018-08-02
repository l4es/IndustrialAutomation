#include "mpWindow.h"
#include "liboapc.h"

mpWindow::mpWindow(wxWindow *parent,wxSize size,struct instData *data)
         :wxMediaCtrl(parent,wxID_ANY,wxEmptyString,wxPoint(0,0),size)
{
   wxLog::EnableLogging(true);
   Connect(GetId(),wxEVT_MEDIA_LOADED,wxMediaEventHandler(mpWindow::OnMediaLoaded));
   Connect(GetId(),wxEVT_MEDIA_STOP,wxMediaEventHandler(mpWindow::OnMediaPlaying));
   Connect(GetId(),wxEVT_MEDIA_PAUSE,wxMediaEventHandler(mpWindow::OnMediaPlaying));
   Connect(GetId(),wxEVT_MEDIA_FINISHED,wxMediaEventHandler(mpWindow::OnMediaPlaying));
   Connect(GetId(),wxEVT_MEDIA_PLAY,wxMediaEventHandler(mpWindow::OnMediaPlaying));
   m_data=data;
}


void mpWindow::load(struct instData *data, char *path)
{
   wxString strPath;

   oapc_unicode_charToStringASCII(path,strlen(path),&strPath);
   if (Load(strPath))
   {
      SetVolume(data->config.m_mVolume/100000.0);
      SetPlaybackRate(data->config.m_mSpeed/1000.0);
   }
}


void mpWindow::OnMediaPlaying(wxMediaEvent& /*evt*/)
{
   if (GetState()==wxMEDIASTATE_PLAYING) m_data->m_digi1=1;
   else m_data->m_digi1=0;
   m_oapc_io_callback(OAPC_DIGI_IO1,m_data->m_callbackID);
}


void mpWindow::OnMediaLoaded(wxMediaEvent& /*evt*/)
{
   m_oapc_io_callback(OAPC_DIGI_IO0,m_data->m_callbackID);
}
