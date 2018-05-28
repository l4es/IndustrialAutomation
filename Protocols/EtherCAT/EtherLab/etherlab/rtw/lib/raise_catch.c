#include <linux/list.h>         /* Whatever for ;-) */
#include <linux/errno.h>        /* Error numbers */
#include <linux/slab.h>         /* kmalloc() */

#include "raise_catch.h"

LIST_HEAD(raise_list);
LIST_HEAD(catch_list);

unsigned int *catch_buf, *catch_buf2;
unsigned int catch_buf_size;
unsigned int update;

struct raise_data {
    /** Here is the list of all raise functions */
    struct list_head list;

    /** The id of this raise */
    const char *id;

    /** The message that is printed when raise is activated */
    const char *msg;

    /** Whether the raise can be reset automatically or has to be done
     * by the user */
    unsigned int auto_reset;

    /** Width of data vector */
    unsigned int width;

    /** How the raise message is formatted in printk() */
    char *formatstr;

    /** Pointer to the address private data that has to be sent to 
     * this module with every call */
    void **priv_data;
};


void
abort_register(void)
{
    struct raise_data *raise_data, *i1;
//    struct catch_data *catch_data, *i2;

    list_for_each_entry_safe(raise_data, i1, &raise_list, list)
        kfree(raise_data);

//    list_for_each_entry_safe(catch_data, i2, &catch_list, list)
//        kfree(catch_data);
}

void
raise_catch_end(void)
{
    abort_register();
}

void
etl_raise(void *data, int offset, int state)
{
    struct raise_data *raise_data = data;
    
    if (state) {
        printk(raise_data->formatstr,
                raise_data->id, offset, raise_data->msg);
    } else if (raise_data->auto_reset) {
    }
}

const char *
raise_catch_start(void)
{
    struct raise_data *raise_data, *i1;

    list_for_each_entry_safe(raise_data, i1, &raise_list, list)
        *raise_data->priv_data = raise_data;

    return NULL;
}

/**
 * etl_register_raise - Register a raise call
 * @id: Name of raise
 * @msg: Message that is printed when raising
 * @width: vector width
 * @priv_data: Data structure to be passed when calling @raise
 *
 * Whenever a new raise block is called to initialise (in mdl_startup),
 * it uses this routine to register itself with the raise-catch manager.
 * This function must be called before calling activate_catch_raise.
 */
const char *
etl_register_raise(
        const char *id, 
        const char *msg, 
        unsigned int auto_reset,
        unsigned int width,
        void **priv_data)
{
    struct raise_data *data, *p;
    
    /* Insert the new raise in the sorted list */
    list_for_each_entry(p, &raise_list, list) {
        if (strncmp(p->id, id, min(strlen(id), strlen(p->id))) == 0) {
            /* Complain if the id is already allocated */
            return "Raise Id duplicated";
        }
    }

    data = kmalloc( sizeof(struct raise_data), GFP_KERNEL);
    if (!data) {
        abort_register();
        return "No memory";
    }
    data->id = id;
    data->msg = msg;
    data->formatstr = (msg[strlen(msg)-1] == '\n') ? 
        "%s[%i]: %s" : "%s[%i]: %s\n";
    data->auto_reset = auto_reset;
    data->width = width;
    data->priv_data = priv_data;

    list_add_tail(&data->list, &raise_list);
    return NULL;
}
