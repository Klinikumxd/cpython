#! /depot/gnu/plat/bin/bash

MYDIR=`dirname $0`
WORKDIR=`pwd`
cd $MYDIR
MYDIR=`pwd`
cd ..
TOPDIR=`pwd`
cd $WORKDIR

# DEFAULT_FORMAT must be upper case...
DEFAULT_FORMAT=PDF
USE_DEFAULT_FORMAT=true
DISCARD_TEMPS=true

HTML_SPLIT_LEVEL=''
L2H_INIT_FILE=$TOPDIR/perl/l2hinit.perl

# This is needed to support kpathsea based TeX installations.  Others are
# not supported.  ;-)
TEXINPUTS=`dirname $MYDIR`/texinputs:$TEXINPUTS
export TEXINPUTS

LOGFILE=/usr/tmp/mkhowto-$LOGNAME-$$.how
LOGGING=''

usage() {
    MYNAME=`basename $0`
    echo "usage: $MYNAME [options...] file ..."
    cat <<EOF

Options specifying formats to build:
    --html		HyperText Markup Language
    --pdf		Portable Document Format (default)
    --ps		PostScript
    --dvi		"DeVice Indepentent" format from TeX

    More than one output format may be specified.

HTML options:
    --address, -a	Specify an address for page footers.
    --split, -s		Specify a section level for page splitting.

Other options:
    --help, -H		Show this text.
    --logging, -l	Log stdout and stderr to a file (*.how).
    --debugging, -D	Echo commands as they are executed.
    --keep, -k		Keep temporary files around.
    --quiet, -q		Do not print command output to stdout.
			(stderr is also lost,  sorry; see *.how for errors)

EOF
    
    exit $1
}

build_html() {
    if [ "$HTML_SPLIT_LEVEL" ] ; then
	if [ "$ADDRESS" ] ; then
	    latex2html -init_file $L2H_INIT_FILE \
	     -address "$ADDRESS" \
	     -split $HTML_SPLIT_LEVEL \
	     $1 || exit $?
	else
	    latex2html -init_file $L2H_INIT_FILE \
	     -split $HTML_SPLIT_LEVEL \
	     $1 || exit $?
	fi
    else
	if [ "$ADDRESS" ] ; then
	    latex2html -init_file $L2H_INIT_FILE \
	     -address "$ADDRESS" \
	     $1 || exit $?
	else
	    latex2html -init_file $L2H_INIT_FILE \
	     $1 || exit $?
	fi
    fi
    if [ "$HTML_SPLIT_LEVEL" != 1 ] ; then
	(cd $FILE; $MYDIR/node2label.pl *.html) || exit $?
    fi
}

build_dvi() {
    latex $1 || exit $?
    latex $1 || exit $?
    if [ -f $1.idx ] ; then
	`dirname $0`/fix_hack $1.idx || exit $?
	makeindex -s $TOPDIR/texinputs/myindex.ist $1.idx || exit $?
    fi
    latex $1 || exit $?
}

build_ps() {
    # note weird sequence of redirects is used to get stderr to the old stdout
    # and the new stdout goes to a file
    dvips -N0 -f $1 >$1.ps || exit $?
}

build_pdf() {
    # We really have to do it three times to get all the page numbers right,
    # since the length of the ToC makes a real difference.
    pdflatex $1 || exit $?
    pdflatex $1 || exit $?
    `dirname $0`/toc2bkm.py -c section $FILE || exit $?
    if [ -f $1.idx ] ; then
	`dirname $0`/fix_hack $1.idx || exit $?
	makeindex -s $TOPDIR/texinputs/myindex.ist $1.idx || exit $?
    fi
    pdflatex $1 || exit $?
}

# figure out what our targets are:
while [ "$1" ] ; do
    case "$1" in
	--pdf|--pd)
	    BUILD_PDF=true
	    USE_DEFAULT_FORMAT=false
	    shift 1
	    ;;
	--ps)
	    BUILD_PS=true
	    USE_DEFAULT_FORMAT=false
	    shift 1
	    ;;
	--dvi|--dv|--d)
	    BUILD_DVI=true
	    USE_DEFAULT_FORMAT=false
	    shift 1
	    ;;
	--html|--htm|--ht)
	    BUILD_HTML=true
	    USE_DEFAULT_FORMAT=false
	    shift 1
	    ;;
	-H|--help|--hel|--he)
	    usage 0
	    ;;
	-a|--address|--addres|--addre|-addr|--add|--ad|--a)
	    ADDRESS="$2"
	    shift 2
	    ;;
	-s|--split|--spli|--spl|--sp|--s)
	    HTML_SPLIT_LEVEL="$2"
	    shift 2
	    ;;
	-l|--logging|--loggin|--loggi|--logg|--log|--lo|--l)
	    LOGGING=true
	    shift 1
	    ;;
	-D|--debugging|--debuggin|--debuggi|--debugg|--debug|--debu|--deb|--de)
	    DEBUGGING=true
	    shift 1
	    ;;
	-k|--keep|--kee|--ke|--k)
	    DISCARD_TEMPS=''
	    shift 1
	    ;;
	-q|--quiet|--quie|--qui|--qu|--q)
	    QUIET=true
	    shift 1
	    ;;
	-*)
	    usage 2
	    ;;
	*)
	    break;;
    esac
done

if [ $# = 0 ] ; then
    usage 2
fi

if [ $USE_DEFAULT_FORMAT = true ] ; then
    eval "BUILD_$DEFAULT_FORMAT=true"
fi

if [ "$DEBUGGING" ] ; then
    set -x
fi

if [ "$QUIET" ] ; then
    exec >/dev/null
fi

for FILE in $@ ; do
    FILE=${FILE%.tex}
    if [ "$BUILD_DVI" -o "$BUILD_PS" ] ; then
	build_dvi $FILE 2>&1 | tee -a $LOGFILE
    fi
    if [ "$BUILD_PDF" ] ; then
	build_pdf $FILE 2>&1 | tee -a $LOGFILE
    fi
    if [ "$BUILD_PS" ] ; then
	build_ps $FILE 2>&1 | tee -a $LOGFILE
    fi
    if [ "$BUILD_HTML" ] ; then
	if [ ! "$BUILD_DVI" -o ! "$BUILD_PDF" ] ; then
	    # need to get aux file
	    build_dvi $FILE 2>&1 | tee -a $LOGFILE
	fi
	build_html $FILE 2>&1 | tee -a $LOGFILE
    fi
    if [ "$DISCARD_TEMPS" ] ; then
	rm -f $FILE.aux $FILE.log $FILE.out $FILE.toc $FILE.bkm 2>&1 \
	 | tee -a $LOGFILE
	if [ ! "$BUILD_DVI" ] ; then
	    rm -f $FILE.dvi 2>&1 | tee -a $LOGFILE
	fi
    fi
    # the the logfile around
    if [ "$LOGGING" ] ; then
	cp $LOGFILE $FILE.how
    fi
    rm -f $LOGFILE
done
