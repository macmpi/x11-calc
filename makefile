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
#  09 Oct 21   0.1   - Initial version - MT
#  22 Nov 21   0.2   - Added data files to backups - MT
#  04 Jan 22         - Added HP35 - MT
#  07 Jan 22         - Added HP80 - MT
#  07 Jan 22         - Added HP70 - MT
#  31 jan 22         - Added support for the HP10C, HP11C, HP12C, HP15C and
#                      HP16C - MT
#  31 Mar 22         - Moved ROMs into target folder - MT
#  11 Dec 22         - Renamed models with continious memory and added HP25
#                      HP33E, and HP38E - MT
#  23 Dec 22         - Changed the order in which simulators are built - MT
#  24 Oct 23         - Selects files using a wildcard, avoids any errors if
#                      no files exist when using tar - MT
#                    - Backs up backup files! (*.h.[0-9], *.h.[0-9]) - MT
#                    - Clean removes execuitable files - MT
#                    - Checks that the bin directory exists before deleting
#                      files - MT
#  01 Nov 23         - Tidied up tabs in comments - MT
#  16 Feb 24         - Removed HP10 - MT
#  25 Feb 24         - Backup all makefiles - MT
#  26 Feb 24         - Defined common options in make comand - MT
#                    - Renamed makefiles to make it harder to use the wrong
#                      makefile on different operating systems - MT
#  02 Mar 24         - Added scripts to FILES - MT
#  04 Mar 24         - Added launcher build - macmpi
#  09 Mar 24         - Individual  calc recipes are  dynamically  generated
#                      and desktop file has built models shortcuts - macmpi
#  10 Mar 24         - Moved new desktop file to TARGET directory - MT
#                    - Create TARGET directory if it doesn't exist - MT
#                    - Fixed error in directory path when TARGET is defined
#                      on the command line - MT
#                    - Simplified directory creation - MT
#  13 Mar 24         - Create  install  recipe to install in given drectory
#                      add DESTDIR, and rename TARGET as BIN - macmpi
#  16 Mar 24         - Updated list of files (used to backup project) - MT
#                      Improve GNU conformance with prefix - macmpi
#  17 Mar 24         - Fail early if source and destination directories are
#                      the same - MT
#  18 Mar 24         - Test that $(prefix) exists before installing - MT
#  20 Mar 24         - Rename launcher into x11-calc - macmpi
#                    - Updated files - MT
#  22 Mar 24         - Simplify model targets auto-generation (portability)
#                      and use same naming as in oyher makefiles - macmpi
#                    - Clean out unwanted files before taking a backup, and
#                      remove symbolic links too - MT
#  23 Mar 24         - Do not delete execuitables - MT
#                    - Do not automatically delete object files - MT
#                    - Removed make.sh from list of files - MT
#                    - Do not clean up files before backing up - MT
#                    - Make clean deletes execuitables (leaving other files
#                      in the same folder untouched) - MT
#                    - Use .DEFAULT target + improve x11-calc deps - macmpi
#                    - Feed x11-calc _models with makefile  MODELS - macmpi
#  24 Mar 24         - Updated FILES - MT
#  28 Mar 24         - Leverage src/makefile.generic - macmpi
#  30 Mar 24         - Now common makefile for Linux/NetBSD/Darwin - macmpi
#  01 Apr 24         - Prepare osf1 inclusion into common makefile - macmpi
#                    - Tested on Tru64 UNIX and fixed up minor issues - MT
#                    - Modified backup to work on Tru64 Linux - MT
#                    - Consolidate Linux/NetBSD/Darwin and OSF1 - macmpi
#  02 Apr 24         - Add differentiated desktop files installs - macmpi
#  03 Apr 24         - Allow more custom desktop files installs - macmpi
#  03 Apr 24         - Fixed possible issue with use of date - MT
#                    - Attempts  to detect the default desktop  environment
#                      based on the existing directory structure - MT
#  04 Apr 24         - Aggregate everything in unique top makefile - macmpi
#


PROGRAM		=  x11-calc

BIN		= bin
SRC		= src
PRG		= prg
ROM		= rom
IMG		= img

FILES	= `ls makefile makefile.*.[0-9] $(SRC)/makefile $(SRC)/makefile.common $(SRC)/makefile.linux $(SRC)/makefile.bsd $(SRC)/makefile.osf1 $(SRC)/makefile.*.[0-9] 2>/dev/null || true`
SOURCE	= `ls $(SRC)/*.c $(SRC)/*.c.[0-9] $(SRC)/*.h $(SRC)/*.h.[0-9] $(SRC)/*.in $(SRC)/*.in.[0-9] 2>/dev/null || true`
DATA	= `ls $(ROM)/$(PROGRAM)*.rom $(ROM)/$(PROGRAM)*.rom.[0-9] $(PRG)/$(PROGRAM)*.dat $(PRG)/$(PROGRAM)*.dat.[0-9] 2>/dev/null || true`
IMAGES	= `ls $(SRC)/*.ico $(SRC)/*.ico.[0-9] $(SRC)/*.png $(SRC)/*.png.[0-9] $(SRC)/*.svg $(SRC)/*.svg.[0-9] $(IMG)/*.png $(IMG)/*.png.[0-9] 2>/dev/null || true`
OTHERS	= `ls $(SRC)/make.com  $(SRC)/make.com.[0-9] *.md *.md.[0-9] .gitignore .gitattributes 2>/dev/null || true`

_date	= `date +'%Y%m%d%H%M'`

CLASSIC		= hp35 hp45 hp70 hp80
WOODSTOCK	= hp21 hp22 hp25 hp25c hp27 hp29c
TOPCAT		= hp67
SPICE		= hp31e hp32e hp33e hp33c hp34c hp37e hp38e hp38c
VOYAGER		= hp10c hp11c hp12c hp15c hp16c
MODELS		= $(CLASSIC) $(WOODSTOCK) $(TOPCAT) $(SPICE) $(VOYAGER)

prefix		= $$HOME/.local
DESTDIR		=

DESKTOP	= default
MENU	= hp35 hp21 hp25c hp29c hp31e hp32e hp33c hp34c hp10c hp11c hp12c hp15c hp16c

.PHONY: clean

all: $(MODELS) $(PROGRAM)

classic: $(CLASSIC) $(PROGRAM)

woodstock: $(WOODSTOCK) $(PROGRAM)

topcat: $(TOPCAT) $(PROGRAM)

spice: $(SPICE) $(PROGRAM)

voyager: $(VOYAGER) $(PROGRAM)

# this is base pre-model compile target:
.DEFAULT:
	@_model="`echo "$@" | sed 's/hp//'`"; \
	cd $(SRC); \
	$(MAKE) -s MODEL=$$_model all

$(PROGRAM): $(BIN)/$(PROGRAM)

$(BIN)/$(PROGRAM): $(SRC)/$(PROGRAM).in
	@mkdir -p $(BIN)
	@_mdls=`echo "$(MODELS)" | tr ' ' '|'` ; \
		sed "s/^_models=\".*/_models=\"$$_mdls\"/" $(SRC)/$(PROGRAM).in > $@
	@chmod +x $@
	@ls $@ | sed "s:$(BIN)/::g"

clean:
	@rm -f $(SRC)/*.o $(SRC)/*.o_*
	@[ -d "$(BIN)" ] && rm -rf $(BIN) || true

install:
	@case "$(DESKTOP)" in \
		default) \
			if [ -d "$$HOME/.local" ]; then \
				$(MAKE) -s install_freedesktop; \
			elif  [ -d "$$HOME/.dt" ]; then \
				$(MAKE) -s install_cde; \
			else \
				echo "Not installing: please specify DESKTOP=<value> (none, freedesktop, cde,...)" && false; \
			fi; \
			;; \
		none|freedesktop|cde) \
			$(MAKE) -s install_$(DESKTOP); \
			;; \
		*) \
			$(MAKE) -s install_freedesktop; \
			;; \
	esac

install_none: $(PRG) $(BIN)
	@[ -n "$(DESTDIR)" ] || [ -d "$(prefix)" ] || \
		{ echo "Please ensure $(prefix) path exists or set DESTDIR for staged install." >&2; exit 1; }
	@mkdir -p "$(DESTDIR)$(prefix)"
	@cp -R $(BIN) "$(DESTDIR)$(prefix)"/ # Fail early if source and destination directories are the same
# prg folder
	@mkdir -p "$(DESTDIR)$(prefix)"/share/$(PROGRAM)/
	@cp -R $(PRG) "$(DESTDIR)$(prefix)"/share/$(PROGRAM)/
	@chmod -R 644 "$(DESTDIR)$(prefix)"/share/$(PROGRAM)/$(PRG)/*

install_freedesktop: install_none 
# desktop integration files install for freedesktop compatible desktops
# (workaround bsd sed -i syntax compatibility issues)
	@mkdir -p "$(DESTDIR)$(prefix)"/share/applications;
	@_dest_file="$(DESTDIR)$(prefix)/share/applications/$(PROGRAM).desktop"; \
	_tmp_file="`mktemp`"; \
	cp $(SRC)/$(PROGRAM).desktop.in "$$_dest_file"; \
	for _item in $(MENU); do \
		printf "\n[Desktop Action $$_item]\nName=• $$_item\nExec=$(PROGRAM) $$_item\n" >> "$$_dest_file"; \
		sed "s/^Actions=.*/&;$$_item/" "$$_dest_file" > "$$_tmp_file" ; mv "$$_tmp_file" "$$_dest_file"; \
	done; \
	chmod 644 "$$_dest_file"; \
	rm -f "$$_tmp_file"
# icon file
	@mkdir -p "$(DESTDIR)$(prefix)"/share/icons/hicolor/scalable/apps
	@cp $(SRC)/$(PROGRAM).svg "$(DESTDIR)$(prefix)"/share/icons/hicolor/scalable/apps/$(PROGRAM).svg
	@chmod 644 "$(DESTDIR)$(prefix)"/share/icons/hicolor/scalable/apps/$(PROGRAM).svg

install_cde: install_none
# desktop integration files install for CDE
# check dtcreate for desktop icon action file
# icons in xpm format 16×16 (x11-calc.t.pm) 32×32 (x11-calc.m.pm) 48×48 (x11-calc.l.pm) in either:
# /etc/dt/appconfig/icons/, /usr/dt/appconfig/icons/ or $HOME/.dt/icons
	@echo "Install application on CDE here!"

backup:
	tar -cpf ..\/$(PROGRAM)-$(_date).tar $(FILES)
	tar -rpf ..\/$(PROGRAM)-$(_date).tar $(SOURCE)
	tar -rpf ..\/$(PROGRAM)-$(_date).tar $(DATA)
	tar -rpf ..\/$(PROGRAM)-$(_date).tar $(IMAGES)
	tar -rpf ..\/$(PROGRAM)-$(_date).tar $(OTHERS)
	echo "$(PROGRAM)-$(_date).tar"
