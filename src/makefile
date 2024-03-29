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
#  29 Mar 24         - Use linx/bsd/osf1 makefiles
#

MODEL	=  21

all:
	@_flavor="`uname -s | tr '[:upper:]' '[:lower:]'`"; \
	case $$_flavor in \
		netbsd) _flavor=bsd;; \
		darwin) _flavor=bsd;; \
	esac; \
	$(MAKE) -s -f "makefile.$$_flavor" MODEL=$(MODEL) $@

.DEFAULT:
	@_flavor="`uname -s | tr '[:upper:]' '[:lower:]'`"; \
	case $$_flavor in \
		netbsd) _flavor=bsd;; \
		darwin) _flavor=bsd;; \
	esac; \
	$(MAKE) -s -f "makefile.$$_flavor" MODEL=$(MODEL) $@

