/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwidget.h"

#include <QMouseEvent>
#include <iostream>


MainWidget::MainWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    geometries(0),
    texture_ground(0),
    angularSpeed(0)
{
    controlMode=ControlMode::BALL_CONTROL;
    currentTime = GetCurrentTime();
}

MainWidget::~MainWidget(){
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    delete texture_ground;
    delete texture_ball;
    delete geometries;
    doneCurrent();
}


void MainWidget::mousePressEvent(QMouseEvent *e){
    mousePressPosition = QVector2D(e->localPos());
}

void MainWidget::mouseReleaseEvent(QMouseEvent *e){
    QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;
    // Rotation axis is perpendicular to the mouse position difference
    // vector
    QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

    // Accelerate angular speed relative to the length of the mouse sweep
    qreal acc = diff.length() / 100.0;

    // Calculate new rotation axis as weighted sum
    rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

    // Increase angular speed
    angularSpeed += 1*acc;
}

void MainWidget::timerEvent(QTimerEvent *)
{
    angularSpeed *= 0.9;
    if (angularSpeed < 0.01) {
        angularSpeed = 0.0;
    } else {
        // Update rotation
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

        update();
    }
}

void MainWidget::keyPressEvent(QKeyEvent *event){
    QVector3D input(0.0f,0.0f,0.0f);
    QVector3D force(0.0,0.0,0.0);
    switch(event->key()){
    case Qt::Key_Z :
    case Qt::Key_Up :
        input+=QVector3D(0.0f,0.0f,1.0/16.0);
        force+=QVector3D(0.0,0.0,1.0);
        break;

    case Qt::Key_S :
    case Qt::Key_Down :
        input+=QVector3D(0.0f,0.0f,-1.0/16.0);
        force+=QVector3D(0.0,0.0,-1.0);
        break;

    case Qt::Key_Q :
    case Qt::Key_Left :
        input+=QVector3D(1.0/16.0,0.0f,0.0f);
        force+=QVector3D(1.0,0.0,0.0);
        break;

    case Qt::Key_D :
    case Qt::Key_Right :
        input+=QVector3D(-1.0/16.0,0.0f,0.0f);
        force+=QVector3D(-1.0,0.0,0.0);
        break;

    case Qt::Key_Shift  :
        input+=QVector3D(0.0f,1.0/16.0,0.0f);
        break;

    case Qt::Key_Space  :
        input+=QVector3D(0.0f,-1.0/16.0,0.0f);
        break;

    case Qt::Key_W:
        geometries->setWireframe(true);
        break;
    case Qt::Key_X:
        geometries->setWireframe(false);
        break;

    case Qt::Key_Tab:
        switch (controlMode){
        case ControlMode::CAM_CONTROL:
            controlMode=ControlMode::BALL_CONTROL;
            break;
        case ControlMode::BALL_CONTROL:
            controlMode=ControlMode::CAM_CONTROL;
            break;
        default:
            std::cout<<"[mainwidget.cpp/keyPressEvent]Warning, Unknown Control Mode"<<std::endl;
        }
        break;
    default:
        break;
    }

    switch (controlMode){
    case ControlMode::CAM_CONTROL:
        projection.translate(input);
        break;
    case ControlMode::BALL_CONTROL:
        //scene.addTranslation(MAIN_NODE_ID,-input);
        scene.addForce(MAIN_NODE_ID, -force);
        break;
    default:
        std::cout<<"[mainwidget.cpp/keyPressEvent]Warning, Unknown Control Mode"<<std::endl;
    }

    update();
}


void MainWidget::initializeGL(){
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    initShaders();
    initTextures();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    geometries = new GeometryEngine;

    initScene();
    timer.start(12, this);
}

void MainWidget::resizeGL(int w, int h)
{
    qreal aspect = qreal(w) / qreal(h ? h : 1);
    const qreal zNear = 0.1, zFar = 30.0, fov = 50.0;
    projection.setToIdentity();
    projection.perspective(fov, aspect, zNear, zFar);

    //Légèrement modifier la position de caméra
    QMatrix4x4 view;
    view.setToIdentity();
    view.translate(0.0, -5.0, -10.0);
    projection *= view;
}


void MainWidget::initShaders(){
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();
}

void MainWidget::initTextures(){
    texture_ground = new QOpenGLTexture(QImage(":/texture/sol.png").mirrored());
    texture_ground->setMinificationFilter(QOpenGLTexture::Nearest);
    texture_ground->setMagnificationFilter(QOpenGLTexture::Nearest);
    texture_ground->setWrapMode(QOpenGLTexture::Repeat);

    texture_ball = new QOpenGLTexture(QImage(":/texture/balldimpled.png").mirrored(true,false));
    texture_ball->setMinificationFilter(QOpenGLTexture::Nearest);
    texture_ball->setMagnificationFilter(QOpenGLTexture::Linear);
    texture_ball->setWrapMode(QOpenGLTexture::Repeat);
}

void MainWidget::initScene(){
    //Making the graph scene
    scene = SceneGraph();

    SceneGraphNode root = SceneGraphNode();
    root.setTransform(Transform( QVector3D(0.0,0.0,0.0) , QVector3D(1.0,1.0,1.0) , QQuaternion(0.0,0.0,0.0,0.0) ));
    scene.AddRoot(root,&root); //Item 0 on scene

    SceneGraphNode sphere_node = SceneGraphNode(&root, objectType::SPHERE);
    sphere_node.setTransform(Transform( QVector3D(2.0,1.0,0.0) , QVector3D(1.0,1.0,1.0) , QQuaternion(0.0,0.0,0.0,0.0) ));
    scene.AddNode(sphere_node,&sphere_node); //Item 1 on Scene
    sphere_node.mobile = true;

    SceneGraphNode cube_node = SceneGraphNode(&root, objectType::CUBE);
    cube_node.setTransform(Transform( QVector3D(0.0,0.0,0.0) , QVector3D(2,0.5,2) , QQuaternion(0.0,0.0,0.0,0.0) ));
    scene.AddNode(cube_node,&cube_node); //Item 2

    SceneGraphNode sphere_node2 = SceneGraphNode(&sphere_node, objectType::SPHERE);
    sphere_node2.setTransform(Transform( QVector3D(0.0,-1,0.0) , QVector3D(0.5,0.5,0.5) , QQuaternion(0.0,0.0,0.0,0.0) ));
    scene.AddNode(sphere_node2,&sphere_node2); //Item 3 on Scene

/*  Level design (caché pour l'instant pour tester la physique)
    SceneGraphNode cube_node2 = SceneGraphNode(&root, objectType::CUBE);
    cube_node2.setTransform(Transform( QVector3D(-0.0,0.5,-4.0) , QVector3D(2,1,2) , QQuaternion(0.0,0.0,0.0,0.0) ));
    scene.AddNode(cube_node2,&cube_node2); //Item 4

    SceneGraphNode cube_node3 = SceneGraphNode(&root, objectType::CUBE);
    cube_node3.setTransform(Transform( QVector3D(4.0,0.5,-4.0) , QVector3D(2,1,2) , QQuaternion(0.0,0.0,0.0,0.0) ));
    scene.AddNode(cube_node3,&cube_node3); //Item 5

    SceneGraphNode cube_node4 = SceneGraphNode(&root, objectType::CUBE);
    cube_node4.setTransform(Transform( QVector3D(8.0,0.5,-4.0) , QVector3D(2,1,2) , QQuaternion(0.0,0.0,0.0,0.0) ));
    scene.AddNode(cube_node4,&cube_node4); //Item 6
    */

}


void MainWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Binding the textures to the matchinng buffers
    texture_ball->bind(objectType::SPHERE);
    texture_ground->bind(objectType::CUBE);


    //std::cout<<GetCurrentTime()<<std::endl;
    scene.manageCollision();
    scene.updateCurrentTime(currentTime);
    scene.displaySceneElements(&program, geometries ,projection, rotation);
    update();
}


void MainWidget::checkCollision(){
    scene.manageCollision();
}
