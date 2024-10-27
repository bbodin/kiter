#include "qtEdge.h"
#include <QPen>

qtEdge::qtEdge(qtNode *sourceNode, qtNode *destNode)
        : source(sourceNode), dest(destNode) {

    // Ensure the edge stays connected between nodes
    setAcceptedMouseButtons(Qt::NoButton);
    source->addEdge(this);
    dest->addEdge(this);

    // Set the pen for the line
    setPen(QPen(Qt::black, 2));
    updatePosition();
}

void qtEdge::updatePosition() {
    if (!source || !dest) {
        return;
    }

    // Set the line from the center of the source node to the center of the destination node
    QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
    setLine(line);
}

qtNode *qtEdge::getSource() const {
    return source;
}

qtNode *qtEdge::getDest() const {
    return dest;
}
