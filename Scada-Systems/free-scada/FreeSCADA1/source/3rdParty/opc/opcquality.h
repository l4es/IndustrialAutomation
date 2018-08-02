#pragma once

#define Q_BAD			(0x00 << 6)
#define Q_UNCERTAIN		(0x01 << 6)
#define Q_GOOD			(0x03 << 6)

#define Q_BAD_UNKNOWN			Q_BAD|(0x00 << 2)
#define Q_BAD_CONFIG_ERROR		Q_BAD|(0x01 << 2)
#define Q_BAD_NOT_CONNECTED		Q_BAD|(0x02 << 2)
#define Q_BAD_DEVICE_FAILURE	Q_BAD|(0x03 << 2)
#define Q_BAD_LAST_KNOWN_VALUE	Q_BAD|(0x05 << 2)
#define Q_BAD_COMM_FAILURE		Q_BAD|(0x06 << 2)

#define Q_UNCERTAIN_UNKNOWN		Q_UNCERTAIN|(0x00 << 2)