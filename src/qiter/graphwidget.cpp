#include "graphwidget.h"
#include "qtNode.h"
#include "qtEdge.h"
#include <QWheelEvent> 
#include <QDebug>

GraphWidget::GraphWidget(QWidget *parent)
        : QGraphicsView(parent), scene(new QGraphicsScene(this)), dataflow(nullptr)
{
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-400, -400, 800, 800);
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(0.8, 0.8);

    qInfo() << "GraphWidget initialized with scene setup and rendering hints.";

    layoutTimer.setInterval(500);  // Update layout every 500 ms
    connect(&layoutTimer, &QTimer::timeout, this, &GraphWidget::layoutStep);
    layoutTimer.start();

}

void GraphWidget::setDataflow(models::Dataflow *df)
{
    this->dataflow = df;
    qInfo() << "Dataflow model set in GraphWidget.";
    this->graph_mutex.lock();
    drawGraph();
    this->graph_mutex.unlock();
}

void GraphWidget::drawGraph() {

    if (!dataflow) {
        qInfo() << "Attempted to draw graph but dataflow is null.";
        return;
    }

    qInfo() << "Starting to draw graph.";

    // Clear existing items
    scene->clear();
    nodes.clear();
    edges.clear();

    std::map<Vertex, qtNode*> nodes_map;

    // Create nodes
    for (auto v : dataflow->vertices()) {
        qtNode *node = new qtNode(this);
        node->setText(QString::fromStdString(dataflow->getVertexName(v)));
        scene->addItem(node);
        nodes_map[v] = node;
        nodes.push_back(node);
        qInfo() << "Node created for vertex" << QString::fromStdString(dataflow->getVertexName(v));
    }

    // Create edges
    for (auto v : dataflow->vertices()) {
        for (auto it : dataflow->out_edges(v)) {
            Edge e = *it;
            Vertex u = dataflow->getEdgeTarget(e);
            qtEdge *edge = new qtEdge(nodes_map[v], nodes_map[u]);

            edges.push_back(edge);
            scene->addItem(edge);
            qInfo() << "Edge created between vertices" << QString::fromStdString(dataflow->getVertexName(v))
                    << "and" << QString::fromStdString(dataflow->getVertexName(u));
        }
    }

    qInfo() << "Graph drawing completed.";
}

void GraphWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        middleMouseButtonPressed = true;
        lastMousePosition = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    QGraphicsView::mousePressEvent(event);
}

void GraphWidget::mouseMoveEvent(QMouseEvent *event) {
    if (middleMouseButtonPressed) {
        QPointF delta = mapToScene(event->pos()) - mapToScene(lastMousePosition.toPoint());
        translate(delta.x(), delta.y());
        lastMousePosition = event->pos();
    }
    QGraphicsView::mouseMoveEvent(event);
}

void GraphWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        middleMouseButtonPressed = false;
        setCursor(Qt::ArrowCursor);
    }
    QGraphicsView::mouseReleaseEvent(event);
}
void GraphWidget::wheelEvent(QWheelEvent *event)
{
    qInfo() << "Wheel event triggered with delta:" << event->angleDelta();
    scaleView(pow(2.0, -event->angleDelta().y() / 240.0));
}

void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    qInfo() << "Scaling view by factor:" << scaleFactor << "Resulting scale factor:" << factor;
    if (factor < 0.07 || factor > 100) {
        qInfo() << "Scale factor out of bounds, adjustment skipped.";
        return;
    }

    scale(scaleFactor, scaleFactor);
}

void sprint_algorithm_step ( std::vector<qtNode*> &nodes, std::vector<qtEdge*> &edges) {

    // Reset forces
    for (qtNode *node : nodes) {
        node->resetForce();
        qInfo() << "Reset force for node at initial position:" << node->pos();
    }

    // Apply repulsive forces between each pair of nodes
    for (qtNode *node1 : nodes) {
        for (qtNode *node2 : nodes) {
            if (node1 != node2) {
                QPointF delta = node1->pos() - node2->pos();
                qreal distance = std::max(delta.manhattanLength(), 1.0);  // Avoid division by zero and extreme forces
                qreal forceMagnitude = 1000 / distance;  // Repulsive force constant
                QPointF force = (forceMagnitude / distance) * delta;

                qInfo() << "Applying repulsive force:" << force << "between nodes at positions:" << node1->pos() << "and" << node2->pos();
                node1->applyForce(force);
                node2->applyForce(-force);
            }
        }
    }

    // Apply attractive forces along edges
    for (qtEdge *edge : edges) {
        qtNode *source = edge->getSource();
        qtNode *dest = edge->getDest();
        QPointF delta = source->pos() - dest->pos();
        qreal distance = std::max(delta.manhattanLength(), 1.0);
        qreal forceMagnitude = distance * distance / 100;  // Attractive force constant
        QPointF force = (forceMagnitude / distance) * delta;

        qInfo() << "Applying attractive force:" << force << "between nodes at positions:" << source->pos() << "and" << dest->pos();
        source->applyForce(-force);
        dest->applyForce(force);
    }

    // Update node positions based on net forces
    for (qtNode *node : nodes) {
        QPointF force = node->getForce();
        // Prevent excessive movement by limiting the displacement
        QPointF displacement = force * 0.1;  // Apply a damping factor to stabilize the motion
        if (displacement.manhattanLength() > 50) {
            displacement = 50 * displacement / displacement.manhattanLength();
        }

        QPointF newPos = node->pos() + displacement;
        node->setPos(newPos);
        qInfo() << "Updated node position to:" << newPos << "with displacement:" << displacement;
    }
}

void GraphWidget::layoutStep() {
    qInfo() << "Starting layout step with node count:" << nodes.size();

    this->graph_mutex.lock();
    // Use dataflow safely assuming it's not modified elsewhere simultaneously
    sprint_algorithm_step (nodes, edges);

    this->graph_mutex.unlock();


    qInfo() << "Layout step completed.";
}
