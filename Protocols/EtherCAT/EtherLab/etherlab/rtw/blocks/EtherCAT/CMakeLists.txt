#############################################################################
#
#  $Id$
#
#  Copyright 2012 Richard Hacker (lerichi at gmx dot net)
#
#  This file is part of the PdServ-MATLAB package.
#
#  PdServ-MATLAB is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  PdServ-MATLAB is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with PdServ-MATLAB. See COPYING. If not, see
#  <http://www.gnu.org/licenses/>.
#
#############################################################################



INSTALL (FILES
    Beckhoff.png
    Baumer.png
    mts-logo.png
    Murr.png
    EtherCAT.tlc
    ec_slave3.c ec_slave3.tlc
    setup.m

    master_state.c master_state.tlc
    domain_state.c domain_state.tlc
    baumer_ecatencoder.m
    mts_temposonics.m

    murr_dio.m
    EtherCATSlave.m

    EtherCATInfo.m
    EtherCATInfoSlave.m
    XmlNode.m
    el30xx_1.m el320x_1.m
    el51xx.m
    ep31xx_1.m ep41xx_1.m ep43xx_1.m
    ep8309.m

    ekxxxx.m
    el1xxx.m el1262.m
    el2xxx.m el2262.m
    el30xx.m el31xx.m el320x.m el3255.m el331x.m el3356.m el37x2.m el31xx_1.m
    el4xxx.m
    el500x.m el5021.m el5101_0010.m el5101.m el515x.m
    el9xxx.m

    ep1xxx.m
    ep2xxx.m
    ep31xx.m
    ep41xx.m
    ep43xx.m

    mtsrseries_check.m

    DESTINATION rtw/blocks/EtherCAT)


