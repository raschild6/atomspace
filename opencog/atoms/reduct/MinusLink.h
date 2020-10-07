/*
 * opencog/atoms/reduct/MinusLink.h
 *
 * Copyright (C) 2015, 2018 Linas Vepstas
 * All Rights Reserved
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef _OPENCOG_MINUS_LINK_H
#define _OPENCOG_MINUS_LINK_H

#include <opencog/atoms/reduct/PlusLink.h>

namespace opencog
{
/** \addtogroup grp_atomspace
 *  @{
 */

/**
 * The MinusLink implements the arithmetic operation of "minus"
 */
class MinusLink : public PlusLink
{
protected:
	void init(void);

	ValuePtr kons(AtomSpace*, bool, const ValuePtr&, const ValuePtr&) const;
public:
	MinusLink(const Handle& a, const Handle& b);
	MinusLink(const HandleSeq&&, Type=MINUS_LINK);

	MinusLink(const MinusLink&) = delete;
	MinusLink& operator=(const MinusLink&) = delete;

	static Handle factory(const Handle&);
};

typedef std::shared_ptr<MinusLink> MinusLinkPtr;
static inline MinusLinkPtr MinusLinkCast(const Handle& h)
   { AtomPtr a(h); return std::dynamic_pointer_cast<MinusLink>(a); }
static inline MinusLinkPtr MinusLinkCast(AtomPtr a)
   { return std::dynamic_pointer_cast<MinusLink>(a); }

#define createMinusLink std::make_shared<MinusLink>

/** @}*/
}

#endif // _OPENCOG_MINUS_LINK_H
