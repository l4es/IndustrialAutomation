/*
 * config.h
 *
 * Contains global defines to configure the stack
 *
 *  Created on: 29.06.2012
 *      Author: mzillgit
 */

#ifndef STACK_CONFIG_H_
#define STACK_CONFIG_H_

/* print debugging information to stdout */
#define DEBUG 0

#define STATIC_ALLOCATION 0

#define STATIC_MODEL 1

/* number of data sets per LN if static option is set */
#define MMS_ALLOW_CLIENTS_TO_CREATE_ASSOCIATION_SPECIFIC_DATASETS 0
#define MMS_NUMBER_OF_ASSOCIATION_SPECIFIC_DATASETS 10 /* -1 for no limit */
#define MMS ALLOW_CLIENTS_TO_CREATE_DOMAIN_DATASETS 1
#define MMS_NUMBER_OF_DOMAIN_DATASETS 10 /* -1 for no limit */

/* number of concurrent TCP connections, -1 for no limit */
#define NUMBER_OF_TCP_CONNECTIONS -1

/* activate TCP keep alive mechanism. 1 -> activate */
#define CONFIG_ACTIVATE_TCP_KEEPALIVE 1

/* time between last message and first keepalive message */
#define CONFIG_TCP_KEEPALIVE_IDLE 20

/* time between subsequent keepalive messages if no ack received */
#define CONFIG_TCP_KEEPALIVE_INTERVAL 5

/* number of not missing keepalive responses until socket is considered dead */
#define CONFIG_TCP_KEEPALIVE_CNT 3

/* timeout while reading from TCP stream in ms */
#define CONFIG_TCP_READ_TIMEOUT_MS 1000

/* Enable datasets */
#define CONFIG_DATASETS 1

/* Enable reporting - Note reporting also includes datasets */
#define CONFIG_REPORTING 0

/* Definition of supported services */
#define MMS_DEFAULT_PROFILE 1

#if MMS_DEFAULT_PROFILE
#define MMS_READ_SERVICE 1
#define MMS_WRITE_SERVICE 1
#define MMS_GET_NAME_LIST 1
#define MMS_GET_VARIABLE_ACCESS_ATTRIBUTES 1
#define MMS_DATA_SET_SERVICE 1
#endif /* MMS_DEFAULT_PROFILE */

#endif /* STACK_CONFIG_H_ */
