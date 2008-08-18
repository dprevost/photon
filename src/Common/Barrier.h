/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of the vdsf (Virtual Data Space Framework) Library.
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifndef PSC_BARRIER_H
#define PSC_BARRIER_H

#if defined (WIN32)
#  include "Common/arch/Barrier-win32.h"
#elif defined (VDS_USE_I386_GCC)
#  include "Common/arch/Barrier-i386-gcc.h"
#elif defined (VDS_USE_X86_64_GCC)
#  include "Common/arch/Barrier-x86_64-gcc.h"
#elif defined (VDS_USE_SPARC_GCC)
#  include "Common/arch/Barrier-sparc-gcc.h"
#elif defined (VDS_USE_PPC_GCC)
#  include "Common/arch/Barrier-ppc-gcc.h"
#elif defined(VDS_USE_PPC_XLC)
#  include "Common/arch/Barrier-ppc-xlc.h"
#else
#  error "Not implemented yet!"
#endif

#endif /* PSC_BARRIER_H */

