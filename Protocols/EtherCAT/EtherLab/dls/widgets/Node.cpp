/*****************************************************************************
 *
 * $Id$
 *
 ****************************************************************************/

#include "Node.h"

using namespace QtDls;

/*****************************************************************************/

Node::Node(Node *p):
    parentNode(p)
{
}

/****************************************************************************/

Node::~Node()
{
}

/****************************************************************************/

Qt::ItemFlags Node::flags() const
{
    return 0;
}

/****************************************************************************/

Node *Node::parent() const
{
    return parentNode;
}

/****************************************************************************/