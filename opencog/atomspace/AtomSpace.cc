/*
 * opencog/atomspace/AtomSpace.cc
 *
 * Copyright (c) 2008-2010 OpenCog Foundation
 * Copyright (c) 2009, 2013 Linas Vepstas
 * All Rights Reserved
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

#include <string>
#include <iostream>
#include <fstream>
#include <list>

#include <stdlib.h>

#include <opencog/atoms/atom_types/types.h>
#include <opencog/atoms/atom_types/NameServer.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/atoms/base/Node.h>
#include <opencog/atoms/value/LinkValue.h>
#include <opencog/atoms/value/ValueFactory.h>

#include "AtomSpace.h"

using namespace opencog;

// ====================================================================

/**
 * Transient atomspaces skip some of the initialization steps,
 * so that they can be constructed more quickly.  Transient atomspaces
 * are typically used as scratch spaces, to hold temporary results
 * during evaluation, pattern matching and inference. Such temporary
 * spaces don't need some of the heavier-weight crud that atomspaces
 * are festooned with.
 */
AtomSpace::AtomSpace(AtomSpace* parent, bool transient) :
    _atom_table(parent? &parent->_atom_table : nullptr, this, transient),
    _read_only(false),
    _copy_on_write(transient)
{
}

AtomSpace::~AtomSpace()
{
}

void AtomSpace::ready_transient(AtomSpace* parent)
{
    _copy_on_write = true;
    _atom_table.ready_transient(parent? &parent->_atom_table : nullptr, this);
}

void AtomSpace::clear_transient()
{
    _atom_table.clear_transient();
}

// An extremely primitive permissions system.
void AtomSpace::set_read_only(void)
{
    _read_only = true;
}

void AtomSpace::set_read_write(void)
{
    _read_only = false;
}

bool AtomSpace::compare_atomspaces(const AtomSpace& space_first,
                                   const AtomSpace& space_second,
                                   bool check_values,
                                   bool emit_diagnostics)
{
    // Compare sizes
    if (space_first.get_size() != space_second.get_size())
    {
        if (emit_diagnostics)
            std::cout << "compare_atomspaces - size " <<
                    space_first.get_size() << " != size " <<
                    space_second.get_size() << std::endl;
        return false;
    }

    // Compare node count
    if (space_first.get_num_nodes() != space_second.get_num_nodes())
    {
        if (emit_diagnostics)
            std::cout << "compare_atomspaces - node count " <<
                    space_first.get_num_nodes() << " != node count " <<
                    space_second.get_num_nodes() << std::endl;
        return false;
    }

    // Compare link count
    if (space_first.get_num_links() != space_second.get_num_links())
    {
        if (emit_diagnostics)
            std::cout << "compare_atomspaces - link count " <<
                    space_first.get_num_links() << " != link count " <<
                    space_second.get_num_links() << std::endl;
        return false;
    }

    // If we get this far, we need to compare each individual atom.

    // Get the atoms in each atomspace.
    HandleSet atomsInFirstSpace, atomsInSecondSpace;
    space_first.get_handleset_by_type(atomsInFirstSpace, ATOM, true);
    space_second.get_handleset_by_type(atomsInSecondSpace, ATOM, true);

    // Uncheck each atom in the second atomspace.
    for (auto atom : atomsInSecondSpace)
        atom->setUnchecked();

    // Loop to see if each atom in the first has a match in the second.
    const AtomTable& table_second = space_second._atom_table;
    for (auto atom_first : atomsInFirstSpace)
    {
        Handle atom_second = table_second.getHandle(atom_first);

        if( false)
        {
        Handle atom_second;
        if (atom_first->is_node())
        {
            atom_second = table_second.getHandle(atom_first->get_type(),
                        std::string(atom_first->get_name()));
        }
        else if (atom_first->is_link())
        {
            atom_second =  table_second.getHandle(atom_first->get_type(),
                        HandleSeq(atom_first->getOutgoingSet()));
        }
        else
        {
             throw opencog::RuntimeException(TRACE_INFO,
                 "AtomSpace::compare_atomspaces - atom not Node or Link");
        }
        }

        // If the atoms don't match because one of them is null.
        if ((atom_first and not atom_second) or
            (atom_second and not atom_first))
        {
            if (emit_diagnostics)
            {
                if (atom_first)
                    std::cout << "compare_atomspaces - first atom " <<
                            atom_first->to_string() << " != NULL " <<
                            std::endl;
                if (atom_second)
                    std::cout << "compare_atomspaces - first atom " <<
                            "NULL != second atom " <<
                            atom_second->to_string() << std::endl;
            }
            return false;
        }

        // If the atoms don't match... Compare the atoms not the pointers
        // which is the default if we just use Handle operator ==.
        if (*((AtomPtr) atom_first) != *((AtomPtr) atom_second))
        {
            if (emit_diagnostics)
                std::cout << "compare_atomspaces - first atom " <<
                        atom_first->to_string() << " != second atom " <<
                        atom_second->to_string() << std::endl;
            return false;
        }

        // Check the values...
        // TODO: this should probably be moved to a method on class Atom.
        if (check_values)
        {
            HandleSet keys_first = atom_first->getKeys();
            HandleSet keys_second = atom_second->getKeys();
            if (keys_first.size() != keys_second.size())
            {
                if (emit_diagnostics)
                    std::cout << "compare_atomspaces - first keys size "
                              << keys_first.size() << " != second keys size "
                              << keys_second.size() << " for "
                              << atom_first->to_short_string() << std::endl;
                return false;
            }

            if (keys_first != keys_second)
            {
                if (emit_diagnostics)
                    std::cout << "compare_atomspaces - key set mistmatch for "
                              << atom_first->to_short_string() << std::endl;
                return false;
            }

            for (const Handle& key: keys_first)
            {
                ValuePtr value_first = atom_first->getValue(key);
                ValuePtr value_second = atom_second->getValue(key);
                if (*value_first != *value_second)
                {
                    if (emit_diagnostics)
                        std::cout << "compare_atomspaces - first value "
                            << value_first->to_string() << " != second value "
                            << value_second->to_string() << " for "
                            << atom_first->to_short_string() << std::endl;
                    return false;
                }
            }
        }

        // Set the check for the second atom.
        atom_second->setChecked();
    }

    // Make sure each atom in the second atomspace has been checked.
    bool all_checked = true;
    for (auto atom : atomsInSecondSpace)
    {
        if (!atom->isChecked())
        {
            if (emit_diagnostics)
                std::cout << "compare_atomspaces - unchecked space atom " <<
                        atom->to_string() << std::endl;
            all_checked = false;
        }
    }
    if (!all_checked)
        return false;

    // If we get this far, then the spaces are equal.
    return true;
}

bool AtomSpace::operator==(const AtomSpace& other) const
{
    return compare_atomspaces(*this, other, CHECK_VALUES,
            DONT_EMIT_DIAGNOSTICS);
}

bool AtomSpace::operator!=(const AtomSpace& other) const
{
    return not operator==(other);
}


// ====================================================================

Handle AtomSpace::add_atom(const Handle& h)
{
    // Cannot add atoms to a read-only atomspace. But if it's already
    // in the atomspace, return it.
    if (_read_only) return _atom_table.getHandle(h);

    // If it is a DeleteLink, then the addition will fail. Deal with it.
    Handle rh;
    try {
        rh = _atom_table.add(h);
    }
    catch (const DeleteException& ex) {
        // Hmmm. Need to notify the backing store
        // about the deleted atom. But how?
    }
    return rh;
}

Handle AtomSpace::add_node(Type t, std::string&& name)
{
    // Cannot add atoms to a read-only atomspace. But if it's already
    // in the atomspace, return it.
    if (_read_only) return _atom_table.getHandle(t, std::move(name));

    return _atom_table.add(createNode(t, std::move(name)));
}

Handle AtomSpace::get_node(Type t, std::string&& name) const
{
    return _atom_table.getHandle(t, std::move(name));
}

Handle AtomSpace::add_link(Type t, HandleSeq&& outgoing)
{
    // Cannot add atoms to a read-only atomspace. But if it's already
    // in the atomspace, return it.
    if (_read_only) return _atom_table.getHandle(t, std::move(outgoing));

    // If it is a DeleteLink, then the addition will fail. Deal with it.
    Handle h(createLink(std::move(outgoing), t));
    try {
        return _atom_table.add(h);
    }
    catch (const DeleteException& ex) {
        // Hmmm. Need to notify the backing store
        // about the deleted atom. But how?
    }
    return Handle::UNDEFINED;
}

Handle AtomSpace::get_link(Type t, HandleSeq&& outgoing) const
{
    return _atom_table.getHandle(t, std::move(outgoing));
}

ValuePtr AtomSpace::add_atoms(const ValuePtr& vptr)
{
    Type t = vptr->get_type();
    if (nameserver().isA(t, ATOM))
    {
        Handle h = add_atom(HandleCast(vptr));
        if (h) return h; // Might be null if AtomSpace is read-only.
        return vptr;
    }

    if (nameserver().isA(t, LINK_VALUE))
    {
        std::vector<ValuePtr> vvec;
        for (const ValuePtr& v : LinkValueCast(vptr)->value())
           vvec.push_back(add_atoms(v));

        return valueserver().create(t, vvec);
    }
    return vptr;
}

// Copy-on-write for setting values.
Handle AtomSpace::set_value(const Handle& h,
                            const Handle& key,
                            const ValuePtr& value)
{
    AtomSpace* has = h->getAtomSpace();

    // Hmm. It's kind-of a user-error, if they give us a naked atom.
    // We could throw here, and force them to fix thier code, or we
    // can silently do what they wanted!? Which will probably expose
    // other hard-to-debug bugs in the user's code ...
    // if (nullptr == has)
    //     throw opencog::RuntimeException(TRACE_INFO,
    //            "Your atom is needs to be placed in an atomspace!")

    // If the atom is in a read-only atomspace (i.e. if the parent
    // is read-only) and this atomspace is read-write, then make
    // a copy of the atom, and then set the value.
    // If this is a COW space, then always copy, no matter what.
    if (nullptr == has or has->_read_only or _copy_on_write) {
        if (has != this and (_copy_on_write or not _read_only)) {
            // Copy the atom into this atomspace
            Handle copy(_atom_table.add(h, true));
            copy->setValue(key, value);
            return copy;
        }

        // No copy needed. Safe to just update.
        if (has == this and not _read_only) {
            h->setValue(key, value);
            return h;
        }
    } else {
        h->setValue(key, value);
        return h;
    }
    throw opencog::RuntimeException(TRACE_INFO,
         "Value not changed; AtomSpace is readonly");
    return Handle::UNDEFINED;
}

// Copy-on-write for setting truth values.
Handle AtomSpace::set_truthvalue(const Handle& h, const TruthValuePtr& tvp)
{
    AtomSpace* has = h->getAtomSpace();
    // Hmm. It's kind-of a user-error, if they give us a naked atom.
    // We could throw here, and force them to fix thier code, or we
    // can silently do what they wanted!? Which will probably expose
    // other hard-to-debug bugs in the user's code ...
    // if (nullptr == has)
    //     throw opencog::RuntimeException(TRACE_INFO,
    //            "Your atom is needs to be placed in an atomspace!")

    // If the atom is in a read-only atomspace (i.e. if the parent
    // is read-only) and this atomspace is read-write, then make
    // a copy of the atom, and then set the value.
    // If this is a COW space, then always copy, no matter what.
    if (nullptr == has or has->_read_only or _copy_on_write) {
        if (has != this and (_copy_on_write or not _read_only)) {
            // Copy the atom into this atomspace
            Handle copy(_atom_table.add(h, true));
            copy->setTruthValue(tvp);
            return copy;
        }

        // No copy needed. Safe to just update.
        if (has == this and not _read_only) {
            h->setTruthValue(tvp);
            return h;
        }
    } else {
        h->setTruthValue(tvp);
        return h;
    }
    throw opencog::RuntimeException(TRACE_INFO,
         "TruthValue not changed; AtomSpace is readonly");
    return Handle::UNDEFINED;
}

std::string AtomSpace::to_string() const
{
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

namespace std {

ostream& operator<<(ostream& out, const opencog::AtomSpace& as) {
    list<opencog::Handle> results;
    as.get_handles_by_type(back_inserter(results), opencog::ATOM, true);
    for (const opencog::Handle& h : results)
	    if (h->getIncomingSetSize() == 0)
		    out << h->to_string() << std::endl;
    return out;
}

} // namespace std
