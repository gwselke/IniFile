require 5.010001;
use strict;
use ExtUtils::MakeMaker 6.64;
use File::ShareDir::Install;

my $mod_name     =  'Config::IniFile::Subst';
my $mod_file     =  'lib/' . $mod_name . '.pm';
$mod_file        =~ s|::|/|g;
my $mod_distname =  $mod_name;
$mod_distname    =~ s|::|-|g;
my $mod_version  = MM->parse_version($mod_file);

WriteMakefile
(
  NAME               => $mod_name,
  VERSION_FROM       => $mod_file,
  ABSTRACT_FROM      => $mod_file,
  LICENSE            => 'artistic_2',
  PL_FILES           => { },
  MIN_PERL_VERSION   => 5.001,
  CONFIGURE_REQUIRES => { 'ExtUtils::MakeMaker'     => 6.64, },
  TEST_REQUIRES      => { 'Test::More'  => 0.82, },
  PREREQ_PM          => {
                          'Carp' => 0,
                          'File::Spec' => 0,
                          'IO::File' => 0,
                          'IO::Handle' => 0,
                          'Module::Build' => '0.42',
                          'Test::More' => 0,
                          'parent' => 0,
                          'version' => '0.77'
                        },
  dist               => { COMPRESS => 'gzip -9f', SUFFIX => 'gz', },
  clean              => { FILES    => $mod_distname . '-*', },
  META_MERGE         => {
                          'meta-spec'    => { version => 2 },
                          dynamic_config => 0,
                          prereqs        => {
                                              test => {
                                                        recommends => {
                                                                        'Test::CheckManifest' => 0,
                                                                        'Test::CheckVersion' => 0,
                                                                        'Test::CPAN::Changes' => 0,
                                                                        'Test::CPAN::Meta' => 0,
                                                                        'Test::CPAN::Meta::JSON' => 0,
                                                                        'Test::Pod' => 1.22,
                                                                        'Test::Pod::Coverage' => 1.0,
                                                                      },
                                                      },
                                            },
                          provides       => {
                                              $mod_name => {
                                                             file    => $mod_file,
                                                             version => $mod_version,
                                                           },
                                            },
                          resources      => {
                                               repository => {
                                                   type => 'git',
                                                   url  => 'https://github.com/gwselke/IniFile.git',
                                                   web  => 'https://github.com/gwselke/IniFile',
                                               },
                                              bugtracker => {
                                                              web    => 'https://github.com/gwselke/IniFile/issues'
                                                            },
                                            },
                        },
);
