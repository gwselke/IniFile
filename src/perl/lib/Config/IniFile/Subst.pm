# Module Config::IniFile::Subst: read and use Windows style INI files, with some added benefits
#
# Free Software by TapirSoft Gisbert& Harald Selke GbR, 1999--2026
# See documentation for copyright notice.
#
package Config::IniFile::Subst;

use 5.010_001;    # Minimal Perl version
use strict;
use warnings;
use feature 'state';
use feature 'say';
our $VERSION = version->declare('v1.0.1');
use vars qw($PROGNAME @ISA @EXPORT @EXPORT_OK);    ## no critic(ProhibitAutomaticExportation)

use Carp;
use File::Spec;
use IO::File;
use IO::Handle;

use parent qw(Exporter);

# No items to export into caller's namespace by default.
@EXPORT = qw( );

# Information on ourselves:
$PROGNAME = 'Config::IniFile::Subst';

## no critic(ProhibitBuiltinHomonyms)

# No preloaded methods go here.

# Global static variable:
state $debug = 0;


# Standard import function to handle modifiers on the use statement:
sub import {
  my @args = @_;
  while ( $_ = shift(@args) ) {
    next if ( $_ eq 'Config::IniFile::Subst' );
    if ( $_ eq 'debug' ) {
      if ( @args && ( $args[0] =~ /^\d+$/ ) ) {    # next arg is numeric, so it must be the debug level
        $debug = shift(@args);
      } else {
        $debug++;
      }
    } else {
      carp("Unknown option '$_' for $PROGNAME");
    }
  } ## end while ( $_ = shift(@args))

  return;
} ## end sub import

## no critic (ProhibitAccessOfPrivateData)


# constructor:
sub new {
  my( $class, @inifiles ) = @_;
  my $this     = {};
  my $sections = {};
  my $inifiles = {};
  bless( $this, $class );

  # Find name of calling programme, i.e., last in chain of calls:
  my $i = 0;
  while ( caller($i) ) { $i++ }

  my $basename = File::Spec->canonpath( ( caller( $i - 1 ) )[1] );
  $basename =~ s|\.[^./\\]*$||x;

  # preparing entries for internal use that are not user accessible:
  $this->{' basename'} = $basename;    # the base name of the file containg our application
  $sections->{''}      = 1;
  $this->{' sections'} = $sections;    # a hash of sections
  $this->{' comment'}  = [];           # an array of comment lines
  $this->{' fallback'} = undef;        # the name of the fallback segment (with brackets)
  $this->{' inifiles'} = $inifiles;    # the names of the currently active input files
  $this->read(@inifiles);

  return $this;
} ## end sub new


# read one or more ini files; if none specified, use a standard name instead:
sub read {
  my( $this, @inifiles ) = @_;
  @inifiles = (undef) unless @inifiles;
  my( $fh, $resolveEscapes );

  for my $ininame (@inifiles) {
    if ( ref($ininame) ne "HASH" ) {

      # file name, possibly empty
      $ininame = $this->{' basename'} . '.ini' if ( !defined($ininame) || ( $ininame eq '' ) );
      print STDERR "$PROGNAME: Reading ini file '$ininame': " if $debug;

      # Check whether we are in for infinite recursion:
      my $absininame = File::Spec->rel2abs($ininame);
      $absininame = lc($absininame) if File::Spec->case_tolerant();
      return scalar( grep { !/^ / } CORE::keys %$this ) if ( exists $this->{' inifiles'}->{$absininame} );
      $this->{' inifiles'}->{$absininame}++;

      $fh ||= IO::File->new();
      if ( !$fh->open( $ininame, 'r' ) ) {
        say STDERR "not found" if $debug;
        next;
      }
      say STDERR "ok" if $debug;

      my $section = '';
      while (<$fh>) {

        # read all the lines
        chomp;
        s/^\s+//;
        s/\s+$//;
        next if /^$/;

        if (/^[#;*]\s*(.*)$/x) {

          # comment line, starting(!) with one of # ; *
          push( @{ $this->{' comment'} }, $1 );
          next;
        } ## end if (/^[#;*]\s*(.*)$/x)

        if (/^fallback\s*:\s*(.*)$/x) {

          # a fallback segment specification
          next if ( $1 eq '' );
          my $segment = _normalize($1);
          $segment = '' if ( $segment =~ /^"\s*"$/ );
          $segment = '' if ( $segment =~ /^'\s*'$/ );
          $segment = '[' . $segment . ']' if ( ( $segment ne '' ) && ( $segment !~ /^\[.*\]$/ ) );
          $this->{' fallback'} = $segment;
          next;
        } ## end if (/^fallback\s*:\s*(.*)$/x)

        if (/^include\s*:\s*(.*)$/x) {

          # an include file specification
          my $filename = $1;
          $this->read($filename) unless ( $filename eq '' );
          next;
        } ## end if (/^include\s*:\s*(.*)$/x)

        if (/^resolve_escapes\s*:\s*(.*)?$/x) {

          # resolve_escapes switched on or off
          $resolveEscapes = ( defined($1) && ( length($1) > 0 ) ) ? $1 : 1;
          next;
        } ## end if (/^resolve_escapes\s*:\s*(.*)?$/x)

        if (/^\[(?:.+)\]$/x) {

          # found a section name enclosed in brackets, possibly a new one.
          $section = _normalize($_);
          $this->{' sections'}{$section} = 1;
          say STDERR "$PROGNAME: entering section '$section'" if ( $debug > 1 );
        } else {

          # finally, a real entry. Split into key name and value.
          my( $key, $val ) = split( /\s*=\s*/, $_, 2 );
          next unless defined $val;
          $key = _normalize($key);
          $key =~ s/^\[.*\]//;    # remove anything looking like a section specifier at the front of the key
          if ( $val =~ /^"(.*)"$/ ) {

            # Value is enclosed in double quotes. Remove them.
            $val = $1;
          } elsif ( $val =~ /^'(.*)'$/ ) {

            # Value is enclosed in single quotes. Remove them.
            $val = $1;
          }
          $val = _unescape($val) if $resolveEscapes;

          say STDERR "$PROGNAME: adding entry '$section$key=$val'" if ( $debug > 1 );
          $this->{ uc( $section . $key ) } = $val;    # store the value under the key, possibly using the section name
        } ## end else [ if (/^\[(?:.+)\]$/x) ]
      } ## end while (<$fh>)
      $fh->close();
      delete $this->{' inifiles'}->{$absininame};
    } else {

      # hash reference, possibly %ENV, but could be any.
      say STDERR "IniFile: Reading ini values from hash" if $debug;
      $this->{ uc($_) } = $ininame->{$_} for ( keys(%$ininame) );
    } ## end else [ if ( ref($ininame) ne ...)]

    say STDERR $PROGNAME, ': ', scalar( keys %{$this} ) - 2, ' config entries' if ( $debug > 1 );
  } ## end for my $ininame (@inifiles)

  return scalar( grep { !/^ / } CORE::keys %$this );
} ## end sub read


# get an ini value, possibly using a default value. resolve variable substitutions etc.
sub get {    ## no critic(RequireArgUnpacking)
  my( $this, $key, $val ) = @_;

  $key = _normalize($key);
  my $ok;
  print STDERR "$PROGNAME: getting key '$key': " if $debug;
  if ( ( exists( $this->{$key} ) ) && ( defined( $this->{$key} ) ) ) {
    $val = $this->{$key};
    $ok++;
  } else {    # no meaningful value found

    if ( defined $this->{' fallback'} ) {    # try in fallback segment
      my $key2 = $key;
      $key2 =~ s/^\[.*?\]//;                 # remove any section specification
      $key2 = $this->{' fallback'} . $key2;  # prepend fallback segment (possibly empty)
      $val = $this->{$key2} if ( ( exists( $this->{$key2} ) ) && ( defined( $this->{$key2} ) ) );
      $ok++;
    } ## end if ( defined $this->{' fallback'...})
  } ## end else [ if ( ( exists( $this->...)))]

  if ( !$ok && ( scalar(@_) < 3 ) ) {

    # no value found and no default value was given. ($val might be undef explicitly)
    if ( $key eq 'LOGFILE' ) {               # special case: log file name
      $val = $this->{' basename'} . '.log';
    } elsif ( $key eq 'ERRFILE' ) {          # special case: error log file name
      $val = $this->{' basename'} . '.err';
    } else {                                 # otherwise: empty string
      $val = '';
    }
  } ## end if ( !$ok && ( scalar(...)))

  # check for variable substitutions enclosed in %...%:
  if ( defined($val) ) {
    my( $changed, $maxchanges, $first );
    $first = $val;
    $first =~ tr/%//cd;

    # maximum value to defuse possible infinite loops; has added benefit
    # of not even bothering to check if less than 2 percent signs are found:
    $changed = $maxchanges = int( length($first) / 2 ) * scalar( keys %$this );
    while ($changed) {
      $changed = 0;
      $first   = '';

      #while ($val =~ /%(.+?)%/) {
      #  $changed++ if exists $this->{uc($1)};
      #  $first .= $` . (exists($this->{uc($1)}) ? $this->{uc($1)} : $&);
      #  $val = $';
      #}
      while ( $val =~ /%(.+?)%/p ) {
        $changed++ if exists $this->{ uc($1) };
        $first .= ${^PREMATCH} . ( exists( $this->{ uc($1) } ) ? $this->{ uc($1) } : ${^MATCH} );
        $val = ${^POSTMATCH};
        print STDERR "\n  substituting '$1' -> '", $first, $val, "'" if ( $debug > 1 );
      } ## end while ( $val =~ /%(.+?)%/p)
      $val = $first . $val;
      last unless $maxchanges--;
    } ## end while ($changed)
  } ## end if ( defined($val) )
  say STDERR "  -> '", ( $val // '<undef>' ), "'" if $debug;

  return $val;
} ## end sub get


# get a list of ini keys defined, possibly within one section only:
sub keys {
  my( $this, $section ) = @_;
  my @keys = ();

  $section = _normalize($section) if defined $section;
  for my $key ( sort( grep { !/^ / } CORE::keys %$this ) ) {
    if ( defined $section ) {

      # if we have been asked to look in one section only, check whether this is the one:
      if ( $section eq '' ) {

        # We should look for the default (empty) section only, so skip if the key includes some section name
        next if ( $key =~ /^\[/ );
      } else {

        # We look for a non-empty section, so skip if the key does not include this section name.
        next unless ( index( $key, $section ) == 0 );
      }
    } ## end if ( defined $section )
    push( @keys, $key );
  } ## end for my $key ( sort( grep...))

  return @keys;
} ## end sub keys


# get a list of ini sections defined:
sub sections {
  my $this = shift;
  my @keys = sort( CORE::keys %{ $this->{' sections'} } );
  return @keys;
}


# programmatically add a new key, or replace an old one. Return previous value.
sub add {
  my( $this, $key, $val ) = @_;
  $key = _normalize($key);
  my $oldval = exists( $this->{$key} ) ? $this->{$key} : undef;
  $this->{$key} = $val;
  if ( $key =~ /^(\[.*\])/ ) {
    $this->{' sections'}{$1} = 1;
  }

  return $oldval;
} ## end sub add


# synonym for add:
sub set {    ## no critic(ProhibitAmbiguousNames)
  my( $this, @args ) = @_;
  return $this->add(@args);
}


# synonym for add:
sub put {
  my( $this, @args ) = @_;
  return $this->add(@args);
}


# delete an ini key. Return previous value.
sub delete {
  my( $this, $key ) = @_;
  $key = _normalize($key);
  print STDERR "$PROGNAME: deleting key '$key': " if $debug;

  my $oldval = exists( $this->{$key} ) ? $this->{$key} : undef;
  delete( $this->{$key} );

  return $oldval;
} ## end sub delete


# get any accumulated comment:               ,
sub comment {
  my($this) = @_;
  return @{ $this->{' comment'} };
}


# query or set the fallback segment
sub fallback {    ## no critic(RequireArgUnpacking)
  my( $this, $newfallback ) = @_;
  my $fallback = $this->{' fallback'};
  if ( scalar(@_) >= 2 ) {    ## no critic(RequireArgUnpacking)
                              # $newfallback might be undef specified explicitly!
    if ( defined $newfallback ) {
      $newfallback         = _normalize($newfallback);
      $newfallback         = '[' . $newfallback . ']' if ( ( $newfallback ne '' ) && ( $newfallback !~ /^\[.*\]$/ ) );
      $this->{' fallback'} = $newfallback;
    } else {
      $this->{' fallback'} = undef;
    }
  } ## end if ( scalar(@_) >= 2 )

  if ( ( defined $fallback ) && ( $fallback ne '' ) ) {
    if ( $fallback =~ /^\[(.*)\]$/ ) {
      $fallback = $1;
    }
  }

  return $fallback;
} ## end sub fallback


# write current values to file (specified by name or file handle), optionally with new comment.
# return number of entries written.
sub write {
  my( $this, $inifile, @comment ) = @_;
  @comment = @{ $this->{' comment'} } if ( @_ < 3 );
  my $fh;
  my $ct = 0;
  my $escaping;

  if ( !( defined $inifile ) ) {
    $@ = 'No file to write to specified';    ## no critic(RequireLocalizedPunctuationVars)
    return;
  }

  if ( ref($inifile) ) {

    # Not a file name, but a glob or (hopefully) a handle.
    $fh = IO::Handle->new();
    if ( ref($inifile) ne 'GLOB' ) {

      # an IO handle or similar?
      if ( !$inifile->can('print') ) {
        $@ = 'Cannot print to unknown object';    ## no critic(RequireLocalizedPunctuationVars)
        return;
      }
    } ## end if ( ref($inifile) ne ...)

    # We have either a glob or something like an IO::Handle (using duck-typing).
    if ( !$fh->fdopen( fileno($inifile), '>' ) ) {
      $@ = 'Cannot write to IO handle';           ## no critic(RequireLocalizedPunctuationVars)
      return;
    }
  } else {
    $fh = IO::File->new();
    my $mode =
      ( $inifile =~ /^\s*[>|]/ )
      ? undef
      : '>';    # guess that we are supposed to write a new file, since we have not been told otherwise
    if ( !$fh->open( $inifile, $mode ) ) {
      $@ = "Cannot open '$inifile'";    ## no critic(RequireLocalizedPunctuationVars)
      return;
    }
  } ## end else [ if ( ref($inifile) ) ]

  # write comment, if any, first:
  for my $txt ( grep { defined } @comment ) {
    my $marker = ( $txt =~ /^\s*[#;*]/ ) ? '' : '# ';    # add comment marker only if not already there
    $fh->say( $marker, $txt );
  }

  # write fallback pragma, if any:
  if ( defined $this->{' fallback'} ) {
    $fh->say( 'fallback: ', $this->fallback() );
  }

  # now write keys (without resolving any replacement indicators (%...%)), ordered by sections:
  for my $section ( $this->sections() ) {
    $fh->say($section) unless ( $section eq '' );
    for my $key ( $this->keys($section) ) {
      my $val = defined( $this->{$key} ) ? $this->{$key} : '';
      ( $val, my $escaped ) = _escape_maybe( $val, $escaping );
      if ( $escaped && !$escaping ) {
        $fh->say('resolve_escapes: 1');
        $escaping = 1;
      }

      my $enclose;    # default: no quoting
      $enclose++ if ( ( $val =~ /^\s+/ ) || ( $val =~ /\s+$/ ) );    # leading or trailing blanks -> need to quote
      $enclose++ if ( $val =~ /^"(?:.*)"$/ );    # value is contained in double quotes -> need to quote
      $enclose++ if ( $val =~ /^'(?:.*)'$/ );    # value is contained in single quotes -> need to quote
      $val = '"' . $val . '"' if $enclose;

      $fh->say( substr( $key, length($section) ), ' = ', $val );
      $ct++;
    } ## end for my $key ( $this->keys...)
  } ## end for my $section ( $this...)
  $fh->flush();
  $fh->close();

  return $ct || "0 but true";
} ## end sub write


my %_escapers = ( '\\a'  => "\a",
                  '\\b'  => "\b",
                  '\\t'  => "\t",
                  '\\r'  => "\r",
                  '\\v'  => "\x0b",
                  '\\f'  => "\f",
                  '\\n'  => "\n",
                  '\\\\' => '\\',
                  '\\?'  => '?',
                  '\\\'' => '\'',
                  '\\"'  => '"',
                );

sub _unescape {

  # internal function to handle C-style escape sequence
  # (\a, \b, \f, \n, \r, \t, \v, \', \", \\, \ooo, \x...)
  my($rest) = @_;
  return unless defined($rest);
  my $start = '';

  # walk through the input until no escape sequences found:
  while ( $rest =~ /(\\.)/p ) {
    $start .= ${^PREMATCH};
    if ( exists $_escapers{$1} ) {

      # easy one-char escape:
      $start .= $_escapers{$1};
      $rest = ${^POSTMATCH};
    } elsif ( $1 eq '\\x' ) {

      # up to 2 hex digits:
      my $t = ${^POSTMATCH};
      if ( $t =~ /([0-9a-f]{1,2})/ipx ) {
        $start .= chr( hex($1) );
        $rest = ${^POSTMATCH};
      } else {
        $rest = $t;
      }
    } elsif ( ( substr( $1, 1 ) ge '0' ) && ( substr( $1, 1 ) le '7' ) ) {

      # up to 3 octal digits:
      my $t = substr( $1, 1 ) . ${^POSTMATCH};
      if ( $t =~ /([0-7]{1,3})/px ) {
        $start .= chr( oct($1) );
        $rest = ${^POSTMATCH};
      } else {

        # cannot happen
      }
    } else {

      # no recognisable escape sequence. just ignore the backslash.
      $rest = substr( $1, 1 ) . ${^PREMATCH};
    }
  } ## end while ( $rest =~ /(\\.)/p)

  return $start . $rest;
} ## end sub _unescape


sub _escape_maybe {

  # escape non-printable chars as \x sequences, and also the escape char if required
  my( $val, $escaping ) = @_;
  if ( $escaping ||= $val =~ /[\x00-\x1f\xff]/ox ) {
    $val =~ s|[\\]|\\\\|g;
    $val =~ s/
              ([\x00-\x1f\xff])                  # Any non-printable char...
             /
             sprintf('\\x%02x', ord($1))         # ... is replaced by two hex digits
             /egx;
  }

  return ( $val, $escaping );
} ## end sub _escape_maybe


sub _normalize {

  # internal function to normalize key spellings
  my $key = uc(shift);
  $key =~ s/^\s+//;
  $key =~ s/\s+$//;
  $key =~ s/\[\s+/[/;
  $key =~ s/\s+\]/]/;
  $key =~ s/\[\]//;
  return $key;
} ## end sub _normalize

# Autoload methods go after =cut, and are processed by the autosplit program.
# But we have none.

1;
__END__

# Below is the documentation for the module.
=pod

=encoding ISO8859-1

=head1 NAME

Config::IniFile::Subst - Flexible Processing of Windows-style ini files with substitution of variables

=head1 SYNOPSIS

  use Config::IniFile::Subst;
  $ini = Config::IniFile::Subst->new();
  $ini->read(@more_inifiles);
  ...
  $val1 = $ini->get($key);
  $val2 = $ini->get($key, $default);
  $ini->add($key, $value);
  $ini->delete($key);
  @keys = $ini->keys($section);
  @sections = $ini->sections();
  $ini->write($file, 'This is', 'a comment');


=head1 DESCRIPTION

Config::IniFile::Subst offers flexible processing of Windows-style ini
files, including the usual features like sections and comment lines.
Several such configuration files may be processed in sequence, where
entries read later may override entries of the same name read earlier.
In this way, larger projects may provide shared configuration files
containing general information and default values, which can then
selectively be used, augmented, and overridden for sub-projects by their
own configuration files. The additional configuration files can be named
programmatically by the developer, or the configuration files can
dynamically include other configuration files. An important feature is
that values can include placeholder markers which will be resolved
dynamically at runtime using the contents of other entries. Specifying
non-existent configuration files is deliberately not treated as an
error. Accessing non-existent keys is handled gracefully.

Configuration entries can also be taken programmatically from hashes,
e.g., from the set of environment variables. Configuration files can be
produced by the module, although comments will, in general, not be
preserved.

A C library is available that offers the same functionality through a
largely compatible API.


=head2 The format of configuration files

Configuration files contain one key/value pair per line, separated by an
equals sign:

  key = value

White space at the beginng and end of the line and around the equals sign
will be ignored. A C<value> should be enclosed in single or double quotes if
it contains leading or trailing blanks. These quotes will be removed before
being passed on to an application. (If a value contains quotes at the start and
at the end, enclose it in another set of quotes.)

Keys should look like identifiers in most common programming languages;
in particular, they must not start with a bracket, and they must not
contain equals signs or colons.

It is not an error for a key to occur more than once. The later entry will
override the earlier one.

Sections can be supplied using the syntax:

  [section]

Keys of the same name, but in different sections are independent of each
other. Section names should follow the same rules like keys; in
particular, they must not contain closing right brackets.

A section always extends up to the next section specifier or to the end
of the file. The same section may be started more than once, in which
case the entries accumulate. Specifying the empty section name C<[]>
will switch back to the unnamed section. Processing a configuration file
always starts out in the unnamed section.

Note that, when using more than one configuration file, several files
may add entries to the same section. Subsequent files will always start
out again in the unnamed section.

Using the C<fallback:> pragma, one section may be declared as a fallback in
which all those values will be looked up by the application that cannot
be found in the section explicitly specified; e.g.,

  fallback: reserve

and somewhere else

  [reserve]
  ...

The segment name specified with C<fallback:> should not contain brackets.

The setting is valid globally, it cannot be activated only for some
subset of configuration entries. If it is specified more than once, the
last specification will override the earlier ones. The unnamed section
can be specified as fallback by specifying the empty name, i.e., two
adjacent (single or double) quotes. It is not an error if the fallback
segment specified does not exist.

The C<include:> pragma may be used within a file to include another
configuration file:

  include: more.ini

Specifying no file or a non-existent file to include is not an error and
will be silently ignored. Include directives may be nested. Recursive
use of a file will be detected and silently prevented. Still, a
configuration file can be read more than once if it is included in more
than one place, or if it is both read in explicitly and included from
some other file.

The C<resolve_escapes:> pragma may be used to change the parsing of
values (but not keys) so that C-style escape sequences are honoured. If
this pragma is specified either without a value or with an integer value
not equal to 0, the sequences C<\a>, C<\b>, C<\f>, C<\n>, C<\r>, C<\t>,
C<\v>, C<\’>, C<\">, C<\?>, C<\\> are replaced by their byte value
equivalences in the ASCII control character range. Also, C<\x> followed
by up to two hex digits will be replaced by the byte value of the hex
digit sequence, and a backslash followed by up to three octal digits
will be replaced by their byte value. Backslashes followed by any other
character will be removed (but the subsequent character will be passed
through). Replacements are done while reading the values from the file,
so it does not apply to such sequences assembled only later through the
placeholder substitution mechanism (cf. below). This behaviour is
applied starting with the next input line up to the end of the file, but
not inherited by included configuration files. The behaviour also stops
starting with the next line, if the pragma is specified with a 0 value.

Note that pragmata use colons, not equals signs.

Comments are started by C<#>, C<*> or C<;> at the start of the line
(possibly preceded by white space). End-of-line comments and multi-line
comments are not supported.

Configuration values may contain placeholder markers which will only be
resolved dynamically at the time of use. In this way, it is possible to
specify central information (e.g., a project-specific root directory)
just once and to reference this value symbolically from any number of
places in a way that is completely transparent to the application. The
syntax is deliberately simplistic. It uses bracketing by percent signs,
similar to the way that DOS/Windows batch files do; e.g.,

  dir = myroot/test/
  logfile = %dir%error.log

In such a case, an application doing C<get('logfile')> will receive the
value C<root/test/error.log> (unless the value of C<dir> has been overridden
in another place). If the value of a referenced key is later changed
programmatically, a different value for the referencing key may be
produced. The order of specification in the configuration file, however,
is immaterial to the resolution process.

Placeholders are resolved recursively, if necessary:

  dir  = %root%test/
  logfile = %dir%error.log
  root = myroot/

The result of doing C<get('logfile')> would then be the same as above. A
simple heuristic is used to prevent infinite recursion.

The substitution takes place only in values of entries. Keys, section
names, and pragmata are not subject to substitution.

The names of keys and sections are case-independent. Values, however,
preserve case.


=head2 Loading the module

When loading the module,the option C<debug> may be specified in order to
have internal information about processing written to STDERR. The option
may be specified more than once or with an integer values to get different
levels of detail. The maximally useful debug level is currently 2. E.g.,

  use Config::IniFile::Subst;                  # no frills loading
  use Config::IniFile::Subst qw(debug);        # load for debug output
  use Config::IniFile::Subst qw(debug debug);  # get more detailed debug output
  use Config::IniFile::Subst debug => 2;       # ditto


=head2 Methods

=over 4

=item new(@inifiles)

The constructor can take the name of one or many configuration files.
When no name or the value C<undef> is present, the name of the
application programme file will be used to construct the configuration
file name by cutting off its extension (if any) and adding C<.ini>. If
more than one file is specified, they will be processed in the order
given. Entries from files read later may override entries read earlier on.
Hence, the order in which files are specified may affect the retrievable
values.

Instead of a file name a reference to a hash may be passed, e.g.,
C<\%ENV>. In this case the key/values pairs of this hash will be used as
configuration entries (disregarding case in the keys).


=item read(@inifiles)

With this method further configuration files can be read at any time.
The entries will accumulate or may override previously present values.
The argument follows the same rules as for the constructor. The total
number of entries currently available in the object is returned.


=item sections()

Yields a list of section names, sorted alphabetically. Each section name
will be enclosed in brackets and in capital letters. The first name will
always be that of the unnamed section (an empty string).


=item keys($section)

When called with no arguments, a list of all existing keys (including
those in sections) will be returned. If a section name (possibly the
empty name) is given, then only keys in this section will be returned.
Keys will be in capital letters only, sorted alphabetically.

Keys contained in a section other than the unnamed section will contain
the section name in brackets prepended to the key name: C<[section]key>

The keys from the fallback segment, if specified, will only be listed
for that segment but not for the other segments, although they would
yield (fallback) values when accessed through other segments.


=item get($key, $default)

Yields the configuration value for the given key, which may contain a
section name in the form C<[section]key>. The unnamed section is accessed
either without a section name, or with an empty pair of brackets C<[]>.

If a key cannot be found and a fallback segment has been specified, the
key is looked up in the fallback segment and its value from there is
returned if found. Otherwise, the default value specified is returned.
If no default value has been given, usually the empty string is
returned. Exceptions are the commonly used keys C<LOGFILE> and
C<ERRFILE>. For these two keys, the default is to take the application
programme file's name, to cut off any file extension and then to add
C<.log> or C<.err>, respectively.

If, for a key not present, you want no empty string returned, but rather
the undefined value, use C<get($key, undef)>


=item add($key, $value)

Sets the value for the key specified (which may contain a section name
in the usual way), independently of whether the key existed previously.
If a section is specified that did not exist before, it will be created.
Returns the previous value of the key, or C<undef> (not the empty
string!), if the key did not exist before.

The fallback segment, if specified, is not affected (unless the specified
segment happens to be the fallback segment).


=item set($key, $value)

Synonym for C<add()>.


=item put($key, $value)

Synonym for C<add()>.


=item delete($key)

Deletes the entry for the specified key (which may contain a section
name in the usual way) and returns the previous value of the key. It is
no error if the key did not exist; in this case, C<undef> is returned.
If the key contains a section name and the key happens to be the last
remaining entry in the section, the section itself is not removed.
(There is no way to remove an entire section.)

The fallback segment, if specified, is not affected (unless the specified
segment happens to be the fallback segment).


=item comment()

Retrieves the accumulated comment lines as an array (with the comment
start characters removed), in the order that they were read.


=item fallback($newfallback)

Gets or sets the fallback segment. If no argument is specified, returns
the name of the fallback segment, not enclosed in brackets. If no
fallback had been specified before, C<undef> is returned.
If an argument is specified, the fallback segment is set to this value.
The previous value is returned.


=item write($file, @comment)

Writes the current set of configuration values to the file specified.
Placeholders will not be resolved but written out literally. The file
may be specified as a file name, a file handle or an L<IO::Handle>
object. If the file argument is missing, or if there is an error opening
or writing the file, C<undef> will be returned with an error text in
C<$@>. Otherwise, the number of entries written will be returned. If no
entries were present, the string "0 but true" will be returned. If the
file already exists, it will be overwritten without warning.

If an array of comments has been specified, each element will be written
as one line of comment at the beginning of the file, and the original
comments (if any) are dropped. Otherwise, the original comments will be
output in the order that they were encountered, but not necessarily in
their original positions, but rather all at the beginning. If no comment
lines that may have been read should be written out, specify C<undef> as
the @comment argument.

Note that this will output all the entries in one consolidated file.
When multiple files have been read, or when entries have been added,
changed, or deleted by other means, there is no record as to their
original source. Hence, C<include:> pragmata will not be reproduced.
The globally acting C<fallback:> pragma, on the other side, will be
preserved. The C<resolve_escapes:> pragma will be used if any value
contains characters in the range from 0x00 to 0x1f.


=back


=head1 WHY YET ANOTHER CONFIGURATION FILE MODULE?

This file is not the result of my personal rite of passage. It exists
also not only for historical reasons, although the first version was
written as early as 1999, when CPAN was not nearly as complete and
well-structured as it is today. Although the CPAN contains a remarkable
set of configuration file readers of high quality, and although several
offer some of the features offered here, none had all the flexibility
our projects needed, in particular, graceful handling of non-existent
entries and files, an inclusion meachanism, and a method for resolving
placeholders in a simple way. There is, indeed, at least one module that
allows inclusion and execution of arbitrary Perl code. This, however,
makes it very hard to prevent unwanted recursion, and it makes it highly
unpractical, to say the least, to have a C library that will handle the
same input files (short of including the whole of Perl into that C
library). In addition, the mechanism provided here is easy and safe
enough even for non-programmers.


=head1 BUGS

Any comment, whether accumulated from config files or specified explicitly,
will always be written at the top of a file generated by this module.


=head1 VERSION

This is Config::IniFile::Subst version 1.000.


=head1 AUTHOR

Gisbert W. Selke, Tapirsoft, E<lt>gisbert@tapirsoft.deE<gt>


=head1 COPYRIGHT

Copyright (C) 1999-2015 by TapirSoft Gisbert & Harald Selke GbR, http://www.tapirsoft.de.

All rights reserved.

This library is free software; you can redistribute it and/or modify it
under the same terms as Perl itself.


=head1 SEE ALSO

Other configuration management modules, in particular those in the namespace Config::.

=cut
