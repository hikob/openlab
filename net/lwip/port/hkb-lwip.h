#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include <stdint.h>

/**
 * @brief Runtime TCP/IP settings.
 */
struct lwip_config {
  uint8_t       *macaddress;
  uint32_t      address;
  uint32_t      netmask;
  uint32_t      gateway;
};

/**
 * Setup the ethernet interface, and all the lwIP stack
 *
 * \param config an optional pointer to the configuration to use
 */
void hkb_lwip_init(struct lwip_config *config);


#endif 
