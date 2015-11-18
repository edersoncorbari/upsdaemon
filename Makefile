#
# Copyright (c) 1995, 2015 Ederson de Moura <ederbsd@gmail.com>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer
#    in this position and unchanged.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

CC=             /usr/bin/cc
SBINDIR=        ${PREFIX}/sbin
MANDIR=         ${PREFIX}/man/man8
SCRIPTDIR=      ${PREFIX}/libexec/upsdaemon
STARTUP=        ${PREFIX}/scripts/upsdaemon

all:    ./src/upsdaemon.c
	${CC} -Wall -DNDEBUG -g -O3 -o ./upsdaemon ./src/upsdaemon.c

install:
	${BSD_INSTALL_PROGRAM} ./upsdaemon $(DESTDIR)${SBINDIR}
	${BSD_INSTALL_MAN} ./doc/upsdaemon.8 $(DESTDIR)${MANDIR}
	mkdir -p $(DESTDIR)${SCRIPTDIR}
	(cd scripts; for i in *; do ${BSD_INSTALL_SCRIPT} $$i $(DESTDIR)${SCRIPTDIR}; done; exit 0;)

clean:
	rm -f ./upsdaemon
	rm -f ${SBINDIR}/upsdaemon
	rm -f ${MANDIR}/man8/upsdaemon.8
	rm -rf ${SCRIPTDIR}/upsdaemon
