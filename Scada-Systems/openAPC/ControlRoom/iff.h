#ifdef ENV_INT
#include "../iff.h"
#endif

#ifndef IFF_H
#define IFF_H

#define CHUNK_FORM            "FORM" // IFF

#define CHUNK_ANNO            "ANNO"
#define CHUNK_APCP            "APCP" // project file
#define CHUNK_APCX            "APCX" // compiled project file
#define CHUNK_APRT            "APRT" // runtime data file
#define CHUNK_GPRJ            "GPRJ" // group project data
#define CHUNK_HOBL            "HOBL" // hmi object list
#define CHUNK_NAME            "NAME"
#define CHUNK_PROJ            "PROJ" // project data
#define CHUNK_VERS            "VERS"
#define CHUNK_USPR            "USPR" // user privilege information
#define CHUNK_USDA            "USDA" // user data

#define CHUNK_HOBL_ADPA       "ADPA" // additional panel
#define CHUNK_HOBL_ANME       "ANME" // angular meter
#define CHUNK_HOBL_ANRE       "ANRE" // angular regulator
#define CHUNK_HOBL_CKBX       "CKBX" // checkbox
#define CHUNK_HOBL_ELIP       "ELIP" // ellipse
#define CHUNK_HOBL_EXHM       "EXHM" // external HMI plug in
#define CHUNK_HOBL_FLTF       "FLTF"
#define CHUNK_HOBL_FLIN       "FLIN" // Free Line
#define CHUNK_HOBL_FRAM       "FRAM" // rectangular frame
#define CHUNK_HOBL_HGAG       "HGAG"
#define CHUNK_HOBL_HSLD       "HSLD"
#define CHUNK_HOBL_IMAG       "IMAG"
#define CHUNK_HOBL_IMBU       "IMBU"
#define CHUNK_HOBL_LCDN       "LCDN" // LCD Number
#define CHUNK_HOBL_LINE       "LINE"
#define CHUNK_HOBL_MTXL       "MTXL" // mutex list
#define CHUNK_HOBL_NUMF       "NUMF"
#define CHUNK_HOBL_PWDF       "PWDF" // password input field
#define CHUNK_HOBL_RABU       "RABU"
#define CHUNK_HOBL_SIBU       "SIBU"
#define CHUNK_HOBL_SIPA       "SIPA"
#define CHUNK_HOBL_STPA       "STPA"
#define CHUNK_HOBL_TAPA       "TAPA"
#define CHUNK_HOBL_TELA       "TELA"
#define CHUNK_HOBL_TOBU       "TOBU"
#define CHUNK_HOBL_TOIB       "TOIB"
#define CHUNK_HOBL_TXTF       "TXTF"
#define CHUNK_HOBL_UMGM       "UMGM" // special panel for user management
#define CHUNK_HOBL_VGAG       "VGAG"
#define CHUNK_HOBL_VSLD       "VSLD"
#define CHUNK_FLOW            "FLOW" // flow list
#define CHUNK_FLOW_BCTR       "BCTR" // binary element counter
#define CHUNK_FLOW_BGTE       "BGTE" // binary gate
#define CHUNK_FLOW_BTGT       "BTGT" // binary triggered gate
#define CHUNK_FLOW_CCMP       "CCMP" // compare characters
#define CHUNK_FLOW_CCTR       "CCTR" // character element counter
#define CHUNK_FLOW_CGTE       "CGTE" // character gate
#define CHUNK_FLOW_CTGT       "CTGT" // character triggered gate
#define CHUNK_FLOW_CVCD       "CVCD" // converter char2digi
#define CHUNK_FLOW_CVCN       "CVCN" // converter char2num
#define CHUNK_FLOW_CVDC       "CVDC" // converter digi2char
#define CHUNK_FLOW_CVDN       "CVDN" // converter digi2num
#define CHUNK_FLOW_CVMC       "CVMC" // converter mixed2char
#define CHUNK_FLOW_CVNB       "CVNB" // converter num2bits
#define CHUNK_FLOW_CVNC       "CVNC" // converter num2char
#define CHUNK_FLOW_CVND       "CVND" // converter num2digi
#define CHUNK_FLOW_CVBP       "CVBP" // converter bin2pair
#define CHUNK_FLOW_CVCP       "CVCP" // converter char2pair
#define CHUNK_FLOW_CVDP       "CVDP" // converter digi2pair
#define CHUNK_FLOW_CVNP       "CVNP" // converter num2pair
#define CHUNK_FLOW_CVPB       "CVPB" // converter pair2bin
#define CHUNK_FLOW_CVPC       "CVPC" // converter pair2char
#define CHUNK_FLOW_CVPD       "CVPD" // converter pair2digi
#define CHUNK_FLOW_CVPN       "CVPN" // converter pair2num
#define CHUNK_FLOW_DCMP       "DCMP" // compare digital
#define CHUNK_FLOW_DCTR       "DCTR" // digital element counter
#define CHUNK_FLOW_DGTE       "DGTE" // digital gate
#define CHUNK_FLOW_DTGT       "DTGT" // digital triggered gate
#define CHUNK_FLOW_EXIO       "EXIO" // flow of an external IO plugin
#define CHUNK_FLOW_FDLY       "FDLY" // flow delay
#define CHUNK_FLOW_FGRP       "FGRP" // flow of a flow group
#define CHUNK_FLOW_FGRP_GRPD   "GRPD" // group definitions
#define CHUNK_FLOW_FGRP_GRPI   "GRPI" // group inputs
#define CHUNK_FLOW_FGRP_GRPO   "GRPO" // group outputs
#define CHUNK_FLOW_FSTA       "FSTA" // flow start / initial program flow begin
#define CHUNK_FLOW_FSTP       "FSTP" // flow stop / application exit
#define CHUNK_FLOW_FTMR       "FTMR" // flow started by timer
#define CHUNK_MISC_ISCO       "ISCO" // interlock server connection
#define CHUNK_FLOW_LBSF       "LBSF" // logic binary shift register
#define CHUNK_FLOW_LCSF       "LCSF" // logic char shift register
#define CHUNK_FLOW_LDSF       "LDSF" // logic digital shift register
#define CHUNK_FLOW_LGBP       "LGBP" // logic binary NOP
#define CHUNK_FLOW_LGCP       "LGCP" // logic characer NOP
#define CHUNK_FLOW_LGDA       "LGDA" // logic digital AND
#define CHUNK_FLOW_LGDN       "LGDN" // logic digital NOT
#define CHUNK_FLOW_LGDO       "LGDO" // logic digital OR
#define CHUNK_FLOW_LGDP       "LGDP" // logic digital NOP
#define CHUNK_FLOW_LGDX       "LGDX" // logic digital XOR
#define CHUNK_FLOW_LGNA       "LGNA" // logic numeric AND
#define CHUNK_FLOW_LGNN       "LGNN" // logic numeric NOT
#define CHUNK_FLOW_LGNO       "LGNO" // logic numeric OR
#define CHUNK_FLOW_LGNP       "LGNP" // logic numeric NOP
#define CHUNK_FLOW_LGNX       "LGNX" // logic numeric XOR
#define CHUNK_FLOW_LNSF       "LNSF" // logic numeric shift register
#define CHUNK_MISC_LOGO       "LOGO" // log output
#define CHUNK_MISC_LOGR       "LOGR" // log recorder
#define CHUNK_FLOW_LRSF       "LRSF" // logic RS flipflop
#define CHUNK_FLOW_LTFF       "LTFF" // logic toggle flipflop
#define CHUNK_FLOW_MAAD       "MAAD" // mathematical addition
#define CHUNK_FLOW_MADI       "MADI" // mathematical division
#define CHUNK_FLOW_MAMU       "MAMU" // mathematical multiplication
#define CHUNK_FLOW_MASU       "MASU" // mathematical subtraction
#define CHUNK_FLOW_NCMP       "NCMP" // compare numbers
#define CHUNK_FLOW_NCTR       "NCTR" // numerical element counter
#define CHUNK_FLOW_NGTE       "NGTE" // numerical gate
#define CHUNK_FLOW_NTGT       "NTGT" // numerical triggered gate
#define CHUNK_FLOW_SOBJ       "SOBJ" // flow of a hmi object
#define CHUNK_MISC_USLI       "USLI" // user log in
#define CHUNK_DEVL            "DEVL" // device list

#define CHUNK_APCG            "APCG" // type group file
#define CHUNK_GRMM            "GRMM" // minimum and maximum IDs within the saved group

#endif
