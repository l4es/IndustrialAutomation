/*****************************************************************************
 *
 *  $Id$
 *
 *  Copyright 2010 Richard Hacker (lerichi at gmx dot net)
 *
 *  This file is part of the pdserv library.
 *
 *  The pdserv library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or (at
 *  your option) any later version.
 *
 *  The pdserv library is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the pdserv library. If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#include "pdserv.h"
#include <stdint.h>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <cerrno>
#include <sys/mman.h>

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

uint16_t var1[2][3][4] = {
    { {1,2,3,4}, {5,6,7}, },
};

struct s1 {
    double d[5];
    char c;
};

struct s2 {
    struct s1 s1[2];
    int16_t i16;
};

int gettime(struct timespec *t)
{
    return clock_gettime(CLOCK_REALTIME, t);
}

int copy_param(const struct pdvariable *,
        void* dst, const void* src, size_t len,
        struct timespec* time,
        void *priv_data)
{
    memcpy(dst, src, len);
    printf("hallo %p\n", priv_data);
    if (time)
        gettime(time);
    return 0;
}

void clear_event(const struct pdevent* /*event*/, size_t /*index*/,
        char /*state*/, void * /*priv_data*/)
{
    printf("%s", __func__);
}

int main(int argc, const char *argv[])
{
    struct pdserv *pdserv =
        pdserv_create(program_invocation_short_name, "1.0", gettime);
    size_t var1_dims[] = {2,3,4};
    struct timespec time;
    double dbl[3] = { 13,14,15};
    double dbltime;
    double tick = 10;
    uint32_t param[] = {1,2,3,4};
    struct pdtask *task[3];
    struct pdevent *event;

    struct s1 s1 = {{1,2,3,4,5}, 'a'};
    struct s2 s2[2] = {
        {
            {
                {{6,7,8,9,10},'b'},
                {{11,12,13,14,15},'c'}
            },
            -123
        },
        {
            {
                {{6,7,8,9,10},'b'},
                {{11,12,13,14,15},'c'}
            },
            -124
        },
    };

    int s1_t = pdserv_create_compound("s1", sizeof(s1));
    int s2_t = pdserv_create_compound("s2", sizeof(struct s2));

    pdserv_compound_add_field(s1_t, "d",
            pd_double_T, offsetof(struct s1, d), 5, NULL);
    pdserv_compound_add_field(s1_t, "c",
            pd_sint8_T, offsetof(struct s1, c), 1, NULL);

    pdserv_compound_add_field(s2_t, "s1", s1_t,
            offsetof(struct s2, s1), 2, NULL);
    pdserv_compound_add_field(s2_t, "i16", pd_sint16_T,
            offsetof(struct s2, i16), 1, NULL);

    if (argc > 1)
        pdserv_config_file(pdserv, argv[1]);

    task[0] = pdserv_create_task(pdserv, 0.1, "Task1");

    assert(pdserv_signal(task[0], 1, "/s1", s1_t, &s1, 1, NULL, NULL, NULL));
    assert(pdserv_signal(task[0], 1, "/s2", s2_t, s2, 2, NULL, NULL, NULL));

    event = pdserv_event(pdserv,"/Event/path",WARN_EVENT,5);

    assert(pdserv_signal(task[0], 1, "/path<lksjdf/to>ljk/double",
            pd_double_T, dbl, 3, NULL, NULL, NULL));

    assert(pdserv_signal(task[0], 1, "/Tme",
            pd_double_T, &dbltime, 1, NULL, NULL, NULL));

    assert(pdserv_signal(task[0], 1, "/path //to/ var2",
            pd_uint16_T, var1, 3, var1_dims, NULL, NULL));

    struct pdvariable *p3 = pdserv_parameter(pdserv, "/path //to/ mdimparam",
            0666, pd_uint16_T, var1, 3, var1_dims, copy_param, (void*) 10);
    assert(p3);

    struct pdvariable *p1 = pdserv_parameter(pdserv,
            "/Taskinfo/Abtastfrequenz", 0666, pd_double_T, &tick, 1, 0, 0, 0);
    assert(p1);

    struct pdvariable *p = pdserv_parameter(pdserv, "/path/to<hide>/param", 0666,
            pd_uint32_T, param, 4, 0, copy_param, (void*)10);
    assert(p);

    struct pdvariable *p4 = pdserv_parameter(pdserv, "/path /to<persistent>/param",
            0666, pd_uint16_T, var1, 2, 0, 0, 0);
    assert(p4);

    struct pdvariable *p5 = pdserv_parameter(pdserv, "/path /to<persistent>/param/p1<persistent=\"0\">/Value",
            0666, pd_uint16_T, var1, 2, 0, 0, 0);
    assert(p5);

    assert(!pdserv_prepare(pdserv));

    // Need to be root
    //assert(!mlockall(MCL_CURRENT|MCL_FUTURE));

    int i;
    while (1) {
        //sleep(1);
        usleep(100000);
        clock_gettime(CLOCK_REALTIME, &time);

        var1[1][0][0] = i++;
        if (!(i%30))
            var1[1][0][0]++;
        dbl[0] += param[0];
        dbl[2] += param[2];
        var1[1][0][1] += param[1];
        dbltime = time.tv_sec + time.tv_nsec * 1.0e-9;
        pdserv_update_statistics(task[0], 10, 20, 30);
        pdserv_update(task[0], &time);

        if (!(i%30)) {
            pdserv_event_set(event, 2, 1, &time);
//            printf("sett event\n");
        }
    }

    pdserv_exit(pdserv);

    return 0;
}
