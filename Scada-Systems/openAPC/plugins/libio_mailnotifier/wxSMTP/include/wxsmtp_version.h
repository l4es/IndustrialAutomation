/****************************************************************************

 Project     : 
 Author      : 
 Description : 

 VERSION INFORMATION:
 File    : $Source$
 Version : $Revision$
 Date    : $Date$
 Author  : $Author$

 History: 
 $Log: wxsmtp_version.h,v $
 Revision 1.11  2004/06/29 11:06:20  tavasti
 - Added OnResponse handler also for initial state (sometimes OnConnect
   arrives after first data)
 - Minor changes in indentation & comments

 Revision 1.10  2004/06/18 15:29:47  tavasti
 Make does not try to check version if CVS/Root is not local file
 (update file containign version number by hand)

 Revision 1.9  2004/05/19 08:53:15  tavasti
 Removed last defaultparameter (bug fix for previous...)

 Revision 1.8  2004/05/19 04:18:27  tavasti
 Fixes based on comments from Edwards John-BLUW23 <jedwards@motorola.com>
 - Changed m_fileName.GetName to GetFullName

 Revision 1.7  2004/05/19 04:06:26  tavasti
 Fixes based on comments from Edwards John-BLUW23 <jedwards@motorola.com>
 - Removed -m486 flags from makefile
 - Added filetypes wav & mp3
 - Removed default arguments from wxmime.cpp (only in .h)
 - Commented out iostream.h includes

 Revision 1.6  2003/11/21 12:36:46  tavasti
 - Makefilet -Wall optioilla
 - Korjattu 'j‰rkev‰t' varoitukset pois (J‰‰nyt muutama joita ei saa
   kohtuudella poistettua)

 Revision 1.5  2003/11/14 15:46:50  tavasti
 Commented out some debug printing

 Revision 1.4  2003/11/14 15:43:09  tavasti
 Sending email with alternatives works

 Revision 1.3  2003/11/13 17:12:15  tavasti
 - Muutettu tiedostojen nimet wx-alkuisiksi

 Revision 1.2  2003/11/07 11:08:50  tavasti
 - Windows-versiossakin dependit kuntoon

 Revision 1.1  2003/11/07 09:17:40  tavasti
 - K‰‰ntyv‰ versio, fileheaderit lis‰tty.


****************************************************************************/

#ifndef WXSMTP_VERSION
#define WXSMTP_VERSION

#define WXSMTP_VERSION_STR "$Header: /v/CVS/olive/notifier/wxSMTP/include/wxsmtp_version.h,v 1.1.1.1 2004/08/27 23:12:14 paul Exp $"


#endif /* WXSMTP_VERSION */
/* 03-06-2004 13:33:48 */
/* 18-06-2004 18:35:05 */
/* 29-06-2004 14:07:32 */
