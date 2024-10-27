#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView>
#include <models/Dataflow.h>
#include <QTimer>
#include <QMutex>

class qtNode;
class qtEdge;

class GraphWidget : public QGraphicsView
{
Q_OBJECT

public:
    GraphWidget(QWidget *parent = nullptr);
    void setDataflow(models::Dataflow *dataflow);
    void drawGraph();
    void startLayout();

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void scaleView(qreal scaleFactor);

private:
    QTimer layoutTimer;
    void layoutStep();
private:
    QGraphicsScene *scene;
    std::vector<qtNode*> nodes;
    std::vector<qtEdge*> edges;
    models::Dataflow *dataflow; 
    bool middleMouseButtonPressed = false;
    QPointF lastMousePosition; 
    QMutex graph_mutex;
};

#endif // GRAPHWIDGET_H
