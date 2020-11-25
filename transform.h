#include <QVector3D>
#include <QQuaternion>
#include <QMatrix4x4>

using namespace std;

#ifndef TRANSFORM_H
#define TRANSFORM_H


class Transform{

private:
    QVector3D s;
    QVector3D t;
    QQuaternion r;
    QMatrix4x4 transformMat;

public:

    Transform();
    Transform(QVector3D translate,QVector3D scale,QQuaternion rotation);


    QVector3D applyToPoint(QVector3D p);
    QVector3D applyToVector(QVector3D v);
    QVector3D applyToVersor(QVector3D v);

    Transform combineWith(Transform &t);
    Transform inverse();
    Transform interpolateWith(Transform &t, float k);

    QQuaternion getRotation();
    QVector3D getTranslation();
    QVector3D getScaling();

    void setRotation(QQuaternion r);
    void setTranslation(QVector3D t);
    void setScaling(QVector3D s);

    void addRotation(QQuaternion r);
    void addTranslation(QVector3D t);
    void addScaling(QVector3D s);


};




#endif // TRANSFORM_H


