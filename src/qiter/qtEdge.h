#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsLineItem>
#include "qtNode.h"

class qtEdge : public QGraphicsLineItem {
public:
    qtEdge(qtNode *sourceNode, qtNode *destNode);

    qtNode *getSource() const;

    qtNode *getDest() const;

    void updatePosition();

private:
    qtNode *source;
    qtNode *dest;
};

#endif // EDGE_H
