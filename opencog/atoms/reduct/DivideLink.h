/*
 * opencog/atoms/reduct/DivideLink.h
 *
 * Copyright (C) 2015,2018 Linas Vepstas
 * All Rights Reserved
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef _OPENCOG_DIVIDE_LINK_H
#define _OPENCOG_DIVIDE_LINK_H

#include <opencog/atoms/reduct/TimesLink.h>

namespace opencog
{
/** \addtogroup grp_atomspace
 *  @{
 */

/**
 * The DivideLink implements the arithmetic operation of "divide".
 */
class DivideLink : public TimesLink
{
protected:
	void init(void);

	ValuePtr kons(AtomSpace*, bool, const ValuePtr&, const ValuePtr&) const;
public:
	DivideLink(const Handle& a, const Handle& b);
	DivideLink(const HandleSeq&&, Type=DIVIDE_LINK);

	DivideLink(const DivideLink&) = delete;
	DivideLink& operator=(const DivideLink&) = delete;

	static Handle factory(const Handle&);
};

typedef std::shared_ptr<DivideLink> DivideLinkPtr;
static inline DivideLinkPtr DivideLinkCast(const Handle& h)
   { AtomPtr a(h); return std::dynamic_pointer_cast<DivideLink>(a); }
static inline DivideLinkPtr DivideLinkCast(AtomPtr a)
   { return std::dynamic_pointer_cast<DivideLink>(a); }

#define createDivideLink std::make_shared<DivideLink>

/** @}*/
}

#endif // _OPENCOG_DIVIDE_LINK_H
