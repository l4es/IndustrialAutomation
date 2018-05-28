/*****************************************************************************
 * vim:tw=78
 * $Id$
 *
 * Copyright (C) 2016       Richard Hacker (lerichi at gmx dot net)
 *
 * This file is part of the PdCom library.
 *
 * The PdCom library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * The PdCom library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the PdCom library. If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

/** @file */

#ifndef PDCOM_PROCESS_H
#define PDCOM_PROCESS_H

#include <stddef.h>
#include <string>
#include <list>
#include <stdint.h>

class IOLayer;
class ProtocolHandler;

namespace PdCom {

    struct Subscriber;
    class Variable;

    /** Base class for PdCom protocol handler
     *
     * This is the base class to interact with real time process server.
     * The PdCom protocol ist implemented using this class.
     *
     * For socket input and output, the library completely relies on a derived
     * class where read(), write(), flush() and connected() methods are
     * reimplemented.
     *
     * When data is available for reading, call asyncData() which in turn
     * calls the reimplemented read() method. 
     *
     * When the protocol is initialized, the reimplemented connected() method
     * is called. Other than startTLS(), login(), none of the command methods
     * listed below may be called prior to connected().
     *
     * After connected(), the following commands can be issued:
     *  * list(): list a directory; returns result in listReply()
     *  * find(): find a variable; returns result in findReply()
     *  * login(): start a SASL login interaction; returns result in
     *  loginReply()
     *  * ping(): ping the server; returns result in pingReply()
     *  * messageHistory(): request a history of process messages; returns
     *  result in multiple calls to processMessage() if required
     *  * Variable interactions
     *  * Variable::Subscription interactions
     *
     * startTLS() and login() may only be called prior to connected() when the
     * library has called startTLSReply() or loginReply() previously.
     *
     * All these commands are non-blocking asynchronous calls and either return
     * the result immediately with the corresponding reply methods or issue a
     * command to the server using excessive (!) calls to write(). Data should
     * be written to a buffer to optimize network communication. To flush the
     * buffer to wire, flush() is issued by the library when required.
     *
     * The server may query presence of the user by issuing an alive() call.
     * Using this call, certain actions could be undertaken by the server if
     * the user is not active any more.
     *
     * Certain actions in the real time process, such as alarms, triggers a
     * call to processMessage().
     *
     * The subscribe() method subscribes a variable using the path only.
     */
    class Process {
        public:
            /** Constructor */
            Process();

            /** Destructor
             *
             * The destructor cleans up all internally allocated structures
             */
            virtual ~Process();

            /** Remote process name string */
            std::string name() const;

            /** Remote process version string */
            std::string version() const;

            /** Reset communications and clean up internal buffers */
            void reset();

            /** Name of application user application.
             *
             * The application name is transferred to the server to be able to
             * identify the clients more easily.
             *
             * \return a descriptive name of your application.
             */
            virtual std::string applicationName() const;

            /** Host name of remote server; used for TLS when multi-hosting
             *
             * \return server host name
             */
            virtual std::string hostname() const;

            /** Read data from server
             *
             * Reimplement this method to transfer data from the server to
             * the library. This method is called within the call to
             * asyncData().
             *
             * Essentially this method is a little wrapper around your
             * socket's `%read()` function:
             * \code
             *   int MyProcess::read(char *buf, size_t count)
             *   {
             *       return ::read(this->socket_fd, buf, count);
             *   }
             * \endcode
             *
             * The method must return the number of bytes read, which may of
             * coarse be less than \p count or even 0. Return values of
             * &lt;=&nbsp;0 are not interpreted by the protocol handler.
             *
             * @param buf data destination
             * @param count buffer size
             *
             * \return
             *    return value of `%read()` function, which in turn will be
             *    returned by asyncData()
             */
            virtual int read(       char *buf, size_t count) = 0;

            /** Write data to server
             *
             * Reimplement this method to transfer data from the library to
             * the server. This method is called when any library
             * operation requires data to be sent to the server.
             *
             * Note: the library makes many calls to write(), so use
             * buffered output otherwise you're in for performance problems!
             *
             * Essentially this method is a little wrapper around your
             * socket's `%write()` function:
             * \code
             * void MyProcess::write(const char *buf, size_t count)
             * {
             *     if (count != ::fwrite(buf, 1, count, this->socket_file)) {
             *         // react to errors, set flags, etc
             *     }
             * }
             * \endcode
             *
             * Note: the library does not have an internal buffer and expects
             * that all data is sent. If your implementation might send less
             * than \p count, it is your responsibility to buffer the data and
             * send it later.
             *
             * @param buf data to be sent
             * @param count number of bytes to send
             */
            virtual void write(const char *buf, size_t count) = 0;

            /** Flush unsent data in output buffer
             *
             * Reimplement this method to flush data in the output buffer.
             *
             * This method tells the user that it is time to flush the
             * output buffer to the wire. The library only expects that data
             * is sent to the server within this call.
             *
             * Essentially this method is a little wrapper around your
             * socket's `fflush()` function:
             * \code
             * void MyProcess::flush()
             * {
             *     if (::fflush(this->socket_file)) {
             *         // react to errors
             *     }
             * }
             * \endcode
             */
            virtual void flush() = 0;

            /** Library entry point for new data.
             *
             * Calling this method tells the library that new data has arrived
             * from the server and is waiting to be processed.
             *
             * The library prepares an input buffer and then calls the
             * reimplemented read() virtual method to read incoming data.
             *
             * The return value of read() is returned by this method, enabling
             * the user to be able to react to read errors, count bytes or
             * even socket close, etc.
             *
             * \return return value of read() method
             */
            int asyncData();

            /** Protocol initialization completed
             *
             * This is a signal emitted by the library to indicate that
             * protocol initialization has been completed and that library
             * operations can be performed thereafter.
             *
             * Reimplement this method to get the signal.
             *
             * Absolutely NO process operations other than asyncData(),
             * startTLS() and login() (and then only due to a previous
             * loginReply() are permitted before this signal has been
             * emitted.
             */
            virtual void connected() = 0;

            /** List a directory path
             *
             * A process command to return all variables and directories
             * within a directory path. The \p path parameter has typical
             * unix character, with forward slashes '/' separating
             * directories.
             *
             * listReply() must be reimplemented to receive the reply
             * to this call.
             *
             * If the directory is cached (for instance a previous call to
             * a similar path, or an entire server listing has been performed),
             * listReply() is called within the context of this call and no
             * server query is performed.
             *
             * If uncached, the library sends a server query and returns
             * immediately. Later on during asyncData(), the virtual method
             * listReply(), is called when the server's reply is
             * processed.
             *
             * As a special case, an empty string (std::string()) for \p path
             * will let the server list all its variables in one go. This
             * possibility must be used with caution, as it can cause heavy
             * network traffic.
             *
             * @param path directory path
             *
             * \return
             *     true if the path was cached
             */
            bool list(const std::string& path) const;

            /** Reply to list() call
             *
             * You must reimplement this method to receive replies to list()
             * calls.
             *
             * Note that a variable can have the same path as a directory!
             * An example is a vector variable with atomized elements.
             *
             * Replies are in strict order of list() calls.
             *
             * @param variables list of variables
             * @param directories string list of directories
             */
            virtual void listReply(
                    std::list<const Variable*>& variables,
                    std::list<std::string>& directories);

            /** Find a variable with a corresponding path
             *
             * If the path search is known (be it successful or unsuccessful),
             * the variable is returned in the call to the reimplemented
             * virtual findReply() method immediately and the method returns
             * true;
             *
             * If unsuccessful, the command is sent to the server to and the
             * call returns immediately with false. Later on during
             * asyncData(), findReply() is called when the server's reply is
             * processed.
             *
             * @param path path of variable to find
             *
             * \return true if path is found immediately (cached)
             */
            bool find(const std::string& path) const;

            /** Reply to find()
             *
             * This virtual method is called within the context of asyncData()
             * when the server's reply to a variable discovery is processed.
             *
             * findReply()ies are called in strict order of find()
             *
             * @param variable pointer to Variable; NULL if not found
             */
            virtual void findReply(const Variable* variable);

            /** Perform SASL login step.
             *
             * @param mech SASL mechanism
             * @param clientData Base64 encoded SASL output data to server
             *
             * Setting both \p mech and \p clientData to NULL will initate the
             * login process.
             *
             * Every call to login() is answered by a loginReply(), unless
             * login is not supported. When login is mandatory, loginReply()
             * will be called automatically.
             *
             * @return true if login is not supported
             */
            bool login(const char* mech, const char* clientData) const;

            /** Logout from server
             */
            void logout() const;

            /** SASL server reply to login()
             *
             * loginReply() may be called without calling login() in the case
             * where login is mandatory. In this case, it is called before
             * connected().
             *
             * @param mechlist Space separated list of supported mechanisms
             * @param serverData Base64 encoded SASL data from server
             * @param finished
             * \parblock
             *   &gt; 0: Login successful\n
             *   0:      Login process not finished; another login() step is required\n
             *   &lt; 0: Login failed
             * \endparblock
             */
            virtual void loginReply(const char* mechlist,
                    const char* serverData, int finished);

            /** Start TLS
             *
             * The initial server communication is in plain text. Encryption on
             * the server side is initiated upon reception of a startTLS()
             * command. The client may continue sending commands to the server,
             * although these are still in plain text. Server replies to these
             * commands are delayed in the server till after successful
             * encryption.  The server acknowledges reception of the
             * startTLS() command using startTLSReply() whereafter the client
             * must switch to TLS.
             *
             * Server replies to commands sent by the client between startTLS()
             * and startTLSReply() are stored in a limited buffer.  The
             * connection will be closed by the server if it overflows. Thus it
             * is not recommended to send commands during this period, at least
             * not ones where the replies are expected to be quite long!
             *
             * Reimplement startTLSReply() to be able to use TLS.
             *
             * \return true on error (server does not support TLS)
             */
            bool startTLS() const;

            /** TLS reply
             *
             * Reimplement this command in the client. Upon reception, TLS
             * must be activated.
             *
             * \sa startTLS()
             */
            virtual void startTLSReply();

            /** Ping server */
            void ping() const;

            /** Ping reply
             *
             * You must reimplement this method to receive the reply to a
             * ping() call.
             */
            virtual void pingReply();

            /** Test from process whether client is alive.
             *
             * In some cases the server may want to know whether the client
             * is still alive. Default implementation is to return true.
             * Reimplement this if you wish to control presence
             *
             * \return true to indicate user presence
             */
            virtual bool alive();

            /** Subscribe to a variable
             *
             * The subscription rate is specified using \p interval.  Depending
             * on the type of variable, signal or parameter, the interval has
             * different meanings.
             *
             * For parameters, the \c interval is ignored completely.
             * Subscribed parameters are only updated when the value changes.
             * It is available in Subscriber::newValue().
             *
             * For signals, the \c interval specifies:
             *     * &lt; 0: The subscription needs to be \ref
             *     Variable::Subscription::poll "poll()"ed every time it is
             *     required. Typically a timer is used to call \ref
             *     Variable::Subscription::poll "poll()" on a regular basis.
             *     The absolute value of \c interval is ignored. The
             *     subscription value is available in Subscriber::newValue().
             *     * 0: Server sends the subscription automatically when the
             *     value changes. Be careful: quantization variances on an
             *     analog signal for example may cause the value to be blasted
             *     at a continuous rate with every calculation cycle! The
             *     subscription value is available in Subscriber::newValue().
             *     * &gt; 0: Server sends the subscription continuously at the
             *     specified rate. The subscription value is available in
             *     Subscriber::newGroupValue().
             *
             * Usually you would use \c interval&nbsp;&lt;=&nbsp;0. It reduces
             * the required bandwidth.  Fixed subscription \c
             * interval&nbsp;&gt;&nbsp;0 is actually only required for logging
             * purposes, such as an oscilloscope or data recorder.
             *
             * @param subscriber pointer to subscriber object that will
             *        receive the subscription
             * @param path  variable path to subscribe
             * @param interval see description
             * @param id arbitrary user specified subscription id. The \c id
             * is copied verbatim to the Variable::Subscription::id or during
             * Subscriber::invalid()
             */
            void subscribe(Subscriber* subscriber,
                    const std::string& path, double interval, int id) const;

            /** Unsubscribe all subscriptions from \p subscriber
             */
            void unsubscribe(Subscriber* subscriber) const;

            /** Turn parameter monitoring on/off
             *
             * Turning parameter monitoring automatically subscribes changed
             * parameters.
             *
             * The value is delivered just as with ordinary subscriptions in
             * newValue(const Variable::Subscription* subscription), with the
             * exception that there was no preceding subscribe() and there is
             * no \ref PdCom::Subscriber::active() "active()" call.
             *
             * There is no automatic unsubscribing. To save memory, it is safe
             * to \ref PdCom::Variable::Subscription::cancel() "cancel()" the
             * automatic parameter subscription immediately within \ref
             * PdCom::Subscriber::newValue() "newValue()".
             */
            void parameterMonitor(Subscriber* subscriber, bool state) const;

            /** Log levels.
             *
             * \sa processMessage()
             * \sa protocolLog()
             */
            enum LogLevel_t {
                Emergency,
                Alert,
                Critical,
                Error,
                Warn,
                Info,
                Debug,
                Trace
            };

            /** Message event from process
             *
             * This method is called whenever the process generates an event.
             * Reimplement this method to receive them.
             *
             * @param level severity
             * @param path event's path
             * @param index event's index in case of a vector; -1 for scalar
             * @param state true: event raised; false event reset
             * @param time_ns time in nanoseconds since epoch
             */
            virtual void processMessage(
                    LogLevel_t level,
                    const std::string& path,
                    int index,
                    bool state,
                    uint64_t time_ns
                    );

            /** Request history of messages from the process.
             *
             * The process stores a configurable history of messages. This
             * method will request the list. Typically it is used once during
             * initial connection.
             *
             * @param limit the reply length; 0 is unlimited
             */
            void messageHistory(unsigned int limit = 0) const;

            /** Message event from protocol handler
             *
             * Reimplement this method to receive logs from the protocol
             * handler.
             *
             * All messages &lt;= Error are fatal and the process must
             * be aborted and reset()
             *
             * @param severity
             * @param message plain text
             */
            virtual void protocolLog(
                    LogLevel_t severity,
                    const std::string& message
                    );

        private:
            ProtocolHandler *protocolHandler;
            IOLayer* const io;

            std::string initialBuffer;
    };
}       // namespace

#endif // PDCOM_PROCESS_H
