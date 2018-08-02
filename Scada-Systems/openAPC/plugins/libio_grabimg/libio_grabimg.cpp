/********************************************************************************************

These sources are distributed in the hope that they will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. All
the information given here, within the interface descriptions and within the specification
are subject to change without notice. Errors and omissions excepted.

These sources demonstrate the usage of the OpenAPC Plug-In programming interface. They can be
used freely according to the OpenAPC Dual License: As long as the sources and the resulting
applications/libraries/Plug-Ins are used together with the OpenAPC software, they are
licensed as freeware. When you use them outside the OpenAPC software they are licensed under
the terms of the GNU General Public License.

For more information please refer to http://www.openapc.com/oapc_license.php

*********************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <wx/wx.h>
#include <wx/image.h>

#ifndef ENV_WINDOWSCE
 #include <sys/types.h>
#endif

#ifndef ENV_WINDOWS
 #include <arpa/inet.h>
 #include <errno.h>
 #include <unistd.h>
 #include <fcntl.h>
#else
// #include <winsock2.h>
 #define MSG_NOSIGNAL 0
 #define snprintf _snprintf
#endif

#include "oapc_libio.h"
#include "liboapc.h"
#include "libio_grabimg.h"

/* XPM */
/* XPM */
static const char *testpattern_xpm[] = {
    /* columns rows colors chars-per-pixel */
    "256 256 256 2",
    "   c black",
    ".  c #020202",
    "X  c #040404",
    "o  c #FBFBFB",
    "O  c gray1",
    "+  c gray99",
    "@  c #FDFDFD",
    "#  c gray100",
    "$  c #FEFEFE",
    "%  c #010101",
    "&  c gray11",
    "*  c gray84",
    "=  c #CDCDCD",
    "-  c #222222",
    ";  c gray24",
    ":  c gray2",
    ">  c #161616",
    ",  c #0E0E0E",
    "<  c #ED1C24",
    "1  c blue",
    "2  c green",
    "3  c gray94",
    "4  c gray23",
    "5  c gray14",
    "6  c #272727",
    "7  c gray6",
    "8  c #0C0C0C",
    "9  c gray98",
    "0  c gray86",
    "q  c gray89",
    "w  c #F9F9F9",
    "e  c gray20",
    "r  c #323232",
    "t  c #252525",
    "y  c #1D1D1D",
    "u  c gray21",
    "i  c #D0D0D0",
    "p  c gray16",
    "a  c #060606",
    "s  c gray17",
    "d  c #2C2C2C",
    "f  c #D2D2D2",
    "g  c gray12",
    "h  c #3E3E3E",
    "j  c #CBCBCB",
    "k  c #202020",
    "l  c #2D2D2D",
    "z  c #ECECEC",
    "x  c gray15",
    "c  c #CECECE",
    "v  c LightGray",
    "b  c gray82",
    "n  c #2F2F2F",
    "m  c gray10",
    "M  c #282828",
    "N  c gray85",
    "B  c gray83",
    "V  c #373737",
    "C  c gray22",
    "Z  c gray18",
    "A  c #343434",
    "S  c #1E1E1E",
    "D  c #393939",
    "F  c gray19",
    "G  c #3F3F3F",
    "H  c #D8D8D8",
    "J  c #2A2A2A",
    "K  c #353535",
    "L  c #3A3A3A",
    "P  c #1B1B1B",
    "I  c #CACACA",
    "U  c gray78",
    "Y  c gray81",
    "T  c #C8C8C8",
    "R  c gray25",
    "E  c #D7D7D7",
    "W  c #313131",
    "Q  c #232323",
    "!  c gray79",
    "~  c #070707",
    "^  c #EFEFEF",
    "/  c #F8F8F8",
    "(  c #181818",
    ")  c gray7",
    "_  c gray5",
    "`  c #F1F1F1",
    "'  c gray9",
    "]  c #EEEEEE",
    "[  c #F3F3F3",
    "{  c gray97",
    "}  c gray93",
    "|  c #131313",
    " . c #F4F4F4",
    ".. c #111111",
    "X. c #151515",
    "o. c gray95",
    "O. c #F6F6F6",
    "+. c gray8",
    "@. c #0B0B0B",
    "#. c gray96",
    "$. c #090909",
    "%. c gray4",
    "&. c #E2E2E2",
    "*. c gray88",
    "=. c #DADADA",
    "-. c #E1E1E1",
    ";. c gray87",
    ":. c gainsboro",
    ">. c #484848",
    ",. c #DDDDDD",
    "<. c gray90",
    "1. c #DFDFDF",
    "2. c #B7B7B7",
    "3. c #E6E6E6",
    "4. c gray92",
    "5. c #E4E4E4",
    "6. c #EAEAEA",
    "7. c #E9E9E9",
    "8. c gray91",
    "9. c #E7E7E7",
    "0. c gray61",
    "q. c gray40",
    "w. c gray77",
    "e. c gray60",
    "r. c #464646",
    "t. c #4B4B4B",
    "y. c #6F6F6F",
    "u. c #555555",
    "i. c #727272",
    "p. c #505050",
    "a. c #626262",
    "s. c #5D5D5D",
    "d. c #606060",
    "f. c #5A5A5A",
    "g. c #414141",
    "h. c gray32",
    "j. c #777777",
    "k. c #6A6A6A",
    "l. c #8B8B8B",
    "z. c gray67",
    "x. c #797979",
    "c. c gray46",
    "v. c gray74",
    "b. c #AFAFAF",
    "n. c gray75",
    "m. c gray68",
    "M. c #A4A4A4",
    "N. c #7C7C7C",
    "B. c gray51",
    "V. c #939393",
    "C. c gray27",
    "Z. c #A7A7A7",
    "A. c #6D6D6D",
    "S. c #494949",
    "D. c #8D8D8D",
    "F. c #848484",
    "G. c #9F9F9F",
    "H. c #959595",
    "J. c gray70",
    "K. c gray59",
    "L. c #434343",
    "P. c #B6B6B6",
    "I. c gray3",
    "U. c #C6C6C6",
    "Y. c gray33",
    "T. c #979797",
    "R. c #C3C3C3",
    "E. c #A2A2A2",
    "W. c #646464",
    "Q. c #C1C1C1",
    "!. c gray73",
    "~. c #A0A0A0",
    "^. c gray66",
    "/. c #9D9D9D",
    "(. c gray34",
    "). c gray71",
    "_. c gray31",
    "`. c #B1B1B1",
    "'. c gray29",
    "]. c #656565",
    "[. c #676767",
    "{. c #4E4E4E",
    "}. c gray56",
    "|. c #101010",
    " X c #5F5F5F",
    ".X c gray76",
    "XX c gray30",
    "oX c #C5C5C5",
    "OX c gray53",
    "+X c #9B9B9B",
    "@X c #6C6C6C",
    "#X c #9A9A9A",
    "$X c #7B7B7B",
    "%X c #C0C0C0",
    "&X c #808080",
    "*X c #BBBBBB",
    "=X c #686868",
    "-X c #717171",
    ";X c gray36",
    ":X c #7E7E7E",
    ">X c gray72",
    ",X c #B9B9B9",
    "<X c DimGray",
    "1X c gray35",
    "2X c #747474",
    "3X c #585858",
    "4X c #A5A5A5",
    "5X c #888888",
    "6X c #B4B4B4",
    "7X c gray62",
    "8X c #898989",
    "9X c gray58",
    "0X c #838383",
    "qX c gray65",
    "wX c gray52",
    "eX c gray57",
    "rX c gray69",
    "tX c gray55",
    "yX c #8E8E8E",
    "uX c #ACACAC",
    "iX c #3C3C3C",
    "pX c #191919",
    "aX c gray80",
    "sX c gray13",
    "dX c #D5D5D5",
    "fX c black",
    "gX c black",
    "hX c black",
    "jX c black",
    "kX c black",
    "lX c black",
    "zX c black",
    "xX c black",
    "cX c black",
    "vX c black",
    "bX c black",
    "nX c black",
    "mX c black",
    "MX c black",
    "NX c black",
    "BX c black",
    "VX c black",
    "CX c black",
    "ZX c black",
    "AX c black",
    "SX c black",
    "DX c black",
    "FX c black",
    "GX c black",
    "HX c black",
    "JX c black",
    "KX c black",
    "LX c black",
    "PX c black",
    "IX c black",
    "UX c black",
    /* pixels */
    "$ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ @ # } #.# $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ ",
    "@ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ + # }  .# @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ ",
    "+ + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + o # z [ $ + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + ",
    "o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o 9 $ 4.o.@ o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o 9 w w w w w w w w w w w w w w w w w w 9 o o o o o ",
    "9 9 9 9 9 9 9 9 9 9 9 { O./ / / O.{ 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 w @ 6.` + 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 + # # # # # # # # # # # # # # # # # # o 9 9 9 9 9 ",
    "w w w w w w w w w / O.# # # # # # # O./ w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w / + 6.` o w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w w 9 ` f B * * * * * * * * * * * * * * v f [ 9 w w w w ",
    "/ / / / / / / w { / # = x.r W r x.j # w { w / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / { o 7.3 9 / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / #.# R.                                    b # O./ / / ",
    "{ { { { { { / #.w / a.    I., I.     X#.w #./ { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { O.9 8.^ w { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { {  .# oX  W =Xa.W.W.W.W.W.W.W.W.W.W.a.=Xp   f # #.{ { { ",
    "O.O.O.O.O.O.#.{ w h.  iXm.7.[ 6.m.G   V z 9  .O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.#.w 9.] / O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.[ # U   :X# # # # # # # # # # # # # # k.  v #  .O.O.O.",
    "#.#.#.#.#. . .w p.  u.+ # / O./ # $ f.  t./  . .#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#. ./ 9.} { #.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.o.# U.  i.o z 3 3 3 3 3 3 3 3 3 3 z o d.  b $ [ #.#.#.",
    " . . . . .` $ z.  p.9  .^ [ [ [ ^  .+ u.  M.$ 3  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .[ { 3.z O. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .` # oX  2X# 3  . . . . . . . . . .3 # a.  b @ o. . . .",
    "[ [ [ [ [ ` # G   H o ^ [ [ [ [ [ 3 9 ,.O C @ ` [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ o.O.<.4.#.[ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ 3 $ w.  2X# ^ [ [ [ [ [ [ [ [ [ [ ^ # a.  b + ` [ [ [ ",
    "o.o.o.o.3 w 0 : C { 3 o.o.o.o.o.o.o.^ w h : * 9 3 o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.` #.<.4. .o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.o.^ @ R.  i.# ] o.o.o.o.o.o.o.o.o.o.] # a.  i o 3 o.o.o.",
    "` ` ` ` ^ w *   s.# } ` ` ` ` ` ` ` } # W.  b 9 ^ ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` 3  .5.6.[ ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ] + .X  i.# } ` ` ` ` ` ` ` ` ` ` } # a.  Y 9 ^ ` ` ` ",
    "3 3 3 3 ] w f   1X$ z 3 3 3 3 3 3 3 z $ d.  = w ] 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 ^ o.q 7.o.3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 ] o .X  i.# z 3 3 3 3 3 3 3 3 3 3 z # d.  c w ] 3 3 3 ",
    "^ ^ ^ ^ ] [ 5.| P ^ ] ^ ^ ^ ^ ^ ^ ^ ^ o.sX..-.[ ] ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ` &.8.` ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ } 9 Q.  i.# 4.^ ^ ^ ^ ^ ^ ^ ^ ^ ^ 4.# d.  = / } ^ ^ ^ ",
    "] ] ] ] ] 4.# s.  rX# 8.^ ] ] ] ] 7.9 ).  (.# 4.] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] 3 &.9.3 ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] z w %X  -X# 6.] ] ] ] ] ] ] ] ] ] 6.# d.  aX{ z ] ] ] ",
    "} } } } } 4.#.j % & aX+ 8.6.6.6.8./ ^ t   U.O.4.} } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } ^ -.3.^ } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } 4./ n.  -X# 7.} } } } } } } } } } 7.# d.  j O.4.} } } ",
    "z z z z z z 7.w }.  pXU.$ 9 9 9 # T 4   c.+ 8.z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z < < < < < < < < < < < < < < < < < < < < 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z z 6.{ v.  y.$ 8.z z z z z z z z z z 8.$ d.  j #.6.z z z ",
    "4.4.4.4.4.4.4.4. .B.    _.7Xm.G.p.     X6.z 6.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.< < < < < < < < < < < < < < < < < < < < 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.4.7.O.v.  y.@ 9.4.4.4.4.4.4.4.4.4.4.9.@  X  I  .7.4.4.4.",
    "6.6.6.6.6.6.6.6.8.o uXu           e ^.o 4.7.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.< < < < < < < < < < < < < < < < < < < < 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.8.#.v.  A./ &.3.3.3.3.3.3.3.3.3.3.&./ s.  ! [ 8.6.6.6.",
    "7.7.7.7.7.7.7.7.7.<.O.^ %X5X8X5Xv.^ { <.8.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.< < < < < < < < < < < < < < < < < < < < 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.7.9. .*X  j.# { o o o o o o o o o o { # q.  T o.9.7.7.7.",
    "8.8.8.8.8.8.8.8.8.8.<.9.` w w w o.8.<.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.< < < < < < < < < < < < < < < < < < < < 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.8.3.[ ,X% u j.A.y.y.y.y.y.y.y.y.y.y.A.j.Z % oX` 3.8.8.8.",
    "9.9.9.9.9.9.9.9.9.9.9.9.<.q q q <.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.< < < < < < < < < < < < < < < < < < < < 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.<.[ 6X                                    Q.3 <.9.9.9.",
    "3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.< < < < < < < < < < < < < < < < < < < < 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.3.7.:.P.>X!.!.!.!.!.!.!.!.!.!.!.!.!.!.2.P.1.8.3.3.3.3.",
    "<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.< < < < < < < < < < < < < < < < < < < < 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 <.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.<.9.3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 9.<.<.<.<.<.",
    "5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.< < < < < < < < < < < < < < < < < < < < 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.5.q &.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.5.5.5.5.5.5.",
    "q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q < < < < < < < < < < < < < < < < < < < < 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q q ",
    "&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.< < < < < < < < < < < < < < < < < < < < 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 &.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.&.",
    "-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.< < < < < < < < < < < < < < < < < < < < 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.",
    "*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.< < < < < < < < < < < < < < < < < < < < 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 *.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.",
    "1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.< < < < < < < < < < < < < < < < < < < < 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.",
    ";.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.< < < < < < < < < < < < < < < < < < < < 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.",
    ",.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.< < < < < < < < < < < < < < < < < < < < 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.",
    ":.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.< < < < < < < < < < < < < < < < < < < < 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 :.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.",
    "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 < < < < < < < < < < < < < < < < < < < < 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ",
    "=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.< < < < < < < < < < < < < < < < < < < < 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 =.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.",
    "N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N =.f dX=.N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N N ",
    "H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H N f B N H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H H ",
    "E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E H b B H E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E E ",
    "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * E i v E * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ",
    "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * E i v E * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ",
    "B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B dXY b dXB B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B ",
    "v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v B c i B v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v ",
    "f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f v = Y v f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f ",
    "b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b f aXc f b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b ",
    "i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i b aXc b i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i ",
    "Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y i j = b Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y ",
    "c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c Y I aXY c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c ",
    "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = c I j = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = ",
    "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = c I j = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = ",
    "j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j aXT ! j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j ",
    "I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I j U ! I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I ",
    "! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! I U T ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ",
    "T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T U.U T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T ",
    "U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U oXU.U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U U ",
    "U.U.U.U.U.U.U.U.U.U.U.U.U.# # # # # # # # # # # # # # # # 9 6.z z z z z z z z z z z z z z z z z 7.[ # # # # # # # # # # # # # # # # # # # O.7.z z z z z z z z z z z z z z z z z 6./ # # # # # # # # # # # # # # # # # # # o.6.z z z z z z z z z z z z z z z z 4.z + # # # # # # # # # # # # # # # # # # @ } 4.z z z z z z z z z z z z z z z z 6.3 # # # # # # # # # # # # # # # # # # # 9 4.z z z z z z z z z z z z z z z z z 7.[ # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # U.U.U.U.U.U.U.U.U.U.U.U.U.",
    "oXoXoXoXoXoXoXoXoXoXoXoXoX# # # # # # # # # # # # # # + # uXt J 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 s 5 yX# + # # # # # # # # # # # # # # # + # B.5 J 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 p t 2.# + # # # # # # # # # # # # # # # $ # 3Xt J 6 6 6 6 6 6 6 6 6 6 6 6 6 6 M 6 l :.# @ # # # # # # # # # # # # # # $ # ] ; x p 6 6 6 6 6 6 6 6 6 6 6 6 6 6 p x r.{ # $ # # # # # # # # # # # # # # @ # i x M 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 J 5 k.# @ # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # oXoXoXoXoXoXoXoXoXoXoXoXoX",
    "w.w.w.w.w.w.w.w.w.w.w.w.w.# # # # # # # # # # # # # # + # N.                                      l.# o # # # # # # # # # # # # # # # + # Y.                                      `.# + # # # # # # # # # # # # # # # $ # F                                       B # @ # # # # # # # # # # # # # # $ # } |                                     ) ` # $ # # # # # # # # # # # # # # @ # f                                       r # $ # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # w.w.w.w.w.w.w.w.w.w.w.w.w.",
    "R.R.R.R.R.R.R.R.R.R.R.R.R.# # # # # # # # # # # # # # + # W.% : % % % % % % % % % % % % % % % X % Z.# + # # # # # # # # # # # # # # # + # '.% X % % % % % % % % % % % % % % % . % n.# + # # # # # # # # # # # # # # # $ # A % X % % % % % % % % % % % % % % . % I.* # @ # # # # # # # # # # # # # # $ # [ - % O % % % % % % % % % % % % % % . % | 4.# $ # # # # # # # # # # # # # # @ # -.) % . % % % % % % % % % % % % % % O % 5 o # $ # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # R.R.R.R.R.R.R.R.R.R.R.R.R.",
    ".X.X.X.X.X.X.X.X.X.X.X.X.X# # # # # # # # # # # # # # $ # S.  O                               %   v.# + # # # # # # # # # # # # # # # $ # iX  O                             %     ! # @ # # # # # # # # # # # # # # $ # @ l   .                             %   ~ * # @ # # # # # # # # # # # # # # $ # #.5   .                             %   _ *.# @ # # # # # # # # # # # # # # $ # } P   .                             %   +.z # $ # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # .X.X.X.X.X.X.X.X.X.X.X.X.X",
    "Q.Q.Q.Q.Q.Q.Q.Q.Q.Q.Q.Q.Q.# # # # # # # # # # # # # $ # + F   .                             %   . b # @ # # # # # # # # # # # # # # $ # + Z   .                             %   O B # @ # # # # # # # # # # # # # # $ # + d   .                             %   ~ * # @ # # # # # # # # # # # # # # $ # 9 J   .                             %   I.* # @ # # # # # # # # # # # # # # $ # w x   .                             %   ~ =.# @ # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # Q.Q.Q.Q.Q.Q.Q.Q.Q.Q.Q.Q.Q.",
    "%X%X%X%X%X%X%X%X%X%X%X%X%X# # # # # # # # # # # # # $ # ^ m   %                             %   7 *.# @ # # # # # # # # # # # # # # $ #  .g   .                             %   $.0 # @ # # # # # # # # # # # # # # $ # { 5   .                             %   ~ * # @ # # # # # # # # # # # # # # $ # @ l   .                             %   O aX# @ # # # # # # # # # # # # # # $ # # K   .                               %   T # + # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # %X%X%X%X%X%X%X%X%X%X%X%X%X",
    "n.n.n.n.n.n.n.n.n.n.n.n.n.# # # # # # # # # # # # # @ # N @.  %                             .   y ^ # $ # # # # # # # # # # # # # # $ # } )   %                             %   |.-.# @ # # # # # # # # # # # # # # $ # { Q   .                             %   ~ * # @ # # # # # # # # # # # # # # # $ # r   O                                   R.# + # # # # # # # # # # # # # # # $ # r.  O                               O   `.# + # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # n.n.n.n.n.n.n.n.n.n.n.n.n.",
    "v.v.v.v.v.v.v.v.v.v.v.v.v.# # # # # # # # # # # # # + # w.                                  .   d @ # $ # # # # # # # # # # # # # # @ # ;.%.  %                             %   ( 8.# $ # # # # # # # # # # # # # # $ # [ y   .                             %   ~ * # @ # # # # # # # # # # # # # # # $ # K   O                               .   ,X# + # # # # # # # # # # # # # # # @ # (.  O                               O   e.# o # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # v.v.v.v.v.v.v.v.v.v.v.v.v.",
    "v.v.v.v.v.v.v.v.v.v.v.v.v.# # # # # # # # # # # # # + # ^.  .                               O   g.# $ # # # # # # # # # # # # # # # @ # i O   %                             .   k ^ # $ # # # # # # # # # # # # # # $ # 3 m   .                             %   O b # @ # # # # # # # # # # # # # # # $ # h   O                               O   b.# + # # # # # # # # # # # # # # # + # =X  X                               X   &X# o # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # v.v.v.v.v.v.v.v.v.v.v.v.v.",
    "*X*X*X*X*X*X*X*X*X*X*X*X*X# # # # # # # # # # # # # + # tX  O                               O   u.# @ # # # # # # # # # # # # # # # + # .X  %                               .   6 #.# $ # # # # # # # # # # # # # # $ # ] '   .                             %   O i # @ # # # # # # # # # # # # # # # $ # g.  O                               O   ~.# + # # # # # # # # # # # # # # # o # x.  X                               X   <X# o # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # *X*X*X*X*X*X*X*X*X*X*X*X*X",
    "*X*X*X*X*X*X*X*X*X*X*X*X*X# # # # # # # # # # # # # o # y.  X                               X   @X# o # # # # # # # # # # # # # # # + # 6X  O                               .   n + # $ # # # # # # # # # # # # # # $ # 6.|.  %                             %   O i # @ # # # # # # # # # # # # # # # @ # t.  O                               O   H.# + # # # # # # # # # # # # # # # + # l.  O                               O   h.# @ # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # *X*X*X*X*X*X*X*X*X*X*X*X*X",
    "!.!.!.!.!.!.!.!.!.!.!.!.!.# # # # # # # # # # # # # @ # Y.  O                               O   0X# o # # # # # # # # # # # # # # # o # M.  O                               O   V # # # # # # # # # # # # # # # # # $ # 6.|.  %                             %   O i # @ # # # # # # # # # # # # # # # @ # {.  O                               O   8X# + # # # # # # # # # # # # # # # + # /.  O                               O   4 # $ # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # !.!.!.!.!.!.!.!.!.!.!.!.!.",
    ",X,X,X,X,X,X,X,X,X,X,X,X,X# # # # # # # # # # # # # # # D   .                               O   #X# + # # # # # # # # # # # # # # # o # eX  O                               O   g.# $ # # # # # # # # # # # # # # # @ # 5.I.  %                             %   O i # @ # # # # # # # # # # # # # # # @ # (.  O                               X   N.# + # # # # # # # # # # # # # # # + # rX  .                               .   t + # $ # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # ,X,X,X,X,X,X,X,X,X,X,X,X,X",
    ">X>X>X>X>X>X>X>X>X>X>X>X>X# # # # # # # # # # # # $ # O.Q   .                               .   `.# + # # # # # # # # # # # # # # # o # &X  O                               O   t.# @ # # # # # # # # # # # # # # # @ # 5.~   %                             %   O i # @ # # # # # # # # # # # # # # # @ # f.  O                               X   y.# + # # # # # # # # # # # # # # # + # Q.  %                               %   > z # $ # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # >X>X>X>X>X>X>X>X>X>X>X>X>X",
    "2.2.2.2.2.2.2.2.2.2.2.2.2.# # # # # # # # # # # # @ # q +.O X O O O O O O O O O O O O O O O O . .X+ w + + + + + + + + + + + + + + + / + y.O ~ O O O O O O O O O O O O O O O a O (.+ 9 + + + + + + + + + + + + + + + 9 + 0 I.O X O O O O O O O O O O O O O O X O a c + 9 + + + + + + + + + + + + + + + w + W.O ~ O O O O O O O O O O O O O O O ~ O W.+ w + + + + + + + + + + + + + + + 9 + j a O X O O O O O O O O O O O O O O X O @.0 + 9 + + + + + + + + + + + + + + + + + @ # # # # # # # # # # # # 2.2.2.2.2.2.2.2.2.2.2.2.2.",
    "P.P.P.P.P.P.P.P.P.P.P.P.P.# # # # # # # # # # # # + # I                                         &.# # # # # # # # # # # # # # # # # # # (.                                      d.# # # # # # # # # # # # # # # # # # # *.                                        H # # # # # # # # # # # # # # # # # # # q.                                      h.# # # # # # # # # # # # # # # # # # # 9.%                                       b # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # P.P.P.P.P.P.P.P.P.P.P.P.P.",
    ").).).).).).).).).).).).).# # # # # # # # # # # # @ # aXr.S.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>._.z.2.P.2.2.2.2.2.2.2.2.2.2.2.2.2.2.2.P.2.@X>.S.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.S.>.c.2.).2.2.2.2.2.2.2.2.2.2.2.2.2.2.2.P.2.Z.'.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.S.E.2.P.2.2.2.2.2.2.2.2.2.2.2.2.2.2.2.).2.j.>.'.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.S.>.k.2.P.2.2.2.2.2.2.2.2.2.2.2.2.2.2.2.P.2.z.p.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.S.>.#X2.).2.2.2.2.2.2.2.2.2.2.2.2.2.2.2.>X).B # $ # # # # # # # # # # ).).).).).).).).).).).).).",
    "6X6X6X6X6X6X6X6X6X6X6X6X6X# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # <.%                                       R.# # # # # # # # # # # # # # # # # # # 8X                                      d # # # # # # # # # # # # # # # # # # # # l                                       D.# # # # # # # # # # # # # # # # # # # n.                                      O 3.# # # # # # # # # # # # # # # # # # # s.                                      {.# @ # # # # # # # # # # 6X6X6X6X6X6X6X6X6X6X6X6X6X",
    "J.J.J.J.J.J.J.J.J.J.J.J.J.# # # # # # # # # # # # # # # @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ o @ U.. . O . . . . . . . . . . . . . . O . O Y @ o @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ w @ N.. a . . . . . . . . . . . . . . . X . e @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ + @ 9 A . X . . . . . . . . . . . . . . . a . B.@ w @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ o @ U . . O . . . . . . . . . . . . . . O . X = @ o @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ w @ N.. a . . . . . . . . . . . . . . . X . D $ # $ # # # # # # # # # J.J.J.J.J.J.J.J.J.J.J.J.J.",
    "J.J.J.J.J.J.J.J.J.J.J.J.J.# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # + # 6X  .                               %   ~ ;.# @ # # # # # # # # # # # # # # # + # -X  X                               O   iX# $ # # # # # # # # # # # # # # # $ # + e   .                               X   :X# o # # # # # # # # # # # # # # # @ # f O   %                             %     n.# + # # # # # # # # # # # # # # # o # V.  X                               .   k [ # $ # # # # # # # # # J.J.J.J.J.J.J.J.J.J.J.J.J.",
    "`.`.`.`.`.`.`.`.`.`.`.`.`.# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # o # ~.  O                               %   ..7.# $ # # # # # # # # # # # # # # # + # q.  O                               O   h # $ # # # # # # # # # # # # # # # $ # + e   .                               X   c.# o # # # # # # # # # # # # # # # @ # ;.~   %                               .   b.# + # # # # # # # # # # # # # # # + # z.  .                               %   7 ;.# @ # # # # # # # # # `.`.`.`.`.`.`.`.`.`.`.`.`.",
    "rXrXrXrXrXrXrXrXrXrXrXrXrX# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # + # 8X  O                               .   g o.# $ # # # # # # # # # # # # # # # @ # ;X  O                               O   L.# $ # # # # # # # # # # # # # # # $ # + e   .                               X   i.# o # # # # # # # # # # # # # # # $ # 3.7   %                               O   G.# o # # # # # # # # # # # # # # # + # R.  .                               %   . T # + # # # # # # # # # rXrXrXrXrXrXrXrXrXrXrXrXrX",
    "b.b.b.b.b.b.b.b.b.b.b.b.b.# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # + # -X  O                               .   d 9 # $ # # # # # # # # # # # # # # # $ # p.  O                               O   S.# @ # # # # # # # # # # # # # # # $ # + e   .                               O   <X# + # # # # # # # # # # # # # # # $ # z (   %                               O   D.# o # # # # # # # # # # # # # # # @ # * O   %                               .   b.# + # # # # # # # # # b.b.b.b.b.b.b.b.b.b.b.b.b.",
    "m.m.m.m.m.m.m.m.m.m.m.m.m.# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # + # f.  O                               O   D # $ # # # # # # # # # # # # # # # # $ # r.  O                               O   t.# @ # # # # # # # # # # # # # # # $ # + e   .                               O   ].# + # # # # # # # # # # # # # # # $ # o.sX  .                               O   x.# + # # # # # # # # # # # # # # # @ # <.|.  %                               O   eX# + # # # # # # # # # m.m.m.m.m.m.m.m.m.m.m.m.m.",
    "m.m.m.m.m.m.m.m.m.m.m.m.m.# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # $ # C.  O                               O   >.# @ # # # # # # # # # # # # # # # # $ # 4   O                               X   u.# + # # # # # # # # # # # # # # # $ # + e   .                               O   ;X# @ # # # # # # # # # # # # # # # $ # / J   .                               O   =X# + # # # # # # # # # # # # # # # $ # { S   .                               X   i.# o # # # # # # # # # m.m.m.m.m.m.m.m.m.m.m.m.m.",
    "m.m.m.m.m.m.m.m.m.m.m.m.m.# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # $ # + e   .                               O   3X# + # # # # # # # # # # # # # # # # # $ r   .                               X   u.# + # # # # # # # # # # # # # # # $ # + e   .                               O   1X# @ # # # # # # # # # # # # # # # $ # $ e   .                               O   u.# + # # # # # # # # # # # # # # # # # # r   .                               O   3X# @ # # # # # # # # # m.m.m.m.m.m.m.m.m.m.m.m.m.",
    "uXuXuXuXuXuXuXuXuXuXuXuXuX# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # $ # [ g   .                               X   k.# + # # # # # # # # # # # # # # # $ # / J   .                               X   s.# o # # # # # # # # # # # # # # # $ # + e   .                               O   p.# @ # # # # # # # # # # # # # # # # $ # iX  O                               O   C.# $ # # # # # # # # # # # # # # # # @ # S.  O                               .   4 # # $ # # # # # # # # uXuXuXuXuXuXuXuXuXuXuXuXuX",
    "z.z.z.z.z.z.z.z.z.z.z.z.z.# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # @ # 3.|.  %                               X   $X# o # # # # # # # # # # # # # # # $ # ` Q   .                               X   d.# o # # # # # # # # # # # # # # # $ # + e   .                               O   t.# @ # # # # # # # # # # # # # # # # @ # r.  O                               .   V @ # $ # # # # # # # # # # # # # # # + #  X  X                               .   5 #.# $ # # # # # # # # z.z.z.z.z.z.z.z.z.z.z.z.z.",
    "z.z.z.z.z.z.z.z.z.z.z.z.z.# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # @ # v a   %                               X   D.# o # # # # # # # # # # # # # # # $ # 4.P   .                               X   q.# o # # # # # # # # # # # # # # # $ # + e   .                               O   L.# $ # # # # # # # # # # # # # # # # @ # h.  O                               .   M O.# $ # # # # # # # # # # # # # # # o # j.  X                               %   ..*.# @ # # # # # # # # z.z.z.z.z.z.z.z.z.z.z.z.z.",
    "^.^.^.^.^.^.^.^.^.^.^.^.^.# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # + # %X                                    O   7X# o # # # # # # # # # # # # # # # @ # 5.+.  %                               X   A.# o # # # # # # # # # # # # # # # $ # + e   .                               O   G # $ # # # # # # # # # # # # # # # # + #  X  O                               %   pX^ # $ # # # # # # # # # # # # # # # o # }.  O                                   . aX# @ # # # # # # # # ^.^.^.^.^.^.^.^.^.^.^.^.^.",
    "Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # + # m.  O                                 .   m.# + # # # # # # # # # # # # # # # @ # ,.8   %                               X   y.# o # # # # # # # # # # # # # # # $ # + e   .                               O   u # $ # # # # # # # # # # # # # # # # + # @X  O                               %   7 q # @ # # # # # # # # # # # # # # # + # ^.  O                                 %   `.# + # # # # # # # # Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.",
    "Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.# # # # # # # # # # # # # $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ + # K.  X % % % % % % % % % % % % % % %   . % v.# + $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ + # E : % % % % % % % % % % % % % % % % % X   $X# o $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ @ $ + e   . % % % % % % % % % % % % % % % O   A $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ o # j.  : % % % % % % % % % % % % % % % .   I.v # + $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ + # %X  .   % % % % % % % % % % % % % % % O % H.# + # # # # # # # # Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.",
    "qXqXqXqXqXqXqXqXqXqXqXqXqX# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # @ # :X  O                                     c # $ # # # # # # # # # # # # # # # $ # c     %                               O   $X# + # # # # # # # # # # # # # # # # # @ e   %                               %   l $ # # # # # # # # # # # # # # # # # + # F.  O                                     oX# @ # # # # # # # # # # # # # # # @ # v .                                   O   j.# o # # # # # # # # qXqXqXqXqXqXqXqXqXqXqXqXqX",
    "4X4X4X4X4X4X4X4X4X4X4X4X4X# # # # # # # # # # # # # $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ o # q.  : % % % % % % % % % % % % % % % .   ~ :.# + $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ + # Q.  % % % % % % % % % % % % % % % % % :   F.# 9 $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ @ $ o A   O % % % % % % % % % % % % % % % O   d o # @ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ 9 # eX  X % % % % % % % % % % % % % % % % .   6X# o $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ @ # <.|.  . % % % % % % % % % % % % % % % X   f.# @ # # # # # # # # 4X4X4X4X4X4X4X4X4X4X4X4X4X",
    "M.M.M.M.M.M.M.M.M.M.M.M.M.# # # # # # # # # # + # E.5 6 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 t t ~.0 N 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 =.0 = K 5 t 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 x 5 u.0 =.0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 H 0 $X5 6 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 t 5 t ).0 N 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 N 0 %Xs 5 t 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 6 5 k.0 N 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 N 0 ].5 x 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 t t p U.0 =.0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 <.# # # # # # # # # # M.M.M.M.M.M.M.M.M.M.M.M.M.",
    "E.E.E.E.E.E.E.E.E.E.E.E.E.# # # # # # # # # # + # 2X                                        R.# # # # # # # # # # # # # # # # # # # 7.$.                                      XX# # # # # # # # # # # # # # # # # # # # -X                                        j # # # # # # # # # # # # # # # # # # # &.O                                       u.# # # # # # # # # # # # # # # # # # # # =X                                        * # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # E.E.E.E.E.E.E.E.E.E.E.E.E.",
    "E.E.E.E.E.E.E.E.E.E.E.E.E.# # # # # # # # # # + # f.. : . . . . . . . . . . . . . . . O . ~ dX@ o @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ o @ B a . O . . . . . . . . . . . . . . . : . ;X@ o @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ 9 @ <X. a . . . . . . . . . . . . . . . O . . oX@ 9 @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ o @ *.) . O . . . . . . . . . . . . . . . : . t.@ o @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ 9 @ :X. : . . . . . . . . . . . . . . . . X . >X@ 9 @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ $ # # # # # # # # # # E.E.E.E.E.E.E.E.E.E.E.E.E.",
    "~.~.~.~.~.~.~.~.~.~.~.~.~.# # # # # # # # # # $ # g.  O                               %   |.3.# $ # # # # # # # # # # # # # # # + # I   %                                 O   q.# + # # # # # # # # # # # # # # # # + # =X  X                               %     U.# + # # # # # # # # # # # # # # # @ # <.|   %                               O   R # $ # # # # # # # # # # # # # # # # + # }.  O                                 X   E.# o # # # # # # # # # # # # # # # # # # # # # # # # # # # # ~.~.~.~.~.~.~.~.~.~.~.~.~.",
    "G.G.G.G.G.G.G.G.G.G.G.G.G.# # # # # # # # # $ # { p   .                               .   - o.# $ # # # # # # # # # # # # # # # + # !.  %                                 X   y.# o # # # # # # # # # # # # # # # # + # s.  X                               %     oX# + # # # # # # # # # # # # # # # $ # 4.pX  .                               .   A # # # # # # # # # # # # # # # # # # o # ~.  O                                 X   l.# o # # # # # # # # # # # # # # # # # # # # # # # # # # # # G.G.G.G.G.G.G.G.G.G.G.G.G.",
    "G.G.G.G.G.G.G.G.G.G.G.G.G.# # # # # # # # # $ # 7.|   %                               .   e @ # $ # # # # # # # # # # # # # # # + # ^.  .                                 O   $X# o # # # # # # # # # # # # # # # # + # ;X  X                               %     Q.# + # # # # # # # # # # # # # # # $ # } P   .                               .   p @ # $ # # # # # # # # # # # # # # # + # `.  %                                 X   i.# o # # # # # # # # # # # # # # # # # # # # # # # # # # # # G.G.G.G.G.G.G.G.G.G.G.G.G.",
    "7X7X7X7X7X7X7X7X7X7X7X7X7X# # # # # # # # # @ # f a   %                               O   C.# @ # # # # # # # # # # # # # # # # o # T.  O                                 O   OX# + # # # # # # # # # # # # # # # # + # Y.  X                               %     Q.# + # # # # # # # # # # # # # # # $ #  .k   .                               .   -  .# $ # # # # # # # # # # # # # # # + # Q.    %                               X   1X# + # # # # # # # # # # # # # # # # # # # # # # # # # # # # 7X7X7X7X7X7X7X7X7X7X7X7X7X",
    "/././././././././././././.# # # # # # # # # + # *X  %                                 O   ;X# + # # # # # # # # # # # # # # # # o # OX  X                                 O   eX# + # # # # # # # # # # # # # # # # + # _.  X                               %     Q.# + # # # # # # # # # # # # # # # $ # O.-   .                               .   m } # $ # # # # # # # # # # # # # # # @ # b X   %                               O   L.# @ # # # # # # # # # # # # # # # # # # # # # # # # # # # # /././././././././././././.",
    "0.0.0.0.0.0.0.0.0.0.0.0.0.# # # # # # # # # + # ~.  O                                 O   i.# + # # # # # # # # # # # # # # # # o # c.  X                                 .   /.# + # # # # # # # # # # # # # # # # + # t.  X                               %     Q.# + # # # # # # # # # # # # # # # $ # + 6   .                               %   | 5.# $ # # # # # # # # # # # # # # # @ # *.@.  %                               .   n @ # $ # # # # # # # # # # # # # # # # # # # # # # # # # # # 0.0.0.0.0.0.0.0.0.0.0.0.0.",
    "+X+X+X+X+X+X+X+X+X+X+X+X+X# # # # # # # # # + # 0X  O                                 X   8X# o # # # # # # # # # # # # # # # # + # ].  X                                 .   ^.# + # # # # # # # # # # # # # # # # @ # L.  O                               %     Q.# + # # # # # # # # # # # # # # # $ # @ s   .                               %   @.:.# @ # # # # # # # # # # # # # # # $ # 6.(   %                               .   m  .# $ # # # # # # # # # # # # # # # # # # # # # # # # # # # +X+X+X+X+X+X+X+X+X+X+X+X+X",
    "#X#X#X#X#X#X#X#X#X#X#X#X#X# # # # # # # # # + # q.  X                                 O   ~.# o # # # # # # # # # # # # # # # # @ # u.  O                                 .   J.# + # # # # # # # # # # # # # # # # @ # L.  O                               %     Q.# + # # # # # # # # # # # # # # # $ # # e   .                               %   X B # @ # # # # # # # # # # # # # # # $ # [ x   .                               %   @.5.# $ # # # # # # # # # # # # # # # # # # # # # # # # # # # #X#X#X#X#X#X#X#X#X#X#X#X#X",
    "e.e.e.e.e.e.e.e.e.e.e.e.e.# # # # # # # # # @ # t.  O                                 %   ).# + # # # # # # # # # # # # # # # # $ # L.  O                                 %   v.# + # # # # # # # # # # # # # # # # $ # 4   O                               %     Q.# + # # # # # # # # # # # # # # # $ # # u   .                                     aX# @ # # # # # # # # # # # # # # # $ # + e   .                               %   . i # @ # # # # # # # # # # # # # # # # # # # # # # # # # # # e.e.e.e.e.e.e.e.e.e.e.e.e.",
    "T.T.T.T.T.T.T.T.T.T.T.T.T.# # # # # # # # $ # @ r   .                               %     j # @ # # # # # # # # # # # # # # # # $ # A   .                               %     U.# + # # # # # # # # # # # # # # # # $ # L   O                               %     Q.# + # # # # # # # # # # # # # # # # $ # G   O                                 %   %X# + # # # # # # # # # # # # # # # # $ # g.  O                                 .   v.# + # # # # # # # # # # # # # # # # # # # # # # # # # # # T.T.T.T.T.T.T.T.T.T.T.T.T.",
    "K.K.K.K.K.K.K.K.K.K.K.K.K.# # # # # # # # $ # 3 &   %                               %   %.,.# @ # # # # # # # # # # # # # # # $ # w 6   .                               %   : c # @ # # # # # # # # # # # # # # # # # $ e   O                               %     Q.# + # # # # # # # # # # # # # # # # $ # L.  O                                 .   ).# + # # # # # # # # # # # # # # # # @ # h.  O                                 O   Z.# + # # # # # # # # # # # # # # # # # # # # # # # # # # # K.K.K.K.K.K.K.K.K.K.K.K.K.",
    "K.K.K.K.K.K.K.K.K.K.K.K.K.# # # # # # # # @ # 0 8   %                               .   m 6.# $ # # # # # # # # # # # # # # # $ # ^ &   .                               %   %.E # @ # # # # # # # # # # # # # # # $ # $ n   .                               %     Q.# + # # # # # # # # # # # # # # # # $ # t.  O                                 .   ^.# + # # # # # # # # # # # # # # # # + # W.  O                                 X   yX# o # # # # # # # # # # # # # # # # # # # # # # # # # # # K.K.K.K.K.K.K.K.K.K.K.K.K.",
    "H.H.H.H.H.H.H.H.H.H.H.H.H.# # # # # # # # + # U.% % %                               .   p / # $ # # # # # # # # # # # # # # # @ # 5...  %                               %   |.;.# @ # # # # # # # # # # # # # # # $ # + l   .                               %     Q.# + # # # # # # # # # # # # # # # # @ # _.  O                                 O   0.# + # # # # # # # # # # # # # # # # + # c.  X                                 X   2X# o # # # # # # # # # # # # # # # # # # # # # # # # # # # H.H.H.H.H.H.H.H.H.H.H.H.H.",
    "9X9X9X9X9X9X9X9X9X9X9X9X9X# # # # # # # # + # m.% O % % % % % % % % % % % % % % % % X % L $ @ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ + $ =.a % . % % % % % % % % % % % % % % % . % > 3.$ + $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ @ $ { x % O % % % % % % % % % % % % % % % . % % %X$ o $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ + $ (.% X % % % % % % % % % % % % % % % % X % eX$ 9 $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ 9 $ 5X% : % % % % % % % % % % % % % % % % : % s.$ 9 $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ # # # # # # # # 9X9X9X9X9X9X9X9X9X9X9X9X9X",
    "V.V.V.V.V.V.V.V.V.V.V.V.V.# # # # # # # # + # l.                                        p.# # # # # # # # # # # # # # # # # # # # Y                                         ( O.# # # # # # # # # # # # # # # # # # # $ k                                         w.# # # # # # # # # # # # # # # # # # # # 1X                                        F.# # # # # # # # # # # # # # # # # # # # /.                                        g.# # # # # # # # # # # # # # # # # # # # # # # # # # # # # V.V.V.V.V.V.V.V.V.V.V.V.V.",
    "eXeXeXeXeXeXeXeXeXeXeXeXeX# # # # # # # # $ # N J.>X2.2.2.2.2.2.2.2.2.2.2.2.2.2.2.2.).,XOXr.S.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.S.r.[.>XP.2.2.2.2.2.2.2.2.2.2.2.2.2.2.2.2.P.>XqXt.>.S.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.S.>.{.m.2.P.2.2.2.2.2.2.2.2.2.2.2.2.2.2.2.P.2.2.a.r.S.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.S.r.D.,X).2.2.2.2.2.2.2.2.2.2.2.2.2.2.2.2.).,XwXr.S.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.S.r.k.2.P.2.2.2.2.2.2.2.2.2.2.2.2.2.2.2.2.P.>X4XS.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.S.r.(.^ # $ # # # # # eXeXeXeXeXeXeXeXeXeXeXeXeX",
    "eXeXeXeXeXeXeXeXeXeXeXeXeX# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # wX                                        t.# # # # # # # # # # # # # # # # # # # # 0                                           } # # # # # # # # # # # # # # # # # # # # V                                         +X# # # # # # # # # # # # # # # # # # # # eX                                        G # # # # # # # # # # # # # # # # # # # # 6.                                          v # @ # # # # # eXeXeXeXeXeXeXeXeXeXeXeXeX",
    "}.}.}.}.}.}.}.}.}.}.}.}.}.# # # # # # # # # # $ + + + + + + + + + + + + + + + + + + w + k.O ~ O O O O O O O O O O O O O O O O ~ O ].+ w + + + + + + + + + + + + + + + + 9 + U.X O X O O O O O O O O O O O O O O O X O ..8.+ o + + + + + + + + + + + + + + + o + + G O a O O O O O O O O O O O O O O O O ~ O }.+ / + + + + + + + + + + + + + + + + w + G.O : O O O O O O O O O O O O O O O O : O r + + o + + + + + + + + + + + + + + + o + ] & O X O O O O O O O O O O O O O O O O O . *X# + # # # # # }.}.}.}.}.}.}.}.}.}.}.}.}.",
    "yXyXyXyXyXyXyXyXyXyXyXyXyX# # # # # # # # # # # # # # # # # # # # # # # # # # # # # @ # u.  O                                 X   2X# o # # # # # # # # # # # # # # # # + # v.                                    %   ..3 # $ # # # # # # # # # # # # # # # $ # # h   O                                 X   l.# o # # # # # # # # # # # # # # # # + # z.  .                                 .   t { # $ # # # # # # # # # # # # # # # $ # o d   .                                 O   ~.# + # # # # # yXyXyXyXyXyXyXyXyXyXyXyXyX",
    "D.D.D.D.D.D.D.D.D.D.D.D.D.# # # # # # # # # # # # # # # # # # # # # # # # # # # # # $ # h   O                                 O   wX# o # # # # # # # # # # # # # # # # + # 6X  %                                 %   | 3 # $ # # # # # # # # # # # # # # # $ # # h   O                                 X   0X# o # # # # # # # # # # # # # # # # + # P.  %                                 %   m 6.# $ # # # # # # # # # # # # # # # # @ # G   O                                 O   0X# + # # # # # D.D.D.D.D.D.D.D.D.D.D.D.D.",
    "D.D.D.D.D.D.D.D.D.D.D.D.D.# # # # # # # # # # # # # # # # # # # # # # # # # # # # $ # o d   .                                 O   T.# + # # # # # # # # # # # # # # # # + # z.  .                                 .   &  .# $ # # # # # # # # # # # # # # # $ # # h   O                                 X   :X# o # # # # # # # # # # # # # # # # + # .X  %                                 %   |.;.# @ # # # # # # # # # # # # # # # # + # u.  O                                 X   q.# + # # # # # D.D.D.D.D.D.D.D.D.D.D.D.D.",
    "tXtXtXtXtXtXtXtXtXtXtXtXtX# # # # # # # # # # # # # # # # # # # # # # # # # # # # $ # } y   .                                 O   ^.# + # # # # # # # # # # # # # # # # o # ~.  O                                 .   &  .# $ # # # # # # # # # # # # # # # $ # # h   O                                 X   j.# + # # # # # # # # # # # # # # # # @ # =     %                               %   X f # @ # # # # # # # # # # # # # # # # o # y.  X                                 O   t.# $ # # # # # tXtXtXtXtXtXtXtXtXtXtXtXtX",
    "l.l.l.l.l.l.l.l.l.l.l.l.l.# # # # # # # # # # # # # # # # # # # # # # # # # # # # @ # *._   %                                 .   ,X# + # # # # # # # # # # # # # # # # + # H.  O                                 .   5 w # $ # # # # # # # # # # # # # # # $ # # h   O                                 X   -X# + # # # # # # # # # # # # # # # # @ # B a   %                                 %   w.# + # # # # # # # # # # # # # # # # o # OX  X                                 .   F + # $ # # # # l.l.l.l.l.l.l.l.l.l.l.l.l.",
    "8X8X8X8X8X8X8X8X8X8X8X8X8X# # # # # # # # # # # # # # # # # # # # # # # # # # # # @ # i %   %                                 %   U # + # # # # # # # # # # # # # # # # + # 8X  O                                 .   x 9 # $ # # # # # # # # # # # # # # # $ # # ;   O                                 O   k.# + # # # # # # # # # # # # # # # # @ # ,._   %                                 %   `.# + # # # # # # # # # # # # # # # # o # 7X  O                                 .   m } # $ # # # # 8X8X8X8X8X8X8X8X8X8X8X8X8X",
    "8X8X8X8X8X8X8X8X8X8X8X8X8X# # # # # # # # # # # # # # # # # # # # # # # # # # # # + # !.  %                                 %   : B # @ # # # # # # # # # # # # # # # # + # :X  O                                 .   s + # $ # # # # # # # # # # # # # # # $ # # L.  O                                 O   ].# + # # # # # # # # # # # # # # # # @ # 3.+.  %                                 .   ~.# + # # # # # # # # # # # # # # # # + # P.  .                                 %   %.* # @ # # # # 8X8X8X8X8X8X8X8X8X8X8X8X8X",
    "5X5X5X5X5X5X5X5X5X5X5X5X5X# # # # # # # # # # # # # # # # # # # # # # # # # # # # + # 4X  O                                 %   , -.# @ # # # # # # # # # # # # # # # # + # 2X  O                                 .   r # # $ # # # # # # # # # # # # # # # # $ # '.  O                                 O   s.# + # # # # # # # # # # # # # # # # $ # ] P   %                                 O   }.# o # # # # # # # # # # # # # # # # @ # =                                       %   %X# + # # # # 5X5X5X5X5X5X5X5X5X5X5X5X5X",
    "OXOXOXOXOXOXOXOXOXOXOXOXOX# # # # # # # # # # # # # # # # # # # # # # # # # # # # o # }.  X                                 %   ' ] # $ # # # # # # # # # # # # # # # # + # =X  O                                 .   r # # $ # # # # # # # # # # # # # # # # $ # S.  O                                 O   3X# @ # # # # # # # # # # # # # # # # $ # { -   .                                 X   :X# o # # # # # # # # # # # # # # # # @ # *.I.  %                                 .   M.# + # # # # OXOXOXOXOXOXOXOXOXOXOXOXOX",
    "wXwXwXwXwXwXwXwXwXwXwXwXwX# # # # # # # # # # # # # # # # # # # # # # # # # # # # + # j.  X                                 .   - / # $ # # # # # # # # # # # # # # # # + # s.  O                                 O   4 # $ # # # # # # # # # # # # # # # # # $ # S.  O                                 O   p.# @ # # # # # # # # # # # # # # # # $ # $ s   .                                 O   @X# + # # # # # # # # # # # # # # # # $ # ] pX  %                                 O   wX# + # # # # wXwXwXwXwXwXwXwXwXwXwXwXwX",
    "F.F.F.F.F.F.F.F.F.F.F.F.F.# # # # # # # # # # # # # # # # # # # # # # # # # # # # @ # d.  O                                 .   W $ # $ # # # # # # # # # # # # # # # # @ # h.  O                                 O   4 # $ # # # # # # # # # # # # # # # # # $ # S.  O                                 O   t.# @ # # # # # # # # # # # # # # # # $ # # u   .                                 O   f.# @ # # # # # # # # # # # # # # # # $ # + M   .                                 X   k.# + # # # # F.F.F.F.F.F.F.F.F.F.F.F.F.",
    "F.F.F.F.F.F.F.F.F.F.F.F.F.# # # # # # # # # # # # # # # # # # # # # # # # # # # # @ # '.  O                                 O   g.# $ # # # # # # # # # # # # # # # # # $ # >.  O                                 O   L.# @ # # # # # # # # # # # # # # # # # $ # S.  O                                 O   L.# @ # # # # # # # # # # # # # # # # # $ # g.  O                                 O   S.# $ # # # # # # # # # # # # # # # # # $ # 4   O                                 O   {.# $ # # # # F.F.F.F.F.F.F.F.F.F.F.F.F.",
    "0X0X0X0X0X0X0X0X0X0X0X0X0X# # # # # # # # # # # # # # # # # # # # # # # # # # # # # $ V   O                                 O   _.# @ # # # # # # # # # # # # # # # # $ # # R   O                                 O   r.# @ # # # # # # # # # # # # # # # # # $ # S.  O                                 O   G # @ # # # # # # # # # # # # # # # # # $ # XX  O                                 O   C # $ # # # # # # # # # # # # # # # # # @ # Y.  O                                 .   r $ # $ # # # 0X0X0X0X0X0X0X0X0X0X0X0X0X",
    "B.B.B.B.B.B.B.B.B.B.B.B.B.# # # # # # # # # # # # # # # # # # # # # # # # # # # $ #  .6   .                                 X    X# o # # # # # # # # # # # # # # # # $ # + V   O                                 X   '.# + # # # # # # # # # # # # # # # # # $ # S.  O                                 O   L # $ # # # # # # # # # # # # # # # # # + # 3X  X                                 .   d w # $ # # # # # # # # # # # # # # # # o # @X  X                                 .   S ] # $ # # # B.B.B.B.B.B.B.B.B.B.B.B.B.",
    "&X&X&X&X&X&X&X&X&X&X&X&X&X# # # # # # # # $ # $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ @ $ 7.X.% . % % % % % % % % % % % % % % % % : % -X$ 9 $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ @ $ { Z % O % % % % % % % % % % % % % % % % : % p.$ o $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ @ $ S.% X % % % % % % % % % % % % % % % % O % u @ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ o $ a.% : % % % % % % % % % % % % % % % % . % k ] $ @ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ 9 $ 0X% : % % % % % % % % % % % % % % % % .   8 0 # @ # # # &X&X&X&X&X&X&X&X&X&X&X&X&X",
    "&X&X&X&X&X&X&X&X&X&X&X&X&X# # # # # # # # # # # # # # # # # # # # # # # # # # # # # 0 O                                         F.# # # # # # # # # # # # # # # # # # # # O.k                                         p.# # # # # # # # # # # # # # # # # # # # # r.                                        l # # # # # # # # # # # # # # # # # # # # # y.                                        7 <.# # # # # # # # # # # # # # # # # # # # G.                                          .X# + # # # &X&X&X&X&X&X&X&X&X&X&X&X&X",
    "H.H.H.H.H.H.H.H.H.H.H.H.H.# # # # # $ # $ W.C iX4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 iX4 ;Xw.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.R.R.-XiXiX4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 L.P.w.R.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w..XR.}.iXiX4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 /.w.R.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.R.w.b.R 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 ; iX&XR.R.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w..Xp.4 iX4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 iXiXd.R.R.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.R.} # $ # # # H.H.H.H.H.H.H.H.H.H.H.H.H.",
    ":X:X:X:X:X:X:X:X:X:X:X:X:X# # # # # $ # o.m                                         r.# # # # # # # # # # # # # # # # # # # # # 3X                                        8 #.# # # # # # # # # # # # # # # # # # # # M.                                          *X# # # # # # # # # # # # # # # # # # # # 1.                                          j.# # # # # # # # # # # # # # # # # # # # # d                                         F # # # # # # # # # # # # # # # # # # # # # # # # # # # :X:X:X:X:X:X:X:X:X:X:X:X:X",
    "N.N.N.N.N.N.N.N.N.N.N.N.N.# # # # # @ # &.|.O X O O O O O O O O O O O O O O O O a O W.+ w + + + + + + + + + + + + + + + + + 9 + '.O a O O O O O O O O O O O O O O O O : O sX] + o + + + + + + + + + + + + + + + + / + T.O a O O O O O O O O O O O O O O O O O X O ).+ w + + + + + + + + + + + + + + + + 9 + 0 7 O X O O O O O O O O O O O O O O O O a O k.+ w + + + + + + + + + + + + + + + + + + @ >.O a O O O O O O O O O O O O O O O O X O 5 #.@ o + + + + + + + + + + + + + + + + + + $ # # # # # N.N.N.N.N.N.N.N.N.N.N.N.N.",
    "N.N.N.N.N.N.N.N.N.N.N.N.N.# # # # # @ # j .   %                                 O   x.# + # # # # # # # # # # # # # # # # # # # iX  O                                 .   p O.# $ # # # # # # # # # # # # # # # # o # V.  X                                   %   P.# + # # # # # # # # # # # # # # # # @ # -.7   %                                 O   s.# + # # # # # # # # # # # # # # # # # @ # (.  O                                 %   ) 7.# $ # # # # # # # # # # # # # # # # # # # # # # # # N.N.N.N.N.N.N.N.N.N.N.N.N.",
    "$X$X$X$X$X$X$X$X$X$X$X$X$X# # # # # + # `.  %                                   O   eX# + # # # # # # # # # # # # # # # # $ # o Z   .                                 .   r 9 # $ # # # # # # # # # # # # # # # # o # }.  X                                   %   P.# + # # # # # # # # # # # # # # # # $ # 3.|   %                                 O   p.# @ # # # # # # # # # # # # # # # # # + # [.  X                                 %   O =.# @ # # # # # # # # # # # # # # # # # # # # # # # # $X$X$X$X$X$X$X$X$X$X$X$X$X",
    "x.x.x.x.x.x.x.x.x.x.x.x.x.# # # # # o # T.  O                                   O   ^.# + # # # # # # # # # # # # # # # # $ #  .g   .                                 O   4 $ # $ # # # # # # # # # # # # # # # # o # wX  X                                   %   P.# + # # # # # # # # # # # # # # # # $ # 4.>   %                                 O   r.# @ # # # # # # # # # # # # # # # # # + # x.  X                                   %   oX# + # # # # # # # # # # # # # # # # # # # # # # # # x.x.x.x.x.x.x.x.x.x.x.x.x.",
    "x.x.x.x.x.x.x.x.x.x.x.x.x.# # # # # + # x.  O                                   %   *X# + # # # # # # # # # # # # # # # # $ # z )   %                                 O   L.# $ # # # # # # # # # # # # # # # # # o # wX  X                                   %   P.# + # # # # # # # # # # # # # # # # $ # 3 m   %                                 O   iX# $ # # # # # # # # # # # # # # # # # + # l.  O                                   .   m.# + # # # # # # # # # # # # # # # # # # # # # # # # x.x.x.x.x.x.x.x.x.x.x.x.x.",
    "j.j.j.j.j.j.j.j.j.j.j.j.j.# # # # # + # s.  O                                 %   X c # @ # # # # # # # # # # # # # # # # @ # ;.%.  %                                 O   XX# @ # # # # # # # # # # # # # # # # # o # x.  X                                   %   P.# + # # # # # # # # # # # # # # # # $ # #.&   .                                 .   r + # $ # # # # # # # # # # # # # # # # + # /.  .                                   X   T.# o # # # # # # # # # # # # # # # # # # # # # # # # j.j.j.j.j.j.j.j.j.j.j.j.j.",
    "j.j.j.j.j.j.j.j.j.j.j.j.j.# # # # # $ # L.  O                                 %   , &.# @ # # # # # # # # # # # # # # # # @ # i X   %                                 O   (.# + # # # # # # # # # # # # # # # # # o # x.  X                                   %   P.# + # # # # # # # # # # # # # # # # $ # { -   .                                 .   p { # $ # # # # # # # # # # # # # # # # + # b.  .                                   X   :X# o # # # # # # # # # # # # # # # # # # # # # # # # j.j.j.j.j.j.j.j.j.j.j.j.j.",
    "c.c.c.c.c.c.c.c.c.c.c.c.c.# # # # $ # w d   .                                 .   & ` # $ # # # # # # # # # # # # # # # # + # .X  %                                   O   a.# + # # # # # # # # # # # # # # # # # o # i.  X                                   %   P.# + # # # # # # # # # # # # # # # # $ # w 6   .                                 .   k ` # $ # # # # # # # # # # # # # # # # + # %X  %                                   X   q.# + # # # # # # # # # # # # # # # # # # # # # # # # c.c.c.c.c.c.c.c.c.c.c.c.c.",
    "c.c.c.c.c.c.c.c.c.c.c.c.c.# # # # $ # 4.>   %                                 .   n 9 # $ # # # # # # # # # # # # # # # # + # 6X  .                                   O   A.# + # # # # # # # # # # # # # # # # # o # @X  X                                   %   P.# + # # # # # # # # # # # # # # # # $ # o Z   .                                 %   > 6.# $ # # # # # # # # # # # # # # # # @ # = X   %                                 O   t.# + # # # # # # # # # # # # # # # # # # # # # # # # c.c.c.c.c.c.c.c.c.c.c.c.c.",
    "2X2X2X2X2X2X2X2X2X2X2X2X2X# # # # @ # dX~   %                                 O   g.# $ # # # # # # # # # # # # # # # # # o # E.  O                                   O   x.# + # # # # # # # # # # # # # # # # # o # <X  X                                   %   P.# + # # # # # # # # # # # # # # # # $ # @ r   .                                 %   _ <.# @ # # # # # # # # # # # # # # # # @ # =._   %                                 O   C # $ # # # # # # # # # # # # # # # # # # # # # # # # 2X2X2X2X2X2X2X2X2X2X2X2X2X",
    "i.i.i.i.i.i.i.i.i.i.i.i.i.# # # # + # v.  % %                                 O   u.# + # # # # # # # # # # # # # # # # # o # eX  O                                   O   wX# + # # # # # # # # # # # # # # # # # o #  X  X                                   %   P.# + # # # # # # # # # # # # # # # # $ # $ D   O                                 %   ~ 0 # @ # # # # # # # # # # # # # # # # $ # 8.X.  %                                 .   t / # $ # # # # # # # # # # # # # # # # # # # # # # # i.i.i.i.i.i.i.i.i.i.i.i.i.",
    "i.i.i.i.i.i.i.i.i.i.i.i.i.# # # # + # E.  O                                   O   @X# + # # # # # # # # # # # # # # # # # o # &X  O                                   O   }.# + # # # # # # # # # # # # # # # # # o #  X  X                                   %   `.# + # # # # # # # # # # # # # # # # $ # # ;   O                                 %   O i # @ # # # # # # # # # # # # # # # # $ #  .sX  .                                 %   | z # $ # # # # # # # # # # # # # # # # # # # # # # # i.i.i.i.i.i.i.i.i.i.i.i.i.",
    "-X-X-X-X-X-X-X-X-X-X-X-X-X# # # # + # wX  O                                   O   F.# + # # # # # # # # # # # # # # # # # o # A.  X                                   O   0.# o # # # # # # # # # # # # # # # # # + # u.  X                                   %   `.# + # # # # # # # # # # # # # # # # # $ # L.  O                                       U.# + # # # # # # # # # # # # # # # # $ # o F   .                                 %   : :.# @ # # # # # # # # # # # # # # # # # # # # # # # -X-X-X-X-X-X-X-X-X-X-X-X-X",
    "y.y.y.y.y.y.y.y.y.y.y.y.y.# # # # + # <X  X                                   O   +X# o # # # # # # # # # # # # # # # # # o # s.  X                                   .   4X# + # # # # # # # # # # # # # # # # # + # h.  X                                   %   `.# + # # # # # # # # # # # # # # # # # $ # S.  O                                   %   *X# + # # # # # # # # # # # # # # # # $ # # R   O                                       U # + # # # # # # # # # # # # # # # # # # # # # # # y.y.y.y.y.y.y.y.y.y.y.y.y.",
    "y.y.y.y.y.y.y.y.y.y.y.y.y.# # # # $ # {.  O                                   %   rX# + # # # # # # # # # # # # # # # # # @ # {.  O                                   %   b.# + # # # # # # # # # # # # # # # # # @ # XX  O                                   %   `.# + # # # # # # # # # # # # # # # # # @ # p.  O                                   .   rX# + # # # # # # # # # # # # # # # # # @ # p.  O                                   O   rX# + # # # # # # # # # # # # # # # # # # # # # # # y.y.y.y.y.y.y.y.y.y.y.y.y.",
    "A.A.A.A.A.A.A.A.A.A.A.A.A.# # # $ # $ u . X . . . . . . . . . . . . . . . . . . . R.@ o @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ $ R . X . . . . . . . . . . . . . . . . . . % >X@ 9 @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ o @ >.. X . . . . . . . . . . . . . . . . . O . m.@ 9 @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ o @ Y.% X . . . . . . . . . . . . . . . . . X . qX@ 9 @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ 9 @ d.. a . . . . . . . . . . . . . . . . . a . +X@ w @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ $ # # # A.A.A.A.A.A.A.A.A.A.A.A.A.",
    "A.A.A.A.A.A.A.A.A.A.A.A.A.# # # $ # ` '                                         . ,.# # # # # # # # # # # # # # # # # # # # # s                                           U # # # # # # # # # # # # # # # # # # # # # L.                                          m.# # # # # # # # # # # # # # # # # # # # # f.                                          +X# # # # # # # # # # # # # # # # # # # # # i.                                          &X# # # # # # # # # # # # # # # # # # # # # # # # # A.A.A.A.A.A.A.A.A.A.A.A.A.",
    "@X@X@X@X@X@X@X@X@X@X@X@X@X# # # @ # -.p & y & & & & & & & & & & & & & & & & y & s i q &.q q q q q q q q q q q q q q q q &.q ,.u P S & & & & & & & & & & & & & & & & y & g *X5.-.q q q q q q q q q q q q q q q q q &.q t.P g & & & & & & & & & & & & & & & & & S P ~.<.*.q q q q q q q q q q q q q q q q q *.<.=Xm k & & & & & & & & & & & & & & & & & g m 8X<.*.q q q q q q q q q q q q q q q q q *.<.B.m g & & & & & & & & & & & & & & & & & g m y.<.*.q q q q q q q q q q q q q q q q q q -.4.# # # @X@X@X@X@X@X@X@X@X@X@X@X@X",
    "k.k.k.k.k.k.k.k.k.k.k.k.k.# # # # # o ^ ^ 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 ] 3 Y pX7 |.7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 ..7 k :.3 ^ 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 ^ 3 7.e 7 ..7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 |.7 !.3 } 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 ] 3 1X7 ) 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 ) 7 V.3 z 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 } 3 :X7 ) 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 ) 7 k.3 } 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 z 3 ^.7 ) 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 ) _ '.$ # $ k.k.k.k.k.k.k.k.k.k.k.k.k.",
    "k.k.k.k.k.k.k.k.k.k.k.k.k.# # # # # # # # # # # # # # # # # # # # # # # # # # # j                                           & #.# # # # # # # # # # # # # # # # # # # # { &                                           = # # # # # # # # # # # # # # # # # # # # # h.                                          9X# # # # # # # # # # # # # # # # # # # # # 8X                                          u.# # # # # # # # # # # # # # # # # # # # # oX                                          Q #.# @ k.k.k.k.k.k.k.k.k.k.k.k.k.",
    "<X<X<X<X<X<X<X<X<X<X<X<X<X# # # # # # $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ o $ ).% O % % % % % % % % % % % % % % % % % O % l w $ @ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ @ $ 6.m % . % % % % % % % % % % % % % % % % . % % = $ + $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ + $ Y.% X % % % % % % % % % % % % % % % % % X % 8X$ 9 $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ 9 $ T.% X % % % % % % % % % % % % % % % % % X % r.$ @ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ + $ H a % . % % % % % % % % % % % % % % % % . % ) q # @ <X<X<X<X<X<X<X<X<X<X<X<X<X",
    "=X=X=X=X=X=X=X=X=X=X=X=X=X# # # # # # # # # # # # # # # # # # # # # # # # # + # G.  O                                   O   D # $ # # # # # # # # # # # # # # # # # @ # q |   %                                 %   % v # @ # # # # # # # # # # # # # # # # # @ # Y.  O                                   O   wX# o # # # # # # # # # # # # # # # # # o # E.  O                                   .   V @ # $ # # # # # # # # # # # # # # # # $ # 4.|.  %                                 %   : j # o =X=X=X=X=X=X=X=X=X=X=X=X=X",
    "[.[.[.[.[.[.[.[.[.[.[.[.[.# # # # # # # # # # # # # # # # # # # # # # # # # + # OX  O                                   O   >.# @ # # # # # # # # # # # # # # # # # @ # =._   %                                 %   O N # @ # # # # # # # # # # # # # # # # # @ # Y.  O                                   O   N.# + # # # # # # # # # # # # # # # # # + # b.  O                                   .   p O.# $ # # # # # # # # # # # # # # # # $ # { -   .                                   .   J.# 9 [.[.[.[.[.[.[.[.[.[.[.[.[.",
    "q.q.q.q.q.q.q.q.q.q.q.q.q.# # # # # # # # # # # # # # # # # # # # # # # # # o # -X  X                                   O   1X# + # # # # # # # # # # # # # # # # # @ # f I.  %                                 %   X =.# @ # # # # # # # # # # # # # # # # # @ # Y.  O                                   O   x.# + # # # # # # # # # # # # # # # # # + # ,X  .                                   %   m ] # $ # # # # # # # # # # # # # # # # # # $ u   O                                   O   K.# 9 q.q.q.q.q.q.q.q.q.q.q.q.q.",
    "].].].].].].].].].].].].].# # # # # # # # # # # # # # # # # # # # # # # # # @ # ;X  O                                   X   k.# + # # # # # # # # # # # # # # # # # @ # ! .   %                                 %   I.<.# @ # # # # # # # # # # # # # # # # # @ # Y.  O                                   O   y.# + # # # # # # # # # # # # # # # # # + # R.    %                                 %   |.&.# @ # # # # # # # # # # # # # # # # # @ # '.  O                                   O   c.# 9 ].].].].].].].].].].].].].",
    "W.W.W.W.W.W.W.W.W.W.W.W.W.# # # # # # # # # # # # # # # # # # # # # # # # # $ # r.  O                                   X   N.# + # # # # # # # # # # # # # # # # # + # Q.  % %                                 %   I.<.# @ # # # # # # # # # # # # # # # # # @ # Y.  O                                   O   @X# + # # # # # # # # # # # # # # # # # @ # = O   %                                 %   $.v # @ # # # # # # # # # # # # # # # # # + # d.  O                                   X   f.# o W.W.W.W.W.W.W.W.W.W.W.W.W.",
    "a.a.a.a.a.a.a.a.a.a.a.a.a.# # # # # # # # # # # # # # # # # # # # # # # # $ # $ F   .                                   X   D.# o # # # # # # # # # # # # # # # # # + # >X  .                                   %   7 7.# $ # # # # # # # # # # # # # # # # # @ # Y.  O                                   O   a.# + # # # # # # # # # # # # # # # # # @ # H $.  %                                 %   % oX# + # # # # # # # # # # # # # # # # # o # $X  X                                   O   G # $ a.a.a.a.a.a.a.a.a.a.a.a.a.",
    "a.a.a.a.a.a.a.a.a.a.a.a.a.# # # # # # # # # # # # # # # # # # # # # # # # $ # o.k   .                                   O   /.# o # # # # # # # # # # # # # # # # # + # m.  .                                   %   ) 4.# $ # # # # # # # # # # # # # # # # # @ # Y.  O                                   O    X# + # # # # # # # # # # # # # # # # # @ # &._   %                                   %   P.# + # # # # # # # # # # # # # # # # # o # 9X  X                                   .   5 / # a.a.a.a.a.a.a.a.a.a.a.a.a.",
    "d.d.d.d.d.d.d.d.d.d.d.d.d.# # # # # # # # # # # # # # # # # # # # # # # # @ # &.|   %                                   .   m.# + # # # # # # # # # # # # # # # # # + # E.  .                                   %   X.} # $ # # # # # # # # # # # # # # # # # @ # Y.  O                                   O   u.# + # # # # # # # # # # # # # # # # # $ # 4.|   %                                   O   qX# + # # # # # # # # # # # # # # # # # + # z.  O                                   %   ..9.# d.d.d.d.d.d.d.d.d.d.d.d.d.",
    "d.d.d.d.d.d.d.d.d.d.d.d.d.# # # # # # # # # # # # # # # # # # # # # # # # @ # v :   %                                   %   v.# + # # # # # # # # # # # # # # # # # + # T.  O                                   .   P ` # $ # # # # # # # # # # # # # # # # # @ # Y.  O                                   O   h.# + # # # # # # # # # # # # # # # # # $ # ` y   .                                   O   9X# + # # # # # # # # # # # # # # # # # + # %X  %                                   %   O f # d.d.d.d.d.d.d.d.d.d.d.d.d.",
    " X X X X X X X X X X X X X# # # # # # # # # # # # # # # # # # # # # # # # + # .X  %                                         c # @ # # # # # # # # # # # # # # # # # + # tX  O                                   .   & ` # $ # # # # # # # # # # # # # # # # # @ # Y.  O                                   O   t.# @ # # # # # # # # # # # # # # # # # $ # O.6   .                                   O   B.# + # # # # # # # # # # # # # # # # # @ # * .   %                                   %   *X#  X X X X X X X X X X X X X",
    "s.s.s.s.s.s.s.s.s.s.s.s.s.# # # # # # # # # # # # # # # # # # # # # # # # + # z.  .                                   %   a :.# @ # # # # # # # # # # # # # # # # # + # &X  O                                   .   5 { # $ # # # # # # # # # # # # # # # # # @ # Y.  O                                   O   >.# @ # # # # # # # # # # # # # # # # # $ # o W   .                                   O   y.# + # # # # # # # # # # # # # # # # # $ # 8.,   %                                   .   ~.# s.s.s.s.s.s.s.s.s.s.s.s.s.",
    "s.s.s.s.s.s.s.s.s.s.s.s.s.# # # # # # # # # # # # # # # # # # # # # # # # + # 9X  O                                   %   ) <.# $ # # # # # # # # # # # # # # # # # + # c.  O                                   .   t / # $ # # # # # # # # # # # # # # # # # @ # Y.  O                                   O   R # $ # # # # # # # # # # # # # # # # # # # # 4   O                                   X   s.# + # # # # # # # # # # # # # # # # # $ # #.sX  .                                   O   wX# s.s.s.s.s.s.s.s.s.s.s.s.s.",
    ";X;X;X;X;X;X;X;X;X;X;X;X;X# # # # # # # # # # # # # # # # # # # # # # # # o # :X  X                                   .   S ^ # $ # # # # # # # # # # # # # # # # # + # k.  X                                   .   s + # $ # # # # # # # # # # # # # # # # # @ # u.  O                                   O   ; # # # # # # # # # # # # # # # # # # # # $ # C.  O                                   O   {.# $ # # # # # # # # # # # # # # # # # $ $ # r   O                                   O   k.# ;X;X;X;X;X;X;X;X;X;X;X;X;X",
    "f.f.f.f.f.f.f.f.f.f.f.f.f.# # # # # # # # # # # # # # # # # # # # # # # # + # =X  X                                   .   p w # $ # # # # # # # # # # # # # # # # # @ # d.  O                                   .   Z $ # $ # # # # # # # # # # # # # # # # # @ # 1X  O                                   .   u @ # $ # # # # # # # # # # # # # # # # # + # _.  O                                   O   h $ # $ # # # # # # # # # # # # # # # # # + # r.  O                                   O   p.# f.f.f.f.f.f.f.f.f.f.f.f.f.",
    "f.f.f.f.f.f.f.f.f.f.f.f.f.# # # # # # # # # # # # # # # # # # # # # # # # @ # h.  O                                   O   K # $ # # # # # # # # # # # # # # # # # # @ # u.  O                                   .   W $ # $ # # # # # # # # # # # # # # # # # @ # 1X  O                                   .   r @ # $ # # # # # # # # # # # # # # # # # + # f.  O                                   .   l 9 # $ # # # # # # # # # # # # # # # # # + #  X  X                                   .   C { f.f.f.f.f.f.f.f.f.f.f.f.f.",
    "1X1X1X1X1X1X1X1X1X1X1X1X1X# # # @ o o o o o o o o o o o o o o o o o o o o 9 + h X ~ X X X X X X X X X X X X X X X X X ~ X r.o 9 o o o o o o o o o o o o o o o o o o 9 o {.X ~ X X X X X X X X X X X X X X X X X ~ X L o 9 o o o o o o o o o o o o o o o o o o w o f.X ~ X X X X X X X X X X X X X X X X X a X Z #.o 9 o o o o o o o o o o o o o o o o o / o [.X ~ X X X X X X X X X X X X X X X X X a X Q ` o 9 o o o o o o o o o o o o o o o o o { o c.X I.X X X X X X X X X X X X X X X X X : X s 0 1X1X1X1X1X1X1X1X1X1X1X1X1X",
    "3X3X3X3X3X3X3X3X3X3X3X3X3X# # # # # # # # # # # # # # # # # # # # # # # # # # &                                           Y.# # # # # # # # # # # # # # # # # # # # # # C                                           F # # # # # # # # # # # # # # # # # # # # # # Y.                                          m # # # # # # # # # # # # # # # # # # # # # # 2X                                          a o.# # # # # # # # # # # # # # # # # # # # # K.                                          : ).3X3X3X3X3X3X3X3X3X3X3X3X3X",
    "(.(.(.(.(.(.(.(.(.(.(.(.(.# $ # R.T.#Xe.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.#XT.y.].[.q.q.q.q.q.q.q.q.q.q.q.q.q.q.q.q.q.[.a.j.0.T.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.i.W.[.q.q.q.q.q.q.q.q.q.q.q.q.q.q.q.q.q.[.a.i.+XT.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.T.+Xj.a.[.q.q.q.q.q.q.q.q.q.q.q.q.q.q.q.q.q.q.W.A.T.#Xe.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.T.0.:Xa.[.q.q.q.q.q.q.q.q.q.q.q.q.q.q.q.q.q.q.].k.V.+Xe.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.T.0.&Xa.[.q.q.q.q.q.q.q.q.q.q.q.q.q.q.q.q.q.q.].A.U.(.(.(.(.(.(.(.(.(.(.(.(.(.",
    "u.u.u.u.u.u.u.u.u.u.u.u.u.# @ # t.                                          | # # # # # # # # # # # # # # # # # # # # # # l.                                            0 # # # # # # # # # # # # # # # # # # # # # U.                                            z.# # # # # # # # # # # # # # # # # # # # # ]                                             i.# # # # # # # # # # # # # # # # # # # # # # 6                                           4 # # # # # # # # # # # # # # # # # # # # # # # u.u.u.u.u.u.u.u.u.u.u.u.u.",
    "u.u.u.u.u.u.u.u.u.u.u.u.u.$ # # iXX a X X X X X X X X X X X X X X X X X a X u + o 9 o o o o o o o o o o o o o o o o o { o c.X I.X X X X X X X X X X X X X X X X X : X _ H o w o o o o o o o o o o o o o o o o o / o J.X ~ X X X X X X X X X X X X X X X X X X a X 4Xo / o o o o o o o o o o o o o o o o o 9 o 3.X.X : X X X X X X X X X X X X X X X X X I.X ].o / o o o o o o o o o o o o o o o o o o 9 + L.X ~ X X X X X X X X X X X X X X X X X a X s 9 o o o o o o o o o o o o o o o o o o o o o $ u.u.u.u.u.u.u.u.u.u.u.u.u.",
    "Y.Y.Y.Y.Y.Y.Y.Y.Y.Y.Y.Y.Y.$ # [ t   .                                   O   >.# $ # # # # # # # # # # # # # # # # # # + # ].  X                                   %   ..*.# @ # # # # # # # # # # # # # # # # # + # b.  O                                     .   qX# + # # # # # # # # # # # # # # # # # $ # z |   %                                   X   f.# + # # # # # # # # # # # # # # # # # # @ # h.  O                                   .   m ` # $ # # # # # # # # # # # # # # # # # # # # Y.Y.Y.Y.Y.Y.Y.Y.Y.Y.Y.Y.Y.",
    "h.h.h.h.h.h.h.h.h.h.h.h.h.+ # q |.  %                                   O   s.# + # # # # # # # # # # # # # # # # # # @ # u.  O                                   %   ( 9.# @ # # # # # # # # # # # # # # # # # + # uX  O                                     O   ~.# o # # # # # # # # # # # # # # # # # $ # ^ m   .                                   O   _.# @ # # # # # # # # # # # # # # # # # # + # ].  O                                   %   8 *.# @ # # # # # # # # # # # # # # # # # # # # h.h.h.h.h.h.h.h.h.h.h.h.h.",
    "h.h.h.h.h.h.h.h.h.h.h.h.h.o # aXO   %                                   X   i.# o # # # # # # # # # # # # # # # # # # $ # L.  O                                   .   k } # $ # # # # # # # # # # # # # # # # # + # E.  O                                     O   ~.# o # # # # # # # # # # # # # # # # # $ # ` y   .                                   O   L.# $ # # # # # # # # # # # # # # # # # # + # c.  O                                       % i # @ # # # # # # # # # # # # # # # # # # # # h.h.h.h.h.h.h.h.h.h.h.h.h.",
    "p.p.p.p.p.p.p.p.p.p.p.p.p.9 # `.  %                                     O   8X# + # # # # # # # # # # # # # # # # # # $ # e   .                                   .   6  .# $ # # # # # # # # # # # # # # # # # + # E.  O                                     O   ~.# o # # # # # # # # # # # # # # # # # $ #  .t   .                                   .   L # # $ # # # # # # # # # # # # # # # # # + # 5X  X                                     .   !.# + # # # # # # # # # # # # # # # # # # # # p.p.p.p.p.p.p.p.p.p.p.p.p.",
    "p.p.p.p.p.p.p.p.p.p.p.p.p.9 # T.  O                                     .   ~.# + # # # # # # # # # # # # # # # # # $ # / 6   .                                   .   n o # $ # # # # # # # # # # # # # # # # # o # T.  O                                     O   ~.# o # # # # # # # # # # # # # # # # # $ # { M   .                                   .   Z o # $ # # # # # # # # # # # # # # # # # + # e.  O                                     O   ~.# o # # # # # # # # # # # # # # # # # # # # p.p.p.p.p.p.p.p.p.p.p.p.p.",
    "_._._._._._._._._._._._._.9 # $X  O                                     .   2.# + # # # # # # # # # # # # # # # # # $ # ] y   .                                   O   u # # # # # # # # # # # # # # # # # # # # o # K.  O                                     O   ~.# o # # # # # # # # # # # # # # # # # $ # 9 n   .                                   .   5 / # $ # # # # # # # # # # # # # # # # # + # z.  .                                     X   8X# o # # # # # # # # # # # # # # # # # # # # _._._._._._._._._._._._._.",
    "{.{.{.{.{.{.{.{.{.{.{.{.{.o # s.  O                                   %   . I # + # # # # # # # # # # # # # # # # # @ # 5...  %                                   O   R # $ # # # # # # # # # # # # # # # # # # o # }.  O                                     O   ~.# o # # # # # # # # # # # # # # # # # $ # + r   .                                   .   P ` # $ # # # # # # # # # # # # # # # # # + # *X  %                                     X   i.# o # # # # # # # # # # # # # # # # # # # # {.{.{.{.{.{.{.{.{.{.{.{.{.",
    "XXXXXXXXXXXXXXXXXXXXXXXXXX@ # L.  O                                   %   _ 0 # @ # # # # # # # # # # # # # # # # # @ # =.:   %                                   O   t.# $ # # # # # # # # # # # # # # # # # # o # 5X  X                                     O   ~.# o # # # # # # # # # # # # # # # # # $ # # L   O                                   %   +.9.# $ # # # # # # # # # # # # # # # # # @ # aX%   %                                   X   f.# + # # # # # # # # # # # # # # # # # # # # XXXXXXXXXXXXXXXXXXXXXXXXXX",
    "t.t.t.t.t.t.t.t.t.t.t.t.t.# w s   .                                   .   ( } # $ # # # # # # # # # # # # # # # # # @ # aX                                        O   (.# $ # # # # # # # # # # # # # # # # # # o # wX  X                                     O   ~.# o # # # # # # # # # # # # # # # # # $ # # iX  O                                   %   , ;.# @ # # # # # # # # # # # # # # # # # @ # N $.  %                                   O   g.# @ # # # # # # # # # # # # # # # # # # # # t.t.t.t.t.t.t.t.t.t.t.t.t.",
    "t.t.t.t.t.t.t.t.t.t.t.t.t.# } +.  %                                   .   M w # $ # # # # # # # # # # # # # # # # # + # *X  %                                     O   a.# @ # # # # # # # # # # # # # # # # # # o # $X  X                                     O   ~.# o # # # # # # # # # # # # # # # # # # $ # L.  O                                   %   I.dX# @ # # # # # # # # # # # # # # # # # @ # 5.X.  %                                   .   l $ # $ # # # # # # # # # # # # # # # # # # # t.t.t.t.t.t.t.t.t.t.t.t.t.",
    "'.'.'.'.'.'.'.'.'.'.'.'.'.# N :   %                                   O   ; $ # # # # # # # # # # # # # # # # # # # + # z.  .                                     O   @X# + # # # # # # # # # # # # # # # # # # o # $X  X                                     O   ~.# o # # # # # # # # # # # # # # # # # # $ # >.  O                                   %   . = # @ # # # # # # # # # # # # # # # # # $ # ^ sX  .                                   .   y ` # $ # # # # # # # # # # # # # # # # # # # '.'.'.'.'.'.'.'.'.'.'.'.'.",
    "S.S.S.S.S.S.S.S.S.S.S.S.S.# R.    %                                   O   p.# @ # # # # # # # # # # # # # # # # # # o # +X  O                                     X   j.# + # # # # # # # # # # # # # # # # # # o # i.  X                                     O   ~.# o # # # # # # # # # # # # # # # # # # @ # _.  O                                     %   w.# + # # # # # # # # # # # # # # # # # $ # 9 d   .                                   %   8 q # @ # # # # # # # # # # # # # # # # # # # S.S.S.S.S.S.S.S.S.S.S.S.S.",
    ">.>.>.>.>.>.>.>.>.>.>.>.>.# z.  O                                     O   ].# + # # # # # # # # # # # # # # # # # # o # l.  X                                     X   &X# o # # # # # # # # # # # # # # # # # # o # y.  X                                     O   ~.# o # # # # # # # # # # # # # # # # # # + # h.  O                                     .   ,X# + # # # # # # # # # # # # # # # # # # # # 4   O                                         v # @ # # # # # # # # # # # # # # # # # # # >.>.>.>.>.>.>.>.>.>.>.>.>.",
    "r.r.r.r.r.r.r.r.r.r.r.r.r.# }.  O                                     O   N.# + # # # # # # # # # # # # # # # # # # + # x.  X                                     O   tX# o # # # # # # # # # # # # # # # # # # o # <X  X                                     O   ~.# o # # # # # # # # # # # # # # # # # # + # f.  O                                     .   m.# + # # # # # # # # # # # # # # # # # # $ # XX  O                                     %   *X# + # # # # # # # # # # # # # # # # # # # r.r.r.r.r.r.r.r.r.r.r.r.r.",
    "r.r.r.r.r.r.r.r.r.r.r.r.r.# c.  O                                     O   9X# + # # # # # # # # # # # # # # # # # # + # <X  O                                     O   T.# + # # # # # # # # # # # # # # # # # # + # d.  X                                     O   ~.# o # # # # # # # # # # # # # # # # # # + #  X  O                                     O   ~.# + # # # # # # # # # # # # # # # # # # @ # s.  O                                     O   4X# + # # # # # # # # # # # # # # # # # # # r.r.r.r.r.r.r.r.r.r.r.r.r.",
    "C.C.C.C.C.C.C.C.C.C.C.C.C.# ;X  O                                     O   z.# + # # # # # # # # # # # # # # # # # # + # u.  O                                     .   E.# + # # # # # # # # # # # # # # # # # # + # d.  X                                     O   7X# o # # # # # # # # # # # # # # # # # # + # [.  O                                     O   H.# + # # # # # # # # # # # # # # # # # # + # y.  X                                     X   yX# o # # # # # # # # # # # # # # # # # # # C.C.C.C.C.C.C.C.C.C.C.C.C.",
    "C.C.C.C.C.C.C.C.C.C.C.C.C.@ L.  O                                   % %   v.# + # # # # # # # # # # # # # # # # # # @ # C.  O                                     .   m.# + # # # # # # # # # # # # # # # # # # + # (.  X                                     O   +X# + # # # # # # # # # # # # # # # # # # + # @X  O                                     X   5X# + # # # # # # # # # # # # # # # # # # o # &X  X                                     X   c.# + # # # # # # # # # # # # # # # # # # # C.C.C.C.C.C.C.C.C.C.C.C.C.",
    "L.L.L.L.L.L.L.L.L.L.L.L.L.} s                                           . i # @ # # # # # # # # # # # # # # # # # # $ # K                                             2.# + # # # # # # # # # # # # # # # # # # + # Y.                                            #X# + # # # # # # # # # # # # # # # # # # + # i.                                        %   x.# o # # # # # # # # # # # # # # # # # # + # }.                                            f.# + # # # # # # # # # # # # # # # # # # # L.L.L.L.L.L.L.L.L.L.L.L.L.",
    "g.g.g.g.g.g.g.g.g.g.g.g.g.Y p 8 _ 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 _ 8 & 3.# @ # # # # # # # # # # # # # # # # # $ # w K 8 , 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 _ 8 U.# + # # # # # # # # # # # # # # # # # # @ # (.8 7 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 7 8 ~.# + # # # # # # # # # # # # # # # # # # + # &X@.|.8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 |.@.j.# o # # # # # # # # # # # # # # # # # # + # ^.8 , 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 7 8 XX# @ # # # # # # # # # # # # # # # # # # # g.g.g.g.g.g.g.g.g.g.g.g.g.",
    "g.g.g.g.g.g.g.g.g.g.g.g.g.&.M.#X0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0./.#XqXw # # # # # # # # # # # # # # # # # # # # # 9 ^.#X/.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.4.# $ # # # # # # # # # # # # # # # # # # $ # 2.T./.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0./.T.H # $ # # # # # # # # # # # # # # # # # # $ # c T./.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0./.T.w.# $ # # # # # # # # # # # # # # # # # # $ # -.#X/.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0./.e.`.# # # # # # # # # # # # # # # # # # # # # g.g.g.g.g.g.g.g.g.g.g.g.g.",
    "R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R g.D a.h.4 g.R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R ",
    "G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G R C a.h.L R G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G ",
    "h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h G V d.p.D G h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h h ",
    "; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; h u d.p.C h ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ",
    "; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; h u d._.C h ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ",
    "4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 iXA  X_.u iX4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 ",
    "L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L 4 e s.{.K 4 L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L L ",
    "D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D L r s.XXA L D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D D ",
    "C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C D W s.t.e D C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C ",
    "V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V C F ;Xt.r C V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V ",
    "u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u V n f.'.W V u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u u ",
    "K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K u Z f.S.F u K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K K ",
    "A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A K l f.S.n K A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A ",
    "e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e A d f.S.Z A e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e e ",
    "r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r A s 3Xr.l e r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r ",
    "W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W e J (.r.d r W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W W ",
    "F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F r p (.C.s W F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F F ",
    "n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n W M u.L.J F n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n ",
    "Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z F 6 u.L.p n Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z ",
    "l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l n x u.L.M Z l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l l ",
    "d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d Z 5 Y.g.6 l d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d ",
    "s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s l Q h.g.x d s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s s ",
    "J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J d - h.R t s J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J J ",
    "p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p s sXh.G 5 J p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p p ",
    "M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M J k p.h Q p M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M ",
    "6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 p g p.h - M 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 ",
    "x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x M S _.; sX6 x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x ",
    "t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t 6 y _.iXk x t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t ",
    "5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 x & {.4 S t 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 ",
    "Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q t P XXL y 5 Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q ",
    "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 5 m XXD & Q - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ",
    "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 5 m XXD & Q - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ",
    "k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k - ( t.C m sXk k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k k ",
    "g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g sX' '.V pXk g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g g ",
    "S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S k > '.u ( g S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S ",
    "y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y g X.S.K ' S y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y y ",
    "& & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & S +.>.A > y & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & ",
    "& & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & S +.>.A > y & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & P S g & & & & & & & ",
    "P P P P P P P P P P P P P y P y m P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P y | r.e X.& P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P P m S ..%.m P P P P P P ",
    "m m m m m m m m m m m m P ..( | y pXm m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m & ..r.r +.P m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m m pXg I.(.eXP pXm m m m m ",
    "( ( ( ( ( ( ( ( ( ( ( & X j.B ; %.y ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( m 7 C.W ) pX( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( y ~ _.O.# u ..m ( ( ( ( ",
    "' ' ' ' ' ' ' ' ' ' P X c.# # 7.L %.& ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' pX, L.F ..( ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' ' & a {.9 # / K |.pX' ' ' ' ",
    "> > > > > > > > > P O i.# :.x.# 6.D I.P > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > ( _ L.n |.' > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > P : XXw ` H $ e 7 ( > > > > ",
    "> > > > > > > > m O i.# q r   <X# 7.D I.m X.> > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > ( _ L.n |.' > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > X.P X t.{ o g.0.# F 7 ( > > > > ",
    "X.X.X.X.X.X.X.( . i.# q n $.& % [.# 7.C ~ m +.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.' 8 L.Z 7 > X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.X.+.m X t./ w u.  m.# F , ' X.X.X.X.",
    "+.+.+.+.+.+.( % i.# &.n I.( | ( % [.# 7.V a pX| +.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.> @.g.l , X.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.| pXO t./ w p.a ~ ^.# n _ > +.+.+.+.",
    "| | | | | '   -X# &.Z ~ ' ) | | '   q.# 6.u : ' ) | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | X.%.g.d _ +.| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | ) ' . '.{ 9 _.% P % ^.# Z 8 X.| | | | ",
    ") ) ) ) +.  i.# -.l a > ..) ) ) ) >   ].# ] u O X...) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) +.$.g.d 8 | ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ..' % S.{ 9 {.  ' X.% ^.# l @.+.) ) ) ) ",
    "......| I.2X# f n : X.|.............X.  a.[ 8.4 %.| ........................................................................................................................................................................................................| I.R s @.) ..............................................................................................................................................................................................................|.>   S.O.o {.  > |.X.  ^.# d %.| ........",
    "7 7 7 ..a c # 8X  > |.7 7 7 7 7 |.7 | _ I.! # :X  | 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 ..a G p $.|.7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 |.7 X.  >.O.9 XX  X.7 |.|   ^.# J I...7 7 7 7 ",
    ", , , |.@.- N # :X  ) 7 , , , 7 , ..@., v.# 7XI.7 7 , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , |.: G M I.7 , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , 7 , +.  r.O.9 t.  | , 7 , )   Z.# p a |., , , , ",
    ", , , _ |.O k H # N.  .., _ , _ |.%._ *X# 7X. 8 , _ , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , |.: h M I.7 , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , _ )   r.{ w t.  | _ , , , )   Z.# M a |., , , , ",
    "_ _ _ _ 8 |.X g E # N.  |._ 8 7 $._ *X# /.% _ , 8 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ 7 X iXx ~ , _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ 8 )   r.{ w t.  ) 8 _ _ _ _ ..  Z.# M : 7 _ _ _ _ ",
    "8 8 8 8 8 @.|.O g * # N.  7 , I.8 *X# 0.% 8 _ @.8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 , O iXx a _ 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 @.|.  C.w 9 t.  ) 8 _ _ _ _ _ |.% Z.# M X , 8 8 8 8 ",
    "@.@.@.@.@.@.%.7 . g dX# $X  $.@.*X# 0.% 8 8 %.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@._ . iXx : 8 @.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.%.,   r./ #.r.  _ ~ ~ ~ ~ ~ ~ ~ @.  4X# 6 O _ @.@.@.@.",
    "%.%.%.%.%.%.%.$., . S dX# j., v.# +X  @.@.$.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.8 % 4 t O @.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.$.8   L.o.[ h.O ' ) ) ) ) ) ) ) ) > $.Z.+ t . 8 %.%.%.%.",
    "$.$.$.$.$.$.$.$.I._ % y E # f # +X  %.%.I.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.@.  L 5 . %.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.@.O r # # # 3 + w 9 9 9 9 9 9 9 9 9 / # # s   @.$.$.$.$.",
    "~ ~ ~ ~ ~ ~ ~ ~ ~ ~ @.  y * # e.  I.$.~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ $.  D -   I.~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ $.O t 2.n..XT oXU.U.U.U.U.U.U.U.U.oXw..Xz.) : I.~ ~ ~ ~ ",
    "a a a a a a a a a a a %.  - =X% a I.a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a I.  C sX  ~ a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a ~ %                                     X ~ a a a a a ",
    ": : : : : : : : : : a : I.    a a : a : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : ~   V sX  a : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : a : a I.I.I.I.I.I.I.I.I.I.I.I.I.I.I.I.I.I.a : a : : : : ",
    ": : : : : : : : : : : : X a I.: X : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : ~   V sX  a : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : ",
    "X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X a   V k   : X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X ",
    "O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O :   u g   X O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O ",
    ". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . X   K S   O . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
    "% % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % O   A y   . % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % "
};


/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]={"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<oapc-config>\
 <flowimage>%s</flowimage>\
 <dialogue>\
  <general>\
   <param text=\"Interface Number\">\
    <name>device</name>\
    <type>integer</type>\
    <default>%d</default>\
    <min>0</min>\
    <max>12</max>\
   </param>\
   <param text=\"Don't scale (use hardware resolution)\">\
    <name>autosize</name>\
    <type>checkbox</type>\
    <default>%d</default>\
   </param>\
   <param text=\"Scale to image width\">\
    <name>width</name>\
    <type>integer</type>\
    <default>%d</default>\
    <min>4</min>\
    <max>1920</max>\
    <disableon>autosize</disableon>\
   </param>\
   <param text=\"Scale to image height\">\
    <name>height</name>\
    <type>integer</type>\
    <default>%d</default>\
    <min>4</min>\
    <max>1920</max>\
    <disableon>autosize</disableon>\
   </param>\
   <param text=\"Brightness\">\
    <name>"OAPC_ID_BRIGHTNESS"</name>\
    <type>integer</type>\
    <default>%d</default>\
    <min>-100</min>\
    <max>100</max>\
   </param>\
   <param text=\"Contrast\">\
    <name>"OAPC_ID_CONTRAST"</name>\
    <type>integer</type>\
    <default>%d</default>\
    <min>-100</min>\
    <max>100</max>\
   </param>\
   <param text=\"Gamma\">\
    <name>"OAPC_ID_GAMMA"</name>\
    <type>integer</type>\
    <default>%d</default>\
    <min>-100</min>\
    <max>100</max>\
   </param>\
   <param text=\"Red\">\
    <name>"OAPC_ID_RED"</name>\
    <type>integer</type>\
    <default>%d</default>\
    <min>-100</min>\
    <max>100</max>\
   </param>\
   <param text=\"Green\">\
    <name>"OAPC_ID_GREEN"</name>\
    <type>integer</type>\
    <default>%d</default>\
    <min>-100</min>\
    <max>100</max>\
   </param>\
   <param text=\"Blue\">\
    <name>"OAPC_ID_BLUE"</name>\
    <type>integer</type>\
    <default>%d</default>\
    <min>-100</min>\
    <max>100</max>\
   </param>\
   <param text=\"Convert to greyscale\">\
    <name>greyscale</name>\
    <type>checkbox</type>\
    <default>%d</default>\
   </param>\
   <param text=\"Mirror X\" name=\"mirrorx\">\
    <name>mirrorx</name>\
    <type>checkbox</type>\
    <default>%d</default>\
   </param>\
   <param text=\"Mirror Y\" name=\"mirrory\">\
    <type>checkbox</type>\
    <default>%d</default>\
   </param>\
   <param text=\"Rotate\" name=\"rotation\">\
    <type>option</type>\
    <value>no rotation</value>\
    <value>90 deg</value>\
    <value>180 deg</value>\
    <value>270 deg</value>\
    <default>%d</default>\
   </param>\
   <param text=\"Show test pattern (no image capture)\" name=\"testpattern\">\
    <type>checkbox</type>\
    <default>%d</default>\
    <disableon>noise</disableon>\
   </param>\
   <param text=\"Test pattern image\" name=\"testimage\">\
    <type>fileload</type>\
    <default>%s</default>\
    <min>0</min>\
    <max>%d</max>\
    <ffilter>Bitmap image|*.png;*.PNG;*.gif;*.GIF;*.bmp;*.bmp|All Files|*.*</ffilter>\
    <disableon>noise</disableon>\
   </param>\
   <param text=\"Show noise (no image capture)\" name=\"noise\">\
    <type>checkbox</type>\
    <default>%d</default>\
    <disableon>testpattern</disableon>\
   </param>\
  </general>\
  <oapchelppanel>\
   <out0>a single grabbed frame</out0>\
  </oapchelppanel>\
 </dialogue>\
</oapc-config>\n"};
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAADAFBMVEUEAgT8/gSFg4TEw8RIREikpKUsKCnk5uZkYmS1tbUYFRjT1dZaVVaUlpU8ODj08/S8vb97eXrMy82srq4xMDEPCwxSTUwfGxyMjoycnpz8/vzc2tzs6uxsamxcWlxEPjwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMWEmGAAABEUlEQVRYw+3Ya46DIBQFYJQiIFheIiJI97/LIU3Htgu4JDPhJBd/fjlIjAGhhjkapVOd6lSnOvW3KEVkPI7cgkrrjfNt3VXCApiyO1nWcPJ9WiN1xkJuYLLaRTfep50zPxFBYI+FosJqvpRC4vQAoxxRTsbs5JK5HKmbGBhV5lHfvNGyTvEnZgWMYshL6f1WnoM2tsNtYAhSliKfSyl3H8EoHIyiV7INsFTOguacU12AKZqSFSklURccCCB1JvuOhqWsxlccKMUwf4eAUo/lI2YGpNZ5DiHUeT053Oc2mq/E/m/RqU79R6rJZcmLGodhGJtQGFUK4SbvqtaCLvVL1VrQpa4TiMBLXRQGL3W0vHj8AQJc66n4HGsjAAAAAElFTkSuQmCC";
static char                libname[]="Capture Image";
static char                xmldescr[MAX_XML_SIZE+1];
static struct libio_config save_config;



/**
This function has to return the name that is used to display this library within the main application.
The name returned here has to be short and descriptive
@param[out] data pointer to the beginning of an char-array that contains the name
@return the length of the name structure or 0 in case of an error
*/
OAPC_EXT_API char *oapc_get_name(void)
{
   return libname;
}



/**
This function returns a set of OAPC_HAS_... flags that describe the general capabilities of this
library. These flags can be taken from oapc_libio.h
@return or-concatenated flags that describe the libraries capabilities
*/
OAPC_EXT_API unsigned long oapc_get_capabilities(void)
{
   return OAPC_HAS_OUTPUTS|
          OAPC_HAS_XML_CONFIGURATION|
          OAPC_ACCEPTS_PLAIN_CONFIGURATION|
          OAPC_FLOWCAT_IO;
}



/**
When the OAPC_HAS_INPUTS flag is set, the application calls this function to get the configuration
for the inputs. Here "input" is defined from the libraries point of view, means data that are sent from
the application to the library are input data
@return or-concatenated OAPC_..._IO...-flags that describe which inputs and data types are used or 0 in
        case of an error
*/
OAPC_EXT_API unsigned long oapc_get_input_flags(void)
{
   return 0;
   // avoid it to submit the same IO number for different data types, that would cause displaying problems for the flow symbol
}



/**
When the OAPC_HAS_OUTPUTS flag is set, the application calls this function to get the configuration
for the outputs. Here "output" is defined from the libraries point of view, means data that are sent from
the library to the application are output data
@return or-concatenated OAPC_..._IO...-flags that describe which outputs and data types are used or 0 in case
        of an error
*/
OAPC_EXT_API unsigned long oapc_get_output_flags(void)
{
   return OAPC_BIN_IO0;
}

/** has to be called after create instance and load data */
OAPC_EXT_API unsigned long oapc_get_config_info_data(void *instanceData,struct config_info *fillStruct)
{
    struct config_info *cfgInfo;
    struct instData    *data;

    data=(struct instData*)instanceData;

    if (!fillStruct) return OAPC_ERROR;
    cfgInfo=(struct config_info*)fillStruct;
    memset(cfgInfo,0,sizeof(struct config_info));

    cfgInfo->version=OAPC_CONFIG_INFO_VERSION;
    cfgInfo->length=sizeof(struct config_info);
    cfgInfo->configType=OAPC_CONFIG_TYPE_IMAGECAPTURE;

    cfgInfo->imageCapture.version=OAPC_CONFIG_IMAGE_CAPTURE_VERSION;
    cfgInfo->imageCapture.length=sizeof(struct config_image_capture);
    cfgInfo->imageCapture.frameDelay=(unsigned short)data->config.cycleTime;
    cfgInfo->imageCapture.frameWidth=data->config.width;
    cfgInfo->imageCapture.frameHeight=data->config.height;

    return OAPC_OK;
}

/**
When the OAPC_HAS_XML_CONFIGURATION capability flag was set this function is called to retrieve an XML
structure that describes which configuration information have to be displayed within the main application.
@param[out] data pointer to the beginning of an char-array that contains the XML data
@return the length of the XML structure or 0 in case of an error
*/
OAPC_EXT_API char *oapc_get_config_data(void* instanceData)
{
   struct instData *data;
   unsigned char    rotation=1;

   data=(struct instData*)instanceData;

   if ((data->config.flags & LIBIO_GRABIMG_FLAG_ROT90)!=0) rotation=2;
   else if ((data->config.flags & LIBIO_GRABIMG_FLAG_ROT180)!=0) rotation=3;
   else if ((data->config.flags & LIBIO_GRABIMG_FLAG_ROT270)!=0) rotation=4;

   snprintf(xmldescr,MAX_XML_SIZE,xmltempl,flowImage,
                                           data->config.device,
                                           data->config.flags & LIBIO_GRABIMG_FLAG_AUTO_SIZE,data->config.width,data->config.height,
                                           data->config.mBright/1000,
                                           data->config.mContrast/1000,
                                           data->config.mGamma/1000,
                                           data->config.mR/1000,
                                           data->config.mG/1000,
                                           data->config.mB/1000,
                                           data->config.flags & LIBIO_GRABIMG_FLAG_GREYSCALE,data->config.flags & LIBIO_GRABIMG_FLAG_MIRRORX,data->config.flags & LIBIO_GRABIMG_FLAG_MIRRORY,
                                           rotation,
                                           data->config.flags & LIBIO_GRABIMG_FLAG_TESTPATTERN,
                                           data->config.m_testimage,MAX_FILENAMESIZE,
                                           data->config.flags & LIBIO_GRABIMG_FLAG_NOISE
                                           );
   return xmldescr;
}

/**
When the OAPC_ACCEPTS_PLAIN_CONFIGURATION capability flag was set this function is called for every configurable parameter
to return the value that was set within the application.
@param[in] name the name of the parameter according to the unique identifier that was set within the <name></name> field of the
           XML config file
@param[in] value the value that was configured for this parameter; in case it is not a string but a number it has to be converted,
           the representation sent by the application is always a string
*/
OAPC_EXT_API void oapc_set_config_data(void* instanceData,const char *name,const char *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (strcmp(name,"device")==0)               data->config.device=atoi(value);
   else if (strcmp(name,"width")==0)           data->config.width=atoi(value);
   else if (strcmp(name,"height")==0)          data->config.height=atoi(value);
   else if (strcmp(name,"autosize")==0)
   {
      if (atoi(value)) data->config.flags|=LIBIO_GRABIMG_FLAG_AUTO_SIZE;
      else data->config.flags&=~LIBIO_GRABIMG_FLAG_AUTO_SIZE;
   }
   else if (strcmp(name,"mirrorx")==0)
   {
      if (atoi(value)) data->config.flags|=LIBIO_GRABIMG_FLAG_MIRRORX;
      else data->config.flags&=~LIBIO_GRABIMG_FLAG_MIRRORX;
   }
   else if (strcmp(name,"mirrory")==0)
   {
      if (atoi(value)) data->config.flags|=LIBIO_GRABIMG_FLAG_MIRRORY;
      else data->config.flags&=~LIBIO_GRABIMG_FLAG_MIRRORY;
   }
   else if(strcmp(name,"rotation")==0)
   {
      data->config.flags&=~(LIBIO_GRABIMG_FLAG_ROT90|LIBIO_GRABIMG_FLAG_ROT180|LIBIO_GRABIMG_FLAG_ROT270);
      if (atoi(value)==2) data->config.flags|=LIBIO_GRABIMG_FLAG_ROT90;
      else if (atoi(value)==3) data->config.flags|=LIBIO_GRABIMG_FLAG_ROT180;
      else if (atoi(value)==4) data->config.flags|=LIBIO_GRABIMG_FLAG_ROT270;
   }
   else if (strcmp(name,OAPC_ID_BRIGHTNESS)==0)      data->config.mBright=(int)atoi(value)*1000.0;
   else if (strcmp(name,OAPC_ID_CONTRAST)==0)        data->config.mContrast=(int)atoi(value)*1000.0;
   else if (strcmp(name,OAPC_ID_GAMMA)==0)           data->config.mGamma=(int)atoi(value)*1000.0;
   else if (strcmp(name,OAPC_ID_RED)==0)             data->config.mR=(int)atoi(value)*1000.0;
   else if (strcmp(name,OAPC_ID_GREEN)==0)           data->config.mG=(int)atoi(value)*1000.0;
   else if (strcmp(name,OAPC_ID_BLUE)==0)            data->config.mB=(int)atoi(value)*1000.0;
   else if (strcmp(name,"greyscale")==0)
   {
       if (atoi(value)) data->config.flags|=LIBIO_GRABIMG_FLAG_GREYSCALE;
       else data->config.flags&=~LIBIO_GRABIMG_FLAG_GREYSCALE;
   }
   else if (strcmp(name,OAPC_ID_CYCLETIME)==0) data->config.cycleTime=atoi(value);
   else if (strcmp(name,"testpattern")==0)
   {
      if (atoi(value))
      {
         data->config.flags|=LIBIO_GRABIMG_FLAG_TESTPATTERN;
         data->config.flags&=~LIBIO_GRABIMG_FLAG_NOISE;
      }
      else data->config.flags&=~LIBIO_GRABIMG_FLAG_TESTPATTERN;
   }
   else if (strcmp(name,"testimage")==0) 
    strncpy(data->config.m_testimage,value,MAX_FILENAMESIZE);
   else if (strcmp(name,"noise")==0)
   {       
      if (atoi(value))
      {
         data->config.flags|=LIBIO_GRABIMG_FLAG_NOISE;
         data->config.flags&=~LIBIO_GRABIMG_FLAG_TESTPATTERN;
      }
      else data->config.flags&=~LIBIO_GRABIMG_FLAG_NOISE;
   }
}


OAPC_EXT_API unsigned long oapc_get_config_value(const void* instanceData,const char *name,char *value,const int valLength)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (strcmp(name,OAPC_ID_BRIGHTNESS)==0)
   {
      snprintf(value,valLength,"%f",data->config.mBright/1000.0);
      return OAPC_OK;
   }
   else if (strcmp(name,OAPC_ID_CONTRAST)==0)
   {
      snprintf(value,valLength,"%f",data->config.mContrast/1000.0);
      return OAPC_OK;
   }
   else if (strcmp(name,OAPC_ID_GAMMA)==0)
   {
      snprintf(value,valLength,"%f",data->config.mGamma/1000.0);
      return OAPC_OK;
   }
   else if (strcmp(name,OAPC_ID_RED)==0)
   {
      snprintf(value,valLength,"%f",data->config.mR/1000.0);
      return OAPC_OK;
   }
   else if (strcmp(name,OAPC_ID_GREEN)==0)
   {
      snprintf(value,valLength,"%f",data->config.mG/1000.0);
      return OAPC_OK;
   }
   else if (strcmp(name,OAPC_ID_BLUE)==0)
   {
      snprintf(value,valLength,"%f",data->config.mB/1000.0);
      return OAPC_OK;
   }
   return OAPC_ERROR_INVALID_INPUT;
}


/**
This function delivers the data that are stored within the project file by the main application. It is
recommended to put two fields "version" and "length" into the data structure that is handed over here.
So when the data to be saved need to be changed it is easy to recognize which version of the data structure
is used, possible conversions can be done easier in oapc_set_loaded_data().<BR>
PLEASE NOTE: In order to keep the resulting project files compatible with all possible platforms the
             application is running at you have to store all values in network byteorder
@param[out] length the total effective length of the data that are returned by this function
@return the data that have to be saved
*/
OAPC_EXT_API char *oapc_get_save_data(void* instanceData,unsigned long *length)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   *length=sizeof(struct libio_config);
   save_config.version  =htons(2);
   save_config.length   =htons((unsigned short)*length);
   save_config.device   =htonl(data->config.device);
   save_config.width    =htonl(data->config.width);
   save_config.height   =htonl(data->config.height);
   save_config.flags    =htonl(data->config.flags);
   save_config.mBright  =htonl(data->config.mBright);
   save_config.mContrast=htonl(data->config.mContrast);
   save_config.mGamma   =htonl(data->config.mGamma);
   save_config.mR       =htonl(data->config.mR);
   save_config.mG       =htonl(data->config.mG);
   save_config.mB       =htonl(data->config.mB);
   save_config.cycleTime=htonl(data->config.cycleTime);
   strncpy(save_config.m_testimage,data->config.m_testimage,MAX_FILENAMESIZE);
   
   return (char*)&save_config;
}

/**
This function receives the data that have been stored within the project file by the main application. It is
recommended to check if the returned data are really what the library expects. To do that two fields
"version" and "length" within the saved data structure should be checked.<BR>
PLEASE NOTE: In order to keep the resulting project files compatible with all possible platforms the
             application is running at you have to convert all the values that have been stored in network
             byteorder back to the local byteorder
@param[in] length the total effective length of the data that are handed over to this function
@param[in] data the configuration data that are loaded for this library
*/
OAPC_EXT_API void oapc_set_loaded_data(void* instanceData,unsigned long length,char *loadedData)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (length>sizeof(struct libio_config)) length=sizeof(struct libio_config);
   memcpy(&save_config,loadedData,length);
   data->config.version  =ntohs(save_config.version);
   data->config.length   =ntohs(save_config.length);
   data->config.device   =ntohl(save_config.device);
   data->config.width    =ntohl(save_config.width);
   data->config.height   =ntohl(save_config.height);
   data->config.flags    =ntohl(save_config.flags);
   data->config.mBright  =ntohl(save_config.mBright);
   data->config.mContrast=ntohl(save_config.mContrast);
   data->config.mGamma   =ntohl(save_config.mGamma);
   data->config.mR       =ntohl(save_config.mR);
   data->config.mG       =ntohl(save_config.mG);
   data->config.mB       =ntohl(save_config.mB);
   data->config.cycleTime=ntohl(save_config.cycleTime);
   if (data->config.version>=2)
    strncpy(data->config.m_testimage,save_config.m_testimage,MAX_FILENAMESIZE);
}

/**
This function handles all internal data initialisation and has to allocate a memory area where all
data are stored into that are required to operate this Plug-In. This memory area can be used by the
Plug-In freely, it is handed over with every function call so that the Plug-In cann access its
values. The memory area itself is released by the main application, therefore it has to be allocated
using malloc().
@return pointer where the allocated and pre-initialized memory area starts
*/
OAPC_EXT_API void* oapc_create_instance2(unsigned long flags)
{
   struct instData *data;

   flags=flags; // removing "unused" warning
   data=(struct instData*)malloc(sizeof(struct instData));
   if (!data) return NULL;
   memset(data,0,sizeof(struct instData));

   data->config.width=640;
   data->config.height=480;
   data->config.cycleTime=100;
   data->config.flags=LIBIO_GRABIMG_FLAG_AUTO_SIZE;
#ifdef ENV_WINDOWS
   data->config.flags|=LIBIO_GRABIMG_FLAG_MIRRORY;
#endif

   return data;
}

/**
This function is called finally, it has to be used to release the instance data structure that was created
during the call of oapc_create_instance()
*/
OAPC_EXT_API void oapc_delete_instance(void* instanceData)
{
   if (instanceData) free(instanceData);
}


void *recvLoop(void *arg)
{
   int                   rOffset,gOffset,bOffset;
   struct oapc_bin_head *bin=NULL;
   struct instData      *data;
   int                   brightness;
   bool                  noColourManipulation;
   wxImage              *inImage=NULL;
   int                   prevGamma=-100000000,prevContrast=-100000000;
   float                 gammaRamp[256],cValue=1.0;


   data=(struct instData*)arg;
   data->running=true;

   while (data->running)
   {
      noColourManipulation=((data->config.mBright==0) && (data->config.mContrast==0) && (data->config.mGamma==0) &&
                            (data->config.mR==0) && (data->config.mG==0) && (data->config.mB==0));
      rOffset=(int)OAPC_ROUND(data->config.mR/1000.0,0);
      gOffset=(int)OAPC_ROUND(data->config.mG/1000.0,0);
      bOffset=(int)OAPC_ROUND(data->config.mB/1000.0,0);
      brightness=(int)OAPC_ROUND(data->config.mBright/1000.0,0);

      if (prevGamma!=data->config.mGamma)
      {
         float gamma,f1,ycValue;
         int   i;

         prevGamma=data->config.mGamma;
         gamma=data->config.mGamma/1000.0f;
         if (gamma>=0)
         {
             f1=gamma/100.0f;
             ycValue=1.0f+f1*4.0f;
         }
         else
         {
             f1=fabs((float)gamma)/100.0f;
             ycValue=1.0f-f1*0.8f;
         }
         for (i=0; i<256; i++)
         {
            gammaRamp[i]=std::min(65535.0f,std::max(0.0f,(float)pow((i+1) / 256.0f, (float)ycValue) * 65535.0f + 0.5f));
            gammaRamp[i]=(gammaRamp[i]/65535.0f);
         }
      }
      if (prevContrast!=data->config.mContrast)
      {
         prevContrast=data->config.mContrast;

         cValue =(100.0f+(data->config.mContrast/1000.0f)) / 100.0f;
         cValue*=cValue;
      }

      if (((data->config.flags & LIBIO_GRABIMG_FLAG_TESTPATTERN)!=0) || 
          ((data->config.flags & LIBIO_GRABIMG_FLAG_NOISE)!=0) || 
          ((captureImage(data)==OAPC_OK)) && (data->running))
      {
         int            dataLength,w,h;
         unsigned char *dataSrc;
         
         if (inImage) delete inImage;
         inImage=NULL;
         if (((data->config.flags & LIBIO_GRABIMG_FLAG_AUTO_SIZE)!=0) &&
             ((data->config.flags & (LIBIO_GRABIMG_FLAG_TESTPATTERN|
                                     LIBIO_GRABIMG_FLAG_NOISE|
                                     LIBIO_GRABIMG_FLAG_MIRRORX|LIBIO_GRABIMG_FLAG_MIRRORY|
                                     LIBIO_GRABIMG_FLAG_ROT90|LIBIO_GRABIMG_FLAG_ROT180|LIBIO_GRABIMG_FLAG_ROT90))==0)
             )
         {
             if (((data->config.flags & LIBIO_GRABIMG_FLAG_GREYSCALE)==0) && (noColourManipulation))
             {
                 // RGB24 image is required by user where no colour manipulation, rescaling or flipping
                 // has to be performed with -> can be sent out directly
                 data->rawBinBuf->param1=htonl(data->width);
                 data->rawBinBuf->param2=htonl(data->height);
                 data->rawBinBuf->param3=htonl(24);
                 if (data->running)
                 {
                    oapc_thread_mutex_lock(data->imgMutex);
                    if (data->m_bin) oapc_util_release_bin_data(data->m_bin);
                    data->m_bin=data->rawBinBuf;
                    oapc_thread_mutex_unlock(data->imgMutex);
                    data->rawBinBuf=NULL;
                    oapc_thread_sleep(0); // force a task switch
                    oapc_thread_signal_wait(data->m_signal,data->config.cycleTime*3); // wait not longer than two times the cycle time to let the image not become too old
                 }
                 continue;
             }
             else // RGB24
             {
                dataSrc=(unsigned char*)&data->rawBinBuf->data;
                dataLength=data->config.width*data->config.height;
                w=data->config.width;
                h=data->config.height;
             }
         }
         else
         {
            if ((data->config.flags & LIBIO_GRABIMG_FLAG_TESTPATTERN)!=0)
            {
               if (strlen(data->config.m_testimage)>0)
               {
                  wxString path;

                  oapc_unicode_charToStringASCII(data->config.m_testimage,MAX_FILENAMESIZE,&path);
                  inImage=new wxImage(path,wxBITMAP_TYPE_ANY);
               }
               else inImage=new wxImage(testpattern_xpm);
            }
            else if ((data->config.flags & LIBIO_GRABIMG_FLAG_NOISE)!=0)
            {
               unsigned char *p,val;
               int            x,y;

               inImage=new wxImage(512,288,false);
               p=inImage->GetData();
               srand(time(NULL));
               for (x=0; x<512; x++)
                for (y=0; y<288; y++)
               {
                  val=rand() & 0x3F;
                  *p=val; p++;
                  *p=val; p++;
                  *p=val; p++;
                }
            }
            else inImage=new wxImage(data->width,data->height,(unsigned char*)&data->rawBinBuf->data,true);
            if ((!inImage) || (!inImage->IsOk())) continue; // image manipulation failed

            if (((data->config.flags & LIBIO_GRABIMG_FLAG_AUTO_SIZE)==0) && (data->running))
            {
               wxImage *tmpImage;

               tmpImage=new wxImage(inImage->Scale(data->config.width,data->config.height,wxIMAGE_QUALITY_HIGH));
               delete inImage;
               inImage=tmpImage;
            }
            if (((data->config.flags & LIBIO_GRABIMG_FLAG_MIRRORX)!=0) && (data->running))
            {
               wxImage *tmpImage;

               tmpImage=new wxImage(inImage->Mirror(true));
               delete inImage;
               inImage=tmpImage;
            }
            if (((data->config.flags & LIBIO_GRABIMG_FLAG_MIRRORY)!=0) && (data->running)) // todo: integrate this step into data conversion operation below
            {
               wxImage *tmpImage;

               tmpImage=new wxImage(inImage->Mirror(false));
               delete inImage;
               inImage=tmpImage;
            }
            if (((data->config.flags & LIBIO_GRABIMG_FLAG_ROT90)!=0) && (data->running))
            {
               wxImage *tmpImage;

               tmpImage=new wxImage(inImage->Rotate90(true));
               delete inImage;
               inImage=tmpImage;
            }
            else if (((data->config.flags & LIBIO_GRABIMG_FLAG_ROT180)!=0) && (data->running))
            {
               wxImage *tmpImage;

#if wxCHECK_VERSION(2,9,0)
               tmpImage=new wxImage(inImage->Rotate180());
               delete inImage;
               inImage=tmpImage;
#else
               tmpImage=new wxImage(inImage->Rotate90(true));
               delete inImage;
               inImage=new wxImage(tmpImage->Rotate90(true));
               delete tmpImage;
#endif
            }
            else if (((data->config.flags & LIBIO_GRABIMG_FLAG_ROT270)!=0) && (data->running))
            {
               wxImage *tmpImage;

               tmpImage=new wxImage(inImage->Rotate90(false));
               delete inImage;
               inImage=tmpImage;
            }
            if (!inImage->IsOk()) continue; // image manipulation failed
            dataSrc=inImage->GetData();
            w=inImage->GetWidth();
            h=inImage->GetHeight();
            dataLength=w*h;
         }
         if ((data->config.flags & LIBIO_GRABIMG_FLAG_GREYSCALE)!=0)
         {
            int            i;
            float          r,g,b,v;
            unsigned char *s,*c;

            bin=oapc_util_alloc_bin_data(OAPC_BIN_TYPE_IMAGE,OAPC_BIN_SUBTYPE_IMAGE_GREY8,OAPC_COMPRESS_NONE,dataLength);
            if (bin)
            {
               bin->param1=htonl(w);
               bin->param2=htonl(h);
               bin->param3=htonl(8);

               c=(unsigned char*)&bin->data;
               s=dataSrc;
               for(i=0; i<dataLength; i++) if (data->running)
               {
                  r=gammaRamp[*s]; s=s+1;
                  r=((((r- 0.5f) * cValue) + 0.5f) * 255.0f)+brightness+rOffset;

                  g=gammaRamp[*s]; s=s+1;
                  g=((((g- 0.5f) * cValue) + 0.5f) * 255.0f)+brightness+gOffset;

                  b=gammaRamp[*s]; s=s+1;
                  b=((((b- 0.5f) * cValue) + 0.5f) * 255.0f)+brightness+bOffset;

                  v=r*0.299 + g*0.587 + b*0.114;
                  if (v>255) v=255;
                  else if (v<0) v=0;
                  *c=(unsigned char)v;
                  c=c+1;
               }
               if (data->running)
               {
                  oapc_thread_mutex_lock(data->imgMutex);
                  if (data->m_bin) oapc_util_release_bin_data(data->m_bin);
                  data->m_bin=bin;
                  oapc_thread_mutex_unlock(data->imgMutex);
                  bin=NULL;
               }
            } 
         }
         else // RGB24
         {
            int            i;
            float          r,g,b;
            unsigned char *s,*cr,*cg,*cb;

            bin=oapc_util_alloc_bin_data(OAPC_BIN_TYPE_IMAGE,OAPC_BIN_SUBTYPE_IMAGE_RGB24,OAPC_COMPRESS_NONE,dataLength*3);
            if (bin)
            {
               bin->param1=htonl(w);
               bin->param2=htonl(h);
               bin->param3=htonl(24);

#ifdef ENV_LINUX
               if (noColourManipulation) memcpy(&bin->data,dataSrc,dataLength*3);
               else
#endif
               {
#ifdef ENV_WINDOWS
                  cr=(unsigned char*)&bin->data;
                  cg=cr+1;
                  cb=cr+2;
#else
                  cr=(unsigned char*)&bin->data;
                  cg=cr+1;
                  cb=cr+2;
#endif
                  s=dataSrc;
                  for(i=0; i<dataLength; i++) if (data->running)
                  {
                     r=gammaRamp[*s]; s=s+1;
                     r=((((r- 0.5f) * cValue) + 0.5f) * 255.0f)+brightness+rOffset;
                     if (r>255) r=255;
                     else if (r<0) r=0;

                     g=gammaRamp[*s]; s=s+1;
                     g=((((g- 0.5f) * cValue) + 0.5f) * 255.0f)+brightness+gOffset;
                     if (g>255) g=255;
                     else if (g<0) g=0;

                     b=gammaRamp[*s]; s=s+1;
                     b=((((b- 0.5f) * cValue) + 0.5f) * 255.0f)+brightness+bOffset;
                     if (b>255) b=255;
                     else if (b<0) b=0;

#ifdef ENV_LINUX
                     *cr=(unsigned char)r;
                     cr=cr+3;

                     *cg=(unsigned char)g;
                     cg=cg+3;

                     *cb=(unsigned char)b;
                     cb=cb+3;
#else
                     // Windows has R and B exchanged for some reason
                     *cr=(unsigned char)b;
                     cr=cr+3;

                     *cg=(unsigned char)g;
                     cg=cg+3;

                     *cb=(unsigned char)r;
                     cb=cb+3;
#endif
                  }
               }
               if (data->running)
               {
                  oapc_thread_mutex_lock(data->imgMutex);
                  if (data->m_bin) oapc_util_release_bin_data(data->m_bin);
                  data->m_bin=bin;
                  oapc_thread_mutex_unlock(data->imgMutex);
                  bin=NULL;
               }
            }
         }
      }
      if (data->running)
      {
         oapc_thread_sleep(0); // force a task switch
         oapc_thread_signal_wait(data->m_signal,data->config.cycleTime*3); // wait not longer than two times the cycle time to let the image not become too old
      }
   }
   oapc_thread_mutex_lock(data->imgMutex);
   if (data->m_bin) oapc_util_release_bin_data(data->m_bin);
   data->m_bin=NULL;
   oapc_thread_mutex_unlock(data->imgMutex);
   if (bin) oapc_util_release_bin_data(bin);
   if (inImage) delete inImage;
   if (data->rawBinBuf) oapc_util_release_bin_data(data->rawBinBuf);
   data->running=true;
   return NULL;
}

/**
When this function is called everything has to be initialized in order to perform the required operation
@return a return value/error code that informs the main application if the initialization was done successfully
        or not
*/
OAPC_EXT_API unsigned long oapc_init(void* instanceData)
{
   struct instData *data;
   int              ret;

   data=(struct instData*)instanceData;

   data->imgMutex=oapc_thread_mutex_create();
   if (!data->imgMutex) return OAPC_ERROR_NO_MEMORY;

   data->m_signal=oapc_thread_signal_create();
   if (!data->m_signal) return OAPC_ERROR_NO_MEMORY;

   if ((data->config.flags & (LIBIO_GRABIMG_FLAG_TESTPATTERN|LIBIO_GRABIMG_FLAG_NOISE))==0)
   {
      ret=openVideoDevice(data,data->config.device);
      if (ret!=OAPC_OK) return ret;
   }

   data->threadHandle=oapc_thread_create(recvLoop,data);
   if (!data->threadHandle) return OAPC_ERROR_NO_MEMORY;

   return OAPC_OK;
}



/**
This function is called before the application unloads everything, it has to be used to deinitialize
everything and to release used resources.
*/
OAPC_EXT_API unsigned long oapc_exit(void* instanceData)
{
   struct instData *data;
   int              ctr=0;

   data=(struct instData*)instanceData;

   if (data->running)
   {
      data->running=false;
      while ((!data->running) && (ctr<50)) // wait for thread to finish
      {
         oapc_thread_sleep(100);
         ctr++;
      }
      data->running=false;
   }
   if (data->threadHandle) oapc_thread_release(data->threadHandle);

   closeVideoDevice(data);

   if (data->m_signal) oapc_thread_signal_release(data->m_signal);
   data->m_signal=NULL;

   if (data->imgMutex) oapc_thread_release(data->imgMutex);
   data->imgMutex=NULL;

   return OAPC_OK;
}



/**
This function is called by the main application periodically in order to poll the state of the related
output. It has to return the data that are available for that input or - in case there are none available -
the error code OAPC_ERROR_NO_DATA_AVAILABLE to notify the main application, that there is nothing new.
@param[in] output specifies the output where the data are fetched from, here not the OAPC_BIN_IO...-flag is used
           but the plain, 0-based output number
@param[out] value if there are new data available, they have to be stored within that variable, if there are
           no new data, the returned data are ignored by the main application
@return an error code OAPC_ERROR_... in case of an error, OAPC_ERROR_NO_DATA_AVAILABLE in case no new data are
           available or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_get_bin_value(void *instanceData,unsigned long output,struct oapc_bin_head **value)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   if (output==0)
   {
      oapc_thread_mutex_lock(data->imgMutex);
      if (!data->m_bin)
      {
         oapc_thread_mutex_unlock(data->imgMutex);
         return OAPC_ERROR_NO_DATA_AVAILABLE;
      }
      *value=data->m_bin;
      oapc_thread_signal_send(data->m_signal); // signalise to start capturing of next image
      return OAPC_OK;
   }
   else return OAPC_ERROR_NO_SUCH_IO;
}



OAPC_EXT_API void oapc_release_bin_data(void *instanceData,unsigned long /*output*/)
{
   struct instData      *data;

   data=(struct instData*)instanceData;
   oapc_util_release_bin_data(data->m_bin);
   data->m_bin=NULL;
   oapc_thread_mutex_unlock(data->imgMutex);
}
