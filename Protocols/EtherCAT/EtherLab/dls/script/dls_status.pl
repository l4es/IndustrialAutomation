#!/usr/bin/perl -w

#----------------------------------------------------------------------------
#
# DLS - Status- und Kontroll-Script für den DLS-Daemon
#
# $Id$
#
# This file is part of the Data Logging Service (DLS).
#
# DLS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# DLS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along
# with DLS. If not, see <http://www.gnu.org/licenses/>.
#
#----------------------------------------------------------------------------

use strict;
use FileHandle;
use Getopt::Std;

#----------------------------------------------------------------

$| = 1; # Ungepufferte Ausgabe

#----------------------------------------------------------------

my %opt;
my $dls_dir;
my @table;

&main;

#----------------------------------------------------------------
#
#  main
#
#  Hauptfunktion. Führt alle Status-Prüfungen durch und
#  schreibt die Ergebnisse nach STDOUT.
#
#----------------------------------------------------------------

sub main
{
    # Kommandozeile verarbeiten
    &get_options;
    &status;
}

#----------------------------------------------------------------
#
#  get_options
#
#  Verarbeitet die Kommandozeilenparameter
#
#----------------------------------------------------------------

sub get_options
{
    my $opt_ret = getopts "d:h", \%opt;

    &print_usage if !$opt_ret or $opt{'h'} or $#ARGV > 0;

    # Pfad für DLS-Datenverzeichnis ermitteln
    if ($opt{'d'}) {
	$dls_dir = $opt{'d'};
    }
    elsif (defined $ENV{'DLS_DIR'}) {
	$dls_dir = $ENV{'DLS_DIR'};
    }
    else {
	$dls_dir = "/vol/dls_data";
    }
}

#----------------------------------------------------------------
#
#  print_usage
#
#  Gibt die Hilfe über die Kommandozeilenparameter aus
#  und beendet danach den Prozess.
#
#----------------------------------------------------------------

sub print_usage
{
    $0 =~ /^.*\/([^\/]*)$/; # Programmnamen ermitteln

    print "Aufruf: $1 [OPTIONEN]\n";
    print "Optionen:\n";
    print "        -d [Verzeichnis]   DLS-Datenverzeichnis\n";
    print "        -h                 Diese Hilfe anzeigen\n";
    exit 0;
}

#----------------------------------------------------------------
#
#  status
#
#  Gibt einen Statusbericht aus.
#
#----------------------------------------------------------------

sub status
{
    print "\n--- DLS Statusbericht ---------------------\n\n";
    print "Fuer DLS-Datenverzeichnis \"$dls_dir\"\n\n";

    # Das angegebene DLS-Datenverzeichnis überprüfen
    &check_dls_dir;

    # Tabelle ausgeben
    print "\n";
    foreach (@table)
    {
	print "$_\n";
    }
    print "\n";

    exit 0;
}

#----------------------------------------------------------------
#
#  check_dls_dir
#
#  Überprüft, ob der dlsd-Mutterprozess läuft und
#  startet dann die Prüfungen für die einzelnen
#  Job-Verzeichnisse.
#
#----------------------------------------------------------------

sub check_dls_dir
{
    my $pid;
    my $mother_running = 0;
    my $pid_file = "$dls_dir/dlsd.pid";

    unless (-d $dls_dir) {
	print "FEHLER: Verzeichnis \"$dls_dir\" existiert nicht!\n\n";
	return;
    }

    $mother_running = &check_pid($pid_file);

    if ($mother_running == 0) {
	print "DLS-Mutterprozess LAEUFT NICHT!\n";
    }
    else {
	print "DLS-Mutterprozess laeuft mit PID $mother_running.\n";
    }

    &check_jobs;
}

#----------------------------------------------------------------
#
#  check_pid
#
#  Prüft, ob ein Prozess zu einer PID-Datei noch läuft.
#  Gibt die PID zurück, wenn ja, sonst 0.
#
#----------------------------------------------------------------

sub check_pid
{
    my ($pid_file) = @_;

    return 0 if ! -r $pid_file;

    my $pid = `cat $pid_file`;
    chomp $pid;
	
    # Der Inhalt der ersten Zeile muss eine Nummer sein
    unless ($pid =~ /^\d+$/) {
	print "FEHLER: \"$pid_file\" ist korrupt!\n\n";
	return 0;
    }

    `ps ax | grep -q -E "^ *$pid.*dlsd"`;

    return $pid unless $?;
    return 0;
}

#----------------------------------------------------------------
#
#  check_jobs
#
#  Durchläuft die Einträge des DLS-Datenverzeichnisses
#  und startet die Prüfung eines Jobs, wenn ein
#  entsprechendes Unterverzeichnis gefunden wurde.
#
#----------------------------------------------------------------

sub check_jobs
{
    my ($dir_handle, $dir_entry);

    unless (opendir $dir_handle, $dls_dir) {
	print "\nFEHLER: Konnte \"$dls_dir\" nicht öffnen!\n";
	return;
    }

    push @table, "| Job-ID | Beschreibung          | Status    | Prozess       |";
    push @table, "|--------|-----------------------|-----------|---------------|";

    while ($dir_entry = readdir $dir_handle) {
	# Verzeichniseintrag muss von der Form jobXXX sein
	next unless $dir_entry =~ /^job(\d+)$/;

	# Verzeichniseintrag muss ein Verzeichnis sein
	next unless -d "$dls_dir/$dir_entry";

	&check_job($1);
    }

    closedir $dir_handle;

    push @table, "|________|_______________________|___________|_______________|" ;
}

#----------------------------------------------------------------
#
#  check_job
#
#  Überprüft den Status eines einzelnen Jobs
#
#  Parameter: job_id - Auftrags-ID
#
#----------------------------------------------------------------

sub check_job
{
    my ($job_id) = @_;

    my $job_file = "$dls_dir/job$job_id/job.xml";
    my $pid_file = "$dls_dir/job$job_id/dlsd.pid";
    my ($job_xml, $desc, $state, $running, $pid, $proc);
    my ($job_id_6, $desc_21, $state_9, $proc_13);

    $desc = "";
    $state = "";
    $running = 0;

    # job.xml muss existieren
    unless (-r $job_file) {
	print "\nFEHLER: \"$dls_dir/job$job_id\" -";
	print " Datei job.xml existiert nicht!\n";
    }
    else {
	# Versuch, job.xml zu öffnen
	unless (open JOB, $job_file) {
	    print "\nFEHLER: \"$job_file\" - Datei lässt sich nicht öffnen!\n";
	}
	else {
	    # Den gesamten Dateiinhalt einlesen
	    while (<JOB>) {$job_xml .= $_;}
	    close JOB;

	    # Nach <description text="XXX"/> suchen...
	    unless ($job_xml =~ /\<description\s+text\s*=\s*\"(.*)\"\s*\/\>/) {
		print "\nFEHLER: \"$job_file\" -";
		print " <description>-Tag nicht gefunden!\n";
	    }
	    else {
		$desc = $1;
	    }

	    # Nach <state name="XXX"/> suchen...
	    unless ($job_xml =~ /\<state\s+name\s*=\s*\"(.*)\"\s*\/\>/) {
		print "\nFEHLER: \"$job_file\" -";
		print " <state>-Tag nicht gefunden!\n";
	    }
	    else {
		$state = $1;
	    }
	}
    }

    $running = &check_pid($pid_file);

    # Informationen für die Ausgabe auswerten
    $proc = "UNBEKANNT";
    $proc = "laeuft" if $running > 0;
    $proc = "LAEUFT NICHT!" if $running == 0;
    $proc = "" if $state eq "paused";
    $desc = "UNBEKANNT" if $desc eq "";
    $state = "UNBEKANNT" if $state eq "";

    # Felder formatieren
    $job_id_6 = sprintf "%6d", $job_id;
    $desc_21 = sprintf "%21s", $desc;
    $state_9 = sprintf "%9s", $state;
    $proc_13 = sprintf "%13s", $proc;

    # Felder in Tabellenzeile ausgeben
    push @table, "| $job_id_6 | $desc_21 | $state_9 | $proc_13 |";
}

#----------------------------------------------------------------
