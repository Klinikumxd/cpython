"""FAQ Wizard customization module.

Edit this file to customize the FAQ Wizard.  For normal purposes, you
should only have to change the FAQ section titles and the small group
of parameters below it.

"""

# Titles of FAQ sections

SECTION_TITLES = {
    # SectionNumber : SectionTitle; need at least one entry
    1: "General information and availability",
}

# Parameters you definitely want to change

SHORTNAME = "Generic"			# FAQ name with "FAQ" omitted
PASSWORD = ""				# Password for editing
OWNERNAME = "GvR"			# Name for feedback
OWNEREMAIL = "guido@python.org"		# Email for feedback
HOMEURL = "http://www.python.org"	# Related home page
HOMENAME = "Python home"		# Name of related home page
RCSBINDIR = "/usr/local/bin/"		# Directory containing RCS commands
					# (must end in a slash)

# Parameters you can normally leave alone

MAXHITS = 10				# Max #hits to be shown directly
COOKIE_LIFETIME = 28*24*3600		# Cookie expiration in seconds
					# (28*24*3600 = 28 days = 4 weeks)
PROCESS_PREFORMAT = 1                   # toggle whether preformatted text
                                        # will replace urls and emails with 
                                        # HTML links

# Markers appended to title to indicate recently change
# (may contain HTML, e.g. <IMG>); and corresponding 

MARK_VERY_RECENT = " **"		# Changed very recently
MARK_RECENT = " *"			# Changed recently
DT_VERY_RECENT = 24*3600		# 24 hours
DT_RECENT = 7*24*3600			# 7 days

EXPLAIN_MARKS = """
<P>(Entries marked with ** were changed within the last 24 hours;
entries marked with * were changed within the last 7 days.)
<P>
"""

# Version -- don't change unless you edit faqwiz.py

WIZVERSION = "1.0.2"			# FAQ Wizard version

# This parameter is normally overwritten with a dynamic value

FAQCGI = 'faqw.py'			# Relative URL of the FAQ cgi script
import os, sys
FAQCGI = os.path.basename(sys.argv[0]) or FAQCGI
del os, sys

# Perl (re module) style regular expression to recognize FAQ entry
# files: group(1) should be the section number, group(2) should be the
# question number.  Both should be fixed width so simple-minded
# sorting yields the right order.

OKFILENAME = r"^faq(\d\d)\.(\d\d\d)\.htp$"

# Format to construct a FAQ entry file name

NEWFILENAME = "faq%02d.%03d.htp"

# Load local customizations on top of the previous parameters

try:
    from faqcust import *
except ImportError:
    pass

# Calculated parameter names

COOKIE_NAME = SHORTNAME + "-FAQ-Wizard"	# Name used for Netscape cookie
FAQNAME = SHORTNAME + " FAQ"		# Name of the FAQ

# ----------------------------------------------------------------------

# Anything below this point normally needn't be changed; you would
# change this if you were to create e.g. a French translation or if
# you just aren't happy with the text generated by the FAQ Wizard.

# Most strings here are subject to substitution (string%dictionary)

# RCS commands

SH_RLOG = RCSBINDIR + "rlog %(file)s </dev/null 2>&1"
SH_RLOG_H = RCSBINDIR + "rlog -h %(file)s </dev/null 2>&1"
SH_RDIFF = RCSBINDIR + "rcsdiff -r%(prev)s -r%(rev)s %(file)s </dev/null 2>&1"
SH_REVISION = RCSBINDIR + "co -p%(rev)s %(file)s </dev/null 2>&1"
SH_LOCK = RCSBINDIR + "rcs -l %(file)s </dev/null 2>&1"
SH_CHECKIN =  RCSBINDIR + "ci -u %(file)s <%(tfn)s 2>&1"

# Titles for various output pages (not subject to substitution)

T_HOME = FAQNAME + " Wizard " + WIZVERSION
T_ERROR = "Sorry, an error occurred"
T_ROULETTE = FAQNAME + " Roulette"
T_ALL = "The Whole " + FAQNAME
T_INDEX = FAQNAME + " Index"
T_SEARCH = FAQNAME + " Search Results"
T_RECENT = "What's New in the " + FAQNAME
T_SHOW = FAQNAME + " Entry"
T_LOG = "RCS log for %s entry" % FAQNAME
T_REVISION = "RCS revision for %s entry" % FAQNAME
T_DIFF = "RCS diff for %s entry" % FAQNAME
T_ADD = "Add an entry to the " + FAQNAME
T_DELETE = "Deleting an entry from the " + FAQNAME
T_EDIT = FAQNAME + " Edit Wizard"
T_REVIEW = T_EDIT + " - Review Changes"
T_COMMITTED = T_EDIT + " - Changes Committed"
T_COMMITFAILED = T_EDIT + " - Commit Failed"
T_CANTCOMMIT = T_EDIT + " - Commit Rejected"
T_HELP = T_EDIT + " - Help"

# Generic prologue and epilogue

PROLOGUE = '''
<HTML>
<HEAD>
<TITLE>%(title)s</TITLE>
</HEAD>

<BODY BACKGROUND="http://www.python.org/pics/RedShort.gif"
      BGCOLOR="#FFFFFF"
      TEXT="#000000"
      LINK="#AA0000"
      VLINK="#906A6A">
<H1>%(title)s</H1>
'''

EPILOGUE = '''
<HR>
<A HREF="%(HOMEURL)s">%(HOMENAME)s</A> /
<A HREF="%(FAQCGI)s?req=home">%(FAQNAME)s Wizard %(WIZVERSION)s</A> /
Feedback to <A HREF="mailto:%(OWNEREMAIL)s">%(OWNERNAME)s</A>

</BODY>
</HTML>
'''

# Home page

HOME = """
<H2>Search the %(FAQNAME)s:</H2>

<BLOCKQUOTE>

<FORM ACTION="%(FAQCGI)s">
    <INPUT TYPE=text NAME=query>
    <INPUT TYPE=submit VALUE="Search"><BR>
    <INPUT TYPE=radio NAME=querytype VALUE=simple CHECKED>
        Simple string
	/
    <INPUT TYPE=radio NAME=querytype VALUE=regex>
        Regular expression
	/<BR>
    <INPUT TYPE=radio NAME=querytype VALUE=anykeywords>
        Keywords (any)
	/
    <INPUT TYPE=radio NAME=querytype VALUE=allkeywords>
        Keywords (all)
	<BR>
    <INPUT TYPE=radio NAME=casefold VALUE=yes CHECKED>
        Fold case
	/
    <INPUT TYPE=radio NAME=casefold VALUE=no>
        Case sensitive
	<BR>
    <INPUT TYPE=hidden NAME=req VALUE=search>
</FORM>

</BLOCKQUOTE>

<HR>

<H2>Other forms of %(FAQNAME)s access:</H2>

<UL>
<LI><A HREF="%(FAQCGI)s?req=index">FAQ index</A>
<LI><A HREF="%(FAQCGI)s?req=all">The whole FAQ</A>
<LI><A HREF="%(FAQCGI)s?req=recent">What's new in the FAQ?</A>
<LI><A HREF="%(FAQCGI)s?req=roulette">FAQ roulette</A>
<LI><A HREF="%(FAQCGI)s?req=add">Add a FAQ entry</A>
<LI><A HREF="%(FAQCGI)s?req=delete">Delete a FAQ entry</A>
</UL>
"""

# Index formatting

INDEX_SECTION = """
<P>
<HR>
<H2>%(sec)s. %(title)s</H2>
<UL>
"""

INDEX_ADDSECTION = """
<P>
<LI><A HREF="%(FAQCGI)s?req=new&amp;section=%(sec)s">Add new entry</A>
(at this point)
"""

INDEX_ENDSECTION = """
</UL>
"""

INDEX_ENTRY = """\
<LI><A HREF="%(FAQCGI)s?req=show&amp;file=%(file)s">%(title)s</A>
"""

LOCAL_ENTRY = """\
<LI><A HREF="#%(sec)s.%(num)s">%(title)s</A>
"""

# Entry formatting

ENTRY_HEADER1 = """
<HR>
<H2><A NAME="%(sec)s.%(num)s">%(title)s</A>\
"""

ENTRY_HEADER2 = """\
</H2>
"""

ENTRY_FOOTER = """
<A HREF="%(FAQCGI)s?req=edit&amp;file=%(file)s">Edit this entry</A> /
<A HREF="%(FAQCGI)s?req=log&amp;file=%(file)s">Log info</A>
"""

ENTRY_LOGINFO = """
/ Last changed on %(last_changed_date)s by
<A HREF="mailto:%(last_changed_email)s">%(last_changed_author)s</A>
"""

# Search

NO_HITS = """
No hits.
"""

ONE_HIT = """
Your search matched the following entry:
"""

FEW_HITS = """
Your search matched the following %(count)s entries:
"""

MANY_HITS = """
Your search matched more than %(MAXHITS)s entries.
The %(count)s matching entries are presented here ordered by section:
"""

# RCS log and diff

LOG = """
Click on a revision line to see the diff between that revision and the
previous one.
"""

REVISIONLINK = """\
<A HREF="%(FAQCGI)s?req=revision&amp;file=%(file)s&amp;rev=%(rev)s"
>%(line)s</A>\
"""
DIFFLINK = """\
 (<A HREF="%(FAQCGI)s?req=diff&amp;file=%(file)s&amp;\
prev=%(prev)s&amp;rev=%(rev)s"
>diff -r%(prev)s -r%(rev)s</A>)\
"""

# Recently changed entries

NO_RECENT = """
<HR>
No %(FAQNAME)s entries were changed in the last %(period)s.
"""

VIEW_MENU = """
<HR>
View entries changed in the last...
<UL>
<LI><A HREF="%(FAQCGI)s?req=recent&amp;days=1">24 hours</A>
<LI><A HREF="%(FAQCGI)s?req=recent&amp;days=2">2 days</A>
<LI><A HREF="%(FAQCGI)s?req=recent&amp;days=3">3 days</A>
<LI><A HREF="%(FAQCGI)s?req=recent&amp;days=7">week</A>
<LI><A HREF="%(FAQCGI)s?req=recent&amp;days=28">4 weeks</A>
<LI><A HREF="%(FAQCGI)s?req=recent&amp;days=365250">millennium</A>
</UL>
"""

ONE_RECENT = VIEW_MENU + """
The following %(FAQNAME)s entry was changed in the last %(period)s:
"""

SOME_RECENT = VIEW_MENU + """
The following %(count)s %(FAQNAME)s entries were changed
in the last %(period)s, most recently changed shown first:
"""

TAIL_RECENT = VIEW_MENU

# Last changed banner on "all" (strftime format)
LAST_CHANGED = "Last changed on %c %Z"

# "Compat" command prologue (this has no <BODY> tag)
COMPAT = """
<H1>The whole %(FAQNAME)s</H1>
See also the <A HREF="%(FAQCGI)s?req=home">%(FAQNAME)s Wizard</A>.
<P>
"""

# Editing

EDITHEAD = """
<A HREF="%(FAQCGI)s?req=help">Click for Help</A>
"""

REVIEWHEAD = EDITHEAD


EDITFORM1 = """
<FORM ACTION="%(FAQCGI)s" METHOD=POST>
<INPUT TYPE=hidden NAME=req VALUE=review>
<INPUT TYPE=hidden NAME=file VALUE=%(file)s>
<INPUT TYPE=hidden NAME=editversion VALUE=%(editversion)s>
<HR>
"""

EDITFORM2 = """
Title: <INPUT TYPE=text SIZE=70 NAME=title VALUE="%(title)s"><BR>
<TEXTAREA COLS=72 ROWS=20 NAME=body>%(body)s
</TEXTAREA><BR>
Log message (reason for the change):<BR>
<TEXTAREA COLS=72 ROWS=5 NAME=log>%(log)s
</TEXTAREA><BR>
Please provide the following information for logging purposes:
<TABLE FRAME=none COLS=2>
    <TR>
	<TD>Name:
	<TD><INPUT TYPE=text SIZE=40 NAME=author VALUE="%(author)s">
    <TR>
	<TD>Email:
	<TD><INPUT TYPE=text SIZE=40 NAME=email VALUE="%(email)s">
    <TR>
	<TD>Password:
	<TD><INPUT TYPE=password SIZE=20 NAME=password VALUE="%(password)s">
</TABLE>

<INPUT TYPE=submit NAME=review VALUE="Preview Edit">
Click this button to preview your changes.
"""

EDITFORM3 = """
</FORM>
"""

COMMIT = """
<INPUT TYPE=submit NAME=commit VALUE="Commit">
Click this button to commit your changes.
<HR>
"""

NOCOMMIT = """
To commit your changes, please enter a log message, your name, email
addres, and the correct password in the form below.
<HR>
"""

CANTCOMMIT_HEAD = """
Some required information is missing:
<UL>
"""
NEED_PASSWD = "<LI>You must provide the correct passwd.\n"
NEED_AUTHOR = "<LI>You must enter your name.\n"
NEED_EMAIL = "<LI>You must enter your email address.\n"
NEED_LOG = "<LI>You must enter a log message.\n"
CANTCOMMIT_TAIL = """
</UL>
Please use your browser's Back command to correct the form and commit
again.
"""

NEWCONFLICT = """
<P>
You are creating a new entry, but the entry number specified is not
correct.
<P>
The two most common causes of this problem are:
<UL>
<LI>After creating the entry yourself, you went back in your browser,
    edited the entry some more, and clicked Commit again.
<LI>Someone else started creating a new entry in the same section and
    committed before you did.
</UL>
(It is also possible that the last entry in the section was physically
deleted, but this should not happen except through manual intervention
by the FAQ maintainer.)
<P>
<A HREF="%(FAQCGI)s?req=new&amp;section=%(sec)s">Click here to try
again.</A>
<P>
"""

VERSIONCONFLICT = """
<P>
You edited version %(editversion)s but the current version is %(version)s.
<P>
The two most common causes of this problem are:
<UL>
<LI>After committing a change, you went back in your browser,
    edited the entry some more, and clicked Commit again.
<LI>Someone else started editing the same entry and committed
    before you did.
</UL>
<P>
<A HREF="%(FAQCGI)s?req=show&amp;file=%(file)s">Click here to reload
the entry and try again.</A>
<P>
"""

CANTWRITE = """
Can't write file %(file)s (%(why)s).
"""

FILEHEADER = """\
Title: %(title)s
Last-Changed-Date: %(date)s
Last-Changed-Author: %(author)s
Last-Changed-Email: %(email)s
Last-Changed-Remote-Host: %(REMOTE_HOST)s
Last-Changed-Remote-Address: %(REMOTE_ADDR)s
"""

LOGHEADER = """\
Last-Changed-Date: %(date)s
Last-Changed-Author: %(author)s
Last-Changed-Email: %(email)s
Last-Changed-Remote-Host: %(REMOTE_HOST)s
Last-Changed-Remote-Address: %(REMOTE_ADDR)s

%(log)s
"""

COMMITTED = """
Your changes have been committed.
"""

COMMITFAILED = """
Exit status %(sts)s.
"""

# Add/Delete

ADD_HEAD = """
At the moment, new entries can only be added at the end of a section.
This is because the entry numbers are also their
unique identifiers -- it's a bad idea to renumber entries.
<P>
Click on the section to which you want to add a new entry:
<UL>
"""

ADD_SECTION = """\
<LI><A HREF="%(FAQCGI)s?req=new&amp;section=%(section)s">%(section)s. %(title)s</A>
"""

ADD_TAIL = """
</UL>
"""

ROULETTE = """
<P>Hit your browser's Reload button to play again.<P>
"""

DELETE = """
At the moment, there's no direct way to delete entries.
This is because the entry numbers are also their
unique identifiers -- it's a bad idea to renumber entries.
<P>
If you really think an entry needs to be deleted,
change the title to "(deleted)" and make the body
empty (keep the entry number in the title though).
"""

# Help file for the FAQ Edit Wizard

HELP = """
Using the %(FAQNAME)s Edit Wizard speaks mostly for itself.  Here are
some answers to questions you are likely to ask:

<P><HR>

<H2>I can review an entry but I can't commit it.</H2>

The commit button only appears if the following conditions are met:

<UL>

<LI>The Name field is not empty.

<LI>The Email field contains at least an @ character.

<LI>The Log message box is not empty.

<LI>The Password field contains the proper password.

</UL>

<P><HR>

<H2>What is the password?</H2>

At the moment, only PSA members will be told the password.  This is a
good time to join the PSA!  See <A
HREF="http://www.python.org/psa/">the PSA home page</A>.

<P><HR>

<H2>Can I use HTML in the FAQ entry?</H2>

No, but if you include a URL or an email address in the text it will
automatigally become an anchor of the right type.  Also, *word*
is made italic (but only for single alphabetic words).

<P><HR>

<H2>How do I delineate paragraphs?</H2>

Use blank lines to separate paragraphs.

<P><HR>

<H2>How do I enter example text?</H2>

Any line that begins with a space or tab is assumed to be part of
literal text.  Blocks of literal text delineated by blank lines are
placed inside &lt;PRE&gt;...&lt;/PRE&gt;.
"""

# Load local customizations again, in case they set some other variables

try:
    from faqcust import *
except ImportError:
    pass
