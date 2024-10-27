#ifndef NODE_H
#define NODE_H

#include "graphwidget.h"
#include "qtEdge.h"
#include <QGraphicsItem>

class qtNode : public QGraphicsEllipseItem {
public:
    qtNode(GraphWidget *parent = nullptr);

    void addEdge(qtEdge *edge) ;
    void setText(const QString &text);
    void applyForce(const QPointF& force);
    void resetForce();
    void adjustTextToFit();
    QPointF getForce() const;

private:
    QPointF currentForce;
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QGraphicsTextItem *label;
    QList<qtEdge *> edgeList;
};

#endif // NODE_H
