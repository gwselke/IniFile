The IniFile Project
===================

This is a multi-language package for handling configuration files in a
flexible, maintainable, language-agnostic way.

Currently, this package supports implementations in C (plain old C) and
Perl. More languages may be added in the future.

For the end user, i.e., for the contents of config files, it should make
no difference which programming language the application uses. For the
application developer, the interfaces to the various supported languages
should feel as similar as possible, given the different environments
those languages provide.

This project offers flexible processing of Windows-style configuration
files, a.k.a. INI files, including the usual features like sections and
comment lines. Several such configuration files may be processed in
sequence, where entries read later may override entries of the same name
read earlier. In this way, larger projects may provide shared
configuration files containing general information and default values,
which can then selectively be used, augmented, and overridden for
sub-projects by their own configuration files. The additional
configuration files can be named programmatically by the developer, or
the configuration files can dynamically include other configuration
files.

An important feature is that values can include placeholder markers
which will be resolved dynamically at runtime using the contents of
other entries. Specifying non-existent configuration files is
deliberately not treated as an error. Accessing non-existent keys is
handled gracefully.

