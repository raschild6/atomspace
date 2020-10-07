//
// examples/c++/basic.cc
//
// A basic example of creating Atoms in an AtomSpace.

#include <opencog/atoms/atom_types/atom_names.h>
#include <opencog/atomspace/AtomSpace.h>

using namespace opencog;

int main()
{
	// Create a new AtomSpace.
	AtomSpace* as = new AtomSpace();

	// Create a ConceptNode Atom, place it in the AtomSpace.
	as->add_atom(Concept("foobar"));

	// Print the atomspace contents.
	// Note that the unique Atom ID's (64-bit hashes) are printed.
	printf("So far its this: %s\n", as->to_string().c_str());

	// Create an EvaluationLink Atom, place it in the AtomSpace.
	as->add_atom(
		Evaluation(
			Predicate("bling"),
			List(
				Concept("foo"),
				Concept("bar"))));

	printf("Now it is this: %s\n", as->to_string().c_str());
}
