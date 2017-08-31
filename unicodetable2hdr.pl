#!/usr/bin/perl

my @table;
my $line;
my $c;
my @a;

while (my $line = <STDIN>) {
    chomp $line;
    $line =~ s/^ +//;
    next if $line =~ m/^#/;
    next if $line =~ m/^\x1A/;

    @a = split(/[ \t]+/,$line);

    $inchar = hex($a[0]);
    $outchar = hex($a[1]);

    next if $inchar < 0 || $inchar > 255;

    $table[$inchar] = $outchar;
}

print "const uint16_t __nameme__to_unicode[256] = {\n";
for ($i=0;$i < 256;$i++) {
    if (($i%8) == 0) {
        print "\t";
    }

    print sprintf("0x%04x",$table[$i]);
    print "," unless $i == 255;
    print " " if $i == 255;

    if (($i%8) == 7) {
        print sprintf("/* 0x%02x - 0x%02x */",$i-7,$i);

        print "\n";
    }
}
print "}\n";

