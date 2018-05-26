Understanding PDO's
===================

This guide is a quick explanation of EtherCAT data types.  
It explains pdos, sync manager, and domains.

Read [EtherLab documentation]. Also see ethercat-1.5.2/include/ecrt.h  

**PDO Listing**

The process data objects (PDOs) are data transferred by an EtherCAT slave.  
The PDO can be an input or output packet, updated each cycle.  

* Input  PDO: the slave will update the data, for the application to read.  
* Output PDO: the application will update the data, for the slave to read.  

The PDO listings for the slave (channels_ in Class simplecat::Slave)  
includes all possible PDOs available for a slave device.    
*See EtherCat Master Commands : cstruct* for obtaining PDOs  

```c
typedef struct {
    uint16_t index; 		/**< PDO entry index. */
    uint8_t subindex; 	    /**< PDO entry subindex. */
    uint8_t bit_length; 	/**< Size of the PDO entry in bit. */
} ec_pdo_entry_info_t;
```


**PDO Selection**

The PDOs that will be synced by the slave (pdos_ in class simplecat::Slave)  
This is a subset of the PDO listing. Only those selected can by synced.  
PDOs that are selected/synced are only updated by the device  
if they are in the domain that cycle. 
*See EtherCat Master Commands : cstruct* for obtaining default selections  

* each ec_pdo_info_t corresponds to a contiguous block of pdos listings
* index is the index of the entry in the array ec_pdo_entry_info_t* entries
* n_entries is the number of entries to include starting from index
* all entries in a pdos_ should be the same type (input or output)
* create multiple pdos_, if the slave should sync multiple types

For example, to select channels_[0], channels_[3], and channels_[4]:  
```c
pdos_  = {{0,1,channels_}, {3,2,channels_}},  
```

```c
typedef struct {
    uint16_t index;                /**< PDO index. */
    unsigned int n_entries;        /**< Number of PDO entries in \a entries to map.
                                        Zero means, that the default mapping shall be
                                        used (this can only be done if the slave is
                                        present at bus configuration time). */
    ec_pdo_entry_info_t *entries;  /**< Array of PDO entries to map. Can either
                                        be \a NULL, or must contain at
                                        least \a n_entries values. */
} ec_pdo_info_t;
```

**Sync Manager**

There are generally 4 sync managers, (syncs_ in SimplECAT::Slave)  
that can be used to sync the pdo selections.  
In general, use one sync manager for all input PDOs, and one for all output PDOs.  
The others can be left out (if no input or output PDOs, then leave that out too)  
Explanation from [Atrias EtherCat Setup]:  
*Although any Sync manager can be configured to behave in any way, by EtherCAT convention*  
*sync managers 0 and 1 should be configured for RX and TX mailbox mode,*  
*and 2 and 3 should be configured for RX and TX buffered mode”*  
*See EtherCat Master Commands : cstruct* for obtaining default sync managers    

* index is the index of the entry in the array ec_pdo_info_t* pdos
* n_pdos is the number of entries to include starting from index
* terminate array with {0xff}

For example, to sync both entries in pdos_, assuming both are outputs:
```c
syncs_  = {{2, EC_DIR_OUTPUT, 2, pdos_ + 0, EC_WD_ENABLE}, {0xff}}  
```

```c
typedef struct {
    uint8_t index;                     /**< Sync manager index. Must be less
                                            than #EC_MAX_SYNC_MANAGERS for a valid sync manager,
                                            but can also be \a 0xff to mark the end of the list. */
    ec_direction_t dir;                /**< Sync manager direction. */
    unsigned int n_pdos;               /**< Number of PDOs in \a pdos. */
    ec_pdo_info_t *pdos;               /**< Array with PDOs to assign. This must contain
                                            at least \a n_pdos PDOs. */
    ec_watchdog_mode_t watchdog_mode;  /**< Watchdog mode. */
} ec_sync_info_t;
```

**Domain**

The domain, i.e. packet, that will be passed around from the master to the slaves.  
It has a size limit of 1484 bytes, but this might be automatically handled.  
It should be a subset of what the slaves are synced to handle,  
The PDO listings, selection, and syncs are all set at initialization.  
However, the domain can be selected every cycle. This allows:  

* Certain data to be queried periodically (such as status data).  
* More data to be synced, by using multiple domains  

*See EtherCat Master Commands : slaves* for obtaining alias and position  

* alias and position are specific to the slave device   
* SimplECAT allows easy assignment of PDOs to domain(s).  
* simplecat::Master::addSlave() populates ec_pdo_entry_reg_t  
* bit position is incorporated, but corrently not used.  

```c
typedef struct {
    uint16_t alias;              /**< Slave alias address. */
    uint16_t position;           /**< Slave position. */
    uint32_t vendor_id;          /**< Slave vendor ID. */
    uint32_t product_code;       /**< Slave product code. */
    uint16_t index;              /**< PDO entry index. */
    uint8_t subindex;            /**< PDO entry subindex. */
    unsigned int *offset;        /**< Pointer to a variable to store the PDO entry's
                                      (byte-)offset in the process data. */
    unsigned int *bit_position;  /**< Pointer to a variable to store a bit
                                      position (0-7) within the \a offset. Can be
                                      NULL, in which case an error is raised if the
                                      PDO entry does not byte-align. */
} ec_pdo_entry_reg_t;
```

**Assigning PDOs to Domains**

SimplECAT lets a user specify the PDOs in each domain by index.  
This mapping must be specified for each slave.

```c++
// map<domain index, vector<channels_ indices> >
typedef std::map<unsigned int, std::vector<unsigned int> > DomainMap;
```

For example, to assign PDOs of a slave to the following domains:  

* PDOs at i=(0,4,8,12) in the PDO Listing (channels_[i]) -> domain 0.  
* PDOs at i=(1,5,9,13) in the PDO Listing (channels_[i]) -> domain 1.

```c++
DomainMap domains_ = {
    {0, {0,4,8,12} },
    {1, {1,5,9,13} }
};
```

[EtherLab documentation]:http://etherlab.org/download/ethercat/ethercat-1.5.2.pdf  


