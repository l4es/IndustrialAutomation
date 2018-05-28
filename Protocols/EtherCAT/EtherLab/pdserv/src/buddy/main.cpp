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

#include "config.h"
#include "../Debug.h"

#include <daemon.h>         // daemon_*()
#include <cerrno>           // errno
#include <sstream>          // ostringstream
#include <sys/types.h>      // wait(), kill()
#include <sys/wait.h>       // wait()
#include <sys/ioctl.h>      // ioctl()
#include <fcntl.h>          // open()
#include <unistd.h>         // getopt(), sleep()
#include <signal.h>         // kill()
#include <cstdlib>          // atoi()
#include <iostream>
#include <fstream>

#include "Main.h"
#include "../Config.h"

int debug = 3;
bool fg = false;
const char* configFile;
const char* default_config = QUOTE(SYSCONFDIR) "/buddy.conf";

/////////////////////////////////////////////////////////////////////////////
void printhelp(const char *exe, const std::string &default_config)
{
    std::cerr << "Usage: " << exe << std::endl
        << "\t-h \t\tHelp" << std::endl
        << "\t-f \t\tStay in foreground (do not fork)" << std::endl
        << "\t-k \t\tKill running daemon and exit" << std::endl
        << "\t-d[level] \tDebug Level (implies foreground)" << std::endl
        << "\t-c <file.conf> \tConfig file (default: "
        << default_config << ')' << std::endl;
}

/////////////////////////////////////////////////////////////////////////////
int kill_daemon()
{
    std::ifstream is(daemon_pid_file_proc_default());
    int pid;

    is >> pid;
    if (is.good()) {
        if (!::kill(-pid, SIGTERM)) {
            time_t timeout = ::time(NULL) + 5;
            do {
                if (::kill(-pid, 0) and errno == ESRCH) {
                    daemon_pid_file_remove();
                    return EXIT_SUCCESS;
                }
                ::usleep(100000);
            } while (::time(NULL) < timeout);

            std::cerr
                << "Failed to react on SIGTERM. Trying SIGKILL..."
                << std::endl;

            ::kill(-pid, SIGKILL);
            timeout = ::time(NULL) + 5;
            do {
                if (::kill(-pid, 0) and errno == ESRCH) {
                    daemon_pid_file_remove();
                    return EXIT_SUCCESS;
                }
                ::usleep(100000);
            } while (::time(NULL) < timeout);

            std::cerr
                << "Failed to react on SIGKILL. Giving up."
                << std::endl;
        }
        else {
            if (errno == ESRCH) {
                std::cerr
                    << "Process group " << pid << " does not exist."
                    << std::endl;
                daemon_pid_file_remove();
            }
            else
                std::cerr << "Failed to terminate daemon: "
                    << strerror(errno)
                    << std::endl;
        }
    }
    else {
        std::cerr << "Failed to read pid from " 
            << daemon_pid_file_proc_default()
            << std::endl;
        daemon_pid_file_remove();
    }

    return EXIT_FAILURE;
}

/////////////////////////////////////////////////////////////////////////////
void parse_command_line (int argc, char **argv)
{
    int opt;

    // Parse command line options
    while ((opt = ::getopt(argc, argv, "d::fkc:h")) != -1) {
        switch (opt) {
            case 'd':
                // debug
                fg = 1;
                debug = optarg ? ::atoi(optarg) : debug+1;
                break;

            case 'k':
                // Kill daemon
                ::exit(kill_daemon());

            case 'f':
                // Foreground
                fg = 1;
                break;

            case 'c':
                if (optarg[0] and ::access(optarg, R_OK)) {
                    int rv = errno;
                    std::cerr << "Error: Cannot read configuration " << optarg
                        << ": " << strerror(rv) << std::endl;
                    ::exit(rv);
                }
                configFile = optarg;
                break;

            default:
                printhelp(daemon_pid_file_ident, default_config);
                ::exit (opt == 'h' ? EXIT_SUCCESS : EXIT_FAILURE);
                break;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
int serve(const std::string& file, const PdServ::Config& config,
        int mainfd)
{
    struct app_properties app_properties;
    int fd, rv;

    const char* const file_cstr = file.c_str();

    // Wait until the file is available
    for (fd = 1; ::access(file_cstr, F_OK); ++fd) {
        rv = -errno;
        daemon_log(LOG_WARNING,
                "Could not access device file %s; Waiting...", file_cstr);
        if (fd > 10)
            goto out;

        ::sleep(1);
    }

    // Open the application's file handle
    fd = ::open(file_cstr, O_NONBLOCK | O_RDONLY);
    if (fd == -1) {
        rv = -errno;
        daemon_log(LOG_ERR,
                "Could not open device %s: %s", file_cstr, strerror(errno));
        goto out;
    }
    daemon_log(LOG_INFO, "Successfully opened %s", file_cstr);

    // Get the properties
    if (::ioctl(fd, GET_APP_PROPERTIES, &app_properties)) {
        rv = -errno;
        daemon_log(LOG_ERR,
                "Could not get process information for %s: %s",
                file_cstr, strerror(errno));
        goto out;
    }

    rv = fork();
    if (!rv) {
        // Child
        daemon_signal_done();
        ::close(mainfd);
        ::exit(Main(app_properties, config[app_properties.name], fd).serve());
    }
    else if (rv < 0) {
        // Error occurred
        daemon_log(LOG_ERR, "fork(): %s", strerror(errno));
    }

    // Parent continuing here

out:
    ::close(fd);

    return rv;
}

#define MAX(a,b) ((a) > (b) ? (a) : (b))

/////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    PdServ::Config config;
    pid_t pid;
    int sigfd, maxfd, rv, etl_main;
    log4cplus::Logger log = log4cplus::Logger::getRoot();
    const char *device_node = "/dev/etl";
    int app[MAX_APPS];
    bool rescan = true;
    std::string path;

    memset(app, 0, sizeof(app));

    daemon_pid_file_ident = daemon_log_ident =
        daemon_ident_from_argv0(argv[0]);

    // First parse command lines
    parse_command_line(argc, argv);

    // Do console logging when in foreground
#if DAEMON_SET_VERBOSITY_AVAILABLE
    if (fg) {
        //daemon_log_use = DAEMON_LOG_SDERR;
        daemon_set_verbosity(debug);
    }
#endif

    if (!fg) {
        // Prepare pipe for daemon to pass return value
        if (daemon_retval_init() < 0) {
            daemon_log(LOG_ERR,
                    "Failed to create communication channel with daemon");
            return errno;
        }

        // Fork off the child
        pid = daemon_fork();
        if (pid < 0) {
            daemon_log(LOG_CRIT, "Failed to fork daemon. Exiting...");
            return errno;
        }
        else if (pid) { /* The parent */
            int rv = daemon_retval_wait(10);

            if (rv < 0) {
                rv = errno;
                daemon_log(LOG_ERR,
                        "Could not receive return value "
                        "from daemon process %i: %s",
                        pid, strerror(errno));
            }
            else if (rv) {
                daemon_log(LOG_ERR,
                        "Daemon process had problems starting: %s",
                        strerror(rv));
            }

            daemon_retval_done();
            return rv;
        }

        /* The daemon */
        daemon_log(LOG_INFO, "Daemon started with pid %i", getpid());
    }

    // Catch SIGCHLD and SIGHUP
    if (daemon_signal_init(SIGCHLD, SIGHUP, -1) < 0) {
        rv = errno;
        daemon_log(LOG_ERR, "Could not install daemon signal handlers");
        goto finished;
    }

    // Use default configuration file if it exists and nothing has been
    // specified on the command line
    if (!configFile and !::access(default_config, R_OK))
        configFile = default_config;

    // Load configuration file
    if (configFile and configFile[0]) {
        const char *err = config.load(configFile);
        if (err) {
            rv = EACCES;
            daemon_log(LOG_ERR, "%s", err);
            goto finished;
        }
        daemon_log(LOG_INFO, "Successfully loaded configuration file %s",
                configFile);
    }

    // Open main etherlab file
    path.append(device_node).append(1, '0');
    etl_main = ::open(path.c_str(), O_NONBLOCK | O_RDWR);
    if (etl_main < 0) {
        rv = errno;
        daemon_log(LOG_ERR, "Could not open main etherlab device %s: %s",
                path.c_str(), strerror(errno));
        goto finished_retval;
    }

    if (!fg) {
        // Remove and create new pid files
        daemon_pid_file_remove();
        if (daemon_pid_file_create() < 0) {
            // Warn in the logger, but otherwise carry on
            daemon_log(LOG_WARNING,
                    "Could not create pid file %s: %s",
                    daemon_pid_file_proc_default(), strerror(errno));
        }

        // Finished all initializing. Can send parent to sleep now
        daemon_retval_send(0);
        daemon_retval_done();
    }

    // Setup file handles for select()
    sigfd = daemon_signal_fd();
    if (sigfd < 0) {
        rv = errno;
        daemon_log(LOG_ERR, "Could not get file descriptor for signals: %s",
                strerror(errno));
        goto finished;
    }
    maxfd = MAX(sigfd,etl_main) + 1;

    while (true) {
        uint32_t apps = 0;

        if (rescan) {
            ::ioctl(etl_main, RTAC_GET_ACTIVE_APPS, &apps);
            for (unsigned app_id = 0;
                    apps and app_id < MAX_APPS; ++app_id, apps >>= 1) {
                if ((apps & 0x01) and !app[app_id]) {
                    // A new application started
                    daemon_log(LOG_INFO, "New application instance %i", app_id);

                    std::ostringstream path;
                    path << device_node << app_id + 1;

                    app[app_id] = serve(path.str(), config, etl_main);
                    if (app[app_id] <= 0) {
                        daemon_log(LOG_ERR,
                                "Could not install buddy "
                                "for real time instance %i", app_id);
                        app[app_id] = 0;
                    }
                }
                else if (!(apps & 0x01) and app[app_id]) {
                    daemon_log(LOG_INFO,
                            "Finished with application instance %i pid %i",
                            app_id, app[app_id]);
                    ::kill(app[app_id], SIGTERM);
                    app[app_id] = 0;
                }
            }

            rescan = false;
        }

        // Wait until the status of a child or that of the main changes
        fd_set fds;
        FD_ZERO(&fds);

        do {
            FD_SET(etl_main, &fds);
            FD_SET(sigfd, &fds);

            if (::select(maxfd, &fds, 0, 0, 0) < 0) {
                // Interrupt by signal handler
                if (errno == EINTR)
                    continue;

                daemon_log(LOG_CRIT, "select() failed: %s", strerror(errno));
                goto finished;
            }

            if (FD_ISSET(sigfd, &fds)) {
                int sig = daemon_signal_next();

                switch (sig) {
                    case SIGCHLD:
                        // One of the children finished
                        pid = wait(&rv);
                        for (size_t i = 0; i < MAX_APPS; ++i) {
                            if (app[i] == pid
                                    and ::kill(pid, 0) and errno == ESRCH) {
                                app[i] = 0;
                                daemon_log(LOG_INFO,
                                        "Child %i finished with return value %i",
                                        pid, rv);
                                break;
                            }
                        }
                        break;

                    case SIGHUP:
                        rescan = true;
                        break;

                    case 0:
                        daemon_log(LOG_ERR, "daemon_signal_next() failed: %s",
                                strerror(errno));
                    case SIGINT:
                    case SIGTERM:
                        goto finished;

                    default:
                        daemon_log(LOG_INFO,
                                "Received unknown signal %i(%s)",
                                sig, strsignal(sig));
                        break;
                }
            }

            rescan |= FD_ISSET(etl_main, &fds);

        } while (!rescan); // ETL main has a new event
    }

    return 0;

finished_retval:
    if (!fg) {
        daemon_retval_send(0);
        daemon_retval_done();
    }

finished:

    daemon_signal_done();
    daemon_log(LOG_NOTICE, "Exiting...");

    return 0;
}
