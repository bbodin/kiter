#include "qtNode.h"
#include "qtEdge.h"
#include <QGraphicsTextItem>
#include <QFontMetricsF>

qtNode::qtNode(GraphWidget *parent)
        : QGraphicsEllipseItem(nullptr)
{
    setRect(-20, -20, 40, 40);  
    setBrush(Qt::yellow);       
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
    setZValue(1);  // Ensure the node is above the edges

    // Create the label for text
    label = new QGraphicsTextItem(this);
    label->setDefaultTextColor(Qt::black);
    // Do not set ItemIgnoresTransformations here; handle scaling manually if necessary
}

void qtNode::setText(const QString &text) {
    label->setPlainText(text);
    adjustTextToFit();
}

void qtNode::adjustTextToFit() {
    // Calculate the available space inside the node with padding
    qreal padding = 4; 
    qreal maxTextWidth = rect().width() - 2 * padding;
    qreal maxTextHeight = rect().height() - 2 * padding;

    QFont font = label->font();
    QFontMetricsF metrics(font);
    QRectF textRect = metrics.boundingRect(label->toPlainText());

    // Adjust the font size to ensure the text fits within the node
    while (textRect.width() > maxTextWidth || textRect.height() > maxTextHeight) {
        font.setPointSizeF(font.pointSizeF() * 0.95); // Reduce font size incrementally
        label->setFont(font);
        metrics = QFontMetricsF(font);
        textRect = metrics.boundingRect(label->toPlainText());
    }

    // Center the text within the node
    qreal xPos = -textRect.width() / 2;
    qreal yPos = -textRect.height() / 2;
    label->setPos(xPos, yPos);
}

void qtNode::addEdge(qtEdge *edge) {
    edgeList.append(edge);
    edge->updatePosition();
}

QVariant qtNode::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionHasChanged) {
        for (qtEdge *edge : qAsConst(edgeList)) {
            edge->updatePosition();
        }
    }
    return QGraphicsEllipseItem::itemChange(change, value);
}
void qtNode::applyForce(const QPointF& force) {
    currentForce += force;
}

void qtNode::resetForce() {
    currentForce = QPointF(0, 0);
}

QPointF qtNode::getForce() const {
    return currentForce;
}
