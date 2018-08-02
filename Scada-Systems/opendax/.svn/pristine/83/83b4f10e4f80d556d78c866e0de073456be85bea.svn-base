if MODBUS
  M_MODBUS = modbus
else
  M_MODBUS = 
endif

if TEST
  M_TEST = test
else
  M_TEST = 
endif

if WIO
  M_WIO = wio
else
  M_WIO = 
endif

if DAXLUA
  M_DAXLUA = daxlua
else
  M_DAXLUA =
endif

if ARDUINO
  M_ARDUINO = arduino
else
  M_ARDUINO =
endif

if HISTLOG
  M_HISTLOG = histlog
else
  M_HISTLOG =
endif

SUBDIRS = daxc $(M_DAXLUA) $(M_MODBUS) $(M_TEST) $(M_WIO) $(M_ARDUINO) $(M_HISTLOG) skel
