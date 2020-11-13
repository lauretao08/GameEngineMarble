#include "transform.h"
#include <QVector4D>


Transform::Transform()
{
    t=QVector3D(0,0,0);
    s=QVector3D(0,0,0);
    r=QQuaternion(0,0,0,0);
    transformMat.setToIdentity();
}

Transform::Transform(QVector3D translate, QVector3D scale, QQuaternion rotation)
{
    t=translate;
    r=rotation;
    s=scale;
    QMatrix3x3 tmp = r.toRotationMatrix();
    transformMat = QMatrix4x4(tmp(0,0)*s.x(),tmp(0,1),tmp(0,2),t.x(),
                              tmp(1,0),tmp(1,1)*s.y(),tmp(1,2),t.y(),
                              tmp(2,0),tmp(2,1),tmp(2,2)*s.z(),t.z(),
                              0,0,0,1);
}

QVector3D Transform::applyToPoint(QVector3D p)
{
    QVector4D tmp = QVector4D(p.x(),p.y(),p.z(),1) * transformMat;
    return QVector3D(tmp.x(),tmp.y(),tmp.z());
}

QVector3D Transform::applyToVector(QVector3D v)
{
    QVector4D tmp = QVector4D(v.x(),v.y(),v.z(),1) * transformMat;
    return QVector3D(tmp.x(),tmp.y(),tmp.z());
}

QVector3D Transform::applyToVersor(QVector3D v)
{
    QVector4D tmp = QVector4D(v.x(),v.y(),v.z(),1) * transformMat;
    return QVector3D(tmp.x(),tmp.y(),tmp.z());
}

Transform Transform::combineWith(Transform &t)
{
    //?
    return t;
}

Transform Transform::inverse()
{
    //?
    return Transform();
}

Transform Transform::interpolateWith(Transform &t, float k)
{
    Transform res;
    res.s = s * k + t.s*(1-k);
    res.t = this->t*k + t.t*(k-1);
    res.r = this->r*k + t.r*(k-1);
    res.r.normalize();
    return res;
}

QQuaternion Transform::getRotation()
{
    return this->r;
}

QVector3D Transform::getTranslation()
{
    return this->t;
}

QVector3D Transform::getScaling()
{
    return this->s;
}

void Transform::setRotation(QQuaternion r)
{
    this->r=r;
    QMatrix3x3 tmp = r.toRotationMatrix();
    transformMat = QMatrix4x4(tmp(0,0)*s.x(),tmp(0,1),tmp(0,2),t.x(),
                              tmp(1,0),tmp(1,1)*s.y(),tmp(1,2),t.y(),
                              tmp(2,0),tmp(2,1),tmp(2,2)*s.z(),t.z(),
                              0,0,0,1);
}

void Transform::setTranslation(QVector3D t)
{
    this->t=t;
    QMatrix3x3 tmp = r.toRotationMatrix();
    transformMat = QMatrix4x4(tmp(0,0)*s.x(),tmp(0,1),tmp(0,2),t.x(),
                              tmp(1,0),tmp(1,1)*s.y(),tmp(1,2),t.y(),
                              tmp(2,0),tmp(2,1),tmp(2,2)*s.z(),t.z(),
                              0,0,0,1);
}

void Transform::setScaling(QVector3D s)
{
    this->s=s;
    QMatrix3x3 tmp = r.toRotationMatrix();
    transformMat = QMatrix4x4(tmp(0,0)*s.x(),tmp(0,1),tmp(0,2),t.x(),
                              tmp(1,0),tmp(1,1)*s.y(),tmp(1,2),t.y(),
                              tmp(2,0),tmp(2,1),tmp(2,2)*s.z(),t.z(),
                              0,0,0,1);
}
