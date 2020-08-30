/*
 * opencog/persist/guile/PersistSCM.cc
 *
 * Copyright (c) 2008 by OpenCog Foundation
 * Copyright (c) 2008, 2009, 2013, 2015 Linas Vepstas <linasvepstas@gmail.com>
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

#ifndef _OPENCOG_PERSIST_SCM_H
#define _OPENCOG_PERSIST_SCM_H

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/atoms/base/Handle.h>
#include <opencog/guile/SchemeModule.h>

namespace opencog
{
/** \addtogroup grp_persist
 *  @{
 */

class PersistSCM : public ModuleWrap
{
private:
	void init(void);

	Handle fetch_atom(Handle);
	Handle fetch_value(Handle, Handle);
	Handle fetch_incoming_set(Handle);
	Handle fetch_incoming_by_type(Handle, Type);
	Handle fetch_query2(Handle, Handle);
	Handle fetch_query4(Handle, Handle, Handle, bool);
	Handle store_atom(Handle);
	void store_value(Handle, Handle);
	void load_type(Type);
	void load_atomspace(void);
	void store_atomspace(void);
	void barrier(void);

public:
	PersistSCM(void);
}; // class

/** @}*/
}  // namespace

extern "C" {
void opencog_persist_init(void);
};

#endif // _OPENCOG_PERSIST_SCM_H

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/guile/SchemePrimitive.h>
#include "StorageNode.h"

using namespace opencog;

// Single global default storage node ...
static StorageNode* sn = nullptr;

PersistSCM::PersistSCM(void)
	: ModuleWrap("opencog persist")
{
	static bool is_init = false;
	if (is_init) return;
	is_init = true;
	module_init();
}

void PersistSCM::init(void)
{
	define_scheme_primitive("fetch-atom",
	             &PersistSCM::fetch_atom, this, "persist");
	define_scheme_primitive("fetch-value",
	             &PersistSCM::fetch_value, this, "persist");
	define_scheme_primitive("fetch-incoming-set",
	             &PersistSCM::fetch_incoming_set, this, "persist");
	define_scheme_primitive("fetch-incoming-by-type",
	             &PersistSCM::fetch_incoming_by_type, this, "persist");
	define_scheme_primitive("fetch-query-2args",
	             &PersistSCM::fetch_query2, this, "persist");
	define_scheme_primitive("fetch-query-4args",
	             &PersistSCM::fetch_query4, this, "persist");
	define_scheme_primitive("store-atom",
	             &PersistSCM::store_atom, this, "persist");
	define_scheme_primitive("store-value",
	             &PersistSCM::store_value, this, "persist");
	define_scheme_primitive("load-atoms-of-type",
	             &PersistSCM::load_type, this, "persist");
	define_scheme_primitive("load-atomspace",
	             &PersistSCM::load_atomspace, this, "persist");
	define_scheme_primitive("store-atomspace",
	             &PersistSCM::store_atomspace, this, "persist");
	define_scheme_primitive("barrier",
	             &PersistSCM::barrier, this, "persist");
}

// =====================================================================

Handle PersistSCM::fetch_atom(Handle h)
{
	return sn->fetch_atom(h);
}

Handle PersistSCM::fetch_value(Handle h, Handle key)
{
	return sn->fetch_value(h, key);
}

Handle PersistSCM::fetch_incoming_set(Handle h)
{
	// The "false" flag here means that the fetch is NOT recursive.
	return sn->fetch_incoming_set(h, false);
}

Handle PersistSCM::fetch_incoming_by_type(Handle h, Type t)
{
	return sn->fetch_incoming_by_type(h, t);
}

Handle PersistSCM::fetch_query2(Handle query, Handle key)
{
	return sn->fetch_query(query, key, Handle::UNDEFINED, false);
}

Handle PersistSCM::fetch_query4(Handle query, Handle key,
                                Handle meta, bool fresh)
{
	return sn->fetch_query(query, key, meta, fresh);
}

/**
 * Store the single atom to the backing store hanging off the
 * atom-space.
 */
Handle PersistSCM::store_atom(Handle h)
{
	sn->store_atom(h);
	return h;
}

void PersistSCM::store_value(Handle h, Handle key)
{
	sn->store_value(h, key);
}

void PersistSCM::load_type(Type t)
{
	sn->fetch_all_atoms_of_type(t);
}

void PersistSCM::load_atomspace(void)
{
	sn->load_atomspace();
}

void PersistSCM::store_atomspace(void)
{
	sn->store_atomspace();
}

void PersistSCM::barrier(void)
{
	sn->barrier();
}

void opencog_persist_init(void)
{
	static PersistSCM patty;
}
