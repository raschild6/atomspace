/*
 * opencog/atoms/reduct/AccumulateLink.cc
 *
 * Copyright (C) 2020 Linas Vepstas
 * All Rights Reserved
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <opencog/atoms/atom_types/atom_types.h>
#include <opencog/atoms/base/ClassServer.h>
#include <opencog/atoms/core/NumberNode.h>
#include "ArithmeticLink.h"
#include "AccumulateLink.h"

using namespace opencog;

AccumulateLink::AccumulateLink(const HandleSeq&& oset, Type t)
    : FunctionLink(std::move(oset), t)
{
	init();
}

AccumulateLink::AccumulateLink(const Handle& a)
    : FunctionLink({a}, ACCUMULATE_LINK)
{
	init();
}

AccumulateLink::AccumulateLink(const Handle& a, const Handle& b)
    : FunctionLink({a, b}, ACCUMULATE_LINK)
{
	init();
}

void AccumulateLink::init(void)
{
	Type tscope = get_type();
	if (not nameserver().isA(tscope, ACCUMULATE_LINK))
		throw InvalidParamException(TRACE_INFO, "Expecting a AccumulateLink");

	size_t nargs = _outgoing.size();
	if (1 != nargs)
		throw InvalidParamException(TRACE_INFO,
			"AccumulateLink expects one, got %s",
			to_string().c_str());
}

// ============================================================

ValuePtr AccumulateLink::execute(AtomSpace* as, bool silent)
{
	ValuePtr vi(ArithmeticLink::get_value(as, silent, _outgoing[0]));
	Type vitype = vi->get_type();

	if (NUMBER_NODE == vitype)
	{
		const std::vector<double>& dvec(NumberNodeCast(vi)->value());
		double acc = 0.0;
		for (double dv : dvec)
			acc += dv;
		return createNumberNode(acc);
	}

	if (nameserver().isA(vitype, FLOAT_VALUE))
	{
		const std::vector<double>& dvec(FloatValueCast(vi)->value());
		double acc = 0.0;
		for (double dv : dvec)
			acc += dv;
		return createFloatValue(acc);
	}

	// If it did not fully reduce, then return the best-possible
	// reduction that we did get.
	if (vi->is_atom())
		return createAccumulateLink(HandleCast(vi));

	// Unable to reduce at all. Just return the original atom.
	return get_handle();
}

DEFINE_LINK_FACTORY(AccumulateLink, ACCUMULATE_LINK);

// ============================================================
