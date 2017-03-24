#ifndef DRAWAREA_H
#define DRAWAREA_H

#include <QWidget>
#include <QVector2D>
#include <QMouseEvent>

namespace Ui {
class DrawArea;
}

class DrawArea : public QWidget
{
    Q_OBJECT



    struct BodyState{
        double time;
        double mass;
        double radius;
        QVector2D pos;
        QVector2D vel;
        QVector2D acc;
    };

    struct SystemState{
        std::vector<BodyState> bodies;
    };

    SystemState _initialConditions;

    //double _timeCounter;
    std::vector<SystemState> _states;
    size_t _counter;
    QPoint _offset;
    QPoint _baseOffset;
    double _scale;
private:
    void paintEvent(QPaintEvent *e);
    void mouseMoveEvent(QMouseEvent *eventMove);
    void mousePressEvent(QMouseEvent *eventPress);
    void mouseReleaseEvent(QMouseEvent *releaseEvent);
    void wheelEvent(QWheelEvent * event);
    void drawCircle(QPainter & painter, double radius, QVector2D center);
    void drawLine(QPainter & painter, QVector2D p1, QVector2D p2);
    QPoint getDisplayCoord(QVector2D vect);
    BodyState getState(size_t index);
public:
    explicit DrawArea(QWidget *parent = 0);
    ~DrawArea();

private:
    Ui::DrawArea *ui;
};

#endif // DRAWAREA_H
