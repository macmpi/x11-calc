#
#  makefile - RPN (Reverse Polish) calculator simulator.
#
#  Copyright(C) 2019 - MT
#
#  This  program is free software: you can redistribute it and/or modify it
#  under  the terms of the GNU General Public License as published  by  the
#  Free  Software Foundation, either version 3 of the License, or (at  your
#  option) any later version.
#
#  This  program  is distributed in the hope that it will  be  useful,  but
#  WITHOUT   ANY   WARRANTY;   without even   the   implied   warranty   of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#  Public License for more details.
#
#  You  should have received a copy of the GNU General Public License along
#  with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#  Note separator (tab) at the beginning of the line CANNOT be a space..!
#
#  09 Oct 21         - Initial version - MT
#  22 Mar 24         - Simple redirection, without extra make.sh - macmpi
#                    - Fixed default rule so it works on NetBSD, Tru64, and
#                      Linux (now we can get rid of make.sh) - MT
#  24 Mar 24         - Pass model number - MT
#  29 Mar 24         - Use linux/bsd/osf1 makefiles -macmpi
#

MODEL	= 21

all:
	@_os="`uname -s | tr '[:upper:]' '[:lower:]'`"; \
	echo "$$_os" | grep -qwE "freebsd|netbsd|darwin" && _os=bsd; \
	$(MAKE) -s -f "makefile.$$_os" MODEL=$(MODEL) $@

.DEFAULT:
	@_os="`uname -s | tr '[:upper:]' '[:lower:]'`"; \
	echo "$$_os" | grep -qwE "freebsd|netbsd|darwin" && _os=bsd; \
	$(MAKE) -s -f "makefile.$$_os" MODEL=$(MODEL) $@
