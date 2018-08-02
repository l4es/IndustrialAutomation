#ifndef NETWORK_COMMON_H
#define NETWORK_COMMON_H

#include "oapc_libio.h"

#define MAX_IP_SIZE       100
#define MAX_CMDVAL_SIZE   250
#define MAX_AUTH_SIZE      20
#define MAX_SENDBUF_SIZE MAX_CMDVAL_SIZE+MAX_CMDVAL_SIZE+MAX_AUTH_SIZE+MAX_AUTH_SIZE+1

#define MAX_SEND_TIMEOUT 500
#define MAX_RECV_TIMEOUT 500

struct libio_config
{
   unsigned short version,length;
   char           ip[MAX_IP_SIZE];
   unsigned short port,incoming;
   char           uname[MAX_AUTH_SIZE],pwd[MAX_AUTH_SIZE];
   unsigned short mode;
};

struct cmd_value_pair
{
   unsigned char  cmd_set,val_char_set,val_num_set,val_digi_set,val_bin_set;
   char           cmd[MAX_CMDVAL_SIZE];
   char           val_char[MAX_CMDVAL_SIZE];
   double         val_num;
   char           val_digi;
   oapc_bin_head *val_bin;
};


struct client_dataset
{
   int            sock;
   unsigned int   loaded;
   unsigned short mode;
   char           buffer[MAX_SENDBUF_SIZE];
   char           uname[MAX_AUTH_SIZE],pwd[MAX_AUTH_SIZE];

   unsigned char  val_char_set,val_cmd_set,val_num_set,val_digi_set,val_bin_set,send_cmd;
   char           val_char[MAX_CMDVAL_SIZE],val_cmd[MAX_CMDVAL_SIZE];
   float          val_num;
   char           val_digi;
   oapc_bin_head *val_bin;
   int            lastBinError;
};


struct cust_instance_data // custom data for some plug-ins that also use the network code
{
#ifdef CUST_DATA_SAMCCI
	int    cmd;
	double X,Y,A;
	bool   on;
	char   name[MAX_CMDVAL_SIZE+1],text[MAX_CMDVAL_SIZE+1];
	bool   retOK;
	double retA;
#else
   char   pad;
#endif
};



struct instData
{
   struct libio_config       config;
   struct cmd_value_pair     cmdValuePair;
   struct client_dataset     client[MAX_INCOMING_CONNECTIONS];
   int                       sock;
   int                       m_callbackID;
   bool                      running;
   struct cust_instance_data cust;
};



bool set_config_data(struct instData* data,const char *name,const char *value);
void convert_to_save_byteorder(struct libio_config *save_config,struct libio_config *current_config);
void convert_from_save_byteorder(struct libio_config *current_config,struct libio_config *save_config);
void init_instancedata(struct instData *data,unsigned short port);
int  check_clients(struct instData *data,struct client_dataset *client,bool checkAuth);
void check_cmd_value_pair(struct instData *data);
void convert_bin_to_network_byteorder(struct oapc_bin_head *net,struct oapc_bin_head *host);
void convert_bin_to_host_byteorder(struct oapc_bin_head *host,struct oapc_bin_head *net);


#endif
