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
#  01 Apr 24         - Consolidate Linux/NetBSD/Darwin and OSF1 - macmpi
#  02 Apr 24         - Add differentiated desktop files installs - macmpi
#  03 Apr 24         - Allow more custom desktop files installs - macmpi
#

include makefile.env

DESKTOP	= default

all:
	@$(MAKE) -s -f "makefile.common" $@

.DEFAULT:
	@$(MAKE) -s -f "makefile.common" $@

clean:
	@$(MAKE) -s -f "makefile.common" $@

install:
	@$(MAKE) -s -f "makefile.common" $@
	@case "$(DESKTOP)" in \
		default) \
			_flavor="`uname | tr '[:upper:]' '[:lower:]'`"; \
			if [ "$$_flavor" = "osf1" ]; then \
				$(MAKE) -s -f "makefile.cde" desktop; \
			else \
				$(MAKE) -s -f "makefile.freedesktop" desktop; \
			fi; \
			;; \
		none) \
			:; \
			;; \
		*) \
			$(MAKE) -s -f "makefile.$(DESKTOP)" desktop; \
			;; \
	esac

backup:
	@$(MAKE) -s -f "makefile.backup" $@

