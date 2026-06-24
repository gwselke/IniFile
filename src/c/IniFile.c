/** Der Konfigurationsdateihandler IniFile.
 ***************************************************************************
 \file IniFile.c

   \author               Gisbert W. Selke

   \date                 2021-07-20

   \version              1.20

   Copyright &copy;      TapirSoft Gisbert und Harald Selke GbR

   \todo                 Alles auf uchar statt char umstellen?

  ****************************************************************************

  \mainpage Die Benutzung von IniFile aus Anwendersicht


  IniFile dient dazu, flexibel Konfigurationsinformationen zu verwalten, die
  in Ini-Dateien im Windows-Stil vorliegen. Ein kompatibles, funktional
  weit gehend identisches Perl-Modul zum gleichen Zweck liegt ebenfalls vor.

  Mehrere Ini-Dateien können nacheinander verarbeitet werden, wobei
  später eingelesene Dateien Vorrang vor früheren haben. Dadurch können
  bei größeren Projekten allgemein gehaltene Konfigurationsdateien für
  Standardwerte benutzt werden, die für einzelne Teilanwendungen durch
  eigene INI-Dateien selektiv überschrieben werden können. IniFile bietet
  auch die Möglichkeit, mit Platzhaltern in Variablenwerten zu operieren.
  Weiterhin ist ein Include-Mechanismus für geschachtelte
  Konfigurationsdateien realisiert.

  INI-Dateien enthalten je Zeile ein Wertepaar

  <tt>key = value</tt>

  Leerzeichen am Anfang, vor und hinter dem Gleichheitszeichen sowie am
  Zeilenende werden ignoriert. Der Werte-Teil kann in einfache oder
  doppelte Hochkommata eingeschlossen werden, wenn er Leerzeichen am
  Anfang oder am Ende enthalten soll. (Diese Hochkommata werden entfernt,
  bevor sie an ein Anwendungsprogramm weitergegeben werden.)

  Fehlt die Angabe von \c value, wird dem Schlüssel der leere String als
  Wert zugewiesen. In diesem Fall kann auch das Gleichheitszeichen entfallen.

  Zusätzlich können Abschnitte verwendet werden:

  <tt>[section]</tt>

  Gleichnamige Schlüssel in verschiedenen Abschnitten sind unabhängig
  voneinander. Eine Abschnittsangabe gilt bis zur nächsten
  Abschnittsspezifikation. In den (unbenannten) Standardabschnitt kann in
  der Ini-Datei durch Angabe des leeren Abschnittsnamens \c [] (oder \c [ ])
  zurückgewechselt werden. Auf Schlüssel in benannten Abschnitten wird
  Bezug genommen (s.u.), indem der Abschnittsname in eckigen Klammern an den
  Anfang des Schlüsselnamens gestellt wird. Schlüssel im unbenannten
  Abschnitt haben kein solches Präfix, dürfen jedoch auch mit einem Präfix
  \c [] (oder \c [ ]) angegeben werden.

  Schlüssel und Abschnittsnamen dürfen keine Leerzeichen, Gleichheitszeichen,
  Doppelpunkte sowie eckige Klammern enthalten.

  Eine Konfigurationsdatei kann mit Hilfe eines Pragmas (siehe unten) festlegen,
  dass ein Abschnitt (üblicherweise der unbenannte) Standardwerte enthält,
  die dann verwendet werden, wenn ein abgefragter Wert nicht gefunden wird.
  Dieses Verhalten kann unabhängig vom Inhalt der Konfigurationsdatei auch
  zur Laufzeit durch einen Funktionsaufruf geändert werden.

  Konfigurationsinformationen werden über ihren Schlüssel angesprochen.
  Konfigurationswerte können Platzhalter enthalten, die erst bei der
  Benutzung aufgelöst werden. Dadurch ist es möglich, zentrale
  Informationen (wie beispielsweise ein projektspezifisches
  Wurzelverzeichnis) nur an einer Stelle zu spezifizieren und an anderen
  Stellen der INI-Dateien symbolisch darauf Bezug zu nehmen, ohne dass die
  Anwendung davon etwas merkt. Platzhalter werden ähnlich wie in
  W2indows-Batch-Dateien durch Prozentzeichen geklammert:

  <tt>
  dir = test/

  logfile = \%dir\%error.log
  </tt>

  Der Anwendung wird durch <tt>ini_get("logfile",...)</tt> der Wert
  <tt>test/error.log</tt> geliefert, sofern der Wert des Schlüssels dir
  nicht in der Zwischenzeit geändert wurde. Der Platzhalter kann am Anfang
  einen Abschnittsnamen in eckigen Klammern enthalten. Ohne Angabe
  eines Abschnitts wird stets der unbenannte Abschnitt verwendet. Wird
  ein leerer oder nur aus Leerzeichen bestehender) Abschnittsname angegeben,
  wird ebenfalls der unbenannte Abschnitt verwendet. Ansonsten sollte der
  Platzhalter keine Leerzeichen enthalten. Die Referenzen werden dynamisch
  rekursiv aufgelöst. Unendliche Rekursion wird durch eine Heuristik verhindert.

  Konfigurationswerte können auch durch das Programm zur Laufzeit
  hinzugefügt, geändert oder gelöscht werden. Die verfügbaren Schlüssel
  und Abschnitte können aufgelistet werden. Konfigurationsdateien können
  auch geschrieben werden.

  Schlüssel- und Abschnittsnamen sind unabhängig von Groß- und
  Kleinschreibung.

  Kommentarzeilen, die mit <tt>#</tt>, \c * oder \c ; anfangen, werden
  ignoriert. Kommentare am Zeilenende werden darüber hinaus nicht unterstützt.

  Weiterhin können einige fest vorgegebene Pragmata in Konfigurationsdateien
  stehen. Sie bestehen aus einem Schlüsselwort, gefolgt von einem Doppelpunkt
  (kein  Gleichheitszeichen!) und dahinter optional einem Wert. Bei
  Pragmata findet die oben dargestellte Platzhalterersetzung nicht statt.
  Zurzeit sind drei Pragmata realisiert:

    -# \c include: gibt den Namen einer weiteren Konfigurationsdatei an (absolut
       oder aber relativ zum Pfad der Anwendung), deren Inhalt an der Stelle
       eingefügt wird, an der die Anweisung auftritt. Geschachtelte Inklusionen
       sind zulässig. Auch wenn eine Inklusionsanweisung innerhalb eines benannten
       Abschnitts auftritt, beginnt die inkludierte Datei im unbenannten Abschnitt.
       Umgekehrt wird beim Erreichen des Dateiendes und bei der Rückkehr in die
       inkludierende Datei der vorher dort geltende Abschnitt wieder aufgenommen.
       Nach dem Einlesen ist nicht mehr rekonstruierbar, welche Informationen
       welcher Datei entstammen.
    -# \c fallback: legt den Namen eines Abschnitts fest, in dem Werte gesucht
       werden, die im explizit oder implizit genannten Abschnitt nicht gefunden
       werden. Wenn der unbenannte Abschnitt als Fallback verwendet werden soll,
       muss dies durch einen leeren Namen (zwei direkt aufeinander folgende
       Anführungszeichen) angegeben werden. Fehlt der Name, wird der
       Fallback-Mechansmus abgeschaltet. Die Einstellung gilt global, kann also
       nicht für einzelne Passagen an- und für andere abgeschaltet werden. Bei
       mehrfachen Angaben gilt die zuletzt gelesene.
    -# \c resolve_escapes: Wenn dieses Pragma ohne Wert oder mit einem ganzzahligen
       Wert ungleich 0 angegeben wird, dann werden Escape-Sequenzen im C-Stil (also
       \\a, \\b, \\f, \\n, \\r, \\t, \\v, \\', \\", \\?, \\\\ sowie \\ gefolgt von
       bis zu drei Oktalziffern und \\x gefolgt von bis zu zwei Hexadezimalziffern)
       in den entsprechenden Byte-Wert umgesetzt. (Zur Angabe eines literalen \\
       sollte dieser anschließend jeweils verdoppelt angegeben werden.) Die Angabe
       eines NULL-Bytes (\\0 oder \\x0) ist nicht ratsam, da C-Programme mit
       Zeichenketten, die diesen Wert enthalten, nicht gut umgehen können. Diese
       Umsetzung findet nur innerhalb von Werten, nicht innerhalb von Schlüsseln
       und nicht innerhalb von Pragmata, statt, und zwar bereits beim Lesen der
       Datei. Die Einstellung gilt von der nächsten Zeile an, nicht jedoch in
       inkludierten Dateien. Tritt die Anweisung in einer inkludierten Datei auf,
       gilt sie nur bis zum Ende dieser Datei. Wird als Wert des Pragmas 0
       angegeben, wird die Ersetzung ab der folgenden Zeile beendet.

  Zusätzlich sind in dem Library-Modul Hilfsroutinen enthalten, die
  plattformunabhängige Möglichkeiten zum Aufbau von Dateipfaden unterstützen.

  Schließlich sind einige Makros in der öffentlichen Schnittstelle enthalten,
  die Low-Level-Funktionen zur Speicherverwaltung usw. ähnlich wie \c stdlib.h
  bieten (<tt>INI_MALLOC</tt>, <tt>INI_REALLOC</tt>, <tt>INI_STRALLOC</tt>,
  <tt>INI_STRREALLOC</tt>, <tt>INI_FREE</tt>, <tt>INI_COPYSTR</tt>).

  Alle Funktionen sind mit dem Präfix \c ini_ versehen, um die
  Namensraumverschmutzung zu reduzieren. Alle Makros tragen das Präfix
  <tt>INI_</tt>.

  \remarks
  Im Normalfall wird bei der Kompilierung eine linkbare Objektdatei erzeugt.
  Alternativ kann bei der Kompilierung das Präprozessor-Symbol \c INI_TEST definiert
  werden. Dann wird eine einfache <tt>main( )</tt>-Funktion mitkompiliert, die beim
  Linken ein ausführbares Testprogramm ergibt. Diesem Testprogramm können
  beim Aufrufen die Namen einer oder mehrerer INI-Dateien (zum Beispiel
  eine der beigelegten Test-INI-Dateien) mitgegeben werden. Die enthaltenen
  Projektdefinitionsdateien für Microsoft Visual Studio definieren dieses
  Symbol in der Debug-Konfiguration (jedoch nicht in der Release-Konfiguration).

  \note
  Die Implementierung benutzt sortierte verkettete Listen von Wertepaaren. Bei
  typischen Initialisierungsdateigrößen hat dies den Vorteil, schlanken Code
  mit geringem Overhead zu ermöglichen. Bei sehr großen Mengen von Einträgen
  kann die Laufzeit sich jedoch deutlich verschlechtern. Für diesen Fall wäre
  eine Implementierung mit Hashes vorzuziehen. Eine solche Umstellung ist
  jedoch nicht geplant. Gleichwohl sollten Anwendungen die innere Datenstruktur
  als opak betrachten.

  \note
  Eine Implementierung als Klasse könnte je nach Anwendungsfall sinnvoll sein.
  Eine solche Umstellung ist jedoch ebenfalls nicht geplant.

  \attention
  Es gibt nur eine Instanz des Konfigurationshandlers pro Prozess. Es ist
  derzeit nicht möglich, mehrere Konfigurationsdateien gleichzeitig unabhängig
  voneinander zu verwalten.

  \attention
  Die Funktionen sind außerdem nicht threadsafe.

  \todo Heuristik zur Verhinderung unendlicher Rekursionen ersetzen durch
  explizite Prüfung darauf, ob bei der Expansion einer Referenz dieselbe
  Referenz erneut auftritt. Möglicherweise diesen Mechanismus ebenso wie den
  zur Verhinderung unendlicher Inklusions-Rekursion auf Stack-(oder Hash-?)Technik
  umstellen. (Niedrige Priorität)

  \todo Bereitstellen als DLL?

  ****************************************************************************

  \$Id: IniFile.c 430 2021-07-09 23:56:33Z gisbert $

  \history

       Datum      Kürzel Version Beschreibung
   <hr>
   \li 2021-07-20 gws    1.20    Segmentnamen aus lauter Leerzeichen sind jetzt
                                 äquivalent zum unbenannten Segment. Ersetzung von
                                 Platzhaltern mit leerem Wert repariert. Funktionen
                                 ini_comments( ), ini_ncomments( ),
                                 ini_add_comment( ) und iniget_license() hinzugefügt.
                                 Dokumentation ergänzt. Testfälle systematisiert.
   \li 2021-03-12 gws    1.12    Fehlerhafte Behandlung bei fehlender Schlüsselangabe
                                 korrigiert. Probleme mit isspace( ), toupper( ) usw.
                                 bei 8-Bit-Zeichen umgangen.
   \li 2021-02-01 gws    1.11    Fehlercode korrigiert, wenn Ini-Datei nicht gefunden
                                 Secure-Funktionen/-Makros eingeführt (aber noch nicht
                                 konsequent eingesetzt, nur ganz am Ende).
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

#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_DEPRECATE
/* Um Microsoft Visual Studio 2005 gluecklich zu machen */
#define _CRT_SECURE_NO_DEPRECATE
#endif
#define WIN32_LEAN_AND_MEAN
#endif

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
/* #include <stdlib.h> */
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#include <winbase.h> /* Für SecureZeroMemory */
#endif
#include "IniFile.h"

/* Wenn das Makro INI_TEST definiert ist, wird ein Executable erzeugt, sonst ein Objektmodul. */
/* #define INI_TEST   oder eben nicht. Sollte per Compiler-Switch gesetzt werden.             */


#define VERSION       ( 0x0114 )  /**< Versionsnummer 2 Bytes binär (Major Version: oberes Byte, Minor Version: unteres Byte). */
#define VERSIONSTRING ( "1.20" )  /**< Version als String (nicht unbedingt rein numerisch). */
#define COPYRIGHT     ( "IniFile is an open source library for managing configuration files.\n" \
                        "It may be used under the Artistic License 2.0\n"                       \
                        "(https://opensource.org/licenses/artistic-license-2.0).\n"             \
                        "Copyright (C) 1998-2021 TapirSoft Gisbert und Harald Selke GbR\n" ) /**< Copyright-Hinweis. */

typedef unsigned char uchar;

/** Hilfs-Makro zum Überspringen von Referenzen auf das unbenannte Segment. */
#define JUMP_EMPTY_SECTION(key) {                                                                                \
                                  /* Eintrag grafieunabhängig suchen in der Liste: */                            \
                                  if ( (key) && ( *(key) == '[' ) ) {                                            \
                                    /* Prüfen, ob das der leere Abschnitt                                        \
                                     * (bzw. Abschnittsname aus lauter Blanks) ist; wenn ja:                     \
                                     * Diese Abschnittsreferenz amputieren.                                      \
                                    */                                                                           \
                                    const char *temp_key;                                                        \
                                    for( temp_key=(key)+1; *temp_key && INI_ISSPACE(*temp_key); temp_key++ ) ;   \
                                    if ( *temp_key == ']' ) (key) = temp_key + 1;                                \
                                  }                                                                              \
                                }

/** Größe des Speichers zum Lesen von Zeilen. */
#define MINIBUFSIZE   (   1024 )

/** Größe des I/O-Buffers. */
#define BUFSIZE       (  65500 )


static TIniEntry *inilist         = NULL;  /**< Anfang der verketteten Liste von Ini-Einträgen.                  */
static TIniEntry *comments        = NULL;  /**< Anfang der verketteten Liste von Ini-Kommentar-Einträgen.        */
static TIniEntry *ininames        = NULL;  /**< Sammlung aktiver Include-Namen, um Rekursion zu vermeiden        */
static char      *logname         = NULL;  /**< Sonderfall: Name der Standard-Protokolldatei.                    */
static char      *errname         = NULL;  /**< Sonderfall: Name der Standard-Fehlerprotokolldatei.              */
static char      *fallbacksection = NULL;  /**< Fallback-Abschnitt für im angegebenen Abschnitt nicht gefundene Einträge. */
static char      *tmpstr          = NULL;  /**< Platz für temporär erzeugte Strings für Rückgabe aus Funktionen. */
static int        recursion       = -1;    /**< Merker für Rekursion bei der Platzhalter-Ersetzung.              */
static int        entrycount      = 0;     /**< Anzahl der Konfigurationseinträge (stets aktuell).               */
static int        sectioncount    = 0;     /**< Anzahl der Abschnitte (gecached).                                */
static int        commentcount    = 0;     /**< Anzahl der Kommentarzeilen (stets aktuell).                      */
static int        errorlinenumber = -1;    /**< Nummer der Konfigurationsdateizeile, bei der ein Fehler auftrat. */
static char      *errorininame    = NULL;  /**< Name der Konfigurationsdatei, in der ein Fehler auftrat (oder NULL). */
static int        got_license     = 0;     /**< Flag, ob Lizenz-/Copryight-Text schon abgefragt wurden.          */


/** Sichere (safe) Allokation von Speicherplatz.
 *
 *  Der Speicherplatz (sofern genügend verfügbar) wird mit 0 initialisiert.
 *
 *  \param[in] size  Angeforderte Speichergröße in Bytes.
 *
 *  \return          Pointer auf den Speicherplatz. NULL, falls nicht genügend Speicher vorhanden.
 */
void __inline *ini_safemalloc( const size_t size ) {
  char *memp = (char *)malloc(size);
  if (memp) inis_wipe_memory( memp, size );

  return (void *)(memp); /* möglicherweise NULL */
}


/** Sichere (safe) Re-Allokation von Speicherplatz.
 *
 *  Falls die Größe 0 und der Pointer NULL ist, ist dies ein NOOP.
 *  Falls die Größe 0 ist, ist diese Funktion identisch mit free( ).
 *  Falls der angegebene Pointer NULL ist, ist diese
 *  Funktion identisch mit ini_safemalloc( ).
 *  Falls die neue Speichergröße größer als die alte ist, wird der neue Teil
 *  nicht initialisiert.
 *  Falls der neue Speicherbereich kleiner als der alte ist, wird der alte
 *  Restspeicher ebenfalls nicht ausgenullt.
 *
 *  \param[in,out] memp  Pointer auf den zu verändernden Speicher. Kann NULL sein.
 *  \param[in] size      Angeforderte Speichergröße in Bytes.
 *
 *  \return              Pointer auf den Speicherplatz. NULL, falls nicht genügend Speicher vorhanden.
 */
void __inline *ini_saferealloc( void *memp, const size_t size ) {
  if ( size == 0 ) {
    if (memp) {
      free(memp);
      memp = NULL;
    }
  } else {
    memp = (memp) ? realloc(memp, size) : INI_MALLOC(size);
  }

  return (void *)(memp); /* möglicherweise NULL */
}


/** Sichere (safe&secure) Neu-Allokation von Speicherplatz.
 *
 *  Falls die neue Größe 0 und der Pointer NULL ist, ist dies ein NOOP.
 *  Falls die neue Größe 0 ist, ist diese Funktion identisch mit INIS_FREE( ).
 *  Falls der angegebene Pointer NULL ist, ist diese
 *  Funktion identisch mit ini_safemalloc( ).
 *  Falls die neue Speichergröße größer als die alte ist, wird der neue Teil
 *  nicht initialisiert.
 *  Falls der neue Speicherbereich kleiner als der alte ist, wird der alte
 *  Restspeicher ebenfalls nicht ausgenullt.
 *
 *  \param[in,out] memp  Pointer auf den zu verändernden Speicher. Kann NULL sein.
 *  \param[in] newsize   Angeforderte Speichergröße in Bytes.
 *  \param[in] oldsize   Bisherige Speichergröße in Bytes.
 *
 *  \return              Pointer auf den Speicherplatz. NULL, falls nicht genügend Speicher vorhanden.
 */
void __inline *inis_saferealloc( void *memp, const size_t newsize, const size_t oldsize ) {
  char *newmemp = NULL;
  if ( newsize == 0 ) {
    INIS_FREE( memp, oldsize );
  } else if ( memp ) {
    /* Speicher neu allozieren, neuen Bereich initialisieren, alten löschen.
     * Das ist u.U. deutlich aufwändiger als ein (non-secure) realloc.
     */
    newmemp = (char *)malloc(newsize);
    if (newmemp) {
      if ( newsize >= oldsize ) {
        memcpy( newmemp, memp, oldsize );
        if ( newsize > oldsize ) inis_wipe_memory( newmemp+oldsize, newsize-oldsize );
      } else {
        memcpy( newmemp, memp, newsize );
      }
      INIS_FREE( memp, oldsize );
    }
  } else {
    /* Speicher-Neuallokation */
    newmemp = INI_MALLOC(newsize);
  }
  return (void *)(newmemp); /* möglicherweise NULL */
}


/** Sicheres (safe und secure) Löschen von Speicherbereichen für sicherheitsrelevante Anwendungen.
 *
 *  Der angegebene Speicherbereich wird so sicher (secure) wie möglich gelöscht.
 *  Stark angelehnt an die ARGON2-Source.
 *
 *  \param[in] memp      Pointer auf den zu löschenden Speicher. Kann NULL sein.
 *  \param[in] size      Speichergröße in Bytes.
 */
void __inline inis_wipe_memory( void *memp, const size_t size ) {
  if ( memp && size ) {
#if defined(_MSC_VER) && ( _MSC_VER >= 1400 ) || defined(__MINGW32__)
    SecureZeroMemory( memp, size );
#elif defined memset_s
    memset_s( memp, size, 0, size );
#elif defined(__OpenBSD__) || ( defined(__GLIBC__) && defined(__GLIBC_PREREQ) && __GLIBC_PREREQ(2,25) )
    explicit_bzero( memp, size );
#else
    static void *( *const volatile memset_sec )( void *, int, size_t ) = &memset;
    memset_sec( memp, 0, size );
#endif
  }
}


/** Isoliert den relevanten Inhalt eines Strings (interne Funktion).
 *
 *  Führende Blanks werden übersprungen, Blanks am Ende werden ausgeblendet.
 *  Wenn der String in einfache oder doppelte Hochkommata eingeschlossen ist,
 *  werden diese ebenfalls ausgeblendet.
 *
 *  \param[in] str Pointer auf den String. Der String kann durch die Funktion destruktiv verändert werden.
 *  \param[in] resolve_escapes Boolescher Wert, der angibt, ob Escape-Sequenzen im C-Stil aufgelöst werden sollen.
 *
 *  \return        Pointer auf den isolierten String.
 */
char *isolate_string( char *str, int resolve_escapes ) {
  char *temp, *temp1;
  unsigned char n;
  size_t k;

  if (str) {
    /* Ggf. Whitespace am Stringanfang überspringen: */
    while ( *str && INI_ISSPACE(*str) ) str++;
    /* Ggf. Whitespace am Stringende entfernen: */
    if ( strlen(str) ) {
      for ( temp=str+strlen(str)-1; INI_ISSPACE(*temp); temp-- ) *temp = '\0';
      /* ggf. Anführungszeichen um Wert entfernen: */
      if ( ( ( k=strlen(str) ) > 1 ) && ( ( *str == '"' ) || ( *str == '\'' ) ) && ( *(str+k-1) == *str ) ) {
        *(str+k-1) = '\0';
        str++;
      }
    }
  }

  if (resolve_escapes) {
    temp1 = str;
    while ( (temp = strchr( temp1, '\\') ) && ( strlen(temp) > 1) ) {
      k = 1;
      switch ( *(temp+1) ) {
        case 'a' : *temp =  7;   break;
        case 'b' : *temp =  8;   break;
        case 't' : *temp =  9;   break;
        case 'n' : *temp = 10;   break;
        case 'v' : *temp = 11;   break;
        case 'f' : *temp = 12;   break;
        case 'r' : *temp = 13;   break;
        case '\\': *temp = '\\'; break;
        case '?' : *temp = '?';  break;
        case '\'': *temp = '\''; break;
        case '"' : *temp = '"';  break;
        case '0' : case '1' : case '2' : case '3' :
        case '4' : case '5' : case '6' : case '7' :
                   /* Oktalzahl mit maximal 3 Stellen */
                   n = 0;
                   temp1 = temp + 1;
                   while ( ( k <= 3 ) && strlen(temp1) && ( *temp1 >= '0' ) && ( *temp1 <= '7' ) ) {
                     n = 8*n + ( *temp1 - '0' );
                     k++;
                     temp1++;
                   }
                   k--;
                   memcpy( temp, &n, 1 );
                   break;
        case 'x' : /* Hexadezimalzahl mit maximal 2 Stellen */
                   n = 0;
                   temp1 = temp + 2;
                   while ( (k <= 2) && strlen(temp1) && INI_ISXDIGIT(*temp1) ) {
                     n = 16*n + ( INI_ISDIGIT(*temp1) ? ( *temp1 - '0' ) : ( INI_CHAR_TOUPPER(*temp1) - 'A' + 10 ) );
                     k++;
                     temp1++;
                   }
                   if ( k > 1 ) {
                     memcpy( temp, &n, 1 );
                   } else {
                     /* Entartungsfall: kein Hex-Zeichen hinter \x gefunden.
                      * Dann müssen wir die Pointer so umsetzen, dass anschließend nur \x überschrieben wird,
                      * ohne dass etwas dafür eingefügt würde.
                      */
                     temp--;
                     k++;
                   }
                   break;
        default :  k = 0;
      }
      if (k) memmove( temp+1, temp+k+1, strlen( temp + 2 ) + 1 );
      temp1 = temp + 1;
    }
  }

  return str;
}


/** Erzeugt aus einem Namen einen sauberen Abschnittsnamen (interne Funktion).
 *
 *  Wenn der Eingabeparameter leer oder NULL oder "[]" oder "[ ]" ist, wird ein leerer String
 *  zurückgeliefert. Sonst wird ein String zurückgeliefert, der garantiert in eckigen
 *  Klammern steht und nur Großbuchstaben enthält.
 *
 *  \param[in] str Ein Pointer auf einen Kandidat für einen Abschnittsnamen, aber
 *                 möglicherweise unsauber. Die Eingabe bleibt garantiert unverändert.
 *
 *  \return Der saubere Abschnittsname in einem frisch allozierten Speicherbereich.
 *          NULL, falls nicht genug Speicherplatz vorhanden.
 *
 */
char *make_section_name( const char *str ) {
  char *sectionname;
  const char *temp;
  size_t len;

  /* Zunächst führende Blanks und eckige Klammern überspringen: */
  while ( str && *str && ( INI_ISSPACE(*str) || ( *str == '[' ) ) ) str++;

  /* Dann Blanks und eckige Klammern am Ende überspringen: */
  if ( str && *str && ( *str != ']' ) ) {
    temp = str + strlen(str) - 1;
    while ( (temp > str) && ( INI_ISSPACE(*temp) || ( *temp == ']' ) ) ) temp--;
    /* Der Name steht jetzt von str bis temp einschließlich, und zwar ohne eckige Klammern. */
    /* Wir wissen auch, dass der String nicht leer ist, also temp >= str. */
    len = (size_t)( temp - str + 1 );
    sectionname = INI_STRALLOC( len+2 );
    if ( !sectionname ) return NULL;
    *sectionname = '[';
    memcpy( sectionname+1, str, len );
    *( sectionname+len+1 ) = ']';
    *( sectionname+len+2 ) = '\0';
    INI_STRUPPER( sectionname+1 );
  } else {
    /* Sonderfall: leere Eingabe bzw. leerer Abschnittsname. */
    sectionname = INI_STRALLOC(0);
    if (sectionname) *sectionname = '\0';
  }

  return sectionname;
}

/** Verarbeitung einer Pragma-Anweisung in einer Ini-Datei (interne Funktion).
 *
 *  Verarbeitet einen Pragma-Eintrag in einer Ini-Datei und ruft die dafür notwendigen
 *  Funktionen auf.
 *
 *  \param[in]  pragma    Pointer auf den Namen des Pragmas.
 *                        Der String kann durch die Funktion destruktiv verändert werden.
 *  \param[in]  value     Pointer auf den Wert. Kann leer sein. Hochkomma- und Escape-Handling
 *                        müssen bereits erledigt sein.
 *                        Der String kann durch die Funktion destruktiv verändert werden.
 *  \param[out] npragma   0, falls INCLUDE gefunden wurde; 1 für FALLBACK; 2 für RESOLVE_ESCAPES;
 *                        -1 bei unbekanntem Pragma. (Nur, falls kein NULL übergeben wurde.)
 *  \param[out] pragmaval Bei RESOLVE_ESCAPES der Boolesche Wert der Setzung. Sonst 0.
 *
 *  \return           0, falls OK; sonst einer der von ini_read( ) zurückgegebenen Werte.
 *                    INI_ERR_NOMEMORY,     falls nicht genügend Speicherplatz verfügbar.
 *                    INI_ERR_NOSUCHPRAGMA, falls die Konfigurationsdatei ein unbekanntes Pragma enthält.
 */
int handle_pragma( char *pragma, char *value, int *npragma, int *pragmaval ) {
  int n;

  /* Pragma-Namen auf Großschreibung umsetzen: */
  INI_STRUPPER(pragma);
  if (pragmaval) *pragmaval = 0;

  if ( INI_STREQUAL( pragma, "INCLUDE" ) ) {
    /* Include-Datei, also ini_read( ) aufrufen. (Das ist rekursiv!) */
    if (npragma) *npragma = 0;
    n = ini_read(value);
  } else if ( INI_STREQUAL( pragma, "FALLBACK" ) ) {
    /* Fallback-Abschnitt setzen: */
    if (npragma) *npragma = 1;
    ini_set_fallbacksection(value);
    n = 0;
  } else if ( INI_STREQUAL( pragma, "RESOLVE_ESCAPES" ) ) {
    /* Ab hier beim Einlesen Escape-Sequenzen im C-Stil umsetzen; bzw.Umsetzung beenden: */
    if (npragma) *npragma = 2;
    if (pragmaval) *pragmaval = ( value && strlen(value) ) ? atol(value) : 1;
    n = 0;
  } else {
    /* Unbekanntes Pragma: */
    if (npragma) *npragma = -1;
    n = INI_ERR_NOSUCHPRAGMA;
  }

  return n;
}


/** Hilfs-Makro für die Fehlerbehandlung in ini_read( ). */
#define ERROR_RETURN( ininame, lineno, retval ) { if (ininame) {                                                       \
                                          errorininame = INI_STRREALLOC( errorininame, INI_STRLEN(ininame) );          \
                                          if (errorininame) INI_COPYSTR( errorininame, ininame, INI_STRLEN(ininame) ); \
                                        } else {                                                                       \
                                          errorininame = INI_STRREALLOC( errorininame, 0 );                            \
                                          *errorininame = '\0';                                                        \
                                        }                                                                              \
                                        INI_FREE(inbuffer);                                                            \
                                        INI_FREE(sectionname);                                                         \
                                        if (ininameentry) {                                                            \
                                          ininames = ininameentry->next;                                               \
                                          INIS_FREE( ininameentry, sizeof( *ininameentry ) );                          \
                                        }                                                                              \
                                        if ( inifile ) {                                                               \
                                          fclose(inifile);                                                             \
                                          inifile = NULL;                                                              \
                                        }                                                                              \
                                        errorlinenumber = lineno;                                                      \
                                        return retval;                                                                 \
                                      }

/** Hilfs-Makro für die Erzeugung der speziellen .log- ind .err-Dateinamen. */
#define MAKE_SPECIAL_FILE_NAME( fn, temp_ptr, ext ) {                                             \
                                       if ( !(fn) ) {                                             \
                                         (fn) = INI_STRALLOC( strlen(temp_ptr) + strlen(ext) );   \
                                         if ( !(fn) ) {                                           \
                                           INI_FREE(temp_ptr);                                    \
                                           ERROR_RETURN( inifilename, lineno, INI_ERR_NOMEMORY )  \
                                         }                                                        \
                                         (void)strcpy( (fn), (temp_ptr) );                        \
                                         (void)strcat( (fn), (ext) );                             \
                                       }                                                          \
                                     }
/** Einlesen einer Konfigurationsdatei.
 *
 *  Die angegebene Konfigurationsdatei wird gelesen, ihre Einträge werden
 *  intern in einer sortierten verketteten Liste gespeichert. Konfligierende
 *  Einträge aus ggf. vorher eingelesenen Konfigurationsdateien werden
 *  überschrieben.
 *  Wenn die angegebene Datei nicht zum Lesen geöffnet werden kann, ist dies
 *  kein Fehler.
 *
 *  \param[in] inifilename Name der einzulesenden Konfigurationsdatei.
 *
 *  \return Anzahl der nach dem Einlesen verfügbaren Einträge.
 *          INI_ERR_NOMEMORY,     falls nicht genügend Speicherplatz verfügbar.
 *          INI_ERR_NOSUCHPRAGMA, falls die Konfigurationsdatei ein unbekanntes Pragma enthält.
 *
 */
int ini_read( const char * const inifilename ) {

  FILE *inifile = NULL;
  TIniEntry *ininameentry = NULL;
  char minibuf[MINIBUFSIZE];
  char *sectionname = NULL;
  char *inbuffer = NULL;
  char *temp1, *temp2, *temp3;
  size_t sectionnamemaxlen, i;
  int npragma, pragmaval, n, lineno = 0, resolve_escapes = 0;

  errorlinenumber = -1;

  if ( !INI_STRLEN(inifilename) ) return ini_nkeys( );

  /* Wir wollen keine unendlichen Include-Schleifen und müssen uns daher merken, wo wir schon waren.
   * Mehrere Verarbeitungen derselben Ini-Datei sind zulässig, aber rekursive Aufrufe nicht.
   * Wir speichern die gerade aktiven Namen in der Linked List ininames, wobei der neueste am Anfang steht.
   */

  ininameentry = ininames;
  while ( ininameentry ) {
    /* Checken, ob unser aktueller Dateiname uns bekannt vorkommt. */
    if ( INI_STREQUAL( ininameentry->value, inifilename ) ) return ini_nkeys( );
      /* Hier waren wir schon einmal, daher hier nicht mehr erneut rekurrieren. */
    ininameentry = ininameentry->next;
  }

  /* Wir sind beruhigt: bisher keine Rekursion. Aber unseren aktuellen Namen merken: */
  ininameentry = (TIniEntry *)INI_MALLOC( sizeof( *ininameentry ) );
  if ( !ininameentry ) return INI_ERR_NOMEMORY;
  ininameentry->key   = NULL;
  ininameentry->value = (char *)inifilename;
  ininameentry->next  = ininames;
  ininames            = ininameentry;

  /* Spezialfälle vorbereiten: Name der Standard-Protokolldatei
   * (wie Ini-Datei, aber mit Extension .log) und der Standard-Fehlerprotokolldatei
   * (wie Ini-Datei, aber mit Extension .err).
   */
  if ( !logname || !errname ) {
    /* Basisnamen bestimmen: */
    temp1 = INI_STRALLOC( strlen(inifilename) );
    if (!temp1) ERROR_RETURN( inifilename, lineno, INI_ERR_NOMEMORY );
    (void)strcpy( temp1, inifilename );
    temp2 = strrchr( temp1, '.' );
    if (temp2) *temp2 = '\0';

    MAKE_SPECIAL_FILE_NAME( logname, temp1, ".log" ); /* Protokolldateiname       */
    MAKE_SPECIAL_FILE_NAME( errname, temp1, ".err" ); /* Fehlerprotokolldateiname */

    INI_FREE(temp1);
  }

  /* Ini-Datei öffnen, I/O-Buffer setzen, falls möglich: */
  inifile = fopen( inifilename, "r" );
  if ( !inifile ) {
    /* Kein Fehler, einfach diese Datei ignorieren. Aber auch aus unserem Ini-Call-Stack entfernen. */
    ininameentry = ininames;
    ininames = ininames->next;
    INIS_FREE( ininameentry, sizeof( *ininameentry ) );
    return ini_nkeys( );
  }
  inbuffer = (char *)INI_MALLOC(BUFSIZE);
  if (inbuffer) setvbuf( inifile, inbuffer, _IOFBF, BUFSIZE );

  /* Abschnittsnamen initialisieren: */
  sectionnamemaxlen = 0;
  sectionname = INI_STRALLOC(sectionnamemaxlen);
  if ( !sectionname ) ERROR_RETURN( inifilename, lineno, INI_ERR_NOMEMORY );
  *sectionname = '\0';

  /* Alle Zeilen durchgehen: */
  while ( fgets( minibuf, MINIBUFSIZE, inifile ) ) {
    lineno++;
    for ( temp1=minibuf; INI_ISSPACE(*temp1) && *temp1; temp1++ ) ; /* Whitespace am Zeilenanfang überspringen. */
    if (!*temp1) continue;
    for ( temp2=temp1+strlen(temp1)-1; INI_ISSPACE(*temp2); temp2-- ) *temp2 = '\0'; /* Whitespace am Zeilenende ignorieren. */

    if ( strchr( "#*;", *temp1 ) ) {
      /* Kommentar in einer gesonderten Liste speichern. */
      n = ini_add_comment(temp1+1);
      if ( n == INI_ERR_NOMEMORY ) ERROR_RETURN( inifilename, lineno, INI_ERR_NOMEMORY );
      continue;
    }

    if ( ( *temp1 == '[' ) && ( *temp2 == ']' ) ) {
      /* Abschnittsname gefunden. */
      for ( temp3=temp1+1; INI_ISSPACE(*temp3) && ( temp3 < temp2 ); temp3++ ) ;
      if ( temp3 == temp2 ) {
        /* Sonderfall: leerer Abschnitt (oder Abschnittsname nur aus Blanks) => unbenannter Abschnitt */
        *sectionname = '\0';
      } else {
        /* Normalfall: ein echter Abschnittsname */
        i = (size_t)( temp2 - temp1 + 1 );
        if ( i > sectionnamemaxlen ) {
          sectionnamemaxlen = i;
          sectionname = INI_STRREALLOC( sectionname, sectionnamemaxlen );
          if ( !sectionname ) ERROR_RETURN( inifilename, lineno, INI_ERR_NOMEMORY );
        }
        INI_COPYSTR( sectionname, temp1, i );
      }
    } else {
      temp2 = strchr( temp1, '=' ); /* Prüfung auf normalen Ini-Eintrag */
      temp3 = strchr( temp1, ':' ); /* Prüfung auf Pragma               */
      if ( temp3 && ( !temp2 || ( temp3 < temp2 ) ) ) {
        /* Pragma gefunden */
        *( temp3++ ) = '\0';
        n = handle_pragma( temp1, isolate_string( temp3, 0 ), &npragma, &pragmaval );
        /* In Pragmata keine Escape-Ersetzung! */
        if ( n < 0 ) {
          /* Kann nur NOSUCHPRAGMA oder NOMEMORY sein. In beiden Fällen die weitere Bearbeitung
           * dieser Ini-Datei einstellen und das Problem nach oben melden.
           */
          if ( errorlinenumber < 1 ) {
            /* Markiere, wo der Fehler aufgefallen ist: */
            ERROR_RETURN( inifilename, lineno, n );
          } else {
            /* Wir reichen nur eine bereits eingetragene Meldung weiter nach oben. */
            return n;
          }
        }
        if ( npragma == 2 ) resolve_escapes = pragmaval; /* falls dies das resolve_escapes-Pragma war */
      } else {
        /* Kein Pragma. */
        if ( temp2 != temp1 ) {
          /* kein leerer Schlüssel */
          if ( !temp2 ) { /* kein Gleichheitszeichen, also: leerer Wert */
            temp2 = temp1 + strlen(temp1);
          } else { /* Gleichheitszeichen gefunden; Wert kann immer noch leer sein. */
            /* Den Schlüssel isolieren: */
            for ( temp3=temp2-1; INI_ISSPACE(*temp3) && (temp3>=temp1); temp3-- ) *temp3 = '\0';
            if ( *temp2 == '=' ) *temp2 = '\0';
            temp2++;
          }
          /* temp1 zeigt jetzt auf den Key (möglicherweise leerer String!),
           * temp2 auf den Wert (möglicherweise leerer String!)
           */
          temp2 = isolate_string( temp2, resolve_escapes );
          /* ggf. Anführungszeichen um Wert entfernen: */
          if ( ( strlen(temp2) > 1 ) && ( ( *temp2 == '"' ) || ( *temp2 == '\'' ) ) &&
              ( *( temp2+strlen(temp2)-1 ) == *temp2 ) ) {
            temp2++;
            *(temp2+strlen(temp2)-1) = '\0';
          }
          if ( strlen(sectionname) ) {
            /* Abschnittsname vorhanden: */
            temp3 = INI_STRALLOC( strlen(sectionname)+strlen(temp1) );
            if ( !temp3 ) ERROR_RETURN( inifilename, lineno, INI_ERR_NOMEMORY );
            INI_COPYSTR( temp3, sectionname, strlen(sectionname) );
            (void)strcat( temp3, temp1 );
            if ( !ini_add( temp3, temp2 ) ) ERROR_RETURN( inifilename, lineno, INI_ERR_NOMEMORY );
            INI_FREE(temp3);
          } else {
            /* Ein wahrscheinlich häufiger Sonderfall: kein benannter Abschnitt, daher schnelleres Handling: */
            if ( !ini_add( temp1, temp2 ) ) ERROR_RETURN( inifilename, lineno, INI_ERR_NOMEMORY );
          }
        }
      }
    }
  }
  fclose(inifile);
  INI_FREE(inbuffer);
  INI_FREE(sectionname);

  /* Nun unseren Namen aus der ininames-Liste streichen, damit wir nicht fälschlich Rekursion vermelden: */
  ininames = ininameentry->next;
  INIS_FREE( ininameentry, sizeof( *ininameentry ) );

  return ini_nkeys( );
}


/** Fügt einen Konfigurationseintrag (Schlüssel/Wert) hinzu.
 *
 *  \param[in] key   Der Schlüssel (darf auch leer oder NULL sein).
 *                   Der Schlüssel darf am Anfang in eckigen Klammern einen Abschnittsnamen enthalten.
 *  \param[in] value Der Wert (darf leer sein). Falls NULL, wird der entsprechende
 *                   Eintrag gelöscht.
 *
 *  \return Anzahl der nach dem Hinzufügen verfügbaren Einträge.
 *          INI_ERR_NOMEMORY, falls nicht genügend Speicherplatz verfügbar.
 *
 */
int ini_add( const char *key, const char * const value ) {
  TIniEntry *inilistptr = inilist;
  TIniEntry *previousinilistptr = NULL;
  TIniEntry *newinilistentry;
  char *tempkey;

  if ( !got_license ) {
    fprintf( stderr, "This software uses the IniFile library.\n%s", COPYRIGHT );
    got_license = 1;
  }

  if ( !value ) return ini_delete(key); /* NULL-Wert: dann löschen. */

  /* Eintrag grafieunabhängig suchen in der Liste: */
  JUMP_EMPTY_SECTION(key);
  tempkey = INI_STRALLOC(INI_STRLEN(key)); /* NULL-Schlüssel wie leeren Schlüssel behandeln. */
  if ( !tempkey ) return INI_ERR_NOMEMORY;
  INI_COPYSTRUPPER( tempkey, key );

  while ( inilistptr && ( strcmp( tempkey, inilistptr->key ) > 0 ) ) {
    previousinilistptr = inilistptr;
    inilistptr = inilistptr->next;
  }

  if ( inilistptr && INI_STREQUAL( tempkey, inilistptr->key ) ) {
    /* gefunden; also alten Wert überschreiben */
    inilistptr->value = INI_STRREALLOC( inilistptr->value, strlen(value) );
    if ( !inilistptr->value ) {
      INI_FREE(tempkey);
      return INI_ERR_NOMEMORY;
    }
    (void)strcpy( inilistptr->value, value );
    INI_FREE(tempkey);
  } else {
    /* Neuen Eintrag hinzufügen. */
    newinilistentry = (TIniEntry *)INI_MALLOC( sizeof(*newinilistentry) );
    if ( !newinilistentry ) {
      INI_FREE(tempkey);
      return INI_ERR_NOMEMORY;
    }
    newinilistentry->key   = tempkey;
    newinilistentry->value = INI_STRALLOC( strlen(value) );
    if ( !newinilistentry->value ) {
      INI_FREE(tempkey);
      INI_FREE(newinilistentry);
      return INI_ERR_NOMEMORY;
    }
    (void)strcpy( newinilistentry->key, tempkey );
    (void)strcpy( newinilistentry->value, value );
    /* Nachfolger des neuen Eintrags wird der in der Sortierung nächstgrößere Alteintrag. */
    newinilistentry->next = inilistptr;

    if ( !previousinilistptr ) {
      /* Einfügung am Anfang, also Anfangspointer  neu setzen. */
      inilist = newinilistentry;
    } else {
      /* Der in der Sortierung voran gehende Eintrag erhält den neuen als Nachfolger. */
      previousinilistptr->next = newinilistentry;
    }
    entrycount++;
  }

  sectioncount = 0; /* Gecacheten Wert invalidieren */

  return entrycount;
}


/** Fügt einen Konfigurationseintrag (Schlüssel/Wert) hinzu; Synonym für ini_add( ).
 *
 *  \param[in] key   Der Schlüssel (darf auch leer oder NULL sein).
 *                   Der Schlüssel darf am Anfang in eckigen Klammern einen Abschnittsnamen enthalten.
 *  \param[in] value Der Wert (darf leer sein). Falls NULL, wird der entsprechende
 *                   Eintrag gelöscht.
 *
 *  \return Anzahl der nach dem Einlesen verfügbaren Einträge.
 *          INI_ERR_NOMEMORY, falls nicht genügend Speicherplatz verfügbar.
 *
 *  \sa ini_add( )
 *
 */
int __inline ini_set( const char *key, const char * const value ) { return ini_add(key, value); }


/** Liefert die Anzahl der Konfigurationseinträge zurück.
 *
 *  \return Die Anzahl der Konfigurationseinträge.
 *
 */
int __inline ini_nkeys(void) { return entrycount; }


/** Liefert die Anzahl der Konfigurationseinträge in einem gegebenen Abschnitt zurück.
 *
 *  \param[in] sectionname Name des Abschnitts. Kann auch NULL, leer oder [] sein.
 *                         Begrenzende eckige Klammern dürfen fehlen.
 *
 *  \return Die Anzahl der Konfigurationseinträge in dem genannten Abschnitt.
 *
 */
int ini_nkeys_section( const char *sectionname ) {
  TIniEntry *inilistptr;
  char *mysectionname;
  size_t sectionnamelen;
  int keycount = 0;

  mysectionname = make_section_name(sectionname);
  if ( !mysectionname ) return INI_ERR_NOMEMORY;
  sectionnamelen = strlen(mysectionname);

  /* Alle Einträge durchgehen, dabei nur die passenden berücksichtigen. */
  for ( inilistptr=inilist; inilistptr; inilistptr=inilistptr->next ) {
    if ( ( ( sectionnamelen != 0 ) &&
           ( strlen( inilistptr->key ) >= sectionnamelen ) &&
           ( memcmp( inilistptr->key, mysectionname, sectionnamelen ) == 0 )
         ) ||
         ( ( sectionnamelen == 0 ) &&
           ( ( strlen(inilistptr->key) == 0 ) || ( *(inilistptr->key) != '[' ) )
         )
       ) keycount++;
  }
  INI_FREE(mysectionname);

  return keycount;
}


/** Liefert die Anzahl der Konfigurationsabschnitte zurück.
 *
 *  \return Die Anzahl der Abschnitte (inkl. des unbenannten Abschnitts).
 *
 */
int ini_nsections(void) {
  TIniEntry *inilistptr;
  char *sectionname, *temp;
  size_t sectionnamemaxlen, i;
  int foundunbenannt = 0;

  if (sectioncount) return sectioncount; /* Gecacheter Wert liegt vor, alle sind froh. */

  sectionnamemaxlen = 0;
  sectionname = INI_STRALLOC(sectionnamemaxlen);
  if ( !sectionname ) return INI_ERR_NOMEMORY;

  *sectionname = '\0';

  /* Alle Einträge durchgehen, dabei Sektionsnamen zählen.
   * Wir wissen: Die Einträge sind grafieunabhängig sortiert, brauchen also keine
   * Historie der gesehenen Einträge nachzuhalten. */
  for ( inilistptr=inilist; inilistptr; inilistptr=inilistptr->next ) {
    if ( strlen(inilistptr->key) ) {
      if ( *(inilistptr->key) == '[' ) {
        /* Eintrag mit benamstem Abschnitt */
        temp = strchr( inilistptr->key, ']' );
        if (temp) { /* Sicherheitshalber; alles andere wäre sehr merkwürdig. */
          i = (size_t)( temp - inilistptr->key + 1 );
          if ( ( i != strlen(sectionname) ) || ( memcmp( sectionname, inilistptr->key, i ) != 0 ) ) {
            /* Neuer Abschnittsname gefunden! */
            sectioncount++;
            if ( i > sectionnamemaxlen ) {
              sectionnamemaxlen = i;
              sectionname = INI_STRREALLOC( sectionname, sectionnamemaxlen );
              if ( !sectionname ) return INI_ERR_NOMEMORY;
            }
            INI_COPYSTR( sectionname, inilistptr->key, i );
          }
        }
      } else {
        /* Unbenannter Abschnitt. */
        if ( !foundunbenannt ) {
          foundunbenannt = 1;
          sectioncount++;
        }
        *sectionname = '\0';
      }
    }
  }
  INI_FREE(sectionname);

  return sectioncount;
}


/** Liefert eine Liste der Schlüssel.
 *
 *  Liefert eine Liste aller definierten Schlüssel zurück.
 *
 *  \param[out] keylist Pointer auf ein Array von Strings, das die Schlüssel aufnimmt.
 *                      Schlüssel in benannten Abschnitten enthalten am Anfang den Abschnittsnamen
 *                      in eckigen Klammern.
 *                      Wird terminiert durch einen NULL-Pointer, sofern Platz dafür vorhanden ist.
 *                      Platz für die Strings wird dynamisch alloziert.
 *                      Kann NULL sein; dann wird Speicher dynamisch alloziert.
 *  \param[in] maxkeys  Maximal zurückzuliefernde Anzahl von Schlüsseln. Kann <= 0 sein,
 *                      dann keine Begrenzung. keylist muss (falls ungleich NULL)
 *                      jedenfalls groß genug sein, um diese Einträge und den
 *                      terminierenden NULL-Eintrag aufzunehmen.
 *
 *  \return Pointer auf das Array von Strings; oder NULL, falls nicht genug Speicherplatz.
 *
 *  \sa ini_nkeys( )
 *
 *  \bug
 *  Bei Abbruch wegen Speichermangel wird bereits allozierter Speicher nicht wieder freigegeben.
 *
 */
char **ini_keys( char **keylist, int maxkeys ) {

  TIniEntry *inilistptr = inilist;
  char **keyptr;
  int keycount;

  if ( !keylist ) {
    /* Speicherplatz allozieren. */
    keycount = ini_nkeys( ) + 1;
    if ( ( maxkeys > 0 ) && ( keycount > maxkeys ) ) keycount = maxkeys;
    if ( maxkeys <= 0 ) maxkeys = keycount;
    keylist = (char **)INI_MALLOC( ( (unsigned)keycount+1 ) * sizeof(char *) );
    if ( !keylist ) return NULL;
  }

  /* Alle Einträge bis zur erlaubten Maximalzahl eintragen. */
  for ( keycount=0, keyptr=keylist; inilistptr && (keycount < maxkeys); keycount++, inilistptr=inilistptr->next ) {
    *keyptr = INI_STRALLOC( strlen(inilistptr->key) );
    if ( !*keyptr ) return NULL;
    (void)strcpy( *(keyptr++), inilistptr->key );
  }

  if ( keycount < maxkeys ) *keyptr = (char *)NULL; /* Terminierenden NULL-Pointer eintragen */

  return keylist;
}


/** Liefert eine Liste der Schlüssel in einem gegebenen Abschnitt.
 *
 *  Liefert eine Liste aller definierten Schlüssel in einem vorgegebenen Abschnitt zurück.
 *
 *  \param[out] keylist    Pointer auf ein Array von Strings, das die Schlüssel aufnimmt.
 *                         Schlüssel in benannten Abschnitten enthalten am Anfang den Abschnittsnamen
 *                         in eckigen Klammern.
 *                         Wird terminiert durch einen NULL-Pointer, sofern Platz dafür vorhanden ist.
 *                         Platz für die Strings wird dynamisch alloziert.
 *                         Kann NULL sein; dann wird Speicher dynamisch alloziert.
 *  \param[in] maxkeys     Maximal zurückzuliefernde Anzahl von Schlüsseln. Kann <= 0 sein,
 *                         dann keine Begrenzung. keylist muss (falls ungleich NULL)
 *                         jedenfalls groß genug sein, um diese Einträge und den
 *                         terminierenden NULL-Eintrag aufzunehmen.
 *  \param[in] sectionname Name des Abschnitts. Kann auch NULL, leer oder [] sein.
 *                         Begrenzende eckige Klammern dürfen fehlen.
 *
 *  \return Pointer auf das Array von Strings; oder NULL, falls nicht genug Speicherplatz.
 *
 *  \sa ini_nkeys_section( )
 *
 *  \bug
 *  Bei Abbruch wegen Speichermangel wird bereits allozierter Speicher nicht wieder freigegeben.
 *
 */
char **ini_keys_section( char **keylist, int maxkeys, const char *sectionname ) {

  TIniEntry *inilistptr = inilist;
  char **keyptr;
  char *mysectionname;
  size_t sectionnamelen;
  int keycount;

  mysectionname = make_section_name(sectionname);
  if ( !mysectionname ) return NULL;
  sectionnamelen = strlen(mysectionname);

  if ( !keylist ) {
    /* Speicherplatz allozieren. */
    keycount = ini_nkeys_section(mysectionname) + 1;
    if ( ( maxkeys > 0 ) && ( keycount > maxkeys ) ) keycount = maxkeys;
    if ( maxkeys <= 0 ) maxkeys = keycount;
    keylist = (char **)INI_MALLOC( ( (unsigned)keycount+1 ) * sizeof(char *) );
    if ( !keylist ) return NULL;
  }

  /* Alle Einträge durchgehen, dabei nur die passenden berücksichtigen, und höchstens bis zur Maximalzahl. */
  for ( inilistptr=inilist, keyptr=keylist, keycount = 0; inilistptr && ( keycount < maxkeys ); inilistptr=inilistptr->next ) {
    if ( ( ( sectionnamelen != 0 ) &&
           ( strlen(inilistptr->key) >= sectionnamelen ) &&
           ( memcmp( inilistptr->key, mysectionname, sectionnamelen ) == 0 )
         ) ||
         ( ( sectionnamelen == 0 ) && ( ( strlen(inilistptr->key) == 0 ) || ( *(inilistptr->key) != '[' ) ) )
       ) {
      *keyptr = INI_STRALLOC( strlen(inilistptr->key) );
      if ( !*keyptr ) return NULL;
      (void)strcpy( *(keyptr++), inilistptr->key );
      keycount++;
    }
  }

  if ( keycount < maxkeys ) *keyptr = (char *)NULL; /* Terminierenden NULL-Pointer eintragen */

  return keylist;
}


/** Liefert eine Liste der Konfigurationsabschnitte zurück.
 *
 *  Liefert eine Liste aller definierten Abschnitte (Sections) zurück.
 *
 *  \param[out] sectionlist Pointer auf ein Array von Strings, das die Abschnittsnamen aufnimmt.
 *                          Die Abschnittsnamen enthalten begrenzende eckige Klammern,
 *                          der unbenannte Abschnitt ist jedoch ein leerer String.
 *                          Wird terminiert durch einen NULL-Pointer, sofern Platz dafür vorhanden ist.
 *                          Platz für die Strings wird dynamisch alloziert.
 *                          Kann NULL sein; dann wird Speicher dynamisch alloziert.
 *  \param[in] maxsections  Maximal zurückzuliefernde Anzahl von Abschnitten. Kann <= 0 sein,
 *                          dann keine Begrenzung. sectionlist muss (falls ungleich NULL)
 *                          jedenfalls groß genug sein, um diese Einträge und den
 *                          terminierenden NULL-Eintrag aufzunehmen.
 *
 *  \return Pointer auf das Array von Strings; oder NULL, falls nicht genug Speicherplatz.
 *
 *  \sa ini_nsections( )
 *
 *  \bug
 *  Bei Abbruch wegen Speichermangel wird bereits allozierter Speicher nicht wieder freigegeben.
 *
 */
char **ini_sections( char **sectionlist, int maxsections ) {
  TIniEntry *inilistptr;
  char **sectionptr;
  char *sectionname, *temp;
  size_t sectionnamemaxlen, i;
  int mysectioncount = 0;
  int foundunbenannt = 0;
  int isNewSection;

  if ( !sectioncount ) {
    /* Nötigenfalls die Anzahl der Abschnitte neu ermitteln */
    if ( ini_nsections( ) < 0 ) return NULL;
  }

  if ( !sectionlist ) {
    /* Speicherplatz allozieren. */
    mysectioncount = sectioncount + 1;
    if ( ( maxsections > 0 ) && ( mysectioncount > maxsections ) ) mysectioncount = maxsections;
    if ( maxsections <= 0 ) maxsections = mysectioncount;
    sectionlist = (char **)INI_MALLOC( ( (unsigned)mysectioncount+1 ) * sizeof(char *) );
    if ( !sectionlist ) return NULL;
  }

  /* Abschnittsnamen initialisieren: */
  sectionnamemaxlen = 0;
  sectionname = INI_STRALLOC(sectionnamemaxlen);
  if ( !sectionname ) return NULL;
  *sectionname = '\0';

  /* Alle Einträge durchgehen und bei jedem neuen Abschnittsnamen diesen in die Liste übernehmen.
   * Wir wissen: Die Einträge sind grafieunabhängig sortiert, brauchen also keine
   * Historie der gesehenen Einträge zu checken. */
  for ( inilistptr=inilist, sectionptr=sectionlist; inilistptr; inilistptr=inilistptr->next ) {
    if ( strlen(inilistptr->key) ) {
      isNewSection = 0;
      if ( *(inilistptr->key) == '[' ) {
        /* Eintrag mit benamstem Abschnitt */
        temp = strchr( inilistptr->key, ']' );
        if (temp) { /* Sicherheitshalber; alles andere wäre sehr merkwürdig. */
          i = (size_t)( temp - inilistptr->key + 1 );
          if ( (i != strlen(sectionname) ) || ( memcmp( sectionname, inilistptr->key, i ) != 0 ) ) {
            /* Neuer Abschnittsname gefunden! */
            if ( i > sectionnamemaxlen ) {
              sectionnamemaxlen = i;
              sectionname = INI_STRREALLOC( sectionname, sectionnamemaxlen );
              if ( !sectionname ) return NULL;
            }
            INI_COPYSTR( sectionname, inilistptr->key, i );
            isNewSection = 1;
          }
        }
      } else {
        /* Unbenannter Abschnitt. */
        if ( !foundunbenannt ) {
          foundunbenannt = 1;
          isNewSection = 1;
        }
        *sectionname = '\0';
      }
      if (isNewSection) {
        /* Neuen Eintrag in die Liste übernehmen */
        *sectionptr = INI_STRALLOC( strlen(sectionname) );
        if ( !*sectionptr ) return NULL;
        (void)strcpy( *(sectionptr++), sectionname );
      }
    }
  }

  if (mysectioncount < maxsections) *sectionptr = (char *)NULL; /* Terminierenden NULL-Pointer eintragen */

  return sectionlist;
}


/** Liefert die Anzahl der Kommentare zurück.
 *
 *  \return Die Anzahl der Kommentare.
 *
 */
int __inline ini_ncomments(void) { return commentcount; }


/** Liefert eine Liste der Kommentare.
 *
 *  Liefert eine Liste aller Kommentare zurück.
 *
 *  \param[out] commentlist Pointer auf ein Array von Strings, das die Kommentare aufnimmt.
 *                          Wird terminiert durch einen NULL-Pointer, sofern Platz dafür vorhanden ist.
 *                          Platz für die Strings wird dynamisch alloziert.
 *                          Kann NULL sein; dann wird Speicher dynamisch alloziert.
 *  \param[in] maxcomment   Maximal zurückzuliefernde Anzahl von Kommentaren. Kann <= 0 sein,
 *                          dann keine Begrenzung. commentlist muss (falls ungleich NULL)
 *                          jedenfalls groß genug sein, um diese Einträge und den
 *                          terminierenden NULL-Eintrag aufzunehmen.
 *
 *  \return Pointer auf das Array von Strings; oder NULL, falls nicht genug Speicherplatz.
 *
 *  \sa ini_ncomments( )
 *
 *  \bug
 *  Bei Abbruch wegen Speichermangel wird bereits allozierter Speicher nicht wieder freigegeben.
 *
 */
char **ini_comments( char **commentlist, int maxcomment ) {

  TIniEntry *inicommentptr = comments;
  char **commentptr;
  int commentcount;

  if ( !commentlist ) {
    /* Speicherplatz allozieren. */
    commentcount = ini_ncomments( ) + 1;
    if ( ( maxcomment > 0 ) && ( commentcount > maxcomment ) ) commentcount = maxcomment;
    if ( maxcomment <= 0 ) maxcomment = commentcount;
    commentlist = (char **)INI_MALLOC( ( (unsigned)commentcount+1 ) * sizeof(char *) );
    if ( !commentlist ) return NULL;
  }

  /* Alle Kommentare bis zur erlaubten Maximalzahl eintragen. */
  for ( commentcount=0, commentptr=commentlist; inicommentptr && (commentcount < maxcomment); commentcount++, inicommentptr=inicommentptr->next ) {
    *commentptr = INI_STRALLOC( strlen(inicommentptr->value) );
    if ( !*commentptr ) return NULL;
    (void)strcpy( *(commentptr++), inicommentptr->value );
  }

  if ( commentcount < maxcomment ) *commentptr = (char *)NULL; /* Terminierenden NULL-Pointer eintragen */

  return commentlist;
}


/** Fügt eine Kommentarzeile hinzu.
 *
 *  \param[in] text Der Kommentar (ohne einleitendes Kommentarzeichen; darf leer sein).
 *
 *  \return Anzahl der nach dem Hinzufügen verfügbaren Kommentarzeilen.
 *          INI_ERR_NOMEMORY, falls nicht genügend Speicherplatz verfügbar.
 *
 */
int ini_add_comment( const char *text ) {
  TIniEntry *commentptr = comments;
  TIniEntry *previouscommentptr = NULL;
  TIniEntry *newcomment;

  if ( !text ) return commentcount;

  /* Neuen Eintrag hinzufügen. */
  newcomment = (TIniEntry *)INI_MALLOC( sizeof(*newcomment) );
  if ( !newcomment ) return INI_ERR_NOMEMORY;
  newcomment->key   = NULL;
  newcomment->next  = NULL;
  newcomment->value = INI_STRALLOC( strlen(text) );
  if ( !newcomment->value ) {
    INI_FREE(newcomment);
    return INI_ERR_NOMEMORY;
  }
  (void)strcpy( newcomment->value, text );

  if ( commentptr ) {
    /* Wir haben schon mindesetns einen Kommentar.
     * Den neuen Eintrag ans Ende der Kommentarliste anhängen:
     */
    while ( commentptr->next ) commentptr = commentptr->next;
    commentptr->next = newcomment;
  } else {
    /* Dies ist unser erster Kommentar. */
    comments = newcomment;
  }

  return ++commentcount;
}


/** Löscht einen Schlüssel, falls es ihn gibt.
 *
 *  \param[in] key   Der Schlüssel (darf auch leer oder NULL sein).
 *                   Der Schlüssel darf am Anfang in eckigen Klammern einen Abschnittsnamen enthalten.
 *
 *  \return Anzahl der nach dem Einlesen verfügbaren Einträge.
 *          INI_ERR_NOMEMORY, falls nicht genügend Speicherplatz verfügbar.
 *
 */
int ini_delete( const char *key ) {
  TIniEntry *inilistptr = inilist;
  TIniEntry *previousinilistptr = NULL;
  char *tempkey;

  /* Eintrag grafieunabhängig suchen in der Liste: */
  JUMP_EMPTY_SECTION(key);
  tempkey = INI_STRALLOC( INI_STRLEN(key) ); /* NULL-Schlüssel wie leeren Schlüssel behandeln. */
  if ( !tempkey ) return INI_ERR_NOMEMORY;
  INI_COPYSTRUPPER(tempkey, key);

  while ( inilistptr && ( strcmp( tempkey, inilistptr->key ) > 0 ) ) {
    previousinilistptr = inilistptr;
    inilistptr = inilistptr->next;
  }

  if ( inilistptr && INI_STREQUAL( tempkey, inilistptr->key ) ) {
    /* gefunden; also löschen. */
    if ( !previousinilistptr ) {
      /* Es war der erste EWintrag der Liste; also Listenanfang neu setzen. */
      inilist = inilistptr->next;
    } else {
      /* Im voran gehenden Eintrag den Pointer auf das nächste Element umsetzen. */
      previousinilistptr->next = inilistptr->next;
    }
    /* Den von diesem Eintrag belegten Speicher freigeben. */
    INI_FREE(inilistptr->key);
    INI_FREE(inilistptr->value);
    INI_FREE(inilistptr);
    entrycount--;
  }
  INI_FREE(tempkey);

  sectioncount = 0; /* Gecacheten Wert invalidieren */

  return entrycount;
}


/** Schreibt die aktuellen Ini-Werte in eine Datei.
 *
 *  Schreibt den aktuellen Gesamtbestand der Schlüssel-/Werte-Paare in eine Datei,
 *  wahlweise mit einem Kommentar. Platzhalter werden nicht aufgelöst, sondern
 *  wörtlich ausgeschrieben.
 *
 *  \param[in] inifilename Name der zu schreibenden Datei. Wenn NULL oder leer,
 *                         wird nach stdout geschrieben.
 *  \param[in] comment     An den Anfang der Datei zu schreibender Kommentar.
 *                         Wenn ein solcher angegeben ist, werden bisher vorhandene
 *                         (eingelesene oder mit ini_add_comment( ) hinzugefügte)
 *                         Kommentare ignoriert.
 *                         Kann mehrzeilig sein. Kann NULL sein.
 *
 *  \return 0, falls OK.
 *          INI_ERR_NOMEMORY, falls nicht genügend Speicherplatz verfügbar.
 *
 *  \sa ini_nkeys( )
 *
 */
int ini_write( const char * const inifilename, const char *comment ) {

  FILE *inifile;
  TIniEntry *inilistptr;
  char *outbuffer = NULL;
  static const char * const delims[ ] = { "", "\"", "'" };
  static const char * const nonprintables = "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f";
  char minibuf[MINIBUFSIZE];
  char *temp, *temp2, *keyptr, *sectionname;
  const char *temp1;
  size_t sectionnamemaxlen, i;
  int isError, isNewSection, hadNamedSection, hasNonprintables, resolve_escapes=0;

  isError = 0;
  if ( INI_STRLEN(inifilename) ) {
    inifile = fopen( inifilename, "w" );
    if (!inifile) return INI_ERR_NOFILE;
    outbuffer = (char *)INI_MALLOC(BUFSIZE);
    if (outbuffer) setvbuf( inifile, outbuffer, _IOFBF, BUFSIZE );
  } else {
    inifile = stdout;
  }

  if ( INI_STRLEN(comment) ) {
    /* Explizit angegebene neue Kommentare schreiben; bisher vorhandene Kommentare ignorieren. */
    while ( INI_STRLEN(comment) ) {
      temp1 = strchr(comment, '\n');
      if (!temp1) temp1 = comment + strlen(comment);
      if ( fprintf( inifile, "# " ) < 0 ) isError = 1;
      if ( !isError ) {
        if ( fwrite( comment, sizeof(char), (size_t)( temp1-comment ), inifile ) < (size_t)( temp1-comment ) ) isError = 1;
      }
      if ( !isError ) {
        if ( fprintf( inifile, "\n" ) < 0 ) isError = 1;
      }
      if (isError) break;
      comment = ( *temp1 == '\0' ) ? temp1 : (temp1+1);
    }
  } else {
  /* Ggf. bereits vorher vorhandene Kommentare schreiben: */
    inilistptr = comments;
    while (inilistptr) {
      if ( fprintf( inifile, "# %s\n", inilistptr->value ) < 0 ) isError = 1;
      inilistptr = inilistptr->next;
    }
  }

  /* Abschnittsnamen initialisieren: */
  sectionnamemaxlen = 0;
  sectionname = INI_STRALLOC(sectionnamemaxlen);
  if ( !sectionname ) return INI_ERR_NOMEMORY;
  *sectionname = '\0';
  hadNamedSection = 0;

  /* Alle Einträge schreiben. */
  for ( inilistptr=inilist; inilistptr && !isError; inilistptr=inilistptr->next ) {
    keyptr = inilistptr->key ? inilistptr->key : delims[0];
    /* Sicherheitshalber Zeilenende-Zeichen auf Blank umsetzen. (Inplace -- you get what you pay for.) */
    while ( ( temp = strchr( keyptr, '\n' ) ) != NULL ) *temp = ' ';
    while ( ( temp = strchr( keyptr, '\r' ) ) != NULL ) *temp = ' ';

    /* Prüfung auf Abschnittswechsel: */
    isNewSection = 0;
    if ( *keyptr == '[' ) {
      /* Eintrag mit benamstem Abschnitt */
      temp = strchr( keyptr, ']' );
      if (temp) { /* Sicherheitshalber; alles andere wäre sehr merkwürdig. */
        i = (size_t)( temp - keyptr + 1 );
        if ( ( i != strlen(sectionname) ) || ( memcmp( sectionname, keyptr, i ) != 0 ) ) {
          /* Neuer Abschnittsname gefunden! */
          if ( i > sectionnamemaxlen ) {
            sectionnamemaxlen = i;
            sectionname = INI_STRREALLOC( sectionname, sectionnamemaxlen );
            if ( !sectionname ) return INI_ERR_NOMEMORY;
          }
          INI_COPYSTR( sectionname, keyptr, i );
          isNewSection = 1;
        }
        keyptr = temp + 1;
        hadNamedSection = 1;
      }
    } else {
      *sectionname = '\0';
    }

    if ( isNewSection ) {
      /* Neuer Abschnitt. Abschnittskennung muss geschrieben werden, falls
       * (i) benannter Abschnitt, oder (ii) unbenannter Abschnitt nach benanntem Abschnitt. */
      if ( INI_STRLEN(sectionname) ) {
        if ( fprintf( inifile, "%s\n", sectionname ) < 0 ) isError = 1;
      } else {
        if (hadNamedSection) {
          if ( fprintf( inifile, "[]\n" ) < 0 ) isError = 1;
        }
      }
    }

    if ( !INI_STRLEN(inilistptr->value) ) {
      /* Leerer Wert: */
      if ( fprintf( inifile, "%s=\n", keyptr ) < 0 ) isError = 1;
    } else {
      /* Nichtleerer Wert. */

      /* Prüfung auf nichtdruckbare Sonderzeichen unterhalb von \x20 (nur im Value): */
      hasNonprintables = ( strcspn( inilistptr->value, nonprintables ) < strlen( inilistptr->value ) );
      if ( hasNonprintables && !resolve_escapes ) {
        if ( fprintf( inifile, "resolve_escapes: 1\n" ) < 0 ) isError = 1;
        resolve_escapes = 1;
      }

      if ( hasNonprintables || ( resolve_escapes && strchr( inilistptr->value, '\\' ) ) ) {
        /* Zeichen für Zeichen kopieren und dabei ggf. in Hex umsetzen: */
        for (temp=inilistptr->value, temp2=minibuf; *temp; temp++, temp2++ ) {
          if ( *temp < ' ' ) {
            if ( temp2-minibuf+4 >= MINIBUFSIZE ) break;
            sprintf( temp2, "\\x%02x", *temp );
            temp2 += 3;
          } else if ( *temp == '\\' ) {
            if ( temp2-minibuf+2 >= MINIBUFSIZE ) break;
            *(temp2++) = *temp;
            *temp2 = *temp;
          } else {
            if ( temp2-minibuf+1 >= MINIBUFSIZE ) break;
            *temp2 = *temp;
          }
        }
        *temp2 = '\0';
      } else {
        i = strlen( inilistptr->value );
        if ( i >= MINIBUFSIZE ) i = MINIBUFSIZE-1;
        memcpy( minibuf, inilistptr->value, i+1 );
      }

      if ( INI_ISSPACE( *(inilistptr->value)) || INI_ISSPACE( *( inilistptr->value+ ( strlen(inilistptr->value)-1 ) ) ) ) {
        temp1 = delims[ strchr( inilistptr->value, '"' ) ? 2 : 1 ];
        if ( fprintf( inifile, "%s=%s%s%s\n", keyptr, temp1, minibuf, temp1 ) < 0 ) isError = 1;
      } else {
        if ( fprintf( inifile, "%s=%s\n", keyptr, minibuf ) < 0 ) isError = 1;
      }
    }
  }
  fflush(inifile);
  if ( inifile != stdout ) fclose(inifile);
  INI_FREE(outbuffer);

  return isError ? INI_ERR_NOFILE : 0;
}


/** Löscht alle Schlüssel, löscht alle Strukturen.
 *
 * \return Nichts.
 *
 */
void ini_destroy(void) {
  TIniEntry *inilistptr = inilist;
  TIniEntry *nextptr;

  while (inilistptr) {
    nextptr = inilistptr->next;
    INIS_STRFREE(inilistptr->key);
    INIS_STRFREE(inilistptr->value);
    INIS_FREE( inilistptr, sizeof(*inilistptr) );
    inilistptr = nextptr;
  }
  inilist = NULL;

  inilistptr = comments;
  while (inilistptr) {
    nextptr = inilistptr->next;
    INIS_STRFREE(inilistptr->value);
    INIS_FREE( inilistptr, sizeof(*inilistptr) );
    inilistptr = nextptr;
  }
  comments = NULL;

  INIS_STRFREE(logname);
  INIS_STRFREE(errname);
  INIS_STRFREE(errorininame);
  INIS_STRFREE(fallbacksection);
  INIS_STRFREE(tmpstr);

  entrycount = sectioncount = commentcount = 0;

  return;
}


/** Setzt den Namen des Fallback-Abschnitts für im angegebenen Abschnitt nicht gefundene Einträge.
 *
 *  Üblicherweise wird ein Schlüssel nur im angegebenen Abschnitt der
 *  Ini-Datei (was der unbenannte Abschnitt sein kann) gesucht. Mit dieser
 *  Funktion kann jedoch der Name eines Fallback-Abschnitts gesetzt werden,
 *  in dem gesucht wird, falls ein Eintrag im angegebenen Abschnitt nicht
 *  gefunden wird. Der Wert kann leer oder [] sein, dann wird der unbenannte
 *  Abschnitt für den Fallback genutzt. Wenn  der Wert jedoch NULL ist, wird
 *  kein Fallback genutzt.
 *
 *  Der Fallback wird nur für die Suche verwendet, nicht beim Löschen usw.
 *
 *  \param[in] sectionname Name des Fallback-Abschnitts. "[]" für den
 *                         Kann auch NULL oder leer sein, um Fallback
 *                         abzuschalten. Begrenzende eckige Klammern dürfen fehlen
 *                         (außer für den leeren Abschnitt).
 *
 *  \return                0, falls OK. INI_ERR_NOMEMORY, falls nicht genug Speicher.
 *
 *  \sa ini_get_fallbacksection( ), ini_get( )
 *
 */
int ini_set_fallbacksection( const char *sectionname ) {

  if ( !sectionname || !*sectionname ) {
    /* Bisheriges Fallback-Segment killen */
    INIS_STRFREE(fallbacksection);
  } else {
    INIS_STRFREE(fallbacksection);
    fallbacksection = make_section_name(sectionname);
    if ( !fallbacksection ) return INI_ERR_NOMEMORY;
  }

  return 0;
}


/** Holt den Namen des Fallback-Abschnitts für im angegebenen Abschnitt nicht gefundene Einträge.
 *
 *  \return  Pointer auf einen temporären statischen String, der den Namen
 *           des aktuellen Fallback-Segments enthält. Kann NULL sein, wenn
 *           kein Fallback-Segment benannt ist oder wenn nicht genug
 *           Speicher vorhanden ist. Wenn der unbenannte Abschnitt der
 *           Fallback-Abschnitt ist, wird ein leerer String zurückgegeben,
 *           sonst der Name des Fallback-Abschnitts in eckigen Klammern.
 *           Der String sollte für weitere Verwendung kopiert werden.
 *           Der Speicherbereich sollte nicht beschrieben oder freigegeben werden.
 *
 *  \sa ini_set_fallbacksection( ), ini_get( )
 *
 */
char *ini_get_fallbacksection(void) {
  if ( !fallbacksection ) return NULL;
  tmpstr = INI_STRREALLOC( tmpstr, strlen(fallbacksection) );
  if (tmpstr) INI_COPYSTR( tmpstr, fallbacksection, strlen(fallbacksection) );

  return tmpstr;
}


/** Holt den Wert, der zu einem gegebenen Konfigurations-Schlüssel gehört.
 *
 *  Ein Wert kann Referenzen auf andere Schlüssel enthalten, indem der
 *  Schlüsselname in Prozentzeichen eingeschlossen wird. Diese Referenzen
 *  werden dynamisch (d. h. mit dem jeweils aktuell vorliegenden Satz von
 *  Konfigurationseinträgen) und rekursiv aufgelöst. Eine Heuristik
 *  verhindert unendliche Rekursion.
 *
 *  \param[out] value       String, der den Ergebniswert aufnimmt. Muss
 *                          mindestens Platz für maxlen Zeichen (inklusive
 *                          NUL) bieten. Falls NULL, wird Speicherplatz
 *                          dynamisch alloziert. Ist der leere String, wenn
 *                          der Schlüssel nicht existiert.
 *  \param[in] key          Der Schlüssel (darf auch leer oder NULL sein).
 *                          Der Schlüssel darf am Anfang in eckigen Klammern
 *                          einen Abschnittsnamen enthalten.
 *  \param[in] defaultvalue Defaultwert, falls der Schlüssel nicht existiert.
 *                          Kann NULL sein.
 *  \param[in] maxlen       Maximale Länge des Werts. Wenn 0, wird als
 *                          Ergebnis stets der leere String geliefert.
 *
 * \return                  Ein Pointer auf den Ergebnisstring.
 *                          NULL, falls nicht genug Speicher vorhanden.
 *
 *  \sa ini_set_fallbacksection( )
 *
 */
char *ini_get( char *value, const char * key, const char * const defaultvalue, const size_t maxlen ) {

  TIniEntry *inilistptr = inilist;
  char *tempkey, *temp = NULL, *temp2, *buffer = NULL;
  const char *temp3;
  size_t len, len2, len3;
  int maxchanges;

  if ( !value ) value = INI_STRALLOC( (maxlen > 0) ? (maxlen-1) : 0 );
  if ( !value ) return NULL;
  *value     = '\0';
  *(value+1) = '\1';  /* Optimistischer Marker, dass wir einen Wert gefunden haben werden */
  if ( maxlen == 0 ) return value;

  JUMP_EMPTY_SECTION(key);
  tempkey = INI_STRALLOC(INI_STRLEN(key)); /* NULL-Schlüssel wie leeren Schlüssel behandeln. */
  if (!tempkey) return NULL;
  INI_COPYSTRUPPER( tempkey, key );
  temp2 = tempkey;

  while ( inilistptr && ( strcmp( temp2, inilistptr->key ) > 0 ) ) inilistptr = inilistptr->next;

  if ( ( !inilistptr || !INI_STREQUAL( temp2, inilistptr->key ) ) && fallbacksection ) {
    /* Wert nicht gefunden, aber ein Fallback-Abschnitt ist gesetzt.
     * Also dort suchen, falls wir nicht eh bereits dort gesucht haben.
     */
    buffer = temp2 = NULL;
    if ( ( strlen(fallbacksection) == 0 ) && ( *tempkey == '[' ) ) {
      /* In den unbenannten Abschnitt gucken, denn das ist der Fallback-Abschnitt,
       * aber wir waren bis jetzt in einem benannten Abschnitt.
       */
      temp2 = strchr( tempkey, ']' );
      if (temp2) temp2++;
    } else if ( ( strlen(fallbacksection) != 0 ) && ( strstr( tempkey, fallbacksection ) != tempkey ) ) {
      /* Wir haben bisher in einem anderen als dem Fallback-Abschnittt gesucht,
       * also suchen wir dort weiter. Zunächst ggf. den benannten Abschnitt ausblenden: */
      temp2 = strchr( tempkey, ']' );
      if (temp2) {
        temp2++;
      } else {
        temp2 = tempkey;
      }
      buffer = INI_STRALLOC( strlen(fallbacksection)+strlen(temp2) );
      if (buffer) {
        (void)strcpy( buffer, fallbacksection );
        (void)strcat( buffer, temp2 );
        temp2 = buffer;
      }
    }
    if (temp2) {
      /* temp2 zeigt jetzt auf einen neuen Namen, mit dem wir es versuchen können. */
      inilistptr = inilist;
      while ( inilistptr && ( strcmp( temp2, inilistptr->key ) > 0 ) ) inilistptr = inilistptr->next;
    }
  }

  if ( inilistptr && temp2 && INI_STREQUAL( temp2, inilistptr->key ) ) {
    /* gefunden; also Wert kopieren. */
    len = INI_STRLEN(inilistptr->value);
    if ( len > maxlen ) len = maxlen;
    INI_COPYSTR( value, inilistptr->value, len );
  } else {
    /* Wert (immer noch) nicht gefunden. Also Defaultwerte abarbeiten. */
    if (defaultvalue) {
      /* Expliziter Defaultwert. */
      temp3 = defaultvalue;
    } else if ( INI_STREQUAL( tempkey, "LOGFILE" ) ) {
      /* Spezialfall: Standard-Protokolldatei */
      temp3 = logname ? logname : "IniFile.log";
    } else if ( INI_STREQUAL( tempkey, "ERRFILE" ) ) {
      /* Spezialfall: Standard-Fehlerprotokolldatei */
      temp3 = errname ? errname : "IniFile.err";
    } else {
      temp3 = "";
      *(value+1) = '\0';
      /* Undokumentiertes Feature: wenn wir keinen Wert gefunden haben,
       * folgt dem Rückgabewert (leerer String) ein weiteres NUL-Byte.
       */
    }
    len = strlen(temp3);
    if ( len > maxlen ) len = maxlen;
    INI_COPYSTR( value, temp3, len );
  }
  INI_FREE(buffer);

  if ( recursion > 0 ) {
    /* Im Rekursionsfall: Einfach Wert zurückliefern; weitere Referenzen werden dort behandelt. */
    INI_FREE(tempkey);
    return value;
  }

  /* Vorbereiten für Auflösen von Referenzen. Heuristik: maximal so viele
   * Referenzen auflösen, wie der ursprüngliche Eintrag an Referenzen besitzt,
   * multipliziert mit der aktuellen Anzahl vorliegender Einträge insgesamt.
   * Also: jeder Eintrag bekommt statistisch gesehen zwei Chancen,
   * expandiert zu werden.
   */
  recursion = 1;
  maxchanges = 0;
  temp = value;
  while (temp) {
    temp = strchr( temp, '%' );
    if (temp) {
      maxchanges++;
      temp++;
    }
  }
  maxchanges = (maxchanges/2) * ini_nkeys( );
  buffer = INI_STRALLOC(maxlen);
  if ( !buffer ) return NULL;

  /* Den gesamten Wert nach Prozentzeichen durchgehen. */
  temp = value;
  while ( ( ( temp = strchr( temp, '%' ) ) != NULL ) && ( maxchanges > 0 ) ) {
    /* Referenz gefunden. Schlüssel extrahieren. */
    if ( ( temp2 = strchr( temp+1, '%' ) ) == NULL) break; /* Nee, war doch keine. */
    len = (size_t)( temp2 - temp - 1 );
    tempkey = INI_STRREALLOC( tempkey, len );
    if ( !tempkey ) return NULL;
    INI_COPYSTR( tempkey, temp+1, len );
    /* Wert der Referenz rekursiv bestimmen. */
    ini_get( buffer, tempkey, NULL, maxlen );
    if ( *buffer || *(buffer+1) ) {
      /* Referenz durch ihren Wert ersetzen, auch dann wenn der gefundene Wert der leere String ist
       * (aber nicht, wenn der Wert nicht gefunden wurde -- dazu benutzen wir den undokumentierten
       * Marker direkt hinter dem Ende des leeren Strings).
       */
      len3 = strlen(buffer);
      len2 = strlen(temp2+1);
      if ( len3+len2 > maxlen ) len2 = maxlen - len3;
      strncat( buffer, temp2+1, len2 );
      *temp = '\0';
      len  = strlen(temp);
      len2 = strlen(buffer);
      if ( len+len2 > maxlen ) len2 = maxlen - len;
      strncat( temp, buffer, len2 );

      if ( ( ( temp2 = strchr( temp+len+len3, '%' ) ) != NULL ) &&
           ( strchr( temp2+1, '%' ) != NULL )
         ) {
        /* Hinter unserer Ersetzung gibt es weitere Ersetzungskandidaten.
         * Die arbeiten wir zuerst ab, ehe wir unseren Rekursionszähler anpassen.
         */
        temp += len + len3;
      } else {
        /* Keine weiteren Ersetzungskandidaten gefunden. Dann noch mal von vorne an prüfen,
         * ob wir rekursiv tätig werden müssen; dabei aber buchführen, damit wir nicht ewig laufen.
         */
        maxchanges--;
        temp = value;
      }
    } else {
      temp++;
    }
  }

  INI_FREE(buffer);
  INI_FREE(tempkey);
  recursion = 0;

  return value;
}


/** Liefert ggf. den Namen der Ini-Datei, in der ein Fehler aufgetreten ist.
 *
 *  Wenn ein Fehler beim Verarbeiten einer Ini-Datei aufgetreten ist, wird hier ihr
 *  Name zurückgeliefert. Wegen des Include-Mechanismus ist das nicht unbedingt die
 *  Datei, mit der ini_read( ) explizit aufgerufen wurde.
 *
 *  \return  Der Name der Konfigurationsdatei, in der ein Fehler aufgetreten ist.
 *           NULL, wenn kein Fehler aufgetreten ist. Ein leerer String, wenn nicht
 *           genug Speicher für den Namen verfügbar war. Der Speicher sollte durch
 *           das aufrufende Programm nicht verändert oder freigegeben werden.
 *
 *  \sa ini_read( ), ini_get_error_linenumber( )
 *
 */
char *ini_get_error_filename(void) {
  static char *dummy = "";
  if ( errorlinenumber < 0 ) return NULL;
  return errorininame ? errorininame : dummy;
}


/** Liefert ggf. die Nummer der Zeile der Ini-Datei, in der ein Fehler aufgetreten ist.
 *
 *  Wenn ein Fehler beim Verarbeiten einer Ini-Datei aufgetreten ist, wird hier die
 *  Nummer der Zeile geliefert. Wegen des Include-Mechanismus ist das nicht unbedingt in der
 *  Datei, mit der ini_read( ) explizit aufgerufen wurde.
 *
 *  \return  Die Zeilennummer. -1, wenn kein Fehler aufgetreten ist.
 *
 *  \sa ini_read( ), ini_get_error_filename( )
 *
 */
int __inline ini_get_error_linenumber(void) { return errorlinenumber; }


/** Liefert die aktuelle Version der Library als Binärwert.
 *
 *  \return  Die Versionsnummer der Library. Unteres Byte: Minor Version; oberes Byte: Major Version.
 *
 */
unsigned short __inline ini_get_version(void) { return VERSION; }


/** Liefert die aktuelle Version der Library als String.
 *
 *  \return  Die Versionsnummer der Library als Pointer auf einen statischen String..
 *
 */
const char __inline *ini_get_version_string(void) { return VERSIONSTRING; }


/** Liefert den Lizenz- und Copyright-Text.
 *
 *  \return  Pointer auf einen temporären statischen String, der den Text
 *           der Lizenz und des Copyrights enthält. Kann NULL sein, wenn
 *           nicht genug Speicher vorhanden ist. Der String sollte für weitere
 *           Verwendung kopiert werden. Der Speicherbereich sollte nicht
 *           beschrieben oder freigegeben werden.
 *
 */
char *ini_get_license(void) {
  tmpstr = INI_STRREALLOC( tmpstr, strlen(COPYRIGHT) );
  if (tmpstr) INI_COPYSTR( tmpstr, COPYRIGHT, strlen(COPYRIGHT) );
  got_license = 1;

  return tmpstr;
}


/** Gibt das Verzeichnistrennzeichen im Unix- oder DOS-Stil zurück.
 *
 *  Der Verzeichnistrenner wird nach Wunsch geliefert: fix im
 *  Unix- oder im DOS-/Windows-Stil, oder variabel (DOS-/Windows-Stil,
 *  falls ein Konfigurationseintrag mit dem Schlüssel DOS existiert und einen
 *  Wert ungleich 0 hat), oder variabel je nach aktuellem Betriebssystem.
 *
 *  \param[in]     option  Falls 0: Verzeichnistrenner im Unix-Stil.
 *                         Falls 2: Verzeichnistrenner im DOS-/Windows-Stil.
 *                         Falls 1: variable Ermittlung (siehe oben).
 *
 *  \return        Das Verzeichnistrennzeichen.
 *
 */
char ini_get_dir_delimiter( const unsigned char option ) {
  char dosbuff[2];
  char delim = '/';
#ifdef _WIN32
#define DEFAULTOPTION ("1")
#else
#define DEFAULTOPTION ("0")
#endif

  /* Bestimmen, welches Verzeichnistrennerformat wir haben wollen. */
  if ( option == 1 ) {
    /* Variabel: Konfigurationswert mit dem Schlüssel DOS ansehen. */
    ini_get( dosbuff, "DOS", DEFAULTOPTION, 1 );
    if ( ( *dosbuff != '\0' ) && ( !INI_ISSPACE(*dosbuff) ) && ( *dosbuff != '0' ) ) delim = '\\';
  } else if ( option == 2 ) {
    /* DOS-/Windows-Stil. */
    delim = '\\';
  }


  return delim;
}


/** Baut einen Dateipfad aus einzelnen Komponenten zusammen.
 *
 *  Hilfsfunktion für das Zusammenbauen von Dateipfaden. Die Eingabe besteht
 *  aus einer variablen Anzahl von Pfadkomponenten: Laufwerksangabe
 *  (optional als erste Komponente), Verzeichnisse und/oder Teiplfade,
 *  optional ein Dateiname am Ende. Die Funktion fügt die Komponenten
 *  mit für die aktuell vorliegende Plattform geeigneten Pfadtrennzeichen
 *  zusammen und entfernt dabei überflüssige Trennzeichen.
 *
 *  \param[in] components Eine variable Anzahl von Pfadkomponenten.
 *                        Die letzte Komponente muss (char *)NULL sein.
 *                        (Typecast beachten!)
 *
 *  \return               Pointer auf einen temporären Speicherbereich
 *                        mit dem zusammengesetzten Pfad. NULL, wenn
 *                        ein Fehler aufgetreten ist (z.B. Speichermangel).
 *                        Der String sollte für weitere Verwendung kopiert
 *                        werden. Der Speicherbereich sollte nicht beschrieben
 *                        oder freigegeben werden.
 *
 */
char *ini_catfile( char *components, ... ) {
  va_list argptr;
  char dirdelim, *comp;
  size_t n, k, i;

  dirdelim = ini_get_dir_delimiter(1);
  tmpstr = INI_STRREALLOC( tmpstr, 0 );
  if ( !tmpstr ) return NULL;
  va_start( argptr, components );
  comp = components;
  while (comp) {
    n = k = strlen(tmpstr);
    i = strlen(comp);
    if ( n && ( *( tmpstr+n-1 ) != ':' ) && ( *( tmpstr+n-1 ) != dirdelim ) ) k++;
             /* tmpstr enthält schon etwas, an das ein Delimiter angehängt werden muss! */
    tmpstr = INI_STRREALLOC( tmpstr, k+i );
    if ( n != k ) *(tmpstr+n) = dirdelim;
    memmove( tmpstr+k, comp, i+1 );
    comp = va_arg( argptr, char * );
  }
  va_end(argptr);

  return ini_fix_dir_delim( tmpstr, 1 );

}


/** Setzt alle Vorkommen von Verzeichnistrennern auf Unix- oder DOS-Stil um.
 *
 *  Hilfsfunktion für das Zusammenbauen von Dateipfaden. Entfernt doppelte
 *  Verzeichnistrenner (außer am Anfang); setzt Verzeichnistrenner fix auf
 *  Unix- oder auf DOS-/Windows-Stil um, oder variabel (DOS-/Windows-Stil,
 *  falls ein Konfigurationseintrag mit dem Schlüssel DOS existiert und einen
 *  Wert ungleich 0 hat), oder variabel je nach aktuellem Betriebssystem.
 *  Diese Aufgabe wird für die typischen Alltagsverwendungen von ini_catfile( )
 *  einfacher gelöst.
 *
 *  \param[in,out] path    Pfadangabe. Wird an Ort und Stelle modifiziert.
 *  \param[in]     option  Falls 0: Umsetzung auf Unix-Stil.
 *                         Falls 2: Umsetzung auf DOS-/Windows-Stil.
 *                         Falls 1: variable Umsetzung (siehe oben).
 *
 *  \return Pointer auf den modifizierten Pfad.
 *
 *  \sa ini_catfile( )
 *
 */
char *ini_fix_dir_delim( char *path, const unsigned char option ) {

  char *temp;
  char src, dst;

  if ( !path ) return path;

  dst = ini_get_dir_delimiter(option);
  src = ( dst == '/' ) ? '\\' : '/';

  while ( ( temp=strchr( path, src ) ) != NULL ) *temp = dst; /* Umsetzen. */

  /* Jetzt nach doppelten Vorkommen von dst suchen. Am Anfang OK, sonst nicht. */
  temp = path;
  while ( *temp && ( *temp == dst ) ) temp++; /* Verzeichnistrenner am Anfang überspringen */
  src = 0;
  while (*temp) {
    if ( *temp == dst ) {
      if (src) {
        /* Mehrfaches Auftreten, also ab aktueller Stelle aufrücken lassen. */
        memmove( temp, temp+1, strlen(temp) );
      } else {
        src = 1;
        temp++;
      }
    } else {
      src = 0;
      temp++;
    }
  }

  return path;
}


/** Hängt einen Verzeichnistrenner an, falls nicht schon vorhanden.
 *
 *  \param[in,out] dirpath Pfadangabe. Wird an Ort und Stelle modifiziert.
 *                         Muss genug Platz für ein weiteres Zeichen bieten.
 *  \param[in]     option  Falls 0: Entfernen des Trenners am Ende (falls vorhanden).
 *                         Falls 1: Anhängen eines Verzeichnistrenners, falls
 *                                  nicht bereits ein Trenner am Ende vorhanden ist.
 *
 *  \return Pointer auf den modifizierten Pfad.
 *
 *  \sa ini_catfile( )
 *
 *  \deprecated
 *  Diese Aufgabe wird von ini_catfile( ) besser gemacht.
 *
 */
char *ini_add_dir_delim( char *dirpath, const unsigned char option ) {
  char *temp;
  if ( !INI_STRLEN(dirpath) ) return dirpath;

  temp = dirpath + strlen(dirpath) - 1;

  if (option) {
    if ( ( *temp != '/' ) && ( *temp != '\\' ) ) {
      (void)strcat( dirpath, ( strchr( dirpath, '/' ) || !strchr( dirpath, '\\' ) ) ? "/" : "\\" );
    }
  } else {
    if ( ( *temp == '/' ) || ( *temp == '\\' ) ) *temp = '\0';
  }

  return dirpath;
}


#ifdef INI_TEST
/** Testdriver (Executable) für den Konfigurationsdateihandler.
 *
 *  Aufruf mit einem oder mehreren Konfigurationsdateinamen.
 *  Zusätzlich kann -x angegeben werden, um z.B. Fallback-Segmente und das Schreiben von
 *  Konfiguratiosndateien zu testen,
 *  Wird -x zweimal angegeben, wird außerdem noch die Programmierschnittstelle getestet.
 */
int main( int argc, char *argv[ ] ) {

  int i, n, runde = 0, extended = 0;
  char **keys, **sections, **mycomments, **temp, **temp1, *temp2, *firstininame = NULL;
  char value[MINIBUFSIZE], value1[MINIBUFSIZE];
  char *testkeys[ ] =  { "Key1", "[ ]Key2", "Test", "logfile", "errfile", "[]Test", "[dummy]Test", "[dummy]Testdir", "Test2", NULL };
  char *testfallback = "[]";


  (void)ini_get_license( );
  for ( i=1; i<argc; i++ ) {
    if ( INI_STREQUAL( argv[i], "-x" ) ) {
      extended++;
    } else {
      if ( !firstininame ) firstininame = argv[i];
      n = ini_read(argv[i]);
      if ( n < 0 ) {
        (void)printf( "Fehler beim Verarbeiten der INI-Datei %s: Ergebniscode: %d\n", argv[i], n );
        (void)printf( "  Fehlerhafte Zeile %d in Datei %s\n", ini_get_error_linenumber( ), ini_get_error_filename( ) );
        return -n;
      }
      (void)printf( "INI-Datei: %s; kumulierte Anzahl Schluessel/Abschnitte: %d/%d\n", argv[i], ini_nkeys( ), ini_nsections( ) );
    }
  }

  (void)printf( "\nListe aller Schluessel:\n" );
  keys = ini_keys(NULL, 0);
  for ( temp=keys; *temp; temp++ ) {
    (void)printf( "Schluessel <%s>: <%s>\n", *temp, ini_get( value, *temp, "nix", MINIBUFSIZE ) );
    free(*temp);
  }
  INI_FREE(keys);

  (void)printf( "\nListe der Abschnitte:\n" );
  sections = ini_sections( NULL, 0 );
  for ( temp=sections; *temp; temp++ ) {
    (void)printf( "<%s>: %d Schluessel\n", *temp, ini_nkeys_section(*temp) );
    keys = ini_keys_section( NULL, 0, *temp );
    for ( temp1=keys; *temp1; temp1++ ) {
      (void)printf( "  Schluessel <%s>: <%s>\n", *temp1, ini_get( value, *temp1, "nix", MINIBUFSIZE ) );
      free(*temp1);
    }
    INI_FREE(keys);
    free(*temp);
  }
  INI_FREE(sections);

  if ( extended ) {

    (void)printf( "\nAnzahl Kommentare: %d\n", ini_ncomments( ) );
    mycomments = ini_comments( NULL, 0 );
    for ( temp=mycomments; *temp; temp++ ) {
      (void)printf( "%s\n", *temp );
      free(*temp);
    }
    INI_FREE(mycomments);

    (void)printf( "\n%d. Runde (nichtdestruktiv), mit Default-Fallback '%s':\n", ++runde, ini_get_fallbacksection( ) );
    for ( temp=testkeys; *temp; temp++ ) {
      (void)printf( "Schluessel <%s>:\n", *temp );
      (void)printf( "  Schluessel ohne Default      : <%s>\n", ini_get( value, *temp, NULL,  MINIBUFSIZE ) );
      (void)printf( "  Schluessel mit  Default 'nix': <%s>\n", ini_get( value, *temp, "nix", MINIBUFSIZE ) );
    }

    ini_set_fallbacksection(testfallback);
    (void)printf( "\n%d. Runde (nichtdestruktiv), mit Fallback '%s':\n", ++runde, ini_get_fallbacksection( ) );
    for ( temp=testkeys; *temp; temp++ ) {
      (void)printf( "Schluessel <%s>:\n", *temp );
      (void)printf("  Schluessel ohne Default      : <%s>\n", ini_get( value, *temp, NULL,  MINIBUFSIZE ) );
      (void)printf("  Schluessel mit  Default 'nix': <%s>\n", ini_get( value, *temp, "nix", MINIBUFSIZE ) );
    }
    ini_set_fallbacksection(NULL);

    if ( extended > 1 ) {
      (void)printf( "\n%d. Runde (destruktiv):\n", ++runde );
      for (temp=testkeys; *temp; temp++ ) {
        (void)printf( "Schluessel <%s>:\n", *temp );
        (void)printf( "  Schluessel ohne Default      : <%s>\n", ini_get( value, *temp, NULL,  MINIBUFSIZE ) );
        (void)printf( "  Schluessel mit  Default 'nix': <%s>\n", ini_get( value, *temp, "nix", MINIBUFSIZE ) );
        (void)printf( "  Setzen auf 'Testwert'...\n");
        ini_add( *temp, "Testwert" );
        (void)printf( "  Schluessel mit  Default 'nix': <%s>\n", ini_get( value, *temp, "nix", MINIBUFSIZE ) );
        (void)printf( "  Entfernen...\n");
        ini_delete(*temp);
        (void)printf( "  Schluessel ohne Default      : <%s>\n", ini_get( value, *temp, NULL,  MINIBUFSIZE ) );
        (void)printf( "  Schluessel mit  Default 'nix': <%s>\n", ini_get( value, *temp, "nix", MINIBUFSIZE ) );
      }
    }

    ini_add_comment( "Dies ist ein neuer Kommentar" );
    (void)printf( "\nAnzahl Kommentare: %d\n", ini_ncomments( ) );
    mycomments = ini_comments( NULL, 0 );
    for ( temp=mycomments; *temp; temp++ ) {
      (void)printf( "%s\n", *temp );
      free(*temp);
    }
    INI_FREE(mycomments);

    if ( firstininame ) {
      temp2 = INI_STRALLOC( strlen(firstininame) + 4 );
      INI_COPYSTR( temp2, firstininame, strlen(firstininame) );
      INI_COPYSTR( temp2+strlen(firstininame), ".new", 4 );
      (void)printf( "\nSchreibe neue Ini-Datei %s\n", temp2 );
    } else {
      temp2 = NULL;
      (void)printf( "\nSchreibe Ini-Datei nach stdout\n" );
    }

    if ( extended > 1 ) {
      ini_write( temp2, "Neuer Kommentar Zeile 1\nUnd Zeile 2" );
    } else {
      ini_write( temp2, NULL );
    }

    INI_FREE(temp2);

    if ( extended > 1 ) {
      (void)printf( "\nHilfsfunktionen\n" );
      ini_get( value1, "TESTDIR", NULL, MINIBUFSIZE );
      INI_COPYSTR( value, value1, MINIBUFSIZE-1 );
      (void)printf( "fixdir0 <%s>: <%s>\n", value1, ini_fix_dir_delim( value, 0 ) );
      INI_COPYSTR( value, value1, MINIBUFSIZE-1 );
      (void)printf( "fixdir1 <%s>: <%s>\n", value1, ini_fix_dir_delim( value, 1 ) );
      INI_COPYSTR( value, value1, MINIBUFSIZE-1 );
      (void)printf( "fixdir2 <%s>: <%s>\n", value1, ini_fix_dir_delim( value, 2 ) );
      INI_COPYSTR( value, value1, MINIBUFSIZE-1 );
      (void)printf( "adddir0 <%s>: <%s>\n", value1, ini_add_dir_delim( value, 0 ) );
      INI_COPYSTR( value, value1, MINIBUFSIZE-1 );
      (void)printf( "adddir1 <%s>: <%s>\n", value1, ini_add_dir_delim( value, 1 ) );
      ini_get( value, "BASIS", NULL, MINIBUFSIZE );
      (void)printf( "catfile <%s> <%s>: <%s>\n", value1, value, ini_catfile( value1, value, (char *)NULL ) );

      (void)printf( "\nInformationsfunktionen\n" );
      (void)printf( "Versionsnummer: %04x\n", ini_get_version( ) );
      (void)printf( "Versionsstring: %s\n",   ini_get_version_string( ) );
      (void)printf( "Lizenz: %s",             ini_get_license( ) );
      if ( ini_get_error_linenumber( ) >= 0 ) {
        (void)printf( "Letzter Fehler in Zeile %d in Datei: %s\n", ini_get_error_linenumber( ), ini_get_error_filename( ) );
      } else {
        (void)printf( "Keine Fehler in den Konfigurationsdateien\n" );
      }
    }
  }

  ini_destroy( );

  if ( extended ) {
    (void)printf( "\nINI-Werte nach Destroy: %d\n", ini_nkeys( ) );
    for ( temp=testkeys; *temp; temp++ ) {
      (void)printf( "Schluessel <%s>: <%s>\n", *temp, ini_get( value, *temp, "nix", MINIBUFSIZE ) );
    }
  }

  return 0;
}

#endif
