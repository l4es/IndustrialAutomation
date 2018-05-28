
  Process-Data Communications Server Library (pdserv)

  Copyright 2010 - 2012  Richard Hacker (lerichi at gmx dot net)

  This file is part of the pdserv library.

  The pdserv library is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or (at your
  option) any later version.

  The pdserv library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with the pdserv library. If not, see <http://www.gnu.org/licenses/>.

  vim: spelllang=en spell

=============================================================================


Motivation

    Suppose that you have written a funky process control program. You have
    lots of input signals, either as a result of sampling field signals or due
    to calculation. Furthermore, these calculations are influenced by
    parameters that you would like to change from time to time.

    These types of programs typically run in real time. A real time process is
    severely limited in the system resources it is allowed to use. Practically
    all system calls involve memory transfers of some sort. New memory space
    has to be allocated, which means that the calling process will be blocked
    for a short while.
    
    For desktop users, this is hardly noticeable, except when swap space is
    being used. For real time applications, this is a show stopper!
    
    So how can you interact with the process without using system calls? How
    can you see what your signals are doing without using some sort of network
    communication?

Description

    This library allows you to do just that without using system calls in the
    context of your application! Of course it uses system calls, otherwise it
    would be rather introverted. These system calls are done in a separate
    communications process that is forked off during initialization. Any
    blocking calls to system functions will thus not influence you original
    application.
    
    The two processes, i. e. your application and the communications process,
    communicate with each other exclusively via shared memory that was
    obtained by using anonymous mmap() (see the manpage) during
    initialisation.
    
    So what prevents this shared memory from being swapped out, thereby
    blocking your process once again? YOU, if you choose! A real time
    application usually calls mlock() somewhere during initialization to tell
    the kernel not to swap out memory to disk. The library does NOT do this
    automatically. First of all, only root is allowed to call mlock().
    Secondly, mlock() is practically called by every real time process
    somewhere anyway. If this is not required, the library will not force you!
    
    This library is not limited to real time processes (in which case you
    would not use mlock() - see above). The only requirement is that an
    update() function is called every time a calculation result is completed.

Concepts

    The library assumes various concepts that need explanation:

    *) Application

        The whole program is a single application, i. e. one and only one
        process in the system's process table. An application consists of one
        or more tasks.

    *) Task

        A task represents a single thread of execution and consists of one or
        more signals that are calculated every time the task executes.  Every
        time a task completes an execution, the library's update() must be
        called, allowing the communications process to copy the newly
        calculated signals to the communications task. For example, you may
        have a slow running task at 10 Hz, but for a particular loop you have
        another running at 1 kHz.

        A task usually implemented as a separate thread running in a
        while(true) loop paced by a timer. Therefore, tasks have a sample time
        property specifying how often the signals are calculated in a second.
        It is the responsibility of your application to ensure that the task's
        timing requirements.  However, the library does not rely on exact
        timing.

    *) Signal

        A signal belongs to a single task.  Signals can have any of the basic
        data types up to 64 bit (8 bytes) and have any dimension. Complex data
        types, such as structures or complex numbers, are not allowed. Its
        value is updated during every run of the task.

        Signals are registered with the library during initialization and
        exist in the space of the task. They are visible via a path property
        that is specified during registration.

        Signals are the results of calculations and are thus read-only by the
        communications process. By using parameters to change the thread of
        execution, signals may be influenced indirectly.

    *) Parameter

        Parameters are similar to signals, but are a property of the
        application rather than that of tasks. The same properties and
        restrictions apply as far as the data type and dimensions are
        concerned.

        Parameters are requested from the library during initialization. The
        application only gets a constant pointer to the parameter, thus
        preventing accidental modification by the application.

        Although parameters exist in the space of the application, the library
        is not aware of any unexpected changes to these. Parameters may only
        be changed upon explicit request of the library.

        During the registration of a parameter, the application can also
        register a callback function that is called when the parameter's value
        is changed by the library. This enables the application to check the
        value before being copied.

Using the Library

    Installing the library will present you a dynamically loadable module and
    a header file to include.

    See the documentation in the pdserv.h as well as the example for
    information to use the library.

    Switches that you need for compiling and linking to the library are
    available using pkg-config command, which reads libpdserv.pc.

        # pkg-config --cflags libpdserv
        # pkg-config --libs libpdserv

    Using the default installation paths (/opt/etherlab), pkg-config will not
    find its specification file using its default values. To help it, call:

        # export PKG_CONFIG_PATH=/opt/etherlab/lib/pkgconfig

    before calling pkg-config.
