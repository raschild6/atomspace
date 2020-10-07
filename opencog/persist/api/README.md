Persist Guile API
-----------------

Provides a scheme module for the generic persistence API. This API
is compatible with anything that uses the generic "backend" API in
the atomspace. (So far, this is only the SQL backend).

To use:
```
(use-modules (opencog persist))
```
The provided functions are:

* `fetch-atom ATOM` --
      Get the current Values (including TruthValues) on `ATOM`.
* `fetch-value ATOM KEY` --
      Get the current Value on `ATOM` at `KEY`.
* `fetch-incoming-set ATOM` --
      Get all of the Atoms that contain `ATOM`.
* `fetch-incoming-by-type ATOM TYPE` --
      Get all Atoms of type TYPE that contain `ATOM`.
* `fetch-query QUERY KEY META FRESH` --
      Perform the `QUERY`, place results at `KEY` and metadata at `META`.
      An earlier cached query may be returned unless `FRESH` is true.
* `load-atoms-of-type TYPE` --
      Get all Atoms of type TYPE.
* `load-atomspace` --
      Get all Atoms (in the peristant store).
* `store-atom ATOM` --
      Put the `ATOM` and all attached Values into the persistant store.
* `store-value ATOM KEY` --
      Put the Value located at `KEY` on `ATOM` into the persistant store.
* `store-atomspace` --
      Put all of the Atoms into the persistant store.
* `barrier` --
      Complete any async, pending load/store operations before
      continuing with the next load/store operation.

Recall that you can always get more information and documentation with
the `,a` `,apropos` `,d` and `,describe` commands. For example, saying
`,a fetch` will list all commands with `fetch` in thier name, and 
`,d fetch-incoming-set` will print the full documentation.
