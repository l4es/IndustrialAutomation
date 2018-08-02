ASN_MODULE_SOURCES=	\
	CPtype.c	\
	CPAPPDU.c	\
	Respondingpresentationselector.c	\
	Presentationcontextdefinitionresultlist.c	\
	Resultlist.c	\
	Result.c	\
	Modeselector.c	\
	Userdata.c	\
	Simplyencodeddata.c	\
	Fullyencodeddata.c	\
	PDVlist.c	\
	Presentationcontextdefinitionlist.c	\
	Contextlist.c	\
	Presentationcontextidentifier.c	\
	Abstractsyntaxname.c	\
	Transfersyntaxname.c	\
	Protocolversion.c	\
	Callingpresentationselector.c	\
	Calledpresentationselector.c	\
	Presentationselector.c	\
	Defaultcontextname.c	\
	Presentationrequirements.c	\
	Usersessionrequirements.c

ASN_MODULE_HEADERS=	\
	CPtype.h	\
	CPAPPDU.h	\
	Respondingpresentationselector.h	\
	Presentationcontextdefinitionresultlist.h	\
	Resultlist.h	\
	Result.h	\
	Modeselector.h	\
	Userdata.h	\
	Simplyencodeddata.h	\
	Fullyencodeddata.h	\
	PDVlist.h	\
	Presentationcontextdefinitionlist.h	\
	Contextlist.h	\
	Presentationcontextidentifier.h	\
	Abstractsyntaxname.h	\
	Transfersyntaxname.h	\
	Protocolversion.h	\
	Callingpresentationselector.h	\
	Calledpresentationselector.h	\
	Presentationselector.h	\
	Defaultcontextname.h	\
	Presentationrequirements.h	\
	Usersessionrequirements.h

ASN_MODULE_HEADERS+=ANY.h
ASN_MODULE_SOURCES+=ANY.c
ASN_MODULE_HEADERS+=INTEGER.h
ASN_MODULE_HEADERS+=NativeEnumerated.h
ASN_MODULE_SOURCES+=INTEGER.c
ASN_MODULE_SOURCES+=NativeEnumerated.c
ASN_MODULE_HEADERS+=NativeInteger.h
ASN_MODULE_SOURCES+=NativeInteger.c
ASN_MODULE_HEADERS+=OBJECT_IDENTIFIER.h
ASN_MODULE_SOURCES+=OBJECT_IDENTIFIER.c
ASN_MODULE_HEADERS+=asn_SEQUENCE_OF.h
ASN_MODULE_SOURCES+=asn_SEQUENCE_OF.c
ASN_MODULE_HEADERS+=asn_SET_OF.h
ASN_MODULE_SOURCES+=asn_SET_OF.c
ASN_MODULE_HEADERS+=constr_CHOICE.h
ASN_MODULE_SOURCES+=constr_CHOICE.c
ASN_MODULE_HEADERS+=constr_SEQUENCE.h
ASN_MODULE_SOURCES+=constr_SEQUENCE.c
ASN_MODULE_HEADERS+=constr_SEQUENCE_OF.h
ASN_MODULE_SOURCES+=constr_SEQUENCE_OF.c
ASN_MODULE_HEADERS+=constr_SET_OF.h
ASN_MODULE_HEADERS+=constr_SET.h
ASN_MODULE_SOURCES+=constr_SET.c
ASN_MODULE_SOURCES+=constr_SET_OF.c
ASN_MODULE_HEADERS+=asn_application.h
ASN_MODULE_HEADERS+=asn_system.h
ASN_MODULE_HEADERS+=asn_codecs.h
ASN_MODULE_HEADERS+=asn_internal.h
ASN_MODULE_HEADERS+=OCTET_STRING.h
ASN_MODULE_SOURCES+=OCTET_STRING.c
ASN_MODULE_HEADERS+=BIT_STRING.h
ASN_MODULE_SOURCES+=BIT_STRING.c
ASN_MODULE_SOURCES+=asn_codecs_prim.c
ASN_MODULE_HEADERS+=asn_codecs_prim.h
ASN_MODULE_HEADERS+=ber_tlv_length.h
ASN_MODULE_SOURCES+=ber_tlv_length.c
ASN_MODULE_HEADERS+=ber_tlv_tag.h
ASN_MODULE_SOURCES+=ber_tlv_tag.c
ASN_MODULE_HEADERS+=ber_decoder.h
ASN_MODULE_SOURCES+=ber_decoder.c
ASN_MODULE_HEADERS+=der_encoder.h
ASN_MODULE_SOURCES+=der_encoder.c
ASN_MODULE_HEADERS+=constr_TYPE.h
ASN_MODULE_SOURCES+=constr_TYPE.c
ASN_MODULE_HEADERS+=constraints.h
ASN_MODULE_SOURCES+=constraints.c
ASN_MODULE_HEADERS+=xer_support.h
ASN_MODULE_SOURCES+=xer_support.c
ASN_MODULE_HEADERS+=xer_decoder.h
ASN_MODULE_SOURCES+=xer_decoder.c
ASN_MODULE_HEADERS+=xer_encoder.h
ASN_MODULE_SOURCES+=xer_encoder.c
ASN_MODULE_HEADERS+=per_support.h
ASN_MODULE_SOURCES+=per_support.c
ASN_MODULE_HEADERS+=per_decoder.h
ASN_MODULE_SOURCES+=per_decoder.c
ASN_MODULE_HEADERS+=per_encoder.h
ASN_MODULE_SOURCES+=per_encoder.c
ASN_CONVERTER_SOURCES+=../cptype_decode.c
ASN_CONVERTER_SOURCES+=../cpatype_encode.c
ASN_CONVERTER_SOURCES+=../../../unity.tests/helpers/test_utilities.c


lib_LTLIBRARIES=libsomething.la
libsomething_la_SOURCES=$(ASN_MODULE_SOURCES) $(ASN_MODULE_HEADERS)

# This file may be used as an input for make(3)
# Remove the lines below to convert it into a pure .am file
TARGET = cptype_decode
CFLAGS += -I.
CFLAGS += -I../../../unity.tests/helpers
OBJS=${ASN_MODULE_SOURCES:.c=.o} ${ASN_CONVERTER_SOURCES:.c=.o}

all: $(TARGET)

$(TARGET): ${OBJS}
	$(CC) $(CFLAGS) -o $(TARGET) ${OBJS} $(LDFLAGS) $(LIBS)

.SUFFIXES:
.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)

regen: regenerate-from-asn1-source

regenerate-from-asn1-source:
	asn1c -fnative-types ../isoPresentationLayer.asn

