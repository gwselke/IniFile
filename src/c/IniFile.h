/** Header-Datei für den Konfigurationsdateihandler IniFile.
 ***************************************************************************
 \file IniFile.h

   \author               Gisbert W. Selke

   \date                 2021-07-20

   \version              1.20

   Copyright &copy;      TapirSoft Gisbert und Harald Selke GbR

  ****************************************************************************

  \par Kurzbeschreibung

  \sa
  IniFile.c

  ****************************************************************************

  \$Id: IniFile.h 426 2007-10-24 22:32:18Z gisbert $

  \history

       Datum      Kürzel Version Beschreibung
   <hr>
   \li 2021-07-20 gws    1.20    Segmentnamen aus lauter Leerzeichen sind jetzt
                                 äquivalent zum unbenannten Segment.
                                 Ersetzung von Platzhaltern mit leerem Wert repariert.
                                 Funktionen ini_comments( ), ini_ncomments( ) und
                                 ini_add_comment( ) hinzugefügt.
                                 Angabe nichtexistenter/nichtlesbarer/fehlender Ini-Dateien
                                 ist kein Fehler mehr.
                                 Dokumentation ergänzt. Testfälle systematisiert.
   \li 2021-03-12 gws    1.12    Fehlerhafte Behandlung bei fehlender Schlüsselangabe
                                 korrigiert. Probleme mit isspace( ), toupper( ) usw.
                                 bei 8-Bit-Zeichen umgangen.
   \li 2021-02-01 gws    1.11    Fehlercode korrigiert, wenn Ini-Datei nicht gefunden
                                 Secure-Funktionen/-Makros eingeführt.
   \li 2007-10-25 gws    1.1     Pragma resolve_escapes eingeführt.
   \li 2007-06-25 gws    1.01    INI_COPYSTR verfeinert, INI_STRLEN eingeführt.
   \li 2007-04-02 gws    1.0     Dokumentation angepasst, Parametertypen genauer gefasst,
                                 ini_get_version( ) hinzugefügt.
   \li 2007-03-11 gws    1.0rc2  Unendliche Rekursion bei Include verhindert.
   \li 2007-02-05 gws    1.0rc1  Include-Mechanismus sowie 2 Pragmata implementiert.
   \li 2007-02-02 gws    0.95    Fallback-Abschnitt und ini_catfile eingeführt.
   \li 2006-04-25 gws    0.942   Deprecation Warnings von Visual Studio 2005 abgewürgt.
   \li 2005-09-27 gws    0.941   Prototyp für ini_destroy( ) korrigiert.
   \li 2005-05-08 gws    0.94    Minimale Erweiterungen.
   \li 2005-05-01 gws    0.2     Verarbeitung von Abschnitten voll integriert.
   \li 2004-11-04 gws    0.12    Minimale Funktionserweiterung.
   \li 1998-xx-xx gws    0.1x    Erste Produktionsversion.

 *****************************************************************************/
#ifndef _IniFile_h
#define _IniFile_h

#ifdef _MSC_VER
#define HAS_MSIZE
#endif
#include <stdlib.h>

/** Rückgabecode, falls keine Ini-Datei angegeben wurde. */
#define INI_ERR_NONAME   ( -1 )

/** Rückgabecode, falls die angegebene Ini-Datei nicht gelesen bzw. geschrieben werden konnte. */
#define INI_ERR_NOFILE   ( -2 )

/** Rückgabecode, falls nicht genügend Speicherplatz vorhanden ist. */
#define INI_ERR_NOMEMORY ( -3 )

/** Rückgabecode, falls ein unbekanntes Pragma angegeben wurde. */
#define INI_ERR_NOSUCHPRAGMA ( -4 )

/** Struktur zum Verwalten von Ini-Einträgen (Schlüssel-Wert-Paare) in einer verketteten Liste. */
typedef struct IniEntry {
  char *key;               /**< Schlüssel. */
  char *value;             /**< Wert.      */
  struct IniEntry *next;   /**< Pointer auf den nächsten Eintrag. */
} TIniEntry;


/** Sichere (safe) Speicherallokation.
 *  \param[in] size      Angeforderte Speichergröße in Bytes.
 *  \return              Pointer auf den Speicherplatz. NULL, falls nicht genügend Speicher vorhanden.
 */
#define INI_MALLOC(size)             ( ini_safemalloc( (size_t)(size) ) )

/** Sichere (safe) Speicherallokation für Strings (hängt Platz für NUL automatisch mit an).
 *  \param[in] len       Angeforderte maximale Stringlänge in Bytes (ohne Zählung des abschließenden NUL).
 *  \return              Pointer auf den Speicherplatz. NULL, falls nicht genügend Speicher vorhanden.
 */
#define INI_STRALLOC(len)            ( (char *)INI_MALLOC( ( (size_t)(len)+1 )*sizeof(char) ) )

/** Sichere (safe) Speicher-Reallokation.
 *  \param[in] memp      Pointer auf den bisherigen Speicherplatz.
 *  \param[in] size      Angeforderte Speichergröße in Bytes.
 *  \return              Pointer auf den Speicherplatz. NULL, falls nicht genügend Speicher vorhanden.
 */
#define INI_REALLOC(memp, size)      ( ini_saferealloc( (memp), (size_t)(size) ) )

/** Sichere (safe & secure) Speicher-Reallokation.
 *  \param[in] memp      Pointer auf den bisherigen Speicherplatz.
 *  \param[in] newsize   Angeforderte Speichergröße in Bytes.
 *  \param[in] oldsize   Bisherige Speichergröße in Bytes.
 *  \return              Pointer auf den Speicherplatz. NULL, falls nicht genügend Speicher vorhanden.
 */
#define INIS_REALLOC(memp, newsize, oldsize)    ( inis_saferealloc( (memp), (size_t)(newsize), (size_t)(oldsize) ) )

#ifdef HAS_MSIZE
/** Sichere (safe & secure) Speicher-Reallokation.
 *  \param[in] memp      Pointer auf den bisherigen Speicherplatz.
 *  \param[in] newsize   Angeforderte Speichergröße in Bytes.
 *  \return              Pointer auf den Speicherplatz. NULL, falls nicht genügend Speicher vorhanden.
 */
#define INIS0_REALLOC(memp, newsize) ( INIS_REALLOC( (memp), (newsize), _msize(memp) ) )
#endif

/** Sichere (safe) Speicher-Reallokation für Strings.
 *  \param[in] memp      Pointer auf den bisherigen Speicherplatz.
 *  \param[in] len       Angeforderte maximale Stringlänge in Bytes (ohne Zählung des abschließenden NUL).
 *  \return              Pointer auf den Speicherplatz. NULL, falls nicht genügend Speicher vorhanden.
 */
#define INI_STRREALLOC(memp, len)    ( (char *)INI_REALLOC( (memp), ( (size_t)(len)+1 )*sizeof(char) ) )

#ifdef HAS_MSIZE
/** Sichere (safe & secure) Speicher-Reallokation für Strings.
 *  \param[in] memp      Pointer auf den bisherigen Speicherplatz.
 *  \param[in] len       Angeforderte maximale Stringlänge in Bytes (ohne Zählung des abschließenden NUL).
 *  \return              Pointer auf den Speicherplatz. NULL, falls nicht genügend Speicher vorhanden.
 */
#define INIS0_STRREALLOC(memp, len)  ( (char *)INIS0_REALLOC( (memp), ( (size_t)(len)+1 )*sizeof(char) ) )
#endif

/** Sichere (safe) Speicherfreigabe.
 *  \param[in,out] memp  Pointer auf den bisherigen Speicherplatz. Wird auf NULL gesetzt.
 */
#define INI_FREE(memp)               { if (memp) free(memp); (memp) = NULL; }

/** Sichere (safe & secure) Speicherfreigabe.
 *  \param[in,out] memp  Pointer auf den bisherigen Speicherplatz. Wird auf NULL gesetzt.
 *  \param[in]     size  Bisherige Speichergröße in Bytes; wird exakt so benutzt.
 */
#define INIS0_FREE(memp, size)        { if (memp) { inis_wipe_memory( (memp), (size) ); free(memp); (memp) = NULL; } }

/** Sichere (safe & secure) Speicherfreigabe (size wird nur genutzt auf Systemen ohne msize( )).
 *  \param[in,out] memp  Pointer auf den bisherigen Speicherplatz. Wird auf NULL gesetzt.
 *  \param[in]     size  Bisherige Speichergröße in Bytes (wird nur genutzt auf Systemen ohne msize( )).
 */
#ifdef HAS_MSIZE
#define INIS_FREE(memp, size)        { INIS0_FREE( (memp), _msize(memp) ); }
#else
#define INIS_FREE(memp, size)        { INIS0_FREE( (memp), (size) ); }
#endif

#ifdef HAS_MSIZE
/** Sichere (safe & secure) Speicherfreigabe (Nur nutzbar auf Systemen mit msize( )).
 *  \param[in,out] memp  Pointer auf den bisherigen Speicherplatz. Wird auf NULL gesetzt.
 */
#define INIS1_FREE(memp)             { INIS0_FREE( (memp), _msize(memp) ); }
#endif

/** Sichere (safe & secure) Speicherfreigaben für Strings.
 *  Wirklich sicher nur auf Systemen, die msize( ) kennen; sonst wird als Best Guess
 * die String-Länge genommen, was zu kurz sein kann.
 *  \param[in,out] memp  Pointer auf den bisherigen Speicherplatz. Wird auf NULL gesetzt.
 */
#ifdef HAS_MSIZE
#define INIS_STRFREE(memp)           { INIS1_FREE(memp); }
#else
#define INIS_STRFREE(memp)           { INIS0_FREE( (memp), INI_STRLEN(memp) ); }
#endif

/** Sicheres Kopieren von Strings (so gut das in C eben geht).
 *  \param[out] dst      Pointer auf den Ziel-Speicherplatz.
 *  \param[in]  src      Pointer auf den Quell-Speicherplatz.
 *  \param[in]  len      Anzahl zu kopierender Bytes. Es werden genau so viele Bytes wie
 *                       angegeben kopiert, selbst wenn der Quellstring kürzer ist.
 *                       Im Ziel-Speicherplatz muss auf jeden Fall Platz für ein zusätzliches
 *                       Zeichen sein, das auf jeden Fall mit NUL gefüllt wird.
 *                       Die Funktion entspricht ungefähr einer etwas sichereren Variante von
 *                       von strncpy( ), kopiert jedoch unter Umständen mehr!
 */
#define INI_COPYSTR( dst, src, len ) {                                           \
                                       if (dst) {                                \
                                         size_t INI_len = (size_t)(len);         \
                                         if (src) {                              \
                                           memcpy( (dst), (src), INI_len );      \
                                           *( (dst)+INI_len ) = '\0';            \
                                         } else {                                \
                                           memset( (dst), '\0', INI_len+1 );     \
                                         }                                       \
                                       }                                         \
                                     }

/** Sichere Bestimmung von String-Längen (so gut das in C eben geht).
 *  \param[in] src       Pointer auf den Quell-Speicherplatz.
 */
#define INI_STRLEN(src) ( (src) ? strlen(src) : 0 )

/** Test zweier Strings auf Gleichheit (etwas sichererer und bequemerer Wrapper um strcmp( )).
 *  \param[in] str1      Pointer auf den ersten der beiden Strings. Kann NULL sein.
 *  \param[in] str2      Pointer auf den ersten der beiden Strings. Kann NULL sein.
 *  \result              TRUE, wenn gleich; sonst FALSE.
 */
#define INI_STREQUAL( str1, str2 ) ( (str1) ? ( (str2) ? ( strcmp((str1), (str2)) == 0 ) : 0 ) : ( (str2) ? 0 : 1 ) )

/** Setzt ein einzelnes Zeichen auf Großschreibung um (char, nicht uchar).
 *  \param[in] c         Das Zeichen (char; darf also auch < 0 sein).
 *  \result              Wie toupper( ).
 */
#define INI_CHAR_TOUPPER(c) ( (char)toupper( (uchar)(c) ) )

/** Testet auf Whitespace (char oder uchar).
 *  \param[in] c         Das Zeichen (char oder uchar; darf also auch < 0 sein).
 *  \result              Wie isspace( ).
 */
#define INI_ISSPACE(c) ( isspace( (uchar)(c) ) )

/** Testet auf Ziffer (char oder uchar).
 *  \param[in] c         Das Zeichen (char oder uchar; darf also auch < 0 sein).
 *  \result              Wie isdigit( ).
 */
#define INI_ISDIGIT(c) ( isdigit( (uchar)(c) ) )

/** Testet auf Hexadezimalziffer (char oder uchar).
 *  \param[in] c         Das Zeichen (char oder uchar; darf also auch < 0 sein).
 *  \result              Wie isxdigit( ).
 */
#define INI_ISXDIGIT(c) ( isxdigit( (uchar)(c) ) )

/** Hilfsfunktion zum Kopieren von Strings bei gleichzeitiger Umsetzung auf Großbuchstaben.
 *  \param[out] dst      Pointer auf den Ziel-Speicherplatz. Muss mindestens so viel Platz wie src bieten.
 *  \param[in]  src      Pointer auf den Quell-Speicherplatz.
 */
#define INI_COPYSTRUPPER( dst, src ) { char *SUC_temp1 = (char *)(dst), *SUC_temp2 = (char *)(src);                \
                                       if (SUC_temp2) {                                                            \
                                         while ( *SUC_temp2 ) *(SUC_temp1++) = INI_CHAR_TOUPPER( *(SUC_temp2++) ); \
                                       }                                                                           \
                                       *SUC_temp1 = '\0';                                                          \
                                     }

/** Hilfsfunktion zum Umsetzen eines Strings auf Großbuchstaben.
 *  \param[in,out] str   Pointer auf den Quell=Ziel-Speicherplatz.
 */
#define INI_STRUPPER(str) { char *SU_temp;                  \
                            if (str) {                      \
                              for ( SU_temp=(str); *SU_temp; SU_temp++ ) *SU_temp = INI_CHAR_TOUPPER( *SU_temp ); \
                            }                               \
                          }


/** Einlesen einer Konfigurationsdatei. */
int ini_read(const char * const inifilename);

/** Holt den Wert, der zu einem gegebenen Konfigurations-Schlüssel gehört. */
char *ini_get( char *value, const char *key, const char * const defaultvalue, const size_t maxlen );

/** Fügt einen Konfigurationseintrag (Schlüssel/Wert) hinzu. */
int ini_add( const char *key, const char * const value );

/** Synonym für ini_add( ): */
int ini_set( const char *key, const char * const value );

/** Liefert eine Liste der Schlüssel. */
char **ini_keys( char **keylist, int maxkeys );

/** Liefert eine Liste der Schlüssel in einem gegebenen Abschnitt. */
char **ini_keys_section( char **keylist, int maxkeys, const char *sectionname );

/** Liefert eine Liste der Konfigurationsabschnitte zurück. */
char **ini_sections( char **sectionlist, int maxsections );

/** Liefert die Anzahl der Konfigurationseinträge zurück. */
int ini_nkeys(void);

/** Liefert die Anzahl der Konfigurationseinträge in einem gegebenen Abschnitt zurück. */
int ini_nkeys_section( const char *sectionname );

/** Liefert die Anzahl der Konfigurationsabschnitte zurück. */
int ini_nsections(void);

/** Liefert die Anzahl der Kommentare zurück. */
int ini_ncomments(void);

/** Liefert eine Liste der Kommentare. */
char **ini_comments( char **commentlist, int maxcomment );

/** Fügt eine Kommentarzeile hinzu. */
int ini_add_comment( const char *text );

/** Löscht einen Schlüssel, falls es ihn gibt. */
int ini_delete( const char *key );

/** Schreibt die aktuellen Ini-Werte in eine Datei. */
int ini_write( const char * const inifilename, const char *comment );

/** Löscht alle Schlüssel, löscht alle Strukturen. */
void ini_destroy(void);

/** Setzt den Namen des Fallback-Abschnitts für im angegebenen Abschnitt nicht gefundene Einträge. */
int ini_set_fallbacksection( const char *sectionname );

/** Holt den Namen des Fallback-Abschnitts für im angegebenen Abschnitt nicht gefundene Einträge. */
char *ini_get_fallbacksection(void);

/** Liefert ggf. den Namen der Ini-Datei, in der ein Fehler aufgetreten ist. */
char *ini_get_error_filename(void);

/** Liefert ggf. die Nummer der Zeile der Ini-Datei, in der ein Fehler aufgetreten ist. */
int ini_get_error_linenumber(void);

/** Liefert die aktuelle Version der Library. */
unsigned short ini_get_version(void);

/** Liefert die aktuelle Version der Library. */
const char *ini_get_version_string(void);

/** Liefert den Lizenz- und Copyright-Text. */
char *ini_get_license(void);

/** Gibt das Verzeichnistrennzeichen im Unix- oder DOS-Stil zurück. */
char ini_get_dir_delimiter( const unsigned char option );

/** Setzt alle Vorkommen von Verzeichnistrennern auf Unix- oder DOS-Stil um. */
char *ini_fix_dir_delim( char *path, const unsigned char option );

/** Hängt einen Verzeichnistrenner an, falls nicht schon vorhanden. */
char *ini_add_dir_delim( char *dirpath, const unsigned char option );

/** Sichere (safe) Allokation von Speicherplatz. */
void *ini_safemalloc( const size_t size );

/** Sichere (safe) Re-Allokation von Speicherplatz. */
void *ini_saferealloc( void *memp, const size_t size );

/** Sicheres (secure) Löschen von Speicherplatz. */
void __inline inis_wipe_memory( void *memp, const size_t size );

#endif
