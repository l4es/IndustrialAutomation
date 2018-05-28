#!/usr/bin/perl -w
#/******************************************************************************
# *
# *  Copyright (C) 2008-2009  Andreas Stewering-Bone, Ingenieurgemeinschaft IgH
# *
# *  This file is part of the IgH EtherLAB Scicos Toolbox.
# *  
# *  The IgH EtherLAB Scicos Toolbox is free software; you can
# *  redistribute it and/or modify it under the terms of the GNU Lesser General
# *  Public License as published by the Free Software Foundation; version 2.1
# *  of the License.
# *
# *  The IgH EtherLAB Scicos Toolbox is distributed in the hope that
# *  it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# *  GNU Lesser General Public License for more details.
# *
# *  You should have received a copy of the GNU Lesser General Public License
# *  along with the IgH EtherLAB Scicos Toolbox. If not, see
# *  <http://www.gnu.org/licenses/>.
# *  
# *  ---
# *  
# *  The license mentioned above concerns the source code only. Using the
# *  EtherCAT technology and brand is only permitted in compliance with the
# *  industrial property and similar rights of Beckhoff Automation GmbH.
# *
# *****************************************************************************/




use IO::File;
use XML::LibXML;
use strict;
use Getopt::Std;


my $inputfile;
my $outputfilehandle;
my $outputfile;
my $datfile;

sub print_debug
{
    my($leveltype,$output)=@_;
    
    print"$leveltype : $output\n";
    exit 0;
}


sub dohelp {
    print 
        "XML to Scilab Struct Parser\n\n", 
        "usage: $0 [-h] [-f Inputfile.xml]\n\n", 
	"-h: this help\n",
	"-f: Inputfile\n";
    exit 0;
}

sub getnum 
{
    use POSIX qw(strtod);
    my $str = shift;
    $str =~ s/^\s+//;
    $str =~ s/\s+$//;
    $! = 0;
    my($num, $unparsed) = strtod($str);
    if (($str eq '') || ($unparsed != 0) || $!) {
	return undef;
    } else {
	return $num;
    }
}

sub is_numeric { defined getnum($_[0]) }



sub dump_nodes
{
    my ($node,$outputfilehandle) = @_;
    my $content;
    my $replacestring;
    my $replacevalue;
    my $redname;
    my $attr;
    my $subnode;
    if($node->nodeName eq 'text')
    {
	$replacestring = $node->parentNode->nodePath();
	$replacestring =~ s/\//./g;
	$replacestring =~ s/\[/\(/g;
	$replacestring =~ s/\]/\)/g;
	$replacestring =~ s/^\.//;
	$redname = $node->nodePath();
	$redname =~ s/\/text\(\)//;

	if(($redname eq $node->parentNode->nodePath()) && ($node->parentNode->hasAttributes))
	{
	    $replacestring = $replacestring . ".TextContent";
	}
	if($node->textContent =~/[a-zA-Z0-9\#]+/)
	{
	    if(&is_numeric($node->textContent))
	    {
		print $outputfilehandle $replacestring . "=" . $node->textContent . ";\n";
	    }
	    else
	    {
		if($node->textContent =~/\#x/) #Hex-String
		{
		    $replacevalue = $node->textContent;
		    $replacevalue =~ s/\#x//;
		    print $outputfilehandle $replacestring . "=hex2dec(\'" . $replacevalue . "\');\n";
		}
		else
		{
		    print $outputfilehandle $replacestring ."=\'" . $node->textContent ."\';\n";
		}
	    }
	}
    }

    if ($node->hasAttributes) 
    {
	foreach $attr ( $node->attributes ) {
	    $replacestring = $node->nodePath();
	    $replacestring =~ s/\//./g;
	    $replacestring =~ s/\[/\(/g;
	    $replacestring =~ s/\]/\)/g;
	    $replacestring =~ s/^\.//;
	    if(&is_numeric($attr->value))
	    {
		print $outputfilehandle $replacestring . "." . $attr->name . "=" . $attr->value . ";\n";
	    }
	    else
	    {
		if($attr->value =~/\#x/) #Hex-String
		{
		    $replacevalue = $attr->value;
		    $replacevalue =~ s/\#x//;
		    print $outputfilehandle $replacestring . "." . $attr->name . "=hex2dec(\'" . $replacevalue . "\');\n";
		}
		else
		{
		    print $outputfilehandle $replacestring ."." . $attr->name . "=\'" . $attr->value ."\';\n";
		}
	    }
	}
    }
    foreach $subnode ($node->getChildnodes)
    {
	&dump_nodes($subnode,$outputfilehandle);
    }
    
}




my %opts = (
	    h => 0,
	    f => '',
            );


getopts('hf:', \%opts);

&dohelp if $opts{'h'};

$inputfile=$opts{'f'};
$outputfile = 'test.xml';
$datfile = $outputfile;
$outputfile =~s/ //g;
$outputfile =~s/\.xml/\.sce/;
$datfile =~s/\.xml/\.dat/;

# Erzeuge ein Parser Objekt
my $parser = XML::LibXML->new(validation => 1);
# Parse die XML Datei, die der Anwender als Parameter uebergibt
my $doc = $parser->parse_file($inputfile);


#Open Outputfile
$outputfilehandle = IO::File->new("> $outputfile") 
    or print_debug("Error","Kann die Datei $outputfile nicht schreiben!\n");


# Gib alle Elementnamen angefangen beim Root-Element aus
&dump_nodes($doc->getDocumentElement,$outputfilehandle);

print $outputfilehandle "save(\'$datfile\',\'EtherCATInfo\');\n";
#print $outputfilehandle "clear EtherCATInfo\n;
$outputfilehandle->close();

