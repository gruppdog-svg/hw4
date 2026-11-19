#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    // Balance convention: balance = height(right subtree) - height(left subtree)
    void insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n);
    void removeFix(AVLNode<Key,Value>* n, int diff);
    void rotateLeft(AVLNode<Key,Value>* n);
    void rotateRight(AVLNode<Key,Value>* n);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // Empty tree: the new node becomes the root
    if (this->root_ == NULL) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        return;
    }

    AVLNode<Key, Value>* cur = static_cast<AVLNode<Key, Value>*>(this->root_);
    while (true) {
        if (new_item.first < cur->getKey()) {
            if (cur->getLeft() == NULL) {
                AVLNode<Key, Value>* n =
                    new AVLNode<Key, Value>(new_item.first, new_item.second, cur);
                cur->setLeft(n);
                // Left side grew by one. If the parent is now unbalanced
                // (balance != 0), its subtree height grew, so keep fixing upward.
                cur->updateBalance(-1);
                if (cur->getBalance() != 0) {
                    insertFix(cur, n);
                }
                return;
            }
            cur = cur->getLeft();
        }
        else if (cur->getKey() < new_item.first) {
            if (cur->getRight() == NULL) {
                AVLNode<Key, Value>* n =
                    new AVLNode<Key, Value>(new_item.first, new_item.second, cur);
                cur->setRight(n);
                cur->updateBalance(1);
                if (cur->getBalance() != 0) {
                    insertFix(cur, n);
                }
                return;
            }
            cur = cur->getRight();
        }
        else {
            // key already exists: overwrite the value, no structural change
            cur->setValue(new_item.second);
            return;
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    AVLNode<Key, Value>* n = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (n == NULL) return;

    // Two children: swap the NODES (not key/values) with the predecessor
    if (n->getLeft() != NULL && n->getRight() != NULL) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(
            BinarySearchTree<Key, Value>::predecessor(n));
        nodeSwap(n, pred);
    }

    // n now has at most one child
    AVLNode<Key, Value>* child = (n->getLeft() != NULL) ? n->getLeft() : n->getRight();
    AVLNode<Key, Value>* parent = n->getParent();

    // Record which side of the parent shrinks:
    //   removed from the left  -> parent's balance goes up by 1
    //   removed from the right -> parent's balance goes down by 1
    int diff = 0;
    if (parent != NULL) {
        diff = (n == parent->getLeft()) ? 1 : -1;
    }

    // Promote the child (or NULL) into n's place
    if (child != NULL) child->setParent(parent);

    if (parent == NULL)              this->root_ = child;
    else if (n == parent->getLeft()) parent->setLeft(child);
    else                             parent->setRight(child);

    delete n;

    removeFix(parent, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

/**
 * Helper: rebalance after an insertion.
 * p is the parent of n, and p's balance has already been updated and is
 * non-zero (so p's subtree height grew). Walk up while heights keep growing.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n)
{
    if (p == NULL) return;
    AVLNode<Key, Value>* g = p->getParent();
    if (g == NULL) return;

    if (p == g->getLeft()) {
        g->updateBalance(-1);

        if (g->getBalance() == 0) {
            return;                 // height did not grow: done
        }
        if (g->getBalance() == -1) {
            insertFix(g, p);        // height grew: keep going up
            return;
        }

        // g->getBalance() == -2: rotate
        if (n == p->getLeft()) {
            // zig-zig
            rotateRight(g);
            p->setBalance(0);
            g->setBalance(0);
        }
        else {
            // zig-zag
            int8_t b = n->getBalance();
            rotateLeft(p);
            rotateRight(g);
            if (b == -1) {
                p->setBalance(0);
                g->setBalance(1);
                n->setBalance(0);
            }
            else if (b == 0) {
                p->setBalance(0);
                g->setBalance(0);
                n->setBalance(0);
            }
            else { // b == 1
                p->setBalance(-1);
                g->setBalance(0);
                n->setBalance(0);
            }
        }
    }
    else {
        g->updateBalance(1);

        if (g->getBalance() == 0) {
            return;
        }
        if (g->getBalance() == 1) {
            insertFix(g, p);
            return;
        }

        // g->getBalance() == 2: rotate
        if (n == p->getRight()) {
            // zig-zig
            rotateLeft(g);
            p->setBalance(0);
            g->setBalance(0);
        }
        else {
            // zig-zag
            int8_t b = n->getBalance();
            rotateRight(p);
            rotateLeft(g);
            if (b == 1) {
                p->setBalance(0);
                g->setBalance(-1);
                n->setBalance(0);
            }
            else if (b == 0) {
                p->setBalance(0);
                g->setBalance(0);
                n->setBalance(0);
            }
            else { // b == -1
                p->setBalance(1);
                g->setBalance(0);
                n->setBalance(0);
            }
        }
    }
}

/**
 * Helper: rebalance after a removal.
 * n is the node whose subtree just got shorter, and diff is the amount to add
 * to n's balance (+1 if its left side shrank, -1 if its right side shrank).
 */
template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* n, int diff)
{
    if (n == NULL) return;

    // Figure out how n's own parent will be affected if n's height shrinks
    AVLNode<Key, Value>* p = n->getParent();
    int ndiff = 0;
    if (p != NULL) {
        ndiff = (n == p->getLeft()) ? 1 : -1;
    }

    int nb = n->getBalance() + diff;

    if (diff == -1) {
        if (nb == -2) {
            // Left-heavy: c is the taller (left) child
            AVLNode<Key, Value>* c = n->getLeft();
            int cb = c->getBalance();

            if (cb == -1) {
                // zig-zig, height shrinks: keep going
                rotateRight(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else if (cb == 0) {
                // zig-zig, height unchanged: stop
                rotateRight(n);
                n->setBalance(-1);
                c->setBalance(1);
            }
            else { // cb == 1
                // zig-zag, height shrinks: keep going
                AVLNode<Key, Value>* g = c->getRight();
                int gb = g->getBalance();
                rotateLeft(c);
                rotateRight(n);
                if (gb == 1) {
                    n->setBalance(0);
                    c->setBalance(-1);
                    g->setBalance(0);
                }
                else if (gb == 0) {
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else { // gb == -1
                    n->setBalance(1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(p, ndiff);
            }
        }
        else if (nb == -1) {
            // was balanced, height unchanged: stop
            n->setBalance(-1);
        }
        else { // nb == 0
            // height shrank: keep going
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
    else { // diff == +1
        if (nb == 2) {
            // Right-heavy: c is the taller (right) child
            AVLNode<Key, Value>* c = n->getRight();
            int cb = c->getBalance();

            if (cb == 1) {
                // zig-zig, height shrinks: keep going
                rotateLeft(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else if (cb == 0) {
                // zig-zig, height unchanged: stop
                rotateLeft(n);
                n->setBalance(1);
                c->setBalance(-1);
            }
            else { // cb == -1
                // zig-zag, height shrinks: keep going
                AVLNode<Key, Value>* g = c->getLeft();
                int gb = g->getBalance();
                rotateRight(c);
                rotateLeft(n);
                if (gb == -1) {
                    n->setBalance(0);
                    c->setBalance(1);
                    g->setBalance(0);
                }
                else if (gb == 0) {
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else { // gb == 1
                    n->setBalance(-1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(p, ndiff);
            }
        }
        else if (nb == 1) {
            n->setBalance(1);
        }
        else { // nb == 0
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
}

/**
 * Helper: right rotation around g. g's left child p becomes the subtree root.
 * Only fixes pointers (and root_); the caller sets the balances.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* g)
{
    AVLNode<Key, Value>* p  = g->getLeft();
    AVLNode<Key, Value>* gp = g->getParent();
    AVLNode<Key, Value>* t  = p->getRight();

    // p takes g's place under g's old parent
    p->setParent(gp);
    if (gp == NULL)              this->root_ = p;
    else if (gp->getLeft() == g) gp->setLeft(p);
    else                         gp->setRight(p);

    // g becomes p's right child
    p->setRight(g);
    g->setParent(p);

    // p's old right subtree becomes g's left subtree
    g->setLeft(t);
    if (t != NULL) t->setParent(g);
}

/**
 * Helper: left rotation around g. g's right child p becomes the subtree root.
 * Only fixes pointers (and root_); the caller sets the balances.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* g)
{
    AVLNode<Key, Value>* p  = g->getRight();
    AVLNode<Key, Value>* gp = g->getParent();
    AVLNode<Key, Value>* t  = p->getLeft();

    p->setParent(gp);
    if (gp == NULL)               this->root_ = p;
    else if (gp->getLeft() == g)  gp->setLeft(p);
    else                          gp->setRight(p);

    p->setLeft(g);
    g->setParent(p);

    g->setRight(t);
    if (t != NULL) t->setParent(g);
}


#endif