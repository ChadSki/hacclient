/*= -*- c-basic-offset: 4; indent-tabs-mode: nil; -*-
 *
 * librsync -- the library for network deltas
 * $Id: mdfour.h,v 1.7 2003/10/17 16:15:21 abo Exp $
 * 
 * Copyright (C) 2000, 2001 by Martin Pool <mbp@samba.org>
 * Copyright (C) 2002, 2003 by Donovan Baarda <abo@minkirri.apana.org.au> 
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "types.h"

struct rs_mdfour {
    int                 A, B, C, D;
#if HAVE_UINT64
    uint64_t            totalN;
#else
    uint32_t            totalN_hi, totalN_lo;
#endif
    int                 tail_len;
    unsigned char       tail[64];
};
