/*
** Copyright (C) 2017 by Kevin L. Mitchell <klmitch@mit.edu>
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the Free
** Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
** MA 02111-1307, USA
*/
#ifndef included_dbprim_redblack_h__
#define included_dbprim_redblack_h__

/** \file
 * \brief Red-black trees.
 *
 * This header file contains the definitions for the red-black tree
 * component of the Database Primitives library.
 */

/** \defgroup dbprim_rbtree Red-black trees
 * \brief Operations for red-black trees.
 *
 * Red-black trees are a form of binary search tree.  One essential
 * feature of binary search trees is that they need to be balanced in
 * order to be efficient.  Many algorithms exist for keeping trees
 * balanced, but among the easiest to implement is the red-black
 * tree.  In a red-black tree, every node is given a color--either red
 * or black--and there are various rules for what color nodes can be
 * present where in the tree.  This library implements these rules,
 * along with functions for traversing the tree in any desired tree
 * order.
 *
 * A red-black tree is represented by a caller-allocated #rb_tree_t
 * structure.  This structure describes various characteristics of the
 * tree, such as the number of nodes in the tree, and includes a
 * pointer to the root node of the tree.  Nodes may be added to the
 * tree using rt_add() or removed from the tree using rt_remove().
 * Additionally, the key on a given node may be changed using the
 * rt_move() function.  Nodes may be looked up with rt_find(), and
 * rt_iter() will execute a user-defined function for each node in the
 * tree in the specified order.  To remove all entries in the tree,
 * simply call the rt_flush() function.  If you must manually iterate
 * through the tree, you may call the rt_next() and rt_prev()
 * functions to determine the next or previous nodes to visit.
 *
 * There are three ways to traverse a binary tree.  The first, known
 * as "preorder," visits the root node, then traverses the left
 * subtree in preorder, then traverses the right subtree in preorder.
 * The second, known an "inorder," traverses the left subtree in
 * inorder, then the root node, then the right subtree in inorder.
 * (This particular ordering retrieves the nodes in lexical order;
 * thus its name.)  The third ordering is known as "postorder"; this
 * ordering traverses the left subtree, the right subtree, then visits
 * the root node.  To iterate over the tree in one of these orderings,
 * simply call rt_iter() (or rt_next() or rt_prev()) with the
 * #RBT_ORDER_PRE, #RBT_ORDER_IN, or #RBT_ORDER_POST flags.  You may
 * OR these flags with #DB_FLAG_REVERSE to reverse the traversal
 * ordering, if you wish.
 */

#ifndef __DBPRIM_LIBRARY__
#include <dbprim/common.h>
#else
#include "common.h"
#endif /* __DBPRIM_LIBRARY__ */

DBPRIM_BEGIN_C_DECLS

/** \ingroup dbprim_rbtree
 * \brief Red-black tree.
 *
 * This structure is the basis of all red-black trees maintained by
 * this library.
 */
typedef struct _rb_tree_s rb_tree_t;

/** \ingroup dbprim_rbtree
 * \brief Red-black tree node.
 *
 * This structure represents a single node in a red-black tree.
 */
typedef struct _rb_node_s rb_node_t;

/** \ingroup dbprim_rbtree
 * \brief Red-black tree iteration callback.
 *
 * This function pointer references a callback used by rb_iter() and
 * rb_flush().  It should return 0 for success.  A non-zero return
 * value will terminate the operation and will become the return
 * value of the call.
 *
 * \param[in]		tree	A pointer to a #rb_tree_t.
 * \param[in]		node	A pointer to the #rb_node_t being
 *				considered.
 * \param[in]		extra	A \c void pointer passed to rt_iter()
 *				or rt_flush().
 *
 * \return	Zero for success, or non-zero to terminate the
 *		iteration.
 */
typedef db_err_t (*rb_iter_t)(rb_tree_t *tree, rb_node_t *node, void *extra);

/** \ingroup dbprim_rbtree
 * \brief Red-black tree comparison callback.
 *
 * This function pointer references a callback used to compare nodes
 * in a red-black tree.  It should return 0 for identical entries,
 * less than 0 if the first key is less than the second, or greater
 * than 0 if the first key is greater than the second.
 *
 * \param[in]		tree	A pointer to a #rb_tree_t.
 * \param[in]		key1	The first key being compared.
 * \param[in]		key2	The second key being compared.
 *
 * \return	Zero if the keys match, less than zero if the first
 *		key orders before the second key, or greater than zero
 *		if the first key orders after the second key.
 */
typedef int (*rb_comp_t)(rb_tree_t *tree, db_key_t *key1, db_key_t *key2);

/** \ingroup dbprim_rbtree
 * \brief Red-black tree node color.
 *
 * This enumeration is used to specify the color of a node of a
 * red-black tree.
 */
enum _rb_color_e {
  RB_COLOR_NONE,	/**< Node is uncolored as of yet. */
  RB_COLOR_RED,		/**< Node is red. */
  RB_COLOR_BLACK	/**< Node is black. */
};

/** \ingroup dbprim_rbtree
 * \brief Red-black tree node color.
 *
 * See the documentation for the enumeration #_rb_color_e.
 */
typedef enum _rb_color_e rb_color_t;

/** \ingroup dbprim_rbtree
 * \brief Red-black tree comparison function.
 *
 * This is a red-black tree comparison function, compatible with
 * #rb_comp_t, based around memcmp().
 *
 * \param[in]		tree	A pointer to a #rb_tree_t.
 * \param[in]		key1	The first key being compared.
 * \param[in]		key2	The second key being compared.
 *
 * \return	Zero if the keys match, less than zero if the first
 *		key orders before the second key, or greater than zero
 *		if the first key orders after the second key.
 */
int rbtree_comp(rb_tree_t *tree, db_key_t *key1, db_key_t *key2);

/** \internal
 * \ingroup dbprim_rbtree
 * \brief Red-black tree structure.
 *
 * This is the implementation of the #rb_tree_t type.
 */
struct _rb_tree_s {
  _db_magic_t	rt_magic;	/**< Magic number. */
  db_flag_t	rt_flags;	/**< Flags associated with the table. */
  db_count_t	rt_count;	/**< Number of nodes in the tree. */
  rb_node_t    *rt_root;	/**< Pointer to the root node of the tree. */
  rb_comp_t	rt_comp;	/**< Function for comparing tree keys. */
  void	       *rt_extra;	/**< Extra data associated with the tree. */
};

/** \internal
 * \ingroup dbprim_rbtree
 * \brief Red-black tree magic number.
 *
 * This is the magic number used for the red-black tree structure.
 */
#define RB_TREE_MAGIC 0xd52695be

/** \internal
 * \ingroup dbprim_rbtree
 * \brief Flag indicating red-black tree is frozen.
 *
 * This flag, if set on a red-black tree, indicates that the tree is
 * frozen and may not be modified.
 */
#define RBT_FLAG_FREEZE 0x80000000 /* tree frozen */

/** \ingroup dbprim_rbtree
 * \brief Red-black tree static initializer.
 *
 * This macro statically initializes a #rb_tree_t.
 *
 * \param[in]		comp	A #rb_comp_t function pointer for a
 *				comparison function.
 * \param[in]		extra	Extra pointer data that should be
 *				associated with the red-black tree.
 */
#define RB_TREE_INIT(comp, extra)   { RB_TREE_MAGIC, 0, 0, 0, (comp), (extra) }

/** \ingroup dbprim_rbtree
 * \brief Red-black tree verification macro.
 *
 * This macro verifies that a given pointer actually does point to a
 * red-black tree.
 *
 * \warning	This macro evaluates the \p tree argument twice.
 *
 * \param[in]		tree	A pointer to a #rb_tree_t.
 *
 * \return	Boolean \c true if \p tree is a valid red-black tree
 *		or \c false otherwise.
 */
#define rt_verify(tree)		((tree) && \
				 (tree)->rt_magic == RB_TREE_MAGIC)

/** \ingroup dbprim_rbtree
 * \brief Determine if a red-black tree is frozen.
 *
 * This macro returns a non-zero value if the tree is currently
 * frozen.  The red-black tree may be frozen if there is an
 * iteration in progress.
 *
 * \param[in]		tree	A pointer to a #rb_tree_t.
 *
 * \return	A zero value if the table is not frozen or a non-zero
 *		value if the table is frozen.
 */
#define rt_frozen(tree)		((tree)->rt_flags & RBT_FLAG_FREEZE)

/** \ingroup dbprim_rbtree
 * \brief Red-black tree count.
 *
 * This macro retrieves the total number of items actually in the
 * red-black tree.
 *
 * \param[in]		tree	A pointer to a #rb_tree_t.
 *
 * \return	A #db_count_t containing a count of the number of
 *		items in the red-black tree.
 */
#define rt_count(tree)		((tree)->rt_count)

/** \ingroup dbprim_rbtree
 * \brief Red-black tree root node.
 *
 * This macro retrieves the root node of the tree.
 *
 * \param[in]		tree	A pointer to a #rb_tree_t.
 *
 * \return	A pointer to a #rb_node_t.
 */
#define rt_root(tree)		((tree)->rt_root)

/** \ingroup dbprim_rbtree
 * \brief Red-black tree comparison function.
 *
 * This macro retrieves the comparison function pointer.
 *
 * \param[in]		tree	A pointer to a #rb_tree_t.
 *
 * \return	A #rb_comp_t.
 */
#define rt_comp(tree)		((tree)->rt_comp)

/** \ingroup dbprim_rbtree
 * \brief Extra pointer data in a red-black tree.
 *
 * This macro retrieves the extra pointer data associated with a
 * particular red-black tree.
 *
 * \param[in]		tree	A pointer to a #rb_tree_t.
 *
 * \return	A pointer to \c void.
 */
#define rt_extra(tree)		((tree)->rt_extra)

/** \ingroup dbprim_rbtree
 * \brief Preorder tree traversal method.
 *
 * If this flag is passed to rt_iter(), a preorder iteration will
 * be performed.
 */
#define RBT_ORDER_PRE	1

/** \ingroup dbprim_rbtree
 * \brief Inorder tree traversal method.
 *
 * If this flag is passed to rt_iter(), an inorder iteration will
 * be performed.
 */
#define RBT_ORDER_IN	2

/** \ingroup dbprim_rbtree
 * \brief Postorder tree traversal method.
 *
 * If this flag is passed to rt_iter(), a postorder iteration will
 * be performed.
 */
#define RBT_ORDER_POST	3

/** \ingroup dbprim_rbtree
 * \brief Tree traversal method mask.
 *
 * This flag mask may be used to obtain the tree traversal order.
 */
#define RBT_ORDER_MASK	0x00000003

/** \ingroup dbprim_rbtree
 * \brief Get the previous node.
 *
 * Obtains the previous node in the given iteration scheme.  See
 * rt_next() for more information.
 *
 * \param[in]		tree	A pointer to a #rb_tree_t.
 * \param[in,out]	node_io	A pointer to a pointer to a
 *				#rb_node_t.  If the pointer to which
 *				node_io points is \c NULL, the first
 *				node will be loaded, otherwise the
 *				next node will be loaded.
 * \param[in]		flags	One of RBT_ORDER_PRE, RBT_ORDER_IN, or
 *				RBT_ORDER_POST, possibly ORed with
 *				DB_FLAG_REVERSE to reverse the order
 *				of iteration.  Zero is not allowed.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_WRONGTABLE	\p start is not in this red-black
 *				tree.
 */
#define rt_prev(tree, node_io, flags) \
	(rt_next((tree), (node_io), (flags) ^ DB_FLAG_REVERSE))

/** \ingroup dbprim_rbtree
 * \brief Dynamically initialize a red-black tree.
 *
 * This function dynamically initializes a red-black tree.
 *
 * \param[in]		tree	A pointer to a #rb_tree_t to be
 *				initialized.
 * \param[in]		comp	A #rb_comp_t function pointer for a
 *				comparison function.
 * \param[in]		extra	Extra pointer data that should be
 *				associated with the tree.
 *
 * \retval DB_ERR_BADARGS	An invalid argument was given.
 */
db_err_t rt_init(rb_tree_t *tree, rb_comp_t comp, void *extra);

/** \ingroup dbprim_rbtree
 * \brief Add a node to a red-black tree.
 *
 * This function adds a node to a red-black tree.
 *
 * \param[in]		tree	A pointer to a #rb_tree_t.
 * \param[in]		node	A pointer to a #rb_node_t to be added
 *				to the tree.
 * \param[in]		key	A pointer to a #db_key_t containing
 *				the key for the node.
 *
 * \retval DB_ERR_BADARGS	An invalid argument was given.
 * \retval DB_ERR_BUSY		The node is already in a tree.
 * \retval DB_ERR_FROZEN	The tree is currently frozen.
 * \retval DB_ERR_DUPLICATE	The entry is a duplicate of an
 *				existing node.
 */
db_err_t rt_add(rb_tree_t *tree, rb_node_t *node, db_key_t *key);

/** \ingroup dbprim_rbtree
 * \brief Move a node in a red-black tree.
 *
 * This function moves an existing node in the red-black tree to
 * correspond to the new key.
 *
 * \param[in]		tree	A pointer to a #rb_tree_t.
 * \param[in]		node	A pointer to a #rb_node_t to be moved.
 *				It must already be in the red-black
 *				tree.
 * \param[in]		key	A pointer to a #db_key_t describing
 *				the new key for the node.
 *
 * \retval DB_ERR_BADARGS	An invalid argument was given.
 * \retval DB_ERR_UNUSED	Node is not in a red-black tree.
 * \retval DB_ERR_WRONGTABLE	Node is not in this tree.
 * \retval DB_ERR_FROZEN	Red-black tree is frozen.
 * \retval DB_ERR_DUPLICATE	New key is a duplicate of an existing
 *				key.
 * \retval DB_ERR_READDFAILED	Unable to re-add node to tree.
 */
db_err_t rt_move(rb_tree_t *tree, rb_node_t *node, db_key_t *key);

/** \ingroup dbprim_rbtree
 * \brief Remove a node from a red-black tree.
 *
 * This function removes the given node from the specified red-black
 * tree.
 *
 * \param[in]		tree	A pointer to a #rb_tree_t.
 * \param[in]		node	A pointer to a #rb_node_t to be
 *				removed from the tree.
 *
 * \retval DB_ERR_BADARGS	An invalid argument was given.
 * \retval DB_ERR_UNUSED	Node is not in a red-black tree.
 * \retval DB_ERR_WRONGTABLE	Node is not in this tree.
 * \retval DB_ERR_FROZEN	Red-black tree is frozen.
 */
db_err_t rt_remove(rb_tree_t *tree, rb_node_t *node);

/** \ingroup dbprim_rbtree
 * \brief Find an entry in a red-black table.
 *
 * This function looks up an entry matching the given \p key.
 *
 * \param[in]		tree	A pointer to a #rb_tree_t.
 * \param[out]		node_p	A pointer to a pointer to a
 *				#rb_node_t.  If \c NULL is passed, the
 *				lookup will be performed and an
 *				appropriate error code returned.
 * \param[in]		key	A pointer to a #db_key_t describing
 *				the item to find.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_NOENTRY	No matching entry was found.
 */
db_err_t rt_find(rb_tree_t *tree, rb_node_t **node_p, db_key_t *key);

/** \ingroup dbprim_rbtree
 * \brief Get the next node.
 *
 * This function obtains the next node in the given iteration scheme.
 * The \p node_io parameter is a value-result parameter--if the node
 * pointer to which it points is \c NULL, the first node for the given
 * iteration order will be loaded; otherwise, the next node in the
 * given iteration order will be loaded.
 *
 * \param[in]		tree	A pointer to a #rb_tree_t.
 * \param[in,out]	node_io	A pointer to a pointer to a
 *				#rb_node_t.  If the pointer to which
 *				node_io points is \c NULL, the first
 *				node will be loaded, otherwise the
 *				next node will be loaded.
 * \param[in]		flags	One of RBT_ORDER_PRE, RBT_ORDER_IN, or
 *				RBT_ORDER_POST, possibly ORed with
 *				DB_FLAG_REVERSE to reverse the order
 *				of iteration.  Zero is not allowed.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_WRONGTABLE	\p start is not in this red-black
 *				tree.
 */
db_err_t rt_next(rb_tree_t *tree, rb_node_t **node_io, db_flag_t flags);

/** \ingroup dbprim_rbtree
 * \brief Iterate over each entry in a red-black tree.
 *
 * This function iterates over every node in a red-black tree in the
 * given traversal order, executing the given \p iter_func on each
 * node.
 *
 * \param[in]		tree	A pointer to a #rb_tree_t.
 * \param[in]		start	A pointer to a #rb_node_t describing
 *				where in the tree to start.  If \c
 *				NULL is passed, the first element of
 *				the tree for the specified order will
 *				be assumed.
 * \param[in]		iter_func
 *				A pointer to a callback function used
 *				to perform user-specified actions on a
 *				node in the red-black tree.  \c NULL
 *				is an invalid value.  See the
 *				documentation for #rb_iter_t for more
 *				information.
 * \param[in]		extra	A \c void pointer that will be passed
 *				to \p iter_func.
 * \param[in]		flags	One of RBT_ORDER_PRE, RBT_ORDER_IN, or
 *				RBT_ORDER_POST, possibly ORed with
 *				DB_FLAG_REVERSE to reverse the order
 *				of iteration.  Zero is not allowed.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_WRONGTABLE	\p start is not in this red-black
 *				tree.
 */
db_err_t rt_iter(rb_tree_t *tree, rb_node_t *start, rb_iter_t iter_func,
		 void *extra, db_flag_t flags);

/** \ingroup dbprim_rbtree
 * \brief Flush a red-black tree.
 *
 * This function flushes a red-black tree--that is, it removes each
 * node from the tree.  If a \p flush_func is specified, it will be
 * called on the node after it has been removed from the tree, and may
 * safely call <CODE>free()</CODE>.
 *
 * \param[in]		tree	A pointer to a #rb_tree_t.
 * \param[in]		flush_func
 *				A pointer to a callback function used
 *				to perform user-specified actions on a
 *				node after removing it from the tree.
 *				May be \c NULL.  See the documentation
 *				for #rb_iter_t for more information.
 * \param[in]		extra	A \c void pointer that will be passed
 *				to \p flush_func.
 *
 * \retval DB_ERR_BADARGS	An argument was invalid.
 * \retval DB_ERR_FROZEN	The red-black tree is frozen.
 */
db_err_t rt_flush(rb_tree_t *tree, rb_iter_t flush_func, void *extra);

/** \internal
 * \ingroup dbprim_rbtree
 * \brief Red-black tree node structure.
 *
 * This is the implementation of the #rb_node_t type.
 */
struct _rb_node_s {
  _db_magic_t	rn_magic;	/**< Magic number. */
  rb_color_t	rn_color;	/**< Node's color. */
  rb_tree_t    *rn_tree;	/**< Tree node is in. */
  rb_node_t    *rn_parent;	/**< Parent of this node. */
  rb_node_t    *rn_left;	/**< Left child of this node. */
  rb_node_t    *rn_right;	/**< Right child of this node. */
  db_key_t	rn_key;		/**< Node's key. */
  void	       *rn_value;	/**< Node's value. */
};

/** \internal
 * \ingroup dbprim_rbtree
 * \brief Red-black tree node magic number.
 *
 * This is the magic number used for the red-black tree structure.
 */
#define RB_NODE_MAGIC 0x3dea4d01

/** \ingroup dbprim_rbtree
 * \brief Red-black tree node static initializer.
 *
 * This macro statically initializes a #rb_node_t.
 *
 * \param[in]		value	A pointer to \c void representing the
 *				object associated with the node.
 */
#define RB_NODE_INIT(value) \
	{ RB_NODE_MAGIC, RB_COLOR_NONE, 0, 0, 0, 0, DB_KEY_INIT(0, 0), (value)}

/** \ingroup dbprim_rbtree
 * \brief Red-black tree node verification macro.
 *
 * This macro verifies that a given pointer actually does point to a
 * red-black tree node.
 *
 * \warning	This macro evaluates the \p node argument twice.
 *
 * \param[in]		node	A pointer to a #rb_node_t.
 *
 * \return	Boolean \c true if \p entry is a valid red-black tree
 *		node or \c false otherwise.
 */
#define rn_verify(node)		((node) && \
				 (node)->rn_magic == RB_NODE_MAGIC)

/** \ingroup dbprim_rbtree
 * \brief Red-black tree node color.
 *
 * This macro retrieves the color of the \p node.
 *
 * \param[in]		node	A pointer to a #rb_node_t.
 *
 * \return	A #rb_color_t value expressing the color of the
 *		\p node.
 */
#define rn_color(node)		((node)->rn_color)

/** \ingroup dbprim_rbtree
 * \brief Red-black tree node's tree pointer.
 *
 * This macro retrieves a pointer to the red-black tree the node
 * is in.
 *
 * \param[in]		node	A pointer to a #rb_node_t.
 *
 * \return	A pointer to a #rb_tree_t.
 */
#define rn_tree(node)		((node)->rn_tree)

/** \ingroup dbprim_rbtree
 * \brief Red-black tree node's parent pointer.
 *
 * This macro retrieves a pointer to the node's parent node.
 *
 * \param[in]		node	A pointer to a #rb_node_t.
 *
 * \return	A pointer to a #rb_node_t representing the parent
 *		of the given \p node.
 */
#define rn_parent(node)		((node)->rn_parent)

/** \ingroup dbprim_rbtree
 * \brief Red-black tree node's left pointer.
 *
 * This macro retrieves a pointer to the node's left node.
 *
 * \param[in]		node	A pointer to a #rb_node_t.
 *
 * \return	A pointer to a #rb_node_t representing the left
 *		node of the given \p node.
 */
#define rn_left(node)		((node)->rn_left)

/** \ingroup dbprim_rbtree
 * \brief Red-black tree node's right pointer.
 *
 * This macro retrieves a pointer to the node's right node.
 *
 * \param[in]		node	A pointer to a #rb_node_t.
 *
 * \return	A pointer to a #rb_node_t representing the right
 *		node of the given \p node.
 */
#define rn_right(node)		((node)->rn_right)

/** \ingroup dbprim_rbtree
 * \brief Red-black tree node's key pointer.
 *
 * This macro retrieves the key associated with the red-black tree
 * node.
 *
 * \param[in]		node	A pointer to a #rb_node_t.
 *
 * \return	A pointer to a #db_key_t.
 */
#define rn_key(node)		(&((node)->rn_key))

/** \ingroup dbprim_rbtree
 * \brief Red-black tree node's value pointer.
 *
 * This macro retrieves the value associated with the red-black
 * tree's node.  It may be treated as an lvalue to change that value.
 * Care should be taken when using this option.
 *
 * \param[in]		node	A pointer to a #rb_node_t.
 *
 * \return	A pointer to \c void representing the value associated
 *		with this node.
 */
#define rn_value(node)		((node)->rn_value)

/** \ingroup dbprim_rbtree
 * \brief Test if a given node is black.
 *
 * This macro safely tests whether a given red-black tree node is
 * black.
 *
 * \warning	This macro evaluates the \p node argument twice.
 *
 * \param[in]		node	A pointer to a #rb_node_t.
 *
 * \return	Boolean \c true if \p node is black or \c false
 *		otherwise.
 */
#define rn_isblack(node)	(!(node) || (node)->rn_color == RB_COLOR_BLACK)

/** \ingroup dbprim_rbtree
 * \brief Test if a given node is red.
 *
 * This macro safely tests whether a given red-black tree node is
 * red.
 *
 * \warning	This macro evaluates the \p node argument twice.
 *
 * \param[in]		node	A pointer to a #rb_node_t.
 *
 * \return	Boolean \c true if \p node is red or \c false
 *		otherwise.
 */
#define rn_isred(node)		((node) && (node)->rn_color == RB_COLOR_RED)

/** \ingroup dbprim_rbtree
 * \brief Test if a given node is the left node of its parent.
 *
 * This macro safely tests whether a given red-black tree node is
 * the left node of its parent.
 *
 * \warning	This macro evaluates the \p node argument three
 *		times.
 *
 * \param[in]		node	A pointer to a #rb_node_t.
 *
 * \return	Boolean \c true if \p node is the left node of its
 *		parent or \c false otherwise.
 */
#define rn_isleft(node)		((node)->rn_parent && \
				 (node)->rn_parent->rn_left == (node))

/** \ingroup dbprim_rbtree
 * \brief Test if a given node is the right node of its parent.
 *
 * This macro safely tests whether a given red-black tree node is
 * the right node of its parent.
 *
 * \warning	This macro evaluates the \p node argument three
 *		times.
 *
 * \param[in]		node	A pointer to a #rb_node_t.
 *
 * \return	Boolean \c true if \p node is the right node of its
 *		parent or \c false otherwise.
 */
#define rn_isright(node)	((node)->rn_parent && \
				 (node)->rn_parent->rn_right == (node))

/** \ingroup dbprim_rbtree
 * \brief Dynamically initialize a red-black tree node.
 *
 * This function dynamically initializes a red-black tree node.
 *
 * \param[in]		node	A pointer to a #rb_tree_t to be
 *				initialized.
 * \param[in]		value	A pointer to \c void which will be the
 *				value of the red-black tree entry.
 *
 * \retval DB_ERR_BADARGS	A \c NULL pointer was passed for \p
 *				node.
 */
db_err_t rn_init(rb_node_t *node, void *value);

DBPRIM_END_C_DECLS

#endif /* included_dbprim_redblack_h__ */
