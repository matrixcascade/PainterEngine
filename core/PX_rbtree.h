
/*
  Red Black Trees
  (C) 1999  Andrea Arcangeli <andrea@suse.de>
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  linux/include/linux/rbtree.h

  To use rbtrees you'll have to implement your own insert and search cores.
  This will avoid us to use callbacks and to drop drammatically performances.
  I know it's not the cleaner way,  but in C (not in C++) to get
  performances and genericity...

  Some example of insert and search follows here. The search is a plain
  normal search over an ordered tree. The insert instead must be implemented
  in two steps: First, the code must insert the element in order as a red leaf
  in the tree, and then the support library function rb_insert_color() must
  be called. Such function will do the not trivial work to rebalance the
  rbtree, if necessary.

-----------------------------------------------------------------------
*/

#ifndef	_PX_RBTREE_H
#define	_PX_RBTREE_H

#undef PX_RB_NULL
#if defined(__cplusplus)
  #define PX_RB_NULL 0
#else
  #define PX_RB_NULL ((void *)0)
#endif

struct px_rb_node
{
#define	RB_RED		0
#define	RB_BLACK	1
	struct px_rb_node *parent;
	unsigned long color;
	struct px_rb_node *rb_right;
	struct px_rb_node *rb_left;
};
    /* The alignment might seem pointless, but allegedly CRIS needs it */

struct px_rb_root
{
	struct px_rb_node *rb_node;
};


//#define rb_parent(r)   ((struct px_rb_node *)((r)->rb_parent_color & ~3))
#define rb_parent(r)   ((struct px_rb_node *)((r)->parent))
//#define rb_color(r)   ((r)->rb_parent_color & 1)
#define rb_color(r)   ((r)->color)
#define rb_is_red(r)   (!rb_color(r))
#define rb_is_black(r) rb_color(r)
// #define rb_set_red(r)  do { (r)->rb_parent_color &= ~1; } while (0)
// #define rb_set_black(r)  do { (r)->rb_parent_color |= 1; } while (0)
#define rb_set_red(r)  do { (r)->color = 0; } while (0)
#define rb_set_black(r)  do { (r)->color = 1; } while (0)
static void rb_set_parent(struct px_rb_node *rb, struct px_rb_node *p)
{
	//rb->rb_parent_color = (rb->rb_parent_color & 3) | (unsigned long)p;
	rb->parent = p;
}
static void rb_set_color(struct px_rb_node *rb, int color)
{
	//rb->rb_parent_color = (rb->rb_parent_color & ~1) | color;
	rb->color = color;
}

#define RB_ROOT	(struct px_rb_root) { PX_RB_NULL, }

#define RB_EMPTY_ROOT(root)	((root)->px_rb_node == PX_RB_NULL)
#define RB_EMPTY_NODE(node)	(rb_parent(node) == node)
#define RB_CLEAR_NODE(node)	(rb_set_parent(node, node))

static void rb_init_node(struct px_rb_node *rb)
{
	//rb->rb_parent_color = 0;
	rb->parent=0;
	rb->color=0;
	rb->rb_right = PX_RB_NULL;
	rb->rb_left = PX_RB_NULL;
	RB_CLEAR_NODE(rb);
}

extern void rb_insert_color(struct px_rb_node *, struct px_rb_root *);
extern void rb_erase(struct px_rb_node *, struct px_rb_root *);

typedef void (*rb_augment_f)(struct px_rb_node *node, void *data);

extern void rb_augment_insert(struct px_rb_node *node,
			      rb_augment_f func, void *data);
extern struct px_rb_node *rb_augment_erase_begin(struct px_rb_node *node);
extern void rb_augment_erase_end(struct px_rb_node *node,
				 rb_augment_f func, void *data);

/* Find logical next and previous nodes in a tree */
extern struct px_rb_node *rb_next(const struct px_rb_node *);
extern struct px_rb_node *rb_prev(const struct px_rb_node *);
extern struct px_rb_node *rb_first(const struct px_rb_root *);
extern struct px_rb_node *rb_last(const struct px_rb_root *);

/* Fast replacement of a single node without remove/rebalance/add/rebalance */
extern void rb_replace_node(struct px_rb_node *victim, struct px_rb_node *newnode, 
			    struct px_rb_root *root);

static  void rb_link_node(struct px_rb_node * node, struct px_rb_node * parent,
				struct px_rb_node ** rb_link)
{
//	node->rb_parent_color = (unsigned long )parent;
	node->parent=parent;
	node->color=0;

	node->rb_left = node->rb_right = PX_RB_NULL;

	*rb_link = node;
}

#endif	/* _LINUX_RBTREE_H */

