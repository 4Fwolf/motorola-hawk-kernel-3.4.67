#ifndef RCV_CUST_KEYS_H
#define RCV_CUST_KEYS_H

#include <linux/input.h>

#define RECOVERY_KEY_DOWN     KEY_VOLUMEDOWN
#define RECOVERY_KEY_UP       KEY_VOLUMEUP
//<2012/11/22-17485-stevenchen, [Common] Modify boot mode menu UI and sync factory & recovery mode UI.
#define RECOVERY_KEY_ENTER    KEY_POWER		//KEY_MENU
#define RECOVERY_KEY_MENU     KEY_VOLUMEUP	//KEY_HOMEPAGE
//>2012/11/22-17485-stevenchen

#endif /* RCV_CUST_KEYS_H */
