/*
 * opencog/atoms/reduct/MinLink.h
 *
 * Copyright (C) 2020 Linas Vepstas
 * All Rights Reserved
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef _OPENCOG_MIN_LINK_H
#define _OPENCOG_MIN_LINK_H

#include <opencog/atoms/core/FunctionLink.h>

namespace opencog
{
/** \addtogroup grp_atomspace
 *  @{
 */

/**
 * The MinLink implements the arithmetic operation of "least of"
 * on a component-by-component level.
 */
class MinLink : public FunctionLink
{
protected:
	void init(void);

public:
	MinLink(const HandleSeq&&, Type=MIN_LINK);

	MinLink(const MinLink&) = delete;
	MinLink& operator=(const MinLink&) = delete;

	virtual ValuePtr execute(AtomSpace*, bool);

	static Handle factory(const Handle&);
};

typedef std::shared_ptr<MinLink> MinLinkPtr;
static inline MinLinkPtr MinLinkCast(const Handle& h)
   { AtomPtr a(h); return std::dynamic_pointer_cast<MinLink>(a); }
static inline MinLinkPtr MinLinkCast(AtomPtr a)
   { return std::dynamic_pointer_cast<MinLink>(a); }

#define createMinLink std::make_shared<MinLink>

/** @}*/
}

#endif // _OPENCOG_MIN_LINK_H
