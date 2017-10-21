===========================
Database Primitives Library
===========================

.. image:: https://travis-ci.org/klmitch/dbprim.svg?branch=modernize
    :target: https://travis-ci.org/klmitch/dbprim

Many programs require some basic in-memory data structures.  In many
languages, the language or standard library provide these basic data
structures, but C does not have this.  This library fills the gap by
providing these database primitives: linked lists, hash tables,
red-black trees, and sparse matrices.

Installing the Library
======================

The Database Primitives Library uses GNU Autotools for building.  The
library can be configured for your system using ``./configure``, and
subsequently built using ``make``.  Use ``make install`` to install
the library into the configured installation location.  For more
details on installation, including how to configure the installation
location, see the text file ``INSTALL`` located in this directory.

Some additional ``Makefile`` targets exist as well; for instance, to
use `Doxygen <http://www.stack.nl/~dimitri/doxygen/>`_ to build the
documentation for the library, use ``make doxygen-doc`` (the `Dot
<http://www.graphviz.org/>`_ tool must also be installed).  A test
suite may also be built and executed using ``make check``.

Data Structure Overview
=======================

The following subsections provide an overview of the available data
structures and how to use them.  For more details, see the
documentation or the ``dbprim.h`` header file (``include/dbprim.h`` in
the source distribution, or installed in ``dbprim/dbprim.h`` in the
selected include path).

Linked Lists
------------

Linked lists are a very basic data structure used in building
databases.  This library provides a simple yet powerful implementation
of generic linked lists, based on two caller-allocated structures.
The ``link_head_t`` structure describes the head of a linked list and
contains information regarding the number of elements in the linked
list as well as pointers referencing the first and last elements in
the list.  The ``link_elem_t`` structure describes a specific element
in the linked list and contains pointers referencing the next and
previous elements in the list, as well as a pointer to the object, a
pointer to the head of the linked list, and a set of user-specified
flags.

Elements may be added at any arbitrary location in the linked list
with ``ll_add()``; moved to any other arbitrary location in the linked
list with ``ll_move()``; or removed from the list with
``ll_remove()``.  In addition, the user may search the list using a
user-defined comparison function with ``ll_find()``; iterate over
every element in the list with ``ll_iter()``; or remove all items from
the list with ``ll_flush()``, optionally executing a user-specified
clean-up function.

Hash Tables
-----------

Hash tables are a basic data structure used in building databases.
Hash tables provide a means of storing data such that an arbitrary
entry may be looked up efficiently.  This library implements a hash
table that may optionally grow and shrink to provide maximum
efficiency.  The implementation is with two kinds of caller-allocated
structures--a ``hash_table_t`` structure that describes the table and
a ``hash_entry_t`` structure for each entry in the table.  The library
allocates a bucket array which must be released with the ``ht_free()``
function when the hash table has been emptied.  Additionally, the hash
table may be manually resized with the ``ht_resize()`` function.

Entries may be added to and removed from the table using the
``ht_add()`` and ``ht_remove()`` functions.  Additionally, the key on
a given entry may be changed using the ``ht_move()`` function.  Of
course, any given entry may be looked up using the ``ht_find()``
function, and ``ht_iter()`` will execute a user-defined function for
each entry in the hash table (in an unspecified order).  The
``ht_flush()`` function will remove all entries from the hash table,
optionally executing a user-specified clean-up function.

Sparse Matrices
---------------

Sparse matrices are advanced data structures used to represent
associations.  For instance, a manager may have several employees, but
several of those employees may report to more than one manager.  (Yes,
this is a contrived example, so sue me.)  The simplest way to
represent such assocations is with a matrix, or a two-dimensional
array.  However, such an implementation cannot easily be extended
dynamically--imagine if a manager retires and two more are hired, for
instance.  It would also use an enormous amount of memory, as most
employees would only report to one or two managers.

A sparse matrix solves this problem by only allocating memory for the
cells in the full matrix which are actually used.  That is, no memory
is allocated to represent Alice reporting to Bob unless Alice actually
does report to Bob.  This is a simple concept, but fairly difficult to
implement efficiently--how do you tell if Alice reports to Bob?  The
solution utilized by this library is to combine the strengths of
linked lists and hash tables.  Each cell is in two linked lists,
rooted at the rows and columns of the matrix, but a hash table is used
when attempting to look up a given cell.  If the cell is allocated,
then there will be an entry in the hash table, and finding the given
cell is as fast as a hash table look-up.

Because sparse matrices are so complicated, there are three structures
and a variety of operations used.  Two of the structures,
``smat_table_t`` and ``smat_head_t``, are caller-allocated.  However,
the third structure, ``smat_entry_t``, must be allocated by the
library.  To avoid too much overhead from ``malloc()``, a free list is
used.  The free list may be managed with the ``smat_cleanup()`` and
``smat_freemem()`` calls.

The ``smat_table_t`` contains the hash table.  Only one of these need
be allocated per type of association--for instance, in the above
example, only one ``smat_table_t`` needs to be allocated to represent
the manager-employee relationship.

The ``smat_head_t`` contains the linked list.  There are actually two
kinds of these structures--one is ``SMAT_LOC_FIRST``, which could be
regarded as a ``row,'' and the other is ``SMAT_LOC_SECOND``, which
could be regarded as a ``column.''  Which one is used for which type
of data is irrelevant, as long as consistency is maintained.  For the
above example, a ``smat_head_t`` for a manager may be
``SMAT_LOC_FIRST``, and one for an employee must then be
``SMAT_LOC_SECOND``.  (These values are set when initializing the
``smat_head_t`` structure.)

An association may be created with the ``st_add()`` function, which
allows an arbitrary ordering in the associated linked lists by the
same mechanism as for the linked list component of the library.  An
association may be removed with ``st_remove()``, or looked up with
``st_find()``.  If iteration over all associations is desired, use the
``st_iter()`` function.  Removing all associations from a table may be
performed with ``st_flush()``, which optionally calls a user-defined
clean-up function.  The associated hash table may be resized with
``st_resize()``, and the bucket table may be released with
``st_free()``.

An association may also be reordered within the linked lists using the
``sh_move()`` function.  If a particular entry is desired, use the
``sh_find()`` function with a user-defined comparison function to
locate it.  Iteration may be performed with the ``sh_iter()``
function, and all entries in a given linked list may be removed with
the ``sh_flush()`` function, which again may optionally call a
user-defined clean-up function.

Red-black Trees
---------------

Red-black trees are a form of binary search tree.  One essential
feature of binary search trees is that they need to be balanced in
order to be efficient.  Many algorithms exist for keeping trees
balanced, but among the easiest to implement is the red-black tree.
In a red-black tree, every node is given a color--either red or
black--and there are various rules for what color nodes can be present
where in the tree.  This library implements these rules, along with
functions for traversing the tree in any desired tree order.

A red-black tree is represented by a caller-allocated ``rb_tree_t``
structure.  This structure describes various characteristics of the
tree, such as the number of nodes in the tree, and includes a pointer
to the root node of the tree.  Nodes may be added to the tree using
``rt_add()`` or removed from the tree using ``rt_remove()``.
Additionally, the key on a given node may be changed using the
``rt_move()`` function.  Nodes may be looked up with ``rt_find()``,
and ``rt_iter()`` will execute a user-defined function for each node
in the tree in the specified order.  To remove all entries in the
tree, simply call the ``rt_flush()`` function.  If you must manually
iterate through the tree, you may call the ``rt_next()`` and
``rt_prev()`` functions to determine the next or previous nodes to
visit.

There are three ways to traverse a binary tree.  The first, known as
"preorder," visits the root node, then traverses the left subtree in
preorder, then traverses the right subtree in preorder.  The second,
known an "inorder," traverses the left subtree in inorder, then the
root node, then the right subtree in inorder.  (This particular
ordering retrieves the nodes in lexical order; thus its name.)  The
third ordering is known as "postorder"; this ordering traverses the
left subtree, the right subtree, then visits the root node.  To
iterate over the tree in one of these orderings, simply call
``rt_iter()`` (or ``rt_next()`` or ``rt_prev()``) with the
``RBT_ORDER_PRE``, ``RBT_ORDER_IN``, or ``RBT_ORDER_POST`` flags.  You
may OR these flags with ``DB_FLAG_REVERSE`` to reverse the traversal
ordering, if you wish.
