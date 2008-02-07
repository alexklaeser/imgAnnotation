#!/usr/bin/perl

# string with usage information
my $usage = <<"EOD";

usage: convertBoxFormat.pl <inDB> <outDb>

This util converts the bounding box information in a .annotations file from the format
<minX> <minY> <maxX> <maxY>
to
<minX> <minY> <width> <height>

EOD

# check command line arguments 
if (@ARGV < 2 || "--help" eq $ARGV[0]) {
	print "$usage";
	exit(0);
}

# get the command line arguments
my $INFILE = shift(@ARGV);
my $OUTFILE = shift(@ARGV);

# load the annotation file
open(FILE, "< $INFILE") || die "failed to open $INFILE\n";
my @DB = <FILE>;
close(FILE);
chomp(@IMAGES); # delete newline characters

# process line by line
open(FILE, "> $OUTFILE") || die "failed to open $OUTFILE\n";
foreach $line (@DB) {
	$line =~ s/(^\s*|\s*$)//g; # trim the line

	# get all the different values
	my @fields = split(/:/, $line);
	my $key = $fields[0];
	$key =~ s/^\s+//;
	$key =~ s/\s+$//;

	if ($key =~ /^bbox$/i) {
		my $value = $fields[1];
		$value =~ s/^\s+//;
		$value =~ s/\s+$//;

		my @fields = split(/\s*,\s*/, $value);
		print FILE "bbox: ".$fields[0].", ".$fields[1].", ".($fields[2] - $fields[0]).", ".($fields[3] - $fields[1])."\n";
	}
	else {
		print FILE $line."\n";
	}
}
close(FILE);

