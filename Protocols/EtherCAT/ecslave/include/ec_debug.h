#ifndef __EC_DEBUG_H__
#define __EC_DEBUG_H__

#define EC_NODE_INFO(node, fmt, args...) \
    printk(KERN_INFO "EtherCAT %u: " fmt, node->index, ##args)

/** Convenience macro for printing node-specific errors to syslog.
 *
 * This will print the message in \a fmt with a prefixed "EtherCAT <INDEX>: ",
 * where INDEX is the node index.
 *
 * \param node EtherCAT node
 * \param fmt format string (like in printf())
 * \param args arguments (optional)
 */
#define EC_NODE_ERR(node, fmt, args...) \
    printk(KERN_ERR "EtherCAT ERROR %u: " fmt, node->index, ##args)

/** Convenience macro for printing node-specific warnings to syslog.
 *
 * This will print the message in \a fmt with a prefixed "EtherCAT <INDEX>: ",
 * where INDEX is the node index.
 *
 * \param node EtherCAT node
 * \param fmt format string (like in printf())
 * \param args arguments (optional)
 */
#define EC_NODE_WARN(node, fmt, args...) \
    printk(KERN_WARNING "EtherCAT WARNING %u: " fmt, node->index, ##args)

/** Convenience macro for printing node-specific debug messages to syslog.
 *
 * This will print the message in \a fmt with a prefixed "EtherCAT <INDEX>: ",
 * where INDEX is the node index.
 *
 * \param node EtherCAT node
 * \param fmt format string (like in printf())
 * \param args arguments (optional)
 */
#define EC_NODE_DBG(node, level, fmt, args...) \
    do { \
        if (node->debug_level >= level) { \
            printk(KERN_DEBUG "EtherCAT DEBUG %u: " fmt, \
                    node->index, ##args); \
        } \
    } while (0)


void ec_print_data(const uint8_t *data, /**< pointer to data */
                   size_t size /**< number of bytes to output */
                   );

ssize_t ec_mac_print(
        const uint8_t *mac, /**< MAC address */
        char *buffer /**< target buffer */
        );

#endif
