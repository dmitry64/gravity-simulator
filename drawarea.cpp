#include "drawarea.h"
#include "ui_drawarea.h"
#include <QPainter>
#include <QtMath>
#include <QDebug>
#include <algorithm>

const double G = 6.67e-11;

DrawArea::DrawArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DrawArea)
{
    ui->setupUi(this);

    _offset = QPoint(0,0);
    _baseOffset = _offset;
    _scale = 0.4;

    BodyState type1;
    type1.time = 0;
    type1.mass = 100000000000;
    type1.radius = 8;
    type1.pos = QVector2D(700,200);
    type1.vel = QVector2D(3.842,0);
    type1.acc = QVector2D(0,0);

    BodyState type2;
    type2.time = 0;
    type2.mass = 100000000000;
    type2.radius = 8;
    type2.pos = QVector2D(1300,900);
    type2.vel = QVector2D(0.3,2.4);
    type2.acc = QVector2D(0,0);

    BodyState type3;
    type3.time = 0;
    type3.mass = 100000000000;
    type3.radius = 8;
    type3.pos = QVector2D(250,900);
    type3.vel = QVector2D(-3.842,2.3);
    type3.acc = QVector2D(0,0);


    BodyState body2;
    body2.time = 0;
    body2.mass = 5000000000;
    body2.radius = 9;
    body2.pos = QVector2D(700,300);
    body2.vel = QVector2D(-1.9,0.0);
    body2.acc = QVector2D(0,0);

    BodyState sun;
    sun.time = 0;
    sun.mass = 180000000000000;
    sun.radius = 30;
    sun.pos = QVector2D(700,1000);
    sun.vel = QVector2D(0.0,0.0);
    sun.acc = QVector2D(0.0,0.0);

    BodyState body4;
    body4.time = 0;
    body4.mass = 90000;
    body4.radius = 3;
    body4.pos = QVector2D(700,120);
    body4.vel = QVector2D(2.982,0.0);
    body4.acc = QVector2D(0,0);

    BodyState body5;
    body5.time = 0;
    body5.mass = 1600000000000;
    body5.radius = 15;
    body5.pos = QVector2D(700,220);
    body5.vel = QVector2D(-3.982,0.0);
    body5.acc = QVector2D(0,0);


    _initialConditions.bodies.push_back(type1);
    _initialConditions.bodies.push_back(type2);
    _initialConditions.bodies.push_back(type3);


    _initialConditions.bodies.push_back(body2);
    _initialConditions.bodies.push_back(sun);
    _initialConditions.bodies.push_back(body4);
    _initialConditions.bodies.push_back(body5);
    _counter = 0;

    SystemState prev = _initialConditions;


    double timeStep = 0.5;

    for(double i=0; i<100000.0; i+=timeStep) {
        SystemState state = prev;

        for(auto it=state.bodies.begin(); it!=state.bodies.end();it++) {
            BodyState & body = it.operator*();
            body.pos += body.vel * timeStep;
        }

        for(auto it=state.bodies.begin(); it!=state.bodies.end();it++) {
            BodyState & body = it.operator*();
            QVector2D direction(0,0);
            QVector2D acc(0,0);
            for(auto it2=state.bodies.begin(); it2!=state.bodies.end();it2++)
            {
                if(it2!=it) {
                    const BodyState & target = it2.operator*();
                    direction = (target.pos - body.pos);
                    double len = direction.length();
                    double fnet = (G * body.mass * target.mass) / (len*len);

                    double theta = qAtan2(body.pos.y() - target.pos.y(),body.pos.x() - target.pos.x());

                    double accel = fnet / body.mass;

                    double Ax = -(accel * cos(theta));
                    double Ay = -(accel * sin(theta));

                    acc += QVector2D(Ax,Ay);
                }
            }

            body.acc = acc;
            body.vel += body.acc * (timeStep);

        }

        prev = state;
        _states.push_back(state);
    }
}

DrawArea::~DrawArea()
{
    delete ui;
}

void DrawArea::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    if(_counter == _states.size()) {
        _counter = 0;
    } else {
        const SystemState & state = _states[_counter];

        if(_counter!=0) {

            painter.setPen(Qt::black);
            for(size_t j=_counter;j>=((_counter>1000) ? (_counter-1000) : 0);j--) {
                if(j==0){
                    break;
                }
                const SystemState & prev = _states[j-1];
                const SystemState & cur = _states[j];
                for(size_t i=0;i<state.bodies.size(); i++) {
                    const BodyState & body = cur.bodies.at(i);
                    const BodyState & prevBody = prev.bodies.at(i);
                    drawLine(painter,prevBody.pos,body.pos);
                }
            }
        }

        for(auto it=state.bodies.begin(); it!=state.bodies.end();it++) {
            const BodyState & body = it.operator*();
            painter.setPen(Qt::blue);
            drawCircle(painter,body.radius,body.pos);
            painter.setPen(Qt::red);
            drawLine(painter,body.pos,body.pos + body.acc*1000);
        }
        _counter ++;
    }
    update();

}

void DrawArea::mouseMoveEvent(QMouseEvent *eventMove)
{
}

void DrawArea::mousePressEvent(QMouseEvent *eventPress)
{
    _baseOffset = eventPress->pos();
}

void DrawArea::mouseReleaseEvent(QMouseEvent *releaseEvent)
{
    _offset+=releaseEvent->pos() - _baseOffset;
}

void DrawArea::wheelEvent(QWheelEvent *event)
{
    _scale += 0.00005*event->delta();
}

void DrawArea::drawCircle(QPainter &painter, double radius, QVector2D center)
{
    int size = radius;
    QVector2D c =center;
    QVector2D lb = QVector2D(c.x() + size, c.y() + size);
    QVector2D rt = QVector2D(c.x() - size, c.y() - size);
    QRect rect(getDisplayCoord(lb),getDisplayCoord(rt));
    painter.drawEllipse(rect);
}

void DrawArea::drawLine(QPainter &painter, QVector2D p1, QVector2D p2)
{
    painter.drawLine(QLine(getDisplayCoord(p1),getDisplayCoord(p2)));
}

QPoint DrawArea::getDisplayCoord(QVector2D vect)
{
    return QPoint(vect.x(),vect.y())*_scale + _offset;
}

DrawArea::BodyState DrawArea::getState(size_t index)
{

}
