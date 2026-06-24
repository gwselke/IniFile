#!perl -T
use 5.006;
use strict;
use warnings;
use File::Spec;
use Test::More tests => 31;

BEGIN { use_ok('Config::IniFile::Subst'); }

my $ini = new_ok('Config::IniFile::Subst' => [undef, 't/test2.ini']);

my(@errs, $n);


# testing sections...
#
my @testsections = ('', '[SECTIONA]', '[SECTIONB]' );
my @sections;
eval { @sections = $ini->sections(); };

is(@sections, @testsections,               'number of sections');
$n = (@sections < @testsections) ? @sections : @testsections;
@errs = ();
@errs = map { "wrong section $_: '$sections[$_]' instead of '$testsections[$_]'" }
          grep { $sections[$_] ne $testsections[$_] } (0..$n-1);
diag(join("\n", @errs)) if @errs;
is(@errs, 0,                               'section names');


# testing keys and values in general...
#
my %testkeys = (
                'ERRFILE'        => '/foo/bar\my.err',
                'KEY1'           => 'wert1',
                'KEY10'          => 'rha"wert2barber',
                'KEY11'          => '%key11%',
                'KEY12'          => 'rha%key11%barblubber ber',
                'KEY13'          => 'rose = rose = rose',
                'KEY2'           =>  '"wert2',
                'KEY3'           =>  '',
                'KEY4'           =>  'bla#bla',
                'KEY5'           =>  'blabla #',
                'KEY6'           =>  'blubber ',
                'KEY7'           =>  'rhabarber',
                'KEY8'           =>  'rhabarber',
                'KEY9'           =>  'rha"wert2barwert1ber',
                'TESTDIR1'       => '/foo/bar',
                'TESTDIR2'       => '/foo/bar/',
                'TESTDIR3'       => "/foo/bar\\",
                '[SECTIONA]KEY1' => 'key1a',
                '[SECTIONA]KEY2' => 'key2a',
                '[SECTIONB]KEY1' => 'key1b',
                );
my @keys;
eval { @keys = $ini->keys(); };
is(scalar(@keys), scalar(keys(%testkeys)), 'number of keys');

my @testkeys = sort(keys(%testkeys));
@errs = ();
$n = (@keys < @testkeys) ? @keys : @testkeys;
for my $i(0..$n-1) {
  if ($keys[$i] ne $testkeys[$i]) {
    push(@errs, "key $i not correct: '$keys[$i]' instead of '$testkeys[$i]'");
  } elsif ($ini->get($keys[$i]) ne $testkeys{$keys[$i]}) {
    push(@errs, "value $i (key '$keys[$i]') not correct: '" . $ini->get($keys[$i]) . "' instead of '$testkeys{$keys[$i]}'");
  }
}
diag(join("\n", @errs)) if @errs;
is(@errs, 0,                               'key/values');


# testing keys in sections...
#
is($ini->get('[]KEY1'), $ini->get('key1'), 'explicitly getting key from unnamed section');

is(scalar($ini->keys('')), @testkeys-3,    'number of keys in default section');
is(scalar($ini->keys('[SECTIONA]')), 2,    'number of keys in section [SECTIONA]');


# testing default values...
#
is($ini->get('KEY99'), '',                 'undefined KEY99 with no default');
is($ini->get('KEY99', 'dummy'), 'dummy',   'undefined KEY99 with default "dummy"');
is($ini->get('KEY99', undef),   undef,     'undefined KEY99 with default undef');

is($ini->get('LOGFILE'), File::Spec->canonpath('t/00-general.log'), 'LOGFILE default');
is(File::Spec->canonpath($ini->get('ERRFILE')), File::Spec->canonpath('/foo/bar\\my.err'), 'ERRFILE non-default');


# testing file comments...
#
my @testcomment = ('test file 1 for module Config::IniFile::Subst',
                   'test file 2 for module Config::IniFile::Subst',
                   'test file 3 for module Config::IniFile::Subst',
                  );
my @comment = $ini->comment();
is(scalar(@comment), scalar(@testcomment), 'number of comment lines');
$n = (@comment < @testcomment) ? @comment : @testcomment;
for my $i(0..$n-1) {
  if ($comment[$i] ne $testcomment[$i]) {
    push(@errs, "comment $i not correct: '$comment[$i]' instead of '$testcomment[$i]'");
  }
}
diag(join("\n", @errs)) if @errs;
is(@errs, 0,                               'comment');


# minimal testing of the (unspecified) fallback segment...
#
is($ini->fallback(), undef,                'fallback segment unspecified');


# testing writing an ini file with original comments...
#
isnt($ini->write('t/test.out'), 0,         'writing an ini file');

my $ini2 = new_ok('Config::IniFile::Subst' => ['t/test.out']);

my @keys2;
eval { @keys2 = $ini2->keys(); };
is(scalar(@keys2), scalar(@keys),          'number of keys in file written');

@errs = ();
$n = (@keys < @keys2) ? @keys : @keys2;
for my $i(0..@keys-1) {
  if ($keys[$i] ne $keys2[$i]) {
    push(@errs, "key $i not correct: '$keys2[$i]' instead of '$keys[$i]'");
  } elsif ($ini->get($keys[$i]) ne $ini2->get($keys[$i])) {
    push(@errs, "value $i (key '$keys[$i]') not correct in written file: '" . $ini2->get($keys[$i]) . "' instead of '" . $ini->get($keys[$i]) . "'");
  }
}
diag(join("\n", @errs)) if @errs;
is(@errs, 0,                               'keys/values');

@comment = $ini2->comment();
is(scalar(@comment), scalar(@testcomment), 'number of comment lines in file written');
$n = (@comment < @testcomment) ? @comment : @testcomment;
for my $i(0..$n-1) {
  if ($comment[$i] ne $testcomment[$i]) {
    push(@errs, "comment $i not correct: '$comment[$i]' instead of '$testcomment[$i]'");
  }
}
diag(join("\n", @errs)) if @errs;
is(@errs, 0,                               'comment in file written');


# testing writing an ini file with explicit comment...
#
ok($ini->write('t/test.out', 'new comment') > 0, 'writing an ini file with comment');
$ini2 = new_ok('Config::IniFile::Subst' => ['t/test.out']);
@comment = $ini2->comment();
is(scalar(@comment), 1,                    'number of comment lines in file written');
is($comment[0], 'new comment',             'explicitly set comment in file');

isnt($ini->write('t/test.out', undef), 0,  'writing an ini file without comment');
$ini2 = new_ok('Config::IniFile::Subst' => ['t/test.out']);
@comment = $ini2->comment();
is(scalar(@comment), 0,                    'number of comment lines in file written');

unlink('t/test.out');


# testing "forgetting" a file name to write to:
#
is($ini->write(), undef,                   'trying to write without specifying a name');

