#include <opencog/atomspace/AtomSpace.h>
#include <deque>

#ifndef _OPENCOG_CONTEXT_H
#define _OPENCOG_CONTEXT_H

namespace opencog {
    extern thread_local std::deque<AtomSpace*> current;

    AtomSpace * get_context_atomspace();
    void push_context_atomspace(AtomSpace *atomspace);
    AtomSpace * pop_context_atomspace();
    void clear_context();
}

#endif // _OPENCOG_CONTEXT_H
