#!perl -T
use 5.006;
use strict;
use warnings;
use Test::More tests => 31;

BEGIN { use_ok('Config::IniFile::Subst'); }

my $ini = new_ok('Config::IniFile::Subst' => ['t/00-general.ini']);


# testing programmatical setting...
#
is($ini->add('key99', 'dummy'),     undef,          'setting new key');
is($ini->get('KEY99'),              'dummy',        'getting new key');

is($ini->add('key99', 'dummy1'),    'dummy',        'previously new value');
is($ini->get('KEY99'),              'dummy1',       'new value of previously new key');

is($ini->add('[sectionc]key99', 'dummy'), undef,    'setting key in new section');
is($ini->get('[sectionc]KEY99'),    'dummy',        'getting key from new section');
my %sections = map { $_ => 1 } $ini->sections();
ok(exists($sections{'[SECTIONC]'}),                 'existence of newly created section');


# testing deletion...
#
is($ini->delete('KEY99'),           'dummy1',       'deleting key');
is($ini->get('KEY99'),              '',             'new value of deleted key');

is($ini->delete('KEY99'),           undef,          'deleting deleted key');
is($ini->get('KEY99'),              '',             'value of doubly deleted key');

is($ini->delete('[sectionc]KEY99'), 'dummy',        'deleting key from new section');
is($ini->get('[sectionc]KEY99'),    '',             'new value of deleted key from new section');
%sections = map { $_ => 1 } $ini->sections();
ok(exists($sections{'[SECTIONC]'}),                 'existence of now empty section');


# testing setting quoted values...
#
is($ini->add("key98", '"dummy"'),   undef,          'setting new key with quotes');
is($ini->get('KEY98'),              '"dummy"',      'getting new key with quotes');


# testing of the fallback segment...
#
is($ini->get('[sectionb]key6'),     '',             'getting non-existent value');
is($ini->fallback(''),              undef,          'previous fallback segment unspecified');
is($ini->fallback(),                '',             'new fallback segment: unnamed');
is($ini->get('[sectionb]key1'),     'key1b',        'getting existent value not needing fallback');
is($ini->get('[sectionb]key2'),     '"wert2',       'getting non-existent value from fallback');
is($ini->fallback('sectiona'),      '',             'previous fallback segment: unnamed');
is($ini->fallback(),                'SECTIONA',     'new fallback segment');
is($ini->get('[sectionb]key2'),     'key2x',        'getting non-existent value from new fallback');


# testing writing...
#
isnt($ini->write('t/test.out'),     0,              'writing an ini file');

my $ini2 = new_ok('Config::IniFile::Subst' => ['t/test.out']);

my @keys = $ini->keys();
my @keys2;
eval { @keys2 = $ini2->keys(); };
is(scalar(@keys2), scalar(@keys),                   'number of keys in file written');

my @errs = ();
my $n = (@keys < @keys2) ? @keys : @keys2;
for my $i(0..@keys-1) {
  if ($keys[$i] ne $keys2[$i]) {
    push(@errs, "key $i not correct: '$keys2[$i]' instead of '$keys[$i]'");
  } elsif ($ini->get($keys[$i]) ne $ini2->get($keys[$i])) {
    push(@errs, "value $i (key '$keys[$i]') not correct in written file: '" . $ini2->get($keys[$i]) . "' instead of '" . $ini->get($keys[$i]) . "'");
  }
}
diag(@errs) if @errs;
is(@errs,                           0,              'key/values');
is($ini->get('[sectionb]key2'),     'key2x',        'getting non-existent value from fallback');

unlink('t/test.out');

