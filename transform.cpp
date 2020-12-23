#include "transform.h"
#include <QVector4D>

Transform::Transform(){
    t=Translation(0,0,0);
    s=QVector3D(0,0,0);
    r=Rotation(0,0,0,0);
    transformMat.setToIdentity();
}

Transform::Transform(Translation translate, QVector3D scale, Rotation rotation){
    t=translate;
    r=rotation;
    s=scale;
    updateMat();
}

void Transform::updateMat(){
    QMatrix3x3 tmp = r.toRotationMatrix();
    this->transformMat = QMatrix4x4(tmp(0,0)*s.x(),tmp(0,1),tmp(0,2),t.x(),
                              tmp(1,0),tmp(1,1)*s.y(),tmp(1,2),t.y(),
                              tmp(2,0),tmp(2,1),tmp(2,2)*s.z(),t.z(),
                              0,0,0,1);
}


QVector3D Transform::applyToPoint(QVector3D p){
    QVector4D tmp = QVector4D(p.x(),p.y(),p.z(),1) * transformMat;
    return QVector3D(tmp.x(),tmp.y(),tmp.z());
}

QVector3D Transform::applyToVector(QVector3D v){
    QVector4D tmp = QVector4D(v.x(),v.y(),v.z(),1) * transformMat;
    return QVector3D(tmp.x(),tmp.y(),tmp.z());
}

QVector3D Transform::applyToVersor(QVector3D v){
    QVector4D tmp = QVector4D(v.x(),v.y(),v.z(),1) * transformMat;
    return QVector3D(tmp.x(),tmp.y(),tmp.z());
}


Transform Transform::combineWith(Transform &t){
    this->addRotation(t.getRotation());
    this->addScaling(t.getScaling());
    this->addTranslation(t.getTranslation());
    return *this;
}

Transform Transform::inverse(){
    //?
    return Transform();
}

Transform Transform::interpolateWith(Transform &t, float k){
    Transform res;
    res.s = s * k + t.s*(1-k);
    res.t = this->t*k + t.t*(k-1);
    res.r = this->r*k + t.r*(k-1);
    res.r.normalize();
    return res;
}


Rotation Transform::getRotation(){
    return this->r;
}

QMatrix3x3 Transform::getRotationAsMatrix(){
    return this->r.toRotationMatrix();
}

Translation Transform::getTranslation(){
    return this->t;
}

QVector3D Transform::getScaling(){
    return this->s;
}


void Transform::setRotation(Rotation r){
    this->r=r;
    updateMat();
}

void Transform::setTranslation(Translation t){
    this->t=t;
    updateMat();
}

void Transform::setScaling(QVector3D s){
    this->s=s;
    updateMat();
}


void Transform::addRotation(Rotation r){
    this->r+=r;
    updateMat();
}

void Transform::addTranslation(Translation t){
    this->t+=t;
    updateMat();
}

void Transform::addScaling(QVector3D s){
    this->s+=s;
    updateMat();
}
