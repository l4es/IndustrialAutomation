/*****************************************************************************
 *
 *  $Id$
 *
 *  vim: expandtab
 *
 ****************************************************************************/

#include <stdio.h>
#include <getopt.h>
#include <libgen.h> // basename()
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <inttypes.h>
#include <unistd.h>  // daemon()
#include <sys/types.h>
#include <pwd.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <list>

#include <Process.h>

using namespace std;

/*****************************************************************************/

string binaryBaseName;

// option variables
bool daemonize = false;
bool restore = false;
string pidPath;
string configPath;
string host;
uint16_t port;
string database;

Process process;

/*****************************************************************************/

string usage()
{
    stringstream str;

    str << "Usage: " << binaryBaseName << " [OPTIONS] <CONFIG>" << endl
        << endl
        << "Global options:" << endl
        << "  --daemon        -d         Become a daemon after" << endl
        << "                             reading configuration." << endl
        << "  --pid-file      -p <PATH>  Write PID to file." << endl
        << "  --restore       -r         Restore last values" << endl
        << "                             on connect (ignored when" << endl
        << "                             using a restore variable;" << endl
        << "                             see sample persist.conf)." << endl
        << "  --host          -o <HOST>  Override host from config file."
        << endl
        << "  --port          -t <PORT>  Override port from config file."
        << endl
        << "  --database      -a <FILE>  Override database from config file."
        << endl
        << "  --help          -h         Show this help." << endl
        << endl;

    return str.str();
}

/*****************************************************************************/

int getOptions(int argc, char **argv)
{
    int c, argCount;
    stringstream str;
    bool helpRequested = false;

    static struct option longOptions[] = {
        //name,       has_arg,           flag, val
        {"daemon",    no_argument,       NULL, 'd'},
        {"help",      no_argument,       NULL, 'h'},
        {"pid-file",  required_argument, NULL, 'p'},
        {"restore",   no_argument,       NULL, 'r'},
        {"host",      required_argument, NULL, 'o'},
        {"port",      required_argument, NULL, 't'},
        {"database",  required_argument, NULL, 'a'},
        {}
    };

    do {
        c = getopt_long(argc, argv, "dhp:ro:t:a:", longOptions, NULL);

        switch (c) {
            case 'd':
                daemonize = true;
                break;

            case 'h':
                helpRequested = true;
                break;

            case 'p':
                pidPath = optarg;
                break;

            case 'r':
                restore = true;
                break;

            case 'o':
                host = optarg;
                break;

            case 't':
                {
                    char *rem;
                    unsigned int val = strtoul(optarg, &rem, 0);

                    if (rem == optarg || *rem || val > 65535) {
                        cerr << "Invalid port number " << optarg << endl;
                        cerr << endl << usage();
                        return 1;
                    }

                    port = val;
                }
                break;

            case 'a':
                database = optarg;
                break;

            case '?':
                cerr << endl << usage();
                return 1;

            default:
                break;
        }
    }
    while (c != -1);

    if (helpRequested) {
        cout << usage();
        return 2;
    }

    argCount = argc - optind;
    if (argCount != 1) {
        cerr << binaryBaseName << " takes one argument:" << endl;
        cerr << usage();
        return 1;
    }

    configPath = argv[optind];
    return 0;
}

/****************************************************************************/

void signal_handler(int signum)
{
    process.signal(signum);
}

/****************************************************************************/

int main(int argc, char **argv)
{
    struct sigaction sa;
    int ret;

    memset(&sa, 0x00, sizeof(sa));
    sa.sa_handler = &signal_handler;

    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        cerr << "Failed to install signal handler!" << endl;
        return 1;
    }
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        cerr << "Failed to install signal handler!" << endl;
        return 1;
    }
    if (sigaction(SIGHUP, &sa, NULL) == -1) {
        cerr << "Failed to install signal handler!" << endl;
        return 1;
    }

    binaryBaseName = basename(argv[0]);

    ret = getOptions(argc, argv);
    if (ret) {
        return ret;
    }

    struct passwd *passwd = getpwuid(getuid());
    if (passwd) {
        process.setLogin(passwd->pw_name);
    } else {
        cerr << "Failed to determine user name: " << strerror(errno);
    }

    try {
        process.readConfig(configPath, host, port, database);
    } catch (Process::Exception &e) {
        cerr << e.what() << endl;
        return 1;
    }

    if (restore) {
        process.restore();
    }

    if (daemonize) {
        cerr << "Now becoming a daemon." << endl;
        ret = daemon(0, 0);
        if (ret != 0) {
            cerr << "Failed to become daemon: " << strerror(errno) << endl;
            return 1;
        }
    }

    if (!pidPath.empty()) {
        process.createPidFile(pidPath);
    }

    try {
        process.run();
    } catch (Process::Exception &e) {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}

/****************************************************************************/
