/******************************************************************************
 *
 *  $Id$
 *
 *  This file is part of the Data Logging Service (DLS).
 *
 *  DLS is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any later
 *  version.
 *
 *  DLS is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with DLS. If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

#include <FL/Fl.H>
#include <FL/fl_ask.H>

/*****************************************************************************/

#include "lib/XmlParser.h"
#include "lib/RingBufferT.h"

#include "CtlGlobals.h"
#include "CtlDialogChannels.h"
#include "CtlDialogMsg.h"

/*****************************************************************************/

#define WIDTH 550
#define HEIGHT 350

/*****************************************************************************/

/**
   Konstruktor

   \param source IP-Adresse oder Hostname der Datenquelle
*/

CtlDialogChannels::CtlDialogChannels(const string &source, uint16_t port)
{
    int x = Fl::w() / 2 - WIDTH / 2;
    int y = Fl::h() / 2 - HEIGHT / 2;

    _source = source;
    _port = port;
    _thread_running = false;

    _wnd = new Fl_Double_Window(x, y, WIDTH, HEIGHT, "Kanäle auswählen");
    _wnd->set_modal();
    _wnd->callback(_callback, this);

    _button_ok = new Fl_Button(WIDTH - 90, HEIGHT - 35, 80, 25, "OK");
    _button_ok->deactivate();
    _button_ok->callback(_callback, this);
    _button_cancel = new Fl_Button(WIDTH - 180, HEIGHT - 35, 80, 25,
                                   "Abbrechen");
    _button_cancel->callback(_callback, this);

    _checkbutton_reduceToOneHz = new Fl_Check_Button(10, HEIGHT - 35,
            WIDTH - 200, 25, "Reduziere auf 1 Hz");
    _checkbutton_reduceToOneHz->set();

    _grid_channels = new Fl_Grid(10, 10, WIDTH - 20, HEIGHT - 55);
    _grid_channels->add_column("name", "Kanal", 200);
    _grid_channels->add_column("unit", "Einheit", 50);
    _grid_channels->add_column("freq", "Abtastrate (Hz)");
    _grid_channels->add_column("type", "Typ");
    _grid_channels->select_mode(flgMultiSelect);
    _grid_channels->callback(_callback, this);
    _grid_channels->hide();

    _box_message = new Fl_Box(10, 10, WIDTH - 20, HEIGHT - 55);
    _box_message->align(FL_ALIGN_CENTER);
    _box_message->label("Empfange Kanäle...");

    _wnd->end();
    _wnd->resizable(_grid_channels);
}

/*****************************************************************************/

/**
   Destruktor
*/

CtlDialogChannels::~CtlDialogChannels()
{
    delete _wnd;
}

/*****************************************************************************/

/**
   Statische Callback-Funktion

   \param sender Widget, dass den Callback ausgelöst hat
   \param data Zeiger auf den Dialog
*/

void CtlDialogChannels::_callback(Fl_Widget *sender, void *data)
{
    CtlDialogChannels *dialog = (CtlDialogChannels *) data;

    if (sender == dialog->_grid_channels) dialog->_grid_channels_callback();
    if (sender == dialog->_button_ok) dialog->_button_ok_clicked();
    if (sender == dialog->_button_cancel) dialog->_button_cancel_clicked();
    if (sender == dialog->_wnd) dialog->_button_cancel_clicked();
}

/*****************************************************************************/

/**
   Callback des Kanal-grids
*/

void CtlDialogChannels::_grid_channels_callback()
{
    unsigned int i;
    stringstream str;

    switch (_grid_channels->current_event())
    {
        case flgContent:
            i = _grid_channels->current_record();

            if (_grid_channels->current_col() == "name")
            {
                _grid_channels->current_content(_channels[i].name);
            }
            else if (_grid_channels->current_col() == "unit")
            {
                _grid_channels->current_content(_channels[i].unit);
            }
            else if (_grid_channels->current_col() == "freq")
            {
                str << _channels[i].frequency;
                _grid_channels->current_content(str.str());
            }
            else if (_grid_channels->current_col() == "type")
            {
                _grid_channels->current_content(
                    channel_type_to_str(_channels[i].type));
            }
            break;

        case flgSelect:
            _button_ok->activate();
            break;

        case flgDeSelect:
            _button_ok->deactivate();
            break;

        case flgDoubleClick:
            _button_ok_clicked();
            break;

        default:
            break;
    }
}

/*****************************************************************************/

/**
   Callback: Der "OK"-Button wurde geklickt
*/

void CtlDialogChannels::_button_ok_clicked()
{
    list<unsigned int>::const_iterator sel_i;

    // Eventuell den Thread abbrechen
    if (_thread_running)
    {
        pthread_cancel(_thread);
    }

    // Liste mit ausgewählten Kanlälen erstellen
    _selected.clear();
    sel_i = _grid_channels->selected_list()->begin();
    while (sel_i != _grid_channels->selected_list()->end())
    {
        if (_checkbutton_reduceToOneHz->value()){
            _channels[*sel_i].frequency=1;
        }

        _selected.push_back(_channels[*sel_i]);
        sel_i++;
    }

    // Fenster schließen
    _wnd->hide();
}

/*****************************************************************************/

/**
   Callback: Der "Abbrechen"-Button wurde geklickt
*/

void CtlDialogChannels::_button_cancel_clicked()
{
    // Eventuell den Thread abbrechen
    if (_thread_running)
    {
        pthread_cancel(_thread);
    }

    // Abbrechen = keine Kanäle ausgewählt
    _selected.clear();

    // Fenster schließen
    _wnd->hide();
}

/*****************************************************************************/

/**
   Dialog zeigen
*/

void CtlDialogChannels::show()
{
    if (pthread_create(&_thread, 0, _static_thread_function, this) == 0)
    {
        _wnd->show();

        while (_wnd->shown()) Fl::wait();
    }
    else
    {
        msg_win->str() << "Konnte keinen neuen Thread starten!";
        msg_win->error();
    }
}

/*****************************************************************************/

void *CtlDialogChannels::_static_thread_function(void *data)
{
    CtlDialogChannels *dialog = (CtlDialogChannels *) data;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);

    Fl::lock();
    dialog->_error = "";
    dialog->_thread_running = true;
    Fl::unlock();

    dialog->_thread_function();

    Fl::lock();
    dialog->_thread_running = false;
    dialog->_thread_finished();
    Fl::unlock();

    return 0;
}

/*****************************************************************************/

void CtlDialogChannels::_thread_function()
{
    stringstream port_str;
    struct addrinfo hints, *result, *rp;
    int ret;
    int socket;
    fd_set read_fds, write_fds;
    int select_ret, recv_ret, send_ret;
    LibDLS::XmlParser xml;
    const LibDLS::XmlTag *tag;
    LibDLS::RealChannel channel;
    string to_send;
    LibDLS::RingBuffer ring(65535);
    char *write_pointer;
    unsigned int write_size;
    bool exit_thread = false;

    /* Obtain address(es) matching host/port */

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Stream socket */
    hints.ai_flags = AI_NUMERICSERV; /* port is numeric. */

    port_str << _port;

    ret = getaddrinfo(_source.c_str(), port_str.str().c_str(),
            &hints, &result);
    if (ret) {
        stringstream err;
        err << "Failed to get address info: " << gai_strerror(ret);
        _error = err.str();
        return;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        socket = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (socket == -1) {
            continue;
        }

        if (connect(socket, rp->ai_addr, rp->ai_addrlen) != -1) {
            break;
        }

        close(socket);
    }

    freeaddrinfo(result);

    if (!rp) {
        _error = "Connection failed!";
        return;
    }

    to_send = "<rk>\n";

    while (!exit_thread)
    {
        // File-Descriptor-Sets nullen und mit Client-FD vorbesetzen
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        FD_SET(socket, &read_fds);
        if (to_send.length() > 0) FD_SET(socket, &write_fds);

        // Warten auf Änderungen oder Timeout
        if ((select_ret = select(socket + 1, &read_fds, &write_fds, 0, 0)) > 0)
        {
            // Eingehende Daten?
            if (FD_ISSET(socket, &read_fds))
            {
                ring.write_info(&write_pointer, &write_size);

                if (!write_size) {
                    _error = "Receive ring buffer full!";
                    exit_thread = true;
                    break;
                }

                // Daten abholen...
                if ((recv_ret = recv(socket, write_pointer,
                                     write_size, 0)) > 0)
                {
                    ring.written(recv_ret);

                    while (1)
                    {
                        try
                        {
                            tag = xml.parse(&ring);
                        }
                        catch (LibDLS::EXmlParserEOF &e)
                            // Tag noch nicht komplett
                        {
                            break;
                        }
                        catch (LibDLS::EXmlParser &e)
                            // Anderer Parsing-Fehler
                        {
                            _error = "Parser error in tag \"" + e.tag + "\"!";
                            exit_thread = true;
                            break;
                        }

                        try
                        {
                            if (tag->title() == "channel")
                            {
                                try
                                {
                                    string type;

                                    channel.name = tag->att("name")->to_str();
                                    if (tag->has_att("unit")) {
                                        channel.unit =
                                            tag->att("unit")->to_str();
                                    }
                                    channel.frequency =
                                        tag->att("HZ")->to_int();

                                    type = tag->att("typ")->to_str();

                                    channel.type =
                                        LibDLS::str_to_channel_type(type);

                                    if (channel.type == LibDLS::TUNKNOWN) {
                                        continue;
                                    }

                                    _channels.push_back(channel);
                                }
                                catch (LibDLS::Exception &e)
                                {
                                    _error = "Reading channel: " + e.msg;
                                    exit_thread = true;
                                    break;
                                }
                            }
                            else if (tag->title() == "channels"
                                     && tag->type() == LibDLS::dxttEnd)
                            {
                                exit_thread = true;
                                break;
                            }
                        }
                        catch (LibDLS::EXmlTag &e)
                        {
                            _error = "Parser: " + e.msg;
                            exit_thread = true;
                            break;
                        }
                    }
                }
                else if (recv_ret == -1)
                {
                    _error = "Error in recv()";
                    break;
                }
            }

            // Bereit zum Senden?
            if (FD_ISSET(socket, &write_fds))
            {
                // Daten senden
                if ((send_ret = send(socket, to_send.c_str(),
                                     to_send.length(), 0)) > 0)
                {
                    to_send.erase(0, send_ret); // Gesendetes entfernen
                }
                else if (send_ret == -1)
                {
                    _error = "Error in send()";
                    break;
                }
            }
        }

        // Select-Fehler
        else if (select_ret == -1)
        {
            _error = "Error in select()";
            break;
        }
    }

    close(socket);
}

/*****************************************************************************/

void CtlDialogChannels::_thread_finished()
{
    _box_message->hide();
    _grid_channels->show();
    _grid_channels->take_focus();

    if (_error != "")
    {
        msg_win->str() << _error;
        msg_win->error();

        _wnd->hide();
    }
    else if (_channels.size() > 0)
    {
        sort(_channels.begin(), _channels.end());
        _grid_channels->record_count(_channels.size());
    }
}

/*****************************************************************************/
