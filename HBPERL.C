#define HBPERL_VERSION "4.03"
/* *************************************************************
				HBPERL 
      
  4.01 was issued as HBP40b2.zip, 19/April/1996.
  4.02 was issued as HBP40b3.zip, 22/April/1996.
  4.03 was released to CPAN on 27th May 1996.
    
  Reminder before release: update date & version number in HBPERL.PL + set 
  timestamp on issued files to version number.

  This program is intended to provide an MS-DOS substitute for the 
  #!perl technique used for running perl scripts under UNIX.  
  Techniques that make perl scripts into batch files which run perl 
  and pass themselves are unsatisfactory, as they can't be written 
  cleanly under pure DOS (4DOS overcomes this problem) and don't 
  handle i/o redirection cleanly.

  This program is intended to be renamed to the same name as a 
  perl script, but retaining the .EXE extension.  When run, it 
  finds out its own  name, then looks for <own_name>.PL and an 
  executable Perl interpreter.  It then runs Perl as a child 
  process, passing <own_name>.pl and its own command line as the 
  perl command line.

  Versions 1 to 3 were written in Turbo Pascal to be awkward, and 
  because I thought that this program would never be useful on an 
  operating system other than MS-DOS. Since there is demand for a 
  Windows NT version, and I need to brush up my C, I thought I'd 
  do a translation.

  This version was written in Microsoft Visual C++ v1.5 (effectively
  MSC v8).  The program is called hbperl, rather than #!perl, since 
  MSCV objects to # as the first character of a source filename.
  
  Compiler options to be used: Stack checking ON, Small memory model,
  compile for 286.

  Enhancement to the basic functionality were suggested by Niclas 
  Borlin, of the Computing Science Department, University. of Umea, 
  Sweden (niclas@cs.umu.se) and Jari Aalto (jaalto@tre.tele.nokia.fi)
  (Is there something scandanavian about #!perl ?). None of their code
  has survived the transition to C, but their support and encouragement 
  is warmly appreciated. Valuable comments on version 4 betas were provided
  by Niclas (again), David Moisan (dmoisan@shore.net) and Ronald G. 
  Valiant (rvaliant@freenet.calgary.ab.ca).

* An environment variable PERLSCRIPTS (contents in PATH format) is 
  included in the search pattern.  The intention is that 
	%PATH% is the fall-back place.
  	%PERL% is the place where you keep Perl executables.
  	%PERLLIB% is where you keep the perl library files
  	%PERLSCRIPTS% is where you keep your own scripts.

* The environment variable PERL_EE contains a list of the filename
  extensions that Perl treats as being Perl scripts. If PERL_EE isn't
  defined, the default is PL. If PERL_EE is defined, then .PL isn't
  automatically a Perl script extension. This DOS command would make
  .PL, .PLS, and .PER into Perl script extensions; they are searched
  for in the order specified in the environment variable.

	set PERL_EE=.pl;.pls;.per

This reverses a change between V1 and V2 in that a blank extension
is not automatically a search target. To restore V2 behaviour:

  set PERL_EE=.;.pl

*  Added another environment variable in V4 to specify the name of the 
   interpreter to be run: %PERL_PROG%.  This makes HBPERL into
   a generic wrapper program.

* One downgrade in v4: hbperl no longer handles environment 
  variables with unix-style pathnames (e.g. c:/foo/bar) itself.
  It relies on DOS to accept '/' in place of '\' in pathnames, 
  which does work.                         
  
* Added a command line parameter, -CGI_DEBUG:

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

                                               
The DOS/C version is not as fanatical about minimising its memory usage
as the Pascal versions, since I expect it to only be used with 
BigPerl, which doesn't use much low memory. The first Pascal version
was written for use with 8086 Perl versions, where memory is in very 
short supply, and the optimisations stayed in place. They haven't been
transfered into C.

4.01 fixes, post sending out beta copies:

Didn't take a space out of "Can't find file %s %s" message, since it's done with die3()
Added a note to the CGI_DEBUG page in HBPERL.PL
Added "Content-type: text/html\n\n" to the start of the -CGI_DEBUG output.

4.02 fixes

Did finally take a space out of "Can't find file %s %s" by writing die3a()
Added some extra debug output via #define SUNPC. This enabled me to find out 
what was wrong when running under the SunPC emulator - HBPERL should be compiled 
for 286 for ease of emulation under that, and under non-Intel NT versions.

4.03 fixes

Added </HTML> to the end of the -CGI_DEBUG output.

To-Do:
 
'!' outstanding, '/' = tick, done.  
'X' = not going to, at least not for now.

X  Invent a way of decrypting Perl scripts and passing them to Perl.
   Doing it via a file would be simple, but the user could CTRL-ALT-DEL 
   out. Therefore, we should intercept INT21H, watch for the script 
   file being opened and closed, and delete it when it's closed.  This 
   is death to self-modifying scripts, but that's OK. Niclas Borlin 
   helped invent this method; previous ideas had involved redirecting 
   DOS file i/o to a memory buffer maintained by #!perl, in which the 
   decrypted script would be stored... which is too hard!

   The _interrupt declarator is the key to doing this on DOS: an 
   interrupt handler can be installed with _dos_setvect() and the 
   old routine can be called via a function pointer.
  
!  Create an NT console version. Should not be hard.

******************************************************************* */

#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <dos.h>
#include <string.h>
#include <process.h>

/* Embed ID strings in executable - if this causes problems, just comment it out 
   Remember to examine executable to make sure these strings have actually got 
   into it: there were some problems with __TIMESTAMP__ during development.
*/

#pragma comment( exestr, "Contact: jgd@cix.compulink.co.uk. " ) 
#pragma comment( exestr, "This program may be redistributed under the same conditions as Perl itself. " ) 
#pragma comment( exestr, "Program copyright (c) J.G.Dallman 1996. " ) 
#define HBPERL_IDSTR "HBPERL for DOS, version " HBPERL_VERSION ", built " __TIMESTAMP__ ". "
#pragma comment( exestr,  HBPERL_IDSTR ) 

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/* OS-specific stuff: 

   _MAX_PATH, _MAX_DRIVE, _MAX_DIR, _MAX_FNAME, and _MAX_EXT are defined in STDLIB.H.                                       
   
   MAX_COMMAND_LINE is the maximum size for an command tail.

   MAX_ENV is an arbirtary maximum size for an environment variable name & value. 
   This would normally be the same as MAX_COMMAND_LINE, but I've increased it to
   allow for people using other environment variable editors.
   
   Access modes: MS-DOS has no proper permission system, so we need #defines. 
   We only check for existence: the vagaries of different operating systems 
   are too much for a simple program like this :) 
*/

#ifdef _MSDOS
#define MAX_COMMAND_LINE 128
#define MAX_ENV 512
#define ACCESS_EXISTS 00
#define EXE_EXTN ".COM;.EXE;.BAT"
#endif

/* Function prototypes */

void die( const char*);
void die3( const char *, const char *, const char *);
void die3a( const char *, const char *, const char *);
char *hbp_strdup( const char* );
char *get_commandline( void);
char *get_ee_list( const char * );
char *get_our_name( const char*[] );
char *get_interpreter_name( void);
void get_fname_home( const char *, char **, char ** );
char *do_search( const char*, const char*, const char*);
char *do_ext_search( const char *, const char *, const char *);
char *do_hbp_find( const char *, const char *, const char* );
void get_int_params( const char *, char **, char **);
void cgi_debug_header( const char *, const char *, const char *, const char *);
void main( const int, const char*[]);

/* Global variables */

int cgi_debug;			/* Flag for CGI-debug output */

#ifdef _MSDOS_CRYPT_JOKE
/* These are only used for the INT21H handler and its associated variables 
   All of this stuff is MS-DOS specific at present */

#define INT21 0x21
typedef void (__interrupt __far *int_funcp_t) ( );
int_funcp_t old_int21;
#endif

/* *************************************************************************
					die & die3 & die3a
                                                                            
Print an error message (or three) and exit with error code for failure.
 ************************************************************************ */
void die( const char *message ) 
{
	fprintf( stderr, "HBPERL.EXE: %s\n", message);
#ifdef _DEBUG	
	getchar();		/* Provide a pause for inspection of the wreckage */
#endif
	if( cgi_debug)
	{
		printf( "</BODY>\n");	/* terminate the HTML */
	}
	exit( EXIT_FAILURE);
}

void die3( const char *a, const char *b, const char *c)
{
	fprintf( stderr, "HBPERL.EXE: %s %s %s\n", a, b, c);
#ifdef _DEBUG	
	getchar();		/* Provide a pause for inspection of the wreckage */
#endif
	if( cgi_debug)
	{
		printf( "</BODY>\n");	/* terminate the HTML */
	}
	exit( EXIT_FAILURE);
}

/* Just the same as die3(), except no space between last two fields */

void die3a( const char *a, const char *b, const char *c)
{
	fprintf( stderr, "HBPERL.EXE: %s %s%s\n", a, b, c);
#ifdef _DEBUG	
	getchar();		/* Provide a pause for inspection of the wreckage */
#endif
	if( cgi_debug)
	{
		printf( "</BODY>\n");	/* terminate the HTML */
	}
	exit( EXIT_FAILURE);
}

/* *************************************************************************
					hbp_strdup
                                                                            
A wrapper for strdup that dies if the heap has run out of space.  This will
probably never happen, but...
 ************************************************************************ */
char *hbp_strdup( const char* s)
{
	char *temp;
	if( (temp = _strdup( s)) == NULL)
		die( "Out of heap space");
	return temp;
}

/* *************************************************************************
				get_commandline()                                                                            
                                                                            
Retrive this process's command line.  This will be passed directly to the 
script; we retrive it from the PSP, rather than using argv[], so that we 
don't destroy spaces in the command line.  As the string in the PSP is a 
Pascal string, we have to copy it into local storage
 ************************************************************************ */

char *get_commandline( void) 
{
	unsigned char __far *psp_cl;			/* note FAR is needed for DOS version */
	unsigned char psp_cl_len; 
	char commandline[MAX_COMMAND_LINE];	/* Temporary buffer */

	_FP_SEG( psp_cl) = _psp;	/* point to the byte at the start of the string */
	_FP_OFF( psp_cl) = 0x80;
	psp_cl_len = *psp_cl;		/* Get the length field */ 
	
	_fmemcpy( commandline, ++psp_cl, psp_cl_len);		/* Copy command line */
	commandline[psp_cl_len] = '\0';				/* Terminate command line */

	return hbp_strdup( commandline);
}

/* *************************************************************************
				get_ee_list                                                                       
                                                                            
Get the list of "executable" filename extensions from the environment,
defaulting it to ".PL". If it isn't the default, make a copy of the string,
so that we can do surgery on it later. 
 ************************************************************************ */
char *get_ee_list( const char *env_name ) 
{
	char *temp;
	
	temp = getenv( env_name);

	return (temp == NULL) ? ".PL" : hbp_strdup( temp);
}

/* *************************************************************************
				get_our_name
                                                                            
Get the name of this instance of the program from argv[0].  For modern DOS 
versions and UNIX this is simply hbp_strdup(argv[0]), but the function is provided 
to provide error-trapping on operating systems that don't provide the name.
It may also be needed with C run-time systems that don't provide a full
pathname as argv[0]. MSCV 1.5 does this, but Turbo Pascal 5.5 didn't.
 ************************************************************************ */
char *get_our_name( const char* argv[])
{
	if (( argv[0] == NULL) || (*argv[0] == '\0'))
	{
		die( "No process name supplied by operating system!");
	}
	return hbp_strdup( argv[0]);
}

/* *************************************************************************
				get_interpreter_name
                                                                            
Return the name of the interpreter that we expect to use. If there was one
noted in the script file and it exists, we'll never get here. This looks
in the environment variable PERL_PROG, and if there isn't anything there,
defaults to "PERL".
 ************************************************************************ */
char *get_interpreter_name( void)
{
	char* temp;
	temp = getenv( "PERL_PROG");
	if( cgi_debug)
	{
	 	printf( "Variable PERL_PROG holds '%s'<br>\n", 
	 						(temp == NULL) ? "<empty>" : temp);
	}
	return (temp == NULL) ? "PERL" : temp ;
}

/* *************************************************************************
				get_fname_home
                                                                            
Break the filename and drive/directory pathname out of a full pathname.
This is normally called with the result of get_our_name() as the first
parameter.
 ************************************************************************ */
void get_fname_home( const char *pname, char **fname, char **homedir ) 
{
	char tdrive[_MAX_DRIVE+_MAX_DIR], 	/* Assemble drive+dir */
			tdir[_MAX_DIR],		/* Holds directory, briefly */
			tname[_MAX_FNAME];	/* Holds filename */
	
	_splitpath( pname, tdrive, tdir, tname, NULL);	/* do the split: dump extension */ 
	
	strcat( tdrive, tdir );	/* tack directory onto filename */
	*fname = hbp_strdup( tname);
	*homedir = hbp_strdup( tdrive);
}

/* *************************************************************************
				do_search
                                                                            
A wrapper round Microsoft's _searchenv, to make the form that we'll use
easier to write.
 ************************************************************************ */
char *do_search( const char *filename, const char *ext, const char *env)
{
	char name[_MAX_FNAME + _MAX_EXT], result[_MAX_PATH];

	if( cgi_debug)
	{
		printf( "Searching for '%s%s' via environment variable '%s',<br>\ncontents '%s'<br>\n",
						filename, ext, env, getenv( env)); 		
	}	
	strcat( strcpy( name, filename), ext);	/* Build name */
	_searchenv( name, env, result);
	if ( strlen( result))
	{
		return hbp_strdup( result);		/* Give it some real memory */
	}
	else
	{
		return NULL;
	}
}

/* *************************************************************************
				do_ext_search
                                                                            
Search across extensions for a given environment variable.  Note that the 
second parameter of strtok is the class of all delimeter characters, not
one single delimeter.
 ************************************************************************ */
char *do_ext_search( const char *env, const char *exts, const char *fname) 
{
	char buf[MAX_ENV], *work, *temp;
	strcpy( buf, exts);
	for ( work = strtok( buf, ";"); work != NULL; work = strtok( NULL, ";") )
	{
		if ((temp = do_search( fname, work, env)) != NULL)
			return temp;
	}
	return NULL;	
}

/* *************************************************************************
				do_hbp_find
                                                                            
Find a file: either the script to be executed, or the Perl interpreter.
The clues we have are: 

1) The filename it must have.
2) One directory to look in.
3) A list of possible extensions for it (for the interpreter, .EXE).
4) A known list of directories to search, in the following order:
   1) The directory passed to this function.
   2) The directories in %PERLSCRIPTS%
   3) The directories in %PERLLIB%
   4) The directory in %PERL%
   5) The directories in %PATH%

We will work through the paths in order, varying the executable
extension faster than the path - this should optimise performance.

 ************************************************************************ */
char *do_hbp_find( const char *extensions, const char *name, const char *home )
{
	char *temp, pbuf[_MAX_PATH], ebuf[MAX_ENV], *work;
	strcpy( ebuf, extensions);

#ifdef SUNPC
	if( cgi_debug)
	{
		printf( "Looking in directory '%s' for '%s with extension(s) %s'<br>\n", 
									home, name, extensions );
	}
#endif
	                                                  
	/* Check in the home directory first */
		                                                  
	for( work = strtok( ebuf, ";"); work != NULL; work = strtok( NULL, ";"))
	{
		if( cgi_debug)
		{
			printf( "Looking in home directory '%s' for '%s%s'<br>\n", 
										home, name, work );
		}
		
		strcat( strcat( strcpy( pbuf, home), name), work);
		if (!_access( pbuf, ACCESS_EXISTS))		/* _access returns 0 for success */
		{
			return hbp_strdup( pbuf);
		}
	}
	
	if ((temp = do_ext_search( "PERLSCRIPTS", extensions, name)) != NULL)
		return temp;

	if ((temp = do_ext_search( "PERLLIB", extensions, name)) != NULL)
		return temp;

	if ((temp = do_ext_search( "PERL", extensions, name)) != NULL)
		return temp;

	if ((temp = do_ext_search( "PATH", extensions, name)) != NULL)
	{
		return temp;
	}
	return NULL;
}

/* *************************************************************************
				get_int_params

Get a candidate interpeter and any parameters for it from the script.
 ************************************************************************ */
void get_int_params( const char *script, char **pname, char **params)
{
	FILE *s;
	char buf[_MAX_PATH], *p;

	/* Initialise answers to empty strings */
	
	*pname = *params = "";
	
	/* Open the script */
	
	if ((s = fopen( script, "rt")) == NULL)
		die3( "Can't open	file", script,  strerror( errno));

	if(( p=fgets( buf, _MAX_PATH, s)) == NULL) /* Hit EoF or error */
	{
		if( feof(s))
		{
			fclose( s);	/* on unexpected EoF, just return */
			return;
		}
						/* On error, die: Perl would hit the error anyway */
		die3( "Error reading", script, strerror( errno));
	}
	
	/* OK, we have the first line of the file, so we can close it */
	
	if( fclose(s) == EOF)
		die3( "Error closing", script, strerror( errno));

	/* Is this a sensible-looking #! line? */
	
	if ((buf[0] == '#') && (buf[1] == '!'))	/* Starts with #! ? */
	{
		p = &buf[2];				/* Point to first char after #! */
		p = strtok( p, " \t\r\n\v");		/* Get first token */
		*pname = hbp_strdup( p);		/* and allocate memory */
		
		p = strtok( NULL, " \t\r\n\v");	/* Skip whites to get to params */		
		
		if ((p !=NULL) && (*p == '-'))      /* If valid-looking params */
			*params = hbp_strdup( p);	/* then get the params */	
	}		
}

#ifdef _MSDOS_CRYPT_JOKE
/* *************************************************************************
				int_probe
                                                                            
INT21H handler used to examine the behaviour of the Perl interpreter.
It only got this far - which is supposed to simply call the old handler,
but doesn't seem to work right - before I abandoned the idea.
 ************************************************************************ */

void __interrupt __far int_probe( 
	unsigned _es, unsigned _ds,
	unsigned _di, unsigned _si,
	unsigned _bp, unsigned _sp,
	unsigned _bx, unsigned _dx,
	unsigned _cx, unsigned _ax,
	unsigned _ip, unsigned _cs,
	unsigned _flags ) 
{ 
	(*old_int21)( _es, _ds, _di, _si, _bp, _sp,
			_bx, _dx, _cx, _ax, _ip, _cs, _flags );
}

#ifdef _MSDOS_CRYPT_JOKE
		/* Install the INT21 probe */
		printf ("Setting INT21 vector\n" );
		old_int21 = _dos_getvect( INT21 );
		_dos_setvect( INT21, &int_probe);	
#endif
	
#ifdef _MSDOS_CRYPT_JOKE
		/* Remove the INT21 probe */
		_dos_setvect( INT21, old_int21);
#endif



#endif                                                                    

/* *************************************************************************
				cgi_debug_header
                                                                            
Output HTML describing what we started with...
 ************************************************************************ */
void cgi_debug_header( const char *fname, const char *home, 
				const char *ee_list, const char *commandline )
{                  
	printf( "Content-type: text/html\n\n");
	printf( "<HTML>\n\n");
	printf( "<HEAD><TITLE>HBPERL -CGI_DEBUG output</TITLE>\n");
	printf( "</HEAD>\n");
	printf( "<BODY>\n");
	printf( "<H1>HBPERL -CGI_DEBUG output</H1>\n");
	printf( "HBPERL running as '%s' from directory '%s'<br>\n", fname, home);
	printf( "Searching for script '%s' with extension '%s'<br>\n", fname, ee_list);
	printf( "Command line parameters are '%s'<br>\n", commandline);
}
/* *************************************************************************
				main
                                                                            
The usual...
 ************************************************************************ */

void main( const int argc, const char *argv[])
{	
	char *commandline;		/* pointer to the command tail of this program */
	char *ee_list;			/* pointer to list of executable extensions */
	char *script_name; 		/* Pointer to pathname of script */
	
	char *our_fname;			/* Our own filename (no extension) */
	char *our_home;			/* Our home drive+dir */
	                              
	char *perl_prog_name;		/* Interpreter name from PERL_PROG */	                              
	char *int_pname;			/* Interpreter's path & filename */
	char *int_params;			/* Interpreter parameters */
	int exit_code;			/* Catch the interpreter's exit code */
      
	exit_code = argc;				/* Just to suppress a warning... */	
	commandline = get_commandline();	/* Get command line early, just in case */
                                              
	/* Get executable extension & our names */
	
	ee_list = get_ee_list( "PERL_EE");	
	get_fname_home( get_our_name( argv), &our_fname, &our_home);
      
      cgi_debug = ((strstr( commandline, "-CGI_DEBUG") != NULL) ? TRUE : FALSE);
      
      if( cgi_debug)
      {
      	cgi_debug_header( our_fname, our_home, ee_list, commandline );
      }
      
	/* Find the script! */
	
	if ((script_name = do_hbp_find( ee_list, our_fname, our_home)) == NULL)
	{
		die3a( "Can't find file", our_fname, ee_list);
	}
	else
	{
	 	if (cgi_debug)
	 	{
	 		printf( "Found file '%s'<br>\n", script_name);
	 	}
	}

	/* Extract any significant information from the first line of the script */

      get_int_params( script_name, &int_pname, &int_params);
      
      if( cgi_debug)
      {
       	printf( "Script yields interpreter pathname '%s', and parameters '%s'<br>\n",
       								int_pname, int_params);
      }
      
	/* Let's see if that interpreter exists - it might well not */

	if( _access( int_pname, ACCESS_EXISTS))	/* _access gives TRUE if *not* found */
	{		
		perl_prog_name = get_interpreter_name();
		if( cgi_debug)
		{
			printf( "Interpreter named in script not found<br>\n");
			printf( "Looking for interpreter '%s'<br>\n", perl_prog_name);
		}
		if ((int_pname = do_hbp_find(  EXE_EXTN, perl_prog_name, 
									our_home)) == NULL)
		{
			die3( "Can't find a", perl_prog_name, "interpreter");
		}
		else
		{
			if( cgi_debug)
			{
				printf( "Found '%s' interpreter '%s'<br>\n", 
							perl_prog_name, int_pname); 
			}
		}
	}
	else
	{
	 	if( cgi_debug)
	 	{
	 	 	printf( "The interpreter file named in the script exists<br>\n");
	 	}
	}	

	if( cgi_debug)
	{
		printf( "HBPERL would execute '%s %s %s %s'<br>\n", 
			int_pname, int_params, script_name, commandline);
		printf( "</BODY>\n</HTML>\n");	/* terminate the HTML */
		exit( EXIT_SUCCESS);
	}
	else
	{
		/* Execute the script! */
		exit_code = _spawnl( _P_WAIT, 	/* Allow interpreter to finish */
					int_pname, 		/* Interpreter name */
					int_pname, 		/* Tell it that's it's name */
					int_params,		/* Parameters to the interpreter */
					script_name,	/* The name of the script */
					commandline,	/* Parameters to the script */
					NULL );		/* Terminator for the argument list */
	if (exit_code != -1)			/* If not a spawn error */
		exit( exit_code);			/* Terminate with same exit code */

	die3( "Error EXECing interpreter", int_pname, strerror( errno));
	}
}

/* *************************** End of HBPERL.C ********************************* */
                           
