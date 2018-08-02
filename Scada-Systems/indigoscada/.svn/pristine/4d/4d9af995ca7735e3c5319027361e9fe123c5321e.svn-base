/******************  Universal EventLogging utility  ****************
 **                     SSP errors translation                     **
 *                                                                  *
 *       Copyright (c) 1996  by Timofei Bondarenko, Kostya Volovich *
 ********************************************************************/

#include <windows.h>
#define SECURITY_WIN32
#include <security.h> /*issperr.h*/
#if !defined (SEC_E_BUFFER_TOO_SMALL) /*&& defined(__MINGW32__)*/
#include <issperr.h>
#endif
#include "unilog.h"

#define MSG(id,text) { id, text " (" #id ")" }
typedef struct
       {
        long id;
        const char *text;
       } msgdesc;

static const msgdesc ssp_errors[] = {
 MSG(SEC_E_OK, "Ok"),
 MSG(SEC_E_INSUFFICIENT_MEMORY, "Not enough memory is available "
                                "to complete this request"),
 MSG(SEC_E_INVALID_HANDLE,      "The handle specified is invalid"),
 MSG(SEC_E_UNSUPPORTED_FUNCTION, "The function requested is not supported"),
 MSG(SEC_E_TARGET_UNKNOWN,      "The specified target is unknown or "
                                "unreachable"),
 MSG(SEC_E_INTERNAL_ERROR,      "The Local Security Authority cannot be "
                                "contacted"),
 MSG(SEC_E_SECPKG_NOT_FOUND,    "The requested security package does not exist"),
 MSG(SEC_E_NOT_OWNER,   "The caller is not the owner of the desired credentials"),
 MSG(SEC_E_CANNOT_INSTALL,      "The security package failed to initialize, "
                                "and cannot be installed"),
 MSG(SEC_E_INVALID_TOKEN, "The token supplied to the function is invalid"),
 MSG(SEC_E_CANNOT_PACK, "The security package is not able to marshall the "
                        "logon buffer, so the logon attempt has failed"),
 MSG(SEC_E_QOP_NOT_SUPPORTED, "The per-message Quality of Protection is "
                        "not supported by the security package"),
 MSG(SEC_E_NO_IMPERSONATION, "The security context does not allow "
                        "impersonation of the client"),
 MSG(SEC_E_LOGON_DENIED,        "The logon attempt failed"),
 MSG(SEC_E_UNKNOWN_CREDENTIALS, "The credentials supplied to the package "
                                "were not recognized"),
 MSG(SEC_E_NO_CREDENTIALS,  "No credentials are available in the security "
                            "package"),
 MSG(SEC_E_MESSAGE_ALTERED, "The message supplied for verification has "
                            "been altered"),
 MSG(SEC_E_OUT_OF_SEQUENCE, "The message supplied for verification is out "
                            "of sequence"),
 MSG(SEC_E_NO_AUTHENTICATING_AUTHORITY, "No authority could be contacted for "
                            "authentication."),
 MSG(SEC_I_CONTINUE_NEEDED, "The function completed successfully, "
                    "but must be called again to complete the context"),
 MSG(SEC_I_COMPLETE_NEEDED, "The function completed successfully, "
                    "but CompleteToken must be called"),
 MSG(SEC_I_COMPLETE_AND_CONTINUE, "The function completed successfully, "
 "but both CompleteToken and this function must be called to complete the context"),
 MSG(SEC_I_LOCAL_LOGON, "The logon was completed, but no network authority "
 "was available. The logon was made using locally known information available."),
 MSG(SEC_E_BAD_PKGID, "The requested security package does not exist"),
 MSG(SEC_E_CONTEXT_EXPIRED, "The context has expired and can no longer "
                            "be used."),
 MSG(SEC_E_INCOMPLETE_MESSAGE, "The supplied message is incomplete. "
                                "The signature was not verified."),
 MSG(SEC_E_INCOMPLETE_CREDENTIALS, "The credentials supplied were not"
 " complete, and could not be verified. The context could not be initialized."),
 MSG(SEC_E_BUFFER_TOO_SMALL, "The buffers supplied to a function was too small."),
 MSG(SEC_I_INCOMPLETE_CREDENTIALS, "The credentials supplied were not complete,"
            " and could not be verified."),
 MSG(SEC_I_RENEGOTIATE, "The context data must be renegotiated with the peer."),
 MSG(SEC_E_WRONG_PRINCIPAL, "The context data must be renegotiated with"
                            " the peer.")
};

const char *ssperror(int err)
{
 int xx;
 for(xx = 0; xx < SIZEOF_ARRAY(ssp_errors); xx++)
   if (ssp_errors[xx].id == err) return ssp_errors[xx].text;
 return "<Unknown SSP error>";
}
