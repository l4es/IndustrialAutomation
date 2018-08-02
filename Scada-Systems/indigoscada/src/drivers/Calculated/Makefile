include ../../../Makefile.inc

clean:
	-	rm -f *.a *.o *.moc.cpp *~ *.bak

.cpp.o:
	$(CXX)  -fPIC $<

OBJS =  Calculated.o CalculatedConfigure.o CalculatedConfigureData.o CalculatedInput.o\
    CalcEntryDlg.o CalcEntryDlgData.o CalculatedInputData.o
	

METAOBJS = Calculated.moc.o CalculatedConfigure.moc.o CalculatedInput.moc.o \
	CalculatedConfigureData.moc.o CalcEntryDlg.moc.o CalcEntryDlgData.moc.o \
	CalculatedInputData.moc.o

Calculated.moc.cpp:  Calculated.hpp
	$(MOC) -o Calculated.moc.cpp Calculated.hpp

CalculatedConfigure.moc.cpp: CalculatedConfigure.h
	$(MOC) -o CalculatedConfigure.moc.cpp CalculatedConfigure.h

CalculatedInput.moc.cpp: CalculatedInput.h
	$(MOC) -o CalculatedInput.moc.cpp CalculatedInput.h

CalculatedInputData.moc.cpp: CalculatedInputData.h
	$(MOC) -o CalculatedInputData.moc.cpp CalculatedInputData.h

CalculatedConfigureData.moc.cpp: CalculatedConfigureData.h
	$(MOC) -o CalculatedConfigureData.moc.cpp CalculatedConfigureData.h

CalcEntryDlg.moc.cpp: CalcEntryDlg.h
	$(MOC) -o CalcEntryDlg.moc.cpp CalcEntryDlg.h
    
CalcEntryDlgData.moc.cpp: CalcEntryDlgData.h
	$(MOC) -o CalcEntryDlgData.moc.cpp CalcEntryDlgData.h

Calculated.so:  $(METAOBJS) $(OBJS)
	g++ -shared -Wl,-soname,Calculated.so -o $(DRIVERDIR)/Calculated.so \
	$(METAOBJS) $(OBJS) ../calc/calc.tab.o -L $(LIBDIR) -lwlib -ldriver -lqsdb -lfl $(LIBS) 

all: Calculated.so

