
#include <opencog/util/oc_assert.h>
#include <opencog/atoms/core/FindUtils.h>
#include <opencog/atoms/pattern/BindLink.h>
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/query/Implicator.h>

using namespace opencog;

/**
 * Default evaluator of implication statements.  Does not consider
 * the truth value of any of the matched clauses; instead, looks
 * purely for a structural match.
 */
static inline Handle imply(AtomSpace* as, Handle hclauses, Handle himplicand)
{
	// Extract the variables; they were not specified.
	FindAtoms fv(VARIABLE_NODE);
	fv.search_set(hclauses);

	HandleSeq vars(fv.varset.begin(), fv.varset.end());

	// Stuff the variables into a proper variable list.
	Handle hvars(createLink(std::move(vars), VARIABLE_LIST));

	HandleSeq oset = {hvars, hclauses, himplicand};

	BindLinkPtr bl(createBindLink(std::move(oset)));

	// Now perform the search.
	Implicator impl(as);
	impl.implicand.push_back(himplicand);

	impl.satisfy(bl);

	// The result_set contains a list of the grounded expressions.
	// Turn it into a true list, and return it.
	HandleSeq hlist;
	QueueValuePtr qv(impl.get_result_queue());
	OC_ASSERT(qv->is_closed(), "Unexpected queue state!");
	std::queue<ValuePtr> vals(qv->wait_and_take_all());
	while (not vals.empty())
	{
		hlist.push_back(HandleCast(vals.front()));
		vals.pop();
	}
	Handle gl = as->add_link(LIST_LINK, std::move(hlist));
	return gl;
}

/**
 * Pattern Matcher. Just run the matcher against the indicated
 * variables and clauses, using the indicated callback.
 */
static inline void match(PatternMatchCallback& pmcb,
                         const HandleSet &vars,
                         const HandleSeq &clauses)
{
	PatternLinkPtr slp(createPatternLink(vars, clauses));
	pmcb.satisfy(slp);
}

static inline Handle bindlink(AtomSpace* as,
                          const Handle& hlink, size_t foo=0)
{
	return HandleCast(hlink->execute(as));
}

static inline Handle satisfying_set(AtomSpace* as,
                          const Handle& hlink, size_t foo=0)
{
	return HandleCast(hlink->execute(as));
}
