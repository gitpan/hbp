#!c:/perl/perl.exe -w
# This is HBPERL.PL. a test and demonstration file for HBPERL.EXE
# To try it out, run HBPERL.EXE, which will run this script with the
# -w flag from the #! line above.  To prove that works, uncomment the 
# line below this one:
# $w_detector = 1 ;
# History: Continually updated to reflect Perl features and improve 
# readability. All '@' were escaped with '\' during v4 development
# to avoid problems with Perl 5 being fussier than Perl 4 about "@abc"
# in a here-document when @abc isn't a defined array.

print <<ENDHELP_INTRO ;
Introduction:

  HBPERL.EXE is a front-end for MS-DOS versions of Perl, intended to provide
  a substitute for the #!perl technique used for running Perl scripts under
  UNIX. I've seen several tricks based on MS-DOS batch files, but haven't
  been happy with them: if a program is run by an MS-DOS batch file, redirect- 
  ing its i/o from the batch file's command line is impossible.

  To use HBPERL.EXE to run a script called FOO.PL, copy HBPERL.EXE as FOO.EXE. 
  When you run FOO.EXE, it finds that its name is FOO.EXE. It then searches 
  for FOO.PL and PERL.EXE and runs Perl. The overall effect is this; the 
  command line:

    foo fred jim harry sally

  will cause FOO.EXE to run the following command as a "child process":

    perl foo.pl fred jim harry sally

-- Press ENTER for more --
ENDHELP_INTRO

getc ;

print <<ENDHELP_HASH_BANG ;  

  HBPERL.EXE handles an initial #! line in a Perl script. For example:

    #!/usr/bin/perl -w          # arguments for perl, not the script

  HBPERL will attempt to run /USR/BIN/PERL - if it can't find that, it will
  search for PERL.EXE. If you put the real pathname of your Perl interpreter 
  there, like this:

    #!c:\\bin\\perl.exe -w

  then HBPERL will try to run C:\\BIN\\PERL.EXE.  '/' works in place of '\\'; 
  don't use '\\\\' here, since Perl doesn't interpret this string and '\\\\'
  will confuse DOS.
  
  HBPERL assumes the interpreter name starts after the #! and terminates at 
  whitespace or end-of-line. The argument starts at the first '-' (if any)
  after the interpreter name and terminates at whitespace or end-of-line.
  Like csh, it only grabs one argument, so Perl's argument-concatenation 
  feature is useful.  For an example, look in HBPERL.PL with a text editor: 
  you'll see how to activate a test for the -w parameter.

-- Press ENTER for more --
ENDHELP_HASH_BANG

getc ;

print <<ENDHELP_SEARCH ;

Operation:

  HBPERL.EXE looks for the script in the directory that it was loaded from,
  in the directories nominated by the environment variables PERLSCRIPTS,
  PERLLIB and PERL, if they exist, and along the PATH, in that order. It
  looks for the Perl interpreter with the pathname from the #! line, if any, 
  and then in the same places it looked for the script.

  As HBPERL.EXE is an EXE file, MS-DOS treats it as a "real" program, in the
  same way as UNIX does with a script. It has its own stdin and stdout
  channels, and these are inherited by its child processes - in this case,
  PERL.EXE. This is working now: PERL.EXE is printing to the stdout that it
  inherited from HBPERL.EXE, and your keypress to get to this page was read
  from the inherited stdin.


-- Press ENTER for more --
ENDHELP_SEARCH

getc ;

print <<ENDHELP_ENV ;

Environment variables:

  MS-DOS implementations of Perl use environment variables a lot: there 
  isn't a "standard" way to set up an MS-DOS machine. HBPERL uses five:

  - PERLSCRIPTS: Holds a list of directories, in the same format as PATH,
    which are searched by HBPERL for the script to be run.
    
  - PERLLIB: Holds a list of directories which are searched for the script,
    and are added to the \@INC array by PERL.EXE (in most implementations)
    and thus searched for library files.
    
  - PERL_EE: Specifies the filename extensions which HBPERL thinks 
    indicate Perl scripts.  If not defined, defaults to ".PL".
    To have no extension, .ZZ and .PLS treated as Perl scripts, use: 
    
        set PERL_EE=.;.ZZ;.PLS
        
  - PERL_PROG: See the next page.
  
  - PERL: Holds the pathname of the directory where the Perl executable is
    installed. Not used by PERL.EXE, but useful for MS-DOS setup.

-- Press ENTER for more --
ENDHELP_ENV

getc ;

print <<ENDHELP_PERL_PROG ;
Overriding the interpreter name

  As of V4, HBPERL can be used as a general script-wrapper.  If the 
  environment variable PERL_PROG is set, then HBPERL will look for an
  executable (.COM, .EXE or .BAT) file with that name and run it.  If 
  a valid interpreter pathname is specified in the script, that will 
  override PERL_PROG.  For example, if you have the well-known LIST.COM
  anywhere in the PATH, then these commands:
  
    set PERL_PROG=list
    hbperl
    
  will run LIST on the current directory.  LIST doesn't list HBPERL.PL 
  because the actual command line still has the "-w" from the first line 
  of HBPERL.PL; if you remove that, it will work.
                         
-- Press ENTER for more --
ENDHELP_PERL_PROG

getc ;

print <<ENDHELP_CGI_DEBUG ;
Debugging support

  If you run HBPERL (or a copy with a different name) with the parameter 
  '-CGI_DEBUG' (character case matters), it won't actually try to run an 
  interpreter.  Instead, it will send an HTML document to stdout which 
  describes in excruciating detail what it's doing.  The HTML is formatted 
  to be as human-readable as possible; I hope that this feature will help
  all the people who try to use HBPERL to run Perl scripts on their WWW 
  servers but can't make it work.

  I've also found it useful for testing HBPERL, and hope that it will be
  useful outside WWW servers when you're trying to figure out if a 
  problem is with environment variables, a pathname in a script, or 
  whatever.
                                                                        
                                                                        
                                                                        
                                                                        
-- Press ENTER for more --
ENDHELP_CGI_DEBUG

getc ;
                         
print <<ENDHELP_DETAILS ;

Files:

    HBPERL.PL    Demonstration/help - you're reading it now
    HBPERL.EXE   Executable
    HBPERL.C     Source code - MSVC++ 1.5 for this 16-bit version

Comments:
  
  This is version 4.0 of HBPERL, produced on 18th March 1996. Copying and
  distribution are permitted under the same conditions as Perl itself. No
  liability of any sort is accepted if this program does any damage to
  anything.

Bugs:

* Time & memory overhead: about 1 second on my 486-66 and 30Kb, when 
  running with BigPerl.  As BigPerl doesn't use much low MS-DOS memory, 
  this is not a problem.
* Needs MS-DOS 3.0 or later. Again, this is not a problem.

-- Press ENTER for more --
ENDHELP_DETAILS

getc ;

print <<ENDHELP_LAST ;

Authorship:
  
  HBPERL was written by John Dallman (jgd\@cix.compulink.co.uk) 
  18 Kimberley Road, Cambridge CB4 1HH.  Versions 1.0 to 3.0 were 
  written in Turbo Pascal; Version 4 onwards are in C.

  Valuable assistance and bugfixes for version 2.0 (see HBPERL.C)
  Niclas Borlin (niclas\@cs.umu.se), Computing Science Department, 
  Uni. of Umea, Sweden.

  Inspiration and testing for version 3.0 by Jari Aalto 
  (jaalto\@tre.tele.nokia.fi).

This script is running under Perl version:
$]
ENDHELP_LAST

