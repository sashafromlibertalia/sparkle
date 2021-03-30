#!/bin/sh

# Execute like
# gmsymbols.sh magick/.libs/libGraphicsMagick.a MAGICK

LIB=$1
SYM=$2

cat <<EOF
/*
  Copyright (C) 2012-2018 GraphicsMagick Group

  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

  Library symbol name-scoping support.
*/

#if !defined(_${SYM}_SYMBOLS_H)
#define _${SYM}_SYMBOLS_H

#if defined(PREFIX_MAGICK_SYMBOLS)

EOF

nm -p ${LIB} | grep ' [DT] ' | egrep -vi '^(Gm)|(GM)|(lt_)|(_)' | \
egrep -v '(MagickError)|(MagickFatalError)|(MagickWarning)|(ThrowException)|(LoadImageText)|(SaveImageText)|(LoadImagesText)|(SaveImagesText)' | \
awk '{ printf("#define %s Gm%s\n", $3, $3); }' | sort

cat <<EOF

#endif /* defined(PREFIX_MAGICK_SYMBOLS) */
#endif /* defined(_${SYM}_SYMBOLS_H) */

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 2
 * fill-column: 78
 * End:
 */
EOF
