/* Master 0, Slave 0, "LIBIX ORDER"
 * Vendor ID:       0x000001ee
 * Product code:    0x0000000e
 * Revision number: 0x00000012
 */

ec_pdo_entry_info_t slave_0_pdo_entries[] = {
    {0x1600, 0x02, 8}, /* RXPDO1 LIBIX */
    {0x1600, 0x01, 32}, /* RXPDO2 LIBIX */
    {0x1a00, 0x02, 32}, /* TXPDO1 LIBIX */
    {0x1a00, 0x01, 16}, /* TXPDO2 LIBIX */
};

ec_pdo_info_t slave_0_pdos[] = {
    {0x1600, 2, slave_0_pdo_entries + 0}, /* LIBIX RX PDO */
    {0x1a00, 2, slave_0_pdo_entries + 2}, /* LIBIX TX PDO */
};

ec_sync_info_t slave_0_syncs[] = {
    {0, EC_DIR_INPUT, 1, slave_0_pdos + 0, EC_WD_DISABLE},
    {1, EC_DIR_OUTPUT, 1, slave_0_pdos + 1, EC_WD_DISABLE},
    {0xff}
};

/* Master 0, Slave 1, "LIBIX ORDER"
 * Vendor ID:       0x000001ee
 * Product code:    0x0000000e
 * Revision number: 0x00000012
 */

ec_pdo_entry_info_t slave_1_pdo_entries[] = {
    {0x1600, 0x02, 8}, /* RXPDO1 LIBIX */
    {0x1600, 0x01, 32}, /* RXPDO2 LIBIX */
    {0x1a00, 0x02, 32}, /* TXPDO1 LIBIX */
    {0x1a00, 0x01, 16}, /* TXPDO2 LIBIX */
};

ec_pdo_info_t slave_1_pdos[] = {
    {0x1600, 2, slave_1_pdo_entries + 0}, /* LIBIX RX PDO */
    {0x1a00, 2, slave_1_pdo_entries + 2}, /* LIBIX TX PDO */
};

ec_sync_info_t slave_1_syncs[] = {
    {0, EC_DIR_INPUT, 1, slave_1_pdos + 0, EC_WD_DISABLE},
    {1, EC_DIR_OUTPUT, 1, slave_1_pdos + 1, EC_WD_DISABLE},
    {0xff}
};

