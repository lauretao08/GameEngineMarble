#include <QVector3D>
#include <QQuaternion>
#include <QMatrix4x4>
#include <iostream>

using namespace std;

#ifndef TRANSFORM_H
#define TRANSFORM_H

#define Translation QVector3D
#define Rotation    QQuaternion

class Transform{

private:
    QVector3D s;
    Translation t;
    Rotation r;
    QMatrix4x4 transformMat;

    void updateMat();
public:

    Transform();
    Transform(Translation translate,QVector3D scale,Rotation rotation);


    QVector3D applyToPoint (QVector3D p);
    QVector3D applyToVector(QVector3D v);
    QVector3D applyToVersor(QVector3D v);

    Transform combineWith(Transform &t);
    Transform inverse();
    Transform interpolateWith(Transform &t, float k);

    Rotation           getRotation();
    QMatrix3x3 getRotationAsMatrix();
    Translation     getTranslation();
    QVector3D           getScaling();


    void setRotation    (Rotation r);
    void setTranslation (Translation t);
    void setScaling     (QVector3D s);

    void addRotation    (Rotation r);
    void addTranslation (Translation t);
    void addScaling     (QVector3D s);

};
#endif // TRANSFORM_H


