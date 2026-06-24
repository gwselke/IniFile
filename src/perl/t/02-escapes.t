#!perl -T
# testing handling of esacape codes
use 5.006;
use strict;
use warnings;
use File::Spec;
use Test::More tests => 8;

BEGIN { use_ok('Config::IniFile::Subst'); }

my $ini = new_ok('Config::IniFile::Subst' => [ 't/02-escapes.ini' ]);

my %testkeys = (
                 'KEY0'  => 'rhabarber',
                 'KEY1'  => "rha\\tbarber",
                 'KEY2'  => "rha\\tbarber",
                 'KEYA'  => "rha\x07barber",
                 'KEYB'  => "rha\x08barber",
                 'KEYBS' => "rha\\barber",
                 'KEYDQ' => 'rha"barber',
                 'KEYF'  => "rha\x0cbarber",
                 'KEYN'  => "rha\x0abarber",
                 'KEYO1' => "rha\x01arber",
                 'KEYO2' => "rha\x09arber",
                 'KEYO3' => 'rhaIarber',
                 'KEYO4' => 'rhaI1arber',
                 'KEYQM' => 'rha?barber',
                 'KEYR'  => "rha\x0dbarber",
                 'KEYSQ' => "rha'barber",
                 'KEYT'  => "rha\x09barber",
                 'KEYV'  => "rha\x0bbarber",
                 'KEYX1' => "rha\x0axarber",
                 'KEYX2' => "rha\xabxarber",
                 'KEYX3' => "rha\xabarber",
                 'KEYZ1' => "rha\\tbarber",
                 'KEYZ2' => "rha\x09barber",
               );

my @keys;
eval { @keys = $ini->keys(); };
is(scalar(@keys), scalar(keys(%testkeys)), 'number of keys');

my @testkeys = sort(keys(%testkeys));
my @errs = ();
my $n = (@keys < @testkeys) ? @keys : @testkeys;
for my $i(0..$n-1) {
  if ($keys[$i] ne $testkeys[$i]) {
    push(@errs, "key $i not correct: '$keys[$i]' instead of '$testkeys[$i]'");
  } elsif ($ini->get($keys[$i]) ne $testkeys{$keys[$i]}) {
    push(@errs, "value $i (key '$keys[$i]') not correct: '" . $ini->get($keys[$i]) . "' instead of '$testkeys{$keys[$i]}'");
  }
}
diag(join("\n", @errs)) if @errs;
is(@errs, 0, 'key/values');


# test writing values with a need for esacape codes
isnt($ini->write('t/02-test.out'), 0, 'writing an ini file');


# test wether written codes are still ok
my $ini2 = new_ok('Config::IniFile::Subst' => [ 't/02-escapes.ini' ]);

eval { @keys = $ini2->keys(); };
is(scalar(@keys), scalar(keys(%testkeys)), 'reread number of keys');

@testkeys = sort(keys(%testkeys));
@errs = ();
$n = (@keys < @testkeys) ? @keys : @testkeys;
for my $i(0..$n-1) {
  if ($keys[$i] ne $testkeys[$i]) {
    push(@errs, "reread key $i not correct: '$keys[$i]' instead of '$testkeys[$i]'");
  } elsif ($ini->get($keys[$i]) ne $testkeys{$keys[$i]}) {
    push(@errs, "reread value $i (key '$keys[$i]') not correct: '" . $ini->get($keys[$i]) . "' instead of '$testkeys{$keys[$i]}'");
  }
}
diag(join("\n", @errs)) if @errs;
is(@errs, 0, 'reread key/values');

unlink('t/02-test.out');
