/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#ifndef IEC104TYPES_H
#define IEC104TYPES_H

#define M_SP_NA_1	1
#define	M_DP_NA_1	3
#define M_BO_NA_1	7
#define M_ME_NA_1	9
#define M_ME_NB_1	11
#define	M_ME_NC_1	13
#define	M_IT_NA_1	15
#define	M_SP_TB_1	30
#define	M_DP_TB_1	31
#define M_BO_TB_1	33
#define M_ME_TD_1	34
#define M_ME_TE_1	35
#define M_ME_TF_1	36
#define	M_IT_TB_1	37
#define	C_SC_NA_1	45
#define	C_DC_NA_1	46
#define	C_SE_NA_1   48
#define	C_SE_NB_1   49
#define	C_SE_NC_1   50
#define	C_BO_NA_1	51
#define	C_SC_TA_1	58
#define	C_DC_TA_1	59
#define	C_SE_TA_1   61
#define	C_SE_TB_1   62
#define	C_SE_TC_1   63
#define	C_BO_TA_1	64
#define	M_EI_NA_1	70	// end of initialization
#define	C_IC_NA_1	100 // general interrogation
#define	C_CS_NA_1	103	// clock sync
#define	C_TS_NA_1	104	// test command
#define	C_RP_NA_1	105	// reset process command
#define	C_CD_NA_1	106	// delay acquisition command (non supported by iec 104)
#define C_TS_TA_1   107	//test command with cp56time2a timestamp
#define F_FR_NA_1	120
#define F_SR_NA_1	121
#define F_SC_NA_1	122
#define F_LS_NA_1	123
#define F_AF_NA_1	124
#define F_SG_NA_1	125
#define F_DR_TA_1	126
#define	M_ME_TN_1 	150		//custom type
#define	M_ME_TO_1 	151		//custom type
#define	M_ME_TP_1 	152		//custom type
#define	M_ME_TQ_1 	153		//custom type
#define	M_ME_TR_1 	154		//custom type
#define	M_ME_TS_1 	155		//custom type
#define	M_ME_TT_1 	156		//custom type
#define	C_EX_IT_1 	200		//custom type
#define	C_LO_ST_1 	201		//custom type

#if defined( _MSC_VER)          /* Microsoft C */
    #pragma pack(1)             /* Byte Alignment   */
#endif

#ifdef WIN32
typedef unsigned __int64 u_int64; //MSVC 6.0 does not accept long long
#elif
typedef unsigned long long u_int64;
#endif

#ifdef WIN32
typedef __int64 int64; //MSVC 6.0 does not accept long long
#elif
typedef long long int64;
#endif

/* 32-bit string state and change data unit */
struct iec_stcd {
	union {
		u_short		st[1];
		u_char		st1	:1;
		u_char		st2	:1;
		u_char		st3	:1;
		u_char		st4	:1;
		u_char		st5	:1;
		u_char		st6	:1;
		u_char		st7	:1;
		u_char		st8	:1;
		u_char		st9	:1;
		u_char		st10:1;
		u_char		st11:1;
		u_char		st12:1;
		u_char		st13:1;
		u_char		st14:1;
		u_char		st15:1;
		u_char		st16:1;
	};

	union {
		u_short		cd[1];
		u_char		cd1	:1;
		u_char		cd2	:1;
		u_char		cd3	:1;
		u_char		cd4	:1;
		u_char		cd5	:1;
		u_char		cd6	:1;
		u_char		cd7	:1;
		u_char		cd8	:1;
		u_char		cd9	:1;
		u_char		cd10:1;
		u_char		cd11:1;
		u_char		cd12:1;
		u_char		cd13:1;
		u_char		cd14:1;
		u_char		cd15:1;
		u_char		cd16:1;
	};
};

/* CP56Time2a timestamp */
typedef struct cp56time2a {
	u_short		msec;
	u_char		min	:6;
	u_char		res1	:1;
	u_char		iv	:1;
	u_char		hour	:5;
	u_char		res2	:2;
	u_char		su	:1;
	u_char		mday	:5;
	u_char		wday	:3;
	u_char		month	:4;
	u_char		res3	:4;
	u_char		year	:7;
	u_char		res4	:1;	
} cp56time2a;


/* CP16time2a timestamp*/
typedef struct cp16time2a {
	u_short		msec; /* milliseconds upto seconds */
}cp16time2a;


//////////////////////IEC types////////////////////////////////////////////////

/* M_SP_NA_1 - single point information with quality description */
typedef struct _iec_type1 {
	u_char		sp	:1; /*single point information 0 off, 1 on*/
	u_char		res	:3;
	u_char		bl	:1; /*1 blocked/ 0 not blocked*/
	u_char		sb	:1; /*1 substituted/ 0 not substituted*/
	u_char		nt	:1; /*1 not_topical/0 topical*/
	u_char		iv	:1; /*1 invalid/0 valid*/
}iec_type1;

/* M_DP_NA_1 - double point information with quality description */
typedef struct _iec_type3 {
	u_char		dp	:2;		/* 0 indeterminate or intermediate state
						    1 determinate state off
						    2 determinate state on
						    3 indeterminate state */
	u_char		res	:2;
	u_char		bl	:1; /* 1 blocked/0 not_blocked*/
	u_char		sb	:1; /* 1 substituted/0 not_substituted */
	u_char		nt	:1; /* 1 not_topical/0 topical */
	u_char		iv	:1; /* 1 invalid/0 valid */
}iec_type3;

/* M_BO_NA_1 - state and change information bit string */
typedef struct _iec_type7 {
	struct iec_stcd	stcd;
	u_char		ov	:1; /* overflow/no overflow */
	u_char		res	:3;
	u_char		bl	:1; /* blocked/not blocked */
	u_char		sb	:1; /* substituted/not substituted */
	u_char		nt	:1; /* not topical/topical */
	u_char		iv	:1; /* valid/invalid */
}iec_type7;

//See clause 7.2.6.6
//NVA := F16[1..16]<-1..+1-2^-15>
//Letter F means positive or negative fixed point number
//F16 means 16 bits in size 

//The maximum value of a fixed-point type is 
//simply the largest value that can be represented 
//in the underlying integer type, 
//multiplied by the scaling factor; and similarly for the minimum value. 
//For example, consider a fixed-point type represented as a binary 
//integer with b=16 bits in two's complement format, 
//with a scaling factor of 1/2^f where f = 15 
//(that is, the last f=15 bits are fraction bits): 
//the minimum representable value is -2^(b-1)/2^f = -1 and
//the maximum value is (2^(b-1)-1)/2^f = +1-2^-15 .

/* M_ME_NA_1 - normalized measured value */
typedef struct _iec_type9 {
	short		mv;	/* normalized value: from -1 to +1 */
	u_char		ov	:1; /* overflow/no overflow */
	u_char		res	:3;
	u_char		bl	:1; /* blocked/not blocked */
	u_char		sb	:1; /* substituted/not substituted */
	u_char		nt	:1; /* not topical/topical */
	u_char		iv	:1; /* valid/invalid */
}iec_type9;

//See clause 7.2.6.7
//SVA := I16[1..16]<-2^15..+2^15 - 1>
//Letter I means positive or negative integer number
//I16 means 16 bits in size 

/* M_ME_NB_1 - scaled measured value */
typedef struct _iec_type11 {
	short		mv;	/* scaled value: from -32768 to 32767*/
	u_char		ov	:1; /* overflow/no overflow */
	u_char		res	:3;
	u_char		bl	:1; /* blocked/not blocked */
	u_char		sb	:1; /* substituted/not substituted */
	u_char		nt	:1; /* not topical/topical */
	u_char		iv	:1; /* valid/invalid */
}iec_type11;

/* M_ME_NC_1 - short floating point measured value */
typedef struct _iec_type13 {
	float		mv;
	u_char		ov	:1; /* overflow/no overflow */
	u_char		res	:3;
	u_char		bl	:1; /* blocked/not blocked */
	u_char		sb	:1; /* substituted/not substituted */
	u_char		nt	:1; /* not topical/topical */
	u_char		iv	:1; /* valid/invalid */
}iec_type13;

//BCR See clause 7.2.6.9
//Counter reading := I32[1..32]<-2^31..+2^31 - 1> 

/* M_IT_NA_1 - itegrated totals */
typedef struct _iec_type15 {
	int		count_read;	/* counter reading */ /*BCR = Binary counter reading, defined 7.2.6.9*/
	u_char		sq	:5; /* sequence number */
	u_char		cy	:1; /* carry: 0 = no counter overflow occurred / 1 = counter overflow occurred*/
	u_char		ca	:1; /* 0 = counter was not adjusted / 1 = counter was adjusted */
	u_char		iv	:1; /* 0 = valid/ 1 = invalid */
}iec_type15;

/* M_SP_TB_1 - single point information with quality description and time tag */
typedef struct _iec_type30 {
	u_char		sp	:1; /* single point information */
	u_char		res	:3;
	u_char		bl	:1; /* blocked/not blocked */
	u_char		sb	:1; /* substituted/not substituted */
	u_char		nt	:1; /* not topical/topical */
	u_char		iv	:1; /* valid/invalid */
	cp56time2a	time;
}iec_type30;

/* M_DP_TB_1 - double point information with quality description and time tag */
typedef struct _iec_type31 {
	u_char		dp	:2; /* double point information */
	u_char		res	:2;
	u_char		bl	:1; /* blocked/not blocked */
	u_char		sb	:1; /* substituted/not substituted */
	u_char		nt	:1; /* not topical/topical */
	u_char		iv	:1; /* valid/invalid */
	cp56time2a	time;
}iec_type31;

/* M_BO_TB_1 - state and change information bit string and time tag  */
typedef struct _iec_type33 {
	struct iec_stcd	stcd;
	u_char		ov	:1; /* overflow/no overflow */
	u_char		res	:3;
	u_char		bl	:1; /* blocked/not blocked */
	u_char		sb	:1; /* substituted/not substituted */
	u_char		nt	:1; /* not topical/topical */
	u_char		iv	:1; /* valid/invalid */
	cp56time2a	time;
}iec_type33;

/* M_ME_TD_1 - normalized measured value with time tag */
typedef struct _iec_type34 {
	short		mv;	/* normalized value */
	u_char		ov	:1; /* overflow/no overflow */
	u_char		res	:3;
	u_char		bl	:1; /* blocked/not blocked */
	u_char		sb	:1; /* substituted/not substituted */
	u_char		nt	:1; /* not topical/topical */
	u_char		iv	:1; /* valid/invalid */
	cp56time2a	time;
}iec_type34;

/* M_ME_TE_1 - scaled measured value with time tag*/
typedef struct _iec_type35 {
	short		mv;	/* scaled value */
	u_char		ov	:1; /* overflow/no overflow */
	u_char		res	:3;
	u_char		bl	:1; /* blocked/not blocked */
	u_char		sb	:1; /* substituted/not substituted */
	u_char		nt	:1; /* not topical/topical */
	u_char		iv	:1; /* valid/invalid */
	cp56time2a	time;
}iec_type35;

/* M_ME_TF_1 - short floating point measurement value and time tag */
typedef struct _iec_type36 {
	float		mv;
	u_char		ov	:1; /* overflow/no overflow */
	u_char		res	:3;
	u_char		bl	:1; /* blocked/not blocked */
	u_char		sb	:1; /* substituted/not substituted */
	u_char		nt	:1; /* not topical/topical */
	u_char		iv	:1; /* valid/invalid */
	cp56time2a	time;
}iec_type36;

/* M_IT_TB_1 - Integrated totals with time tag */
typedef struct _iec_type37 {
	int		    counter; /*binary counter reading*/
	u_char		seqn:5; /*sequence number*/
	u_char		cy	:1; /*carry*/
	u_char		ca	:1; /*counter was adjusted*/
	u_char		iv	:1; /* valid/invalid */
	cp56time2a	time;
}iec_type37;

/* C_SC_NA_1 - Single command */
typedef struct _iec_type45 {
	u_char		scs:1;//See 7.2.6.15 Qualifier of command
	u_char		res:1;
	u_char		qu:5; //See 7.2.6.26 Qualifier of command
	u_char		se:1;//See 7.2.6.26 Qualifier of command
}iec_type45;

/* C_DC_NA_1 - Double command */
typedef struct _iec_type46 {
	u_char		dcs:2;//See 7.2.6.16 Qualifier of command
	u_char		qu:5;//See 7.2.6.26 Qualifier of command
	u_char		se:1;//See 7.2.6.26 Qualifier of command
}iec_type46;

/* C_SE_NA_1 - Set point command, normalized value */
typedef struct _iec_type48 {
	short		sv;	/* normalized value, See 7.2.6*/
	u_char		ql:7;//See 7.2.6.39 Qualifier of command
	u_char		se:1;//See 7.2.6.39 Qualifier of command
}iec_type48;

/* C_SE_NB_1 - Set point command, scaled value */
typedef struct _iec_type49 {
	short		sv;	/* scaled value, See 7.2.6.7 */
	u_char		ql:7;//See 7.2.6.39 Qualifier of command
	u_char		se:1;//See 7.2.6.39 Qualifier of command
}iec_type49;

/* C_SE_NC_1 - Set point command, short floating point number */
typedef struct _iec_type50 {
	float		sv;	/* short floating point number, See 7.2.6.8*/
	u_char		ql:7;//See 7.2.6.39 Qualifier of command
	u_char		se:1;//See 7.2.6.39 Qualifier of command
}iec_type50;

/* C_BO_NA_1 - Bitstring of 32 bit*/
typedef struct _iec_type51 {
	struct iec_stcd	stcd;
}iec_type51;

/* C_SC_TA_1 - Single command with time tag*/
typedef struct _iec_type58 {
	u_char		scs:1;//See 7.2.6.15 Qualifier of command
	u_char		res:1;
	u_char		qu:5; //See 7.2.6.26 Qualifier of command
	u_char		se:1; //See 7.2.6.26 Qualifier of command
	cp56time2a	time; //contains the UTC time
}iec_type58;

/* C_DC_TA_1 - Double command with time tag*/
typedef struct _iec_type59 {
	u_char		dcs:2;//See 7.2.6.16 Qualifier of command
	u_char		qu:5; //See 7.2.6.26 Qualifier of command
	u_char		se:1; //See 7.2.6.26 Qualifier of command
	cp56time2a	time; //contains the UTC time
}iec_type59;

/* C_SE_TA_1 - Set point command, normalized value */
typedef struct _iec_type61 {
	short		sv;	/* normalized value, See 7.2.6*/
	u_char		ql:7;//See 7.2.6.39 Qualifier of command
	u_char		se:1;//See 7.2.6.39 Qualifier of command
	cp56time2a	time; //contains the UTC time
}iec_type61;

/* C_SE_TB_1 - Set point command, scaled value */
typedef struct _iec_type62 {
	short		sv;	/* scaled value, See 7.2.6.7 */
	u_char		ql:7;//See 7.2.6.39 Qualifier of command
	u_char		se:1;//See 7.2.6.39 Qualifier of command
	cp56time2a	time; //contains the UTC time
}iec_type62;

/* C_SE_TC_1 - Set point command, short floating point number */
typedef struct _iec_type63 {
	float		sv;	/* short floating point number, See 7.2.6.8*/
	u_char		ql:7;//See 7.2.6.39 Qualifier of command
	u_char		se:1;//See 7.2.6.39 Qualifier of command
	cp56time2a	time; //contains the UTC time
}iec_type63;

/* C_BO_TA_1 - Bitstring of 32 bit command with time tag*/
typedef struct _iec_type64 {
	struct iec_stcd	stcd;
	cp56time2a	time; //contains the UTC time
}iec_type64;

/* M_EI_NA_1 - End of initialization*/
typedef struct _iec_type70 {
	u_char		coi_ui7:7; /*cause of initialisation*/
	u_char		coi_bs1:1; /*cause of initialisation*/
}iec_type70;

/* C_IC_NA_1 - Interrogation command*/
typedef struct _iec_type100 {
	u_char		qoi; /*qualifier of interrogation*/
}iec_type100;

/* C_CS_NA_1 - Clock synchronisation command*/
typedef struct _iec_type103 {
	cp56time2a	time;
}iec_type103;

/* C_TS_NA_1 - Test command*/
typedef struct _iec_type104 {
	u_short fbp; /*fixed test pattern*/
}iec_type104;

/* C_TS_TA_1 - Test command with time tag*/
typedef struct _iec_type107 {
	u_short fbp; /*fixed test pattern*/
	cp56time2a	time;
}iec_type107;

/* C_RP_NA_1 - Reset process command*/
typedef struct _iec_type105 {
	u_char		qrp; /*qualifier of reset process command*/
}iec_type105;

/* C_CD_NA_1 - Delay acquisition command*/
typedef struct _iec_type106 {
	cp16time2a	two_oct_time; /*two octect binary time cfr 7.2.6.20*/
}iec_type106;

/* F_FR_NA_1 - File ready*/
typedef struct _iec_type120 {
	u_short		nof; /*name of file cfr.7.2.6.33*/
	u_int		lof:24; /*length of file cfr.7.2.6.35*/
	u_int		frq_ui7:7; /*file ready qualifier cfr.7.2.6.28*/
	u_int		frq_bs1:1; /*file ready qualifier cfr.7.2.6.28*/
}iec_type120;

/* F_SR_NA_1 - Section ready*/
typedef struct _iec_type121 {
	u_short		nof; /*name of file cfr.7.2.6.33*/
	u_char		nos; /*name of section cfr.7.2.6.34*/
	u_int		lof:24; /*length of file cfr.7.2.6.35*/
	u_int		srq_ui7:7; /*section ready qualifier cfr.7.2.6.29*/
	u_int		srq_bs1:1; /*section ready qualifier cfr.7.2.6.29*/
}iec_type121;

/* F_SC_NA_1 - Call directory, select file, call file, call section*/
typedef struct _iec_type122 {
	u_short		nof; /*name of file cfr.7.2.6.33*/
	u_char		nos; /*name of section cfr.7.2.6.34*/
	u_char		scq_ui4low:4; /*select and call qualifier cfr.7.2.6.30*/
	u_char		scq_ui4high:4; /*select and call qualifier cfr.7.2.6.30*/
}iec_type122;

/* F_LS_NA_1 - Last section, last segment*/
typedef struct _iec_type123 {
	u_short		nof; /*name of file cfr.7.2.6.33*/
	u_char		nos; /*name of section cfr.7.2.6.34*/
	u_char		lsq; /*last section or segment qualifier cfr.7.2.6.31*/
	u_char		chs; /*checksum cfr.7.2.6.37*/
}iec_type123;

/* F_AF_NA_1 - ACK file, ACK section*/
typedef struct _iec_type124 {
	u_short		nof; /*name of file cfr.7.2.6.33*/
	u_char		nos; /*name of section cfr.7.2.6.34*/
	u_char		afq_ui4low:4; /*ACK file or section qualifier cfr.7.2.6.32*/
	u_char		afq_ui4high:4; /*ACK file or section qualifier cfr.7.2.6.32*/
}iec_type124;

/* F_SG_NA_1 - Segment*/
typedef struct _iec_type125 {
	u_short		nof; /*name of file cfr.7.2.6.33*/
	u_char		nos; /*name of section cfr.7.2.6.34*/
	u_char		los; /*Length of segment cfr.7.2.6.36*/
}iec_type125;

/* F_DR_TA_1 - Directory. E' paccata ! It is packed !*/
typedef struct _iec_type126 {
	u_short		nof; /*name of file or subdirectory cfr.7.2.6.33*/
	u_int		lof:24; /*length of file cfr.7.2.6.35*/
	u_int		sof_status:5; /*Status of file cfr.7.2.6.38*/
	u_int		sof_lfd:1; /*last file of the directory cfr.7.2.6.38*/
	u_int		sof_for:1; /*Name defines file or directory cfr.7.2.6.38*/
	u_int		sof_fa:1; /*file active or waits cfr.7.2.6.38*/
	cp56time2a	time; /*Creation time of the file*/
}iec_type126;

/////////////////////NOT IEC TYPES///custom SCADA types////////

/* M_ME_TN_1 - long floating point measurement value and time tag */
typedef struct _is_type150 {
	double		mv;
	u_char		ov	:1; /* overflow/no overflow */
	u_char		res	:3;
	u_char		bl	:1; /* blocked/not blocked */
	u_char		sb	:1; /* substituted/not substituted */
	u_char		nt	:1; /* not topical/topical */
	u_char		iv	:1; /* valid/invalid */
	cp56time2a	time;
}is_type150;

/* M_ME_TO_1 - 32 bit unsigned int measurement value and time tag */
typedef struct _is_type151 {
	u_int		mv;
	u_char		ov	:1; /* overflow/no overflow */
	u_char		res	:3;
	u_char		bl	:1; /* blocked/not blocked */
	u_char		sb	:1; /* substituted/not substituted */
	u_char		nt	:1; /* not topical/topical */
	u_char		iv	:1; /* valid/invalid */
	cp56time2a	time;
}is_type151;

/* M_ME_TP_1 - 32 bit signed int measurement value and time tag */
typedef struct _is_type152 {
	int			mv;
	u_char		ov	:1; /* overflow/no overflow */
	u_char		res	:3;
	u_char		bl	:1; /* blocked/not blocked */
	u_char		sb	:1; /* substituted/not substituted */
	u_char		nt	:1; /* not topical/topical */
	u_char		iv	:1; /* valid/invalid */
	cp56time2a	time;
}is_type152;

/* M_ME_TQ_1 - 16 bit unsigned short measurement value and time tag */
typedef struct _is_type153 {
	u_short		mv;
	u_char		ov	:1; /* overflow/no overflow */
	u_char		res	:3;
	u_char		bl	:1; /* blocked/not blocked */
	u_char		sb	:1; /* substituted/not substituted */
	u_char		nt	:1; /* not topical/topical */
	u_char		iv	:1; /* valid/invalid */
	cp56time2a	time;
}is_type153;

/* M_ME_TR_1 - 64 bit unsigned int measurement value and time tag */
typedef struct _is_type154 {
    u_int64     mv;
	u_char		ov	:1; /* overflow/no overflow */
	u_char		res	:3;
	u_char		bl	:1; /* blocked/not blocked */
	u_char		sb	:1; /* substituted/not substituted */
	u_char		nt	:1; /* not topical/topical */
	u_char		iv	:1; /* valid/invalid */
	cp56time2a	time;
}is_type154;

/* M_ME_TS_1 - 64 bit signed int measurement value and time tag */
typedef struct _is_type155 {
    int64	    mv;
	u_char		ov	:1; /* overflow/no overflow */
	u_char		res	:3;
	u_char		bl	:1; /* blocked/not blocked */
	u_char		sb	:1; /* substituted/not substituted */
	u_char		nt	:1; /* not topical/topical */
	u_char		iv	:1; /* valid/invalid */
	cp56time2a	time;
}is_type155;

/* M_ME_TT_1 - null terminated C string (7 ascii chars + \0) and time tag */
typedef struct _is_type156 {
	char 		str[8];
	u_char		ov	:1; /* overflow/no overflow */
	u_char		res	:3;
	u_char		bl	:1; /* blocked/not blocked */
	u_char		sb	:1; /* substituted/not substituted */
	u_char		nt	:1; /* not topical/topical */
	u_char		iv	:1; /* valid/invalid */
	cp56time2a	time;
}is_type156;

#if defined( _MSC_VER)          /* Microsoft C */
    #pragma pack()              /* Byte Alignment   */
#endif

#endif