/*
 * Implicator.h
 *
 * Copyright (C) 2009, 2014 Linas Vepstas
 *
 * Author: Linas Vepstas <linasvepstas@gmail.com>  January 2009
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _OPENCOG_IMPLICATOR_H
#define _OPENCOG_IMPLICATOR_H

#include "InitiateSearchMixin.h"
#include "RewriteMixin.h"
#include "SatisfyMixin.h"
#include "TermMatchMixin.h"

namespace opencog {

class Implicator:
	public InitiateSearchMixin,
	public RewriteMixin,
	public TermMatchMixin,
	public SatisfyMixin
{
	public:
		Implicator(AtomSpace* asp) :
			InitiateSearchMixin(asp),
			RewriteMixin(asp),
			TermMatchMixin(asp) {}

			virtual void set_pattern(const Variables& vars,
			                         const Pattern& pat)
			{
				InitiateSearchMixin::set_pattern(vars, pat);
				TermMatchMixin::set_pattern(vars, pat);
			}
};

}; // namespace opencog

#endif // _OPENCOG_IMPLICATOR_H
