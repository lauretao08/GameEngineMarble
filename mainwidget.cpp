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


    }

    update();
}

void MainWidget::keyPressEvent(QKeyEvent *event){
    QVector3D input(0.0f,0.0f,0.0f);
    switch(event->key()){
    case Qt::Key_Z :
    case Qt::Key_Up :
        input+=QVector3D(0.0f,0.0f,1.0/16.0);
        break;

    case Qt::Key_S :
    case Qt::Key_Down :
        input+=QVector3D(0.0f,0.0f,-1.0/16.0);
        break;

    case Qt::Key_Q :
    case Qt::Key_Left :
        input+=QVector3D(1.0/16.0,0.0f,0.0f);
        break;

    case Qt::Key_D :
    case Qt::Key_Right :
        input+=QVector3D(-1.0/16.0,0.0f,0.0f);
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
    case Qt::Key_R:
        scene.respawn();
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
        scene.addForce(MAIN_NODE_ID, -input*16);
        break;
    default:
        std::cout<<"[mainwidget.cpp/keyPressEvent]Warning, Unknown Control Mode"<<std::endl;
    }

    //update();
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
    const qreal zNear = 0.1, zFar = 100.0, fov = 50.0;
    projection.setToIdentity();
    projection.perspective(fov, aspect, zNear, zFar);

    //Légèrement modifier la position de caméra
    QMatrix4x4 view;
    view.setToIdentity();
    view.translate(5.0, -8.0, -20.0);
    view.rotate(QQuaternion::fromEulerAngles(20.0,0.0,0.0));
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
    sphere_node.setTransform(Transform( QVector3D(0.0,2.0,0.0) , QVector3D(1.0,1.0,1.0) , QQuaternion(0.0,0.0,0.0,0.0) ));
    sphere_node.setStatic(false);
    scene.AddNode(sphere_node,&sphere_node); //Item 1 on Scene


    SceneGraphNode cube_node = SceneGraphNode(&root, objectType::CUBE);
    cube_node.setTransform(Transform( QVector3D(0.0,-0.5,0.0) , QVector3D(2,0.0,2) , QQuaternion(0.0,0.0,0.0,0.0) ));
    scene.AddNode(cube_node,&cube_node); //Item 2
    SceneGraphNode cube_node35 = SceneGraphNode(&root, objectType::CUBE);
    cube_node35.setTransform(Transform( QVector3D(0.0,-0.5,-4.0) , QVector3D(2,0.0,2) , QQuaternion(0.0,0.0,0.0,0.0) ));
    scene.AddNode(cube_node35,&cube_node35);

  //Level design (caché pour l'instant pour tester la physique)
    SceneGraphNode cube_node3 = SceneGraphNode(&root, objectType::CUBE);
    cube_node3.setTransform(Transform( QVector3D(8.0,1.5,-8.0) , QVector3D(2,2,2) , QQuaternion(0.0,0.0,0.0,0.0) ));
    scene.AddNode(cube_node3,&cube_node3); //Item 5

    SceneGraphNode cube_node4 = SceneGraphNode(&root, objectType::CUBE);
    cube_node4.setTransform(Transform( QVector3D(12.0,1.5,-8.0) , QVector3D(2,2,2) , QQuaternion(0.0,0.0,0.0,0.0) ));
    scene.AddNode(cube_node4,&cube_node4); //Item 6



    //Group 2
    SceneGraphNode cube_node8 = SceneGraphNode(&root, objectType::CUBE);
    cube_node8.setTransform(Transform( QVector3D(-12.0,1.5,-16.0) , QVector3D(2,2,2) , QQuaternion(0.0,0.0,0.0,0.0) ));
    scene.AddNode(cube_node8,&cube_node8); //Item 6

    SceneGraphNode cube_node9 = SceneGraphNode(&root, objectType::CUBE);
    cube_node9.setTransform(Transform( QVector3D(-16.0,1.5,-16.0) , QVector3D(2,2,2) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(cube_node9,&cube_node9);

    SceneGraphNode cube_node10 = SceneGraphNode(&root, objectType::CUBE);
    cube_node10.setTransform(Transform( QVector3D(-16.0,1.5,-12.0) , QVector3D(2,2,2) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(cube_node10,&cube_node10);

    SceneGraphNode cube_node11 = SceneGraphNode(&root, objectType::CUBE);
    cube_node11.setTransform(Transform( QVector3D(-16.0,1.5,-8.0) , QVector3D(2,2,2) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(cube_node11,&cube_node11);

    SceneGraphNode cube_node19 = SceneGraphNode(&root, objectType::CUBE);
    cube_node19.setTransform(Transform( QVector3D(-20.0,1.5,-16.0) , QVector3D(2,2,2) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(cube_node19,&cube_node19);


    SceneGraphNode cube_node20 = SceneGraphNode(&root, objectType::CUBE);
    cube_node20.setTransform(Transform( QVector3D(-20.0,1.5,-12.0) , QVector3D(2,2,2) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(cube_node20,&cube_node20);

    SceneGraphNode cube_node21 = SceneGraphNode(&root, objectType::CUBE);
    cube_node21.setTransform(Transform( QVector3D(-20.0,1.5,-8.0) , QVector3D(2,2,2) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(cube_node21,&cube_node21);

    SceneGraphNode cube_node28 = SceneGraphNode(&root, objectType::CUBE);
    cube_node28.setTransform(Transform( QVector3D(-20.0,1.5,-4.0) , QVector3D(2,0,2*sqrt(2.0)) , QQuaternion::fromEulerAngles(45.0,0.0,0.0) ));
    scene.AddNode(cube_node28,&cube_node28);
    SceneGraphNode cube_node29 = SceneGraphNode(&root, objectType::CUBE);
    cube_node29.setTransform(Transform( QVector3D(-16.0,1.5,-4.0) , QVector3D(2,0,2*sqrt(2.0)) , QQuaternion::fromEulerAngles(45.0,0.0,0.0) ));
    scene.AddNode(cube_node29,&cube_node29);

    SceneGraphNode cube_node30 = SceneGraphNode(&root, objectType::CUBE);
    cube_node30.setTransform(Transform( QVector3D(-16.0,-0.5,0.0) , QVector3D(2,0,2) , QQuaternion::fromEulerAngles(0.0,0.0,0.0) ));
    scene.AddNode(cube_node30,&cube_node30);
    SceneGraphNode cube_node31 = SceneGraphNode(&root, objectType::CUBE);
    cube_node31.setTransform(Transform( QVector3D(-20.0,-0.5,0.0) , QVector3D(2,0,2) , QQuaternion::fromEulerAngles(0.0,0.0,0.0) ));
    scene.AddNode(cube_node31,&cube_node31);
    SceneGraphNode cube_node32 = SceneGraphNode(&root, objectType::CUBE);
    cube_node32.setTransform(Transform( QVector3D(-12.0,-0.5,0.0) , QVector3D(2,0,2) , QQuaternion::fromEulerAngles(0.0,0.0,0.0) ));
    scene.AddNode(cube_node32,&cube_node32);
    SceneGraphNode cube_node33 = SceneGraphNode(&root, objectType::CUBE);
    cube_node33.setTransform(Transform( QVector3D(-8.0,-0.5,0.0) , QVector3D(2,0,2) , QQuaternion::fromEulerAngles(0.0,0.0,0.0) ));
    scene.AddNode(cube_node33,&cube_node33);
    SceneGraphNode cube_node34 = SceneGraphNode(&root, objectType::CUBE);
    cube_node34.setTransform(Transform( QVector3D(-4.0,-0.5,0.0) , QVector3D(2,0,2) , QQuaternion::fromEulerAngles(0.0,0.0,0.0) ));
    scene.AddNode(cube_node34,&cube_node34);


    //Group 3
    SceneGraphNode cube_node13 = SceneGraphNode(&root, objectType::CUBE);
    cube_node13.setTransform(Transform( QVector3D(-16.0,1.5,-20.0) , QVector3D(2,2,2) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(cube_node13,&cube_node13);

    SceneGraphNode cube_node14 = SceneGraphNode(&root, objectType::CUBE);
    cube_node14.setTransform(Transform( QVector3D(-16.0,1.5,-24.0) , QVector3D(2,2,2) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(cube_node14,&cube_node14);

    SceneGraphNode cube_node15 = SceneGraphNode(&root, objectType::CUBE);
    cube_node15.setTransform(Transform( QVector3D(-16.0,1.5,-28.0) , QVector3D(2,2,2) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(cube_node15,&cube_node15);

    SceneGraphNode cube_node18 = SceneGraphNode(&root, objectType::CUBE);
    cube_node18.setTransform(Transform( QVector3D(-20.0,1.5,-28.0) , QVector3D(2,2,2) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(cube_node18,&cube_node18);

    SceneGraphNode cube_node16 = SceneGraphNode(&root, objectType::CUBE);
    cube_node16.setTransform(Transform( QVector3D(-20.0,1.5,-20.0) , QVector3D(2,2,2) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(cube_node16,&cube_node16);

    SceneGraphNode cube_node17 = SceneGraphNode(&root, objectType::CUBE);
    cube_node17.setTransform(Transform( QVector3D(-20.0,1.5,-24.0) , QVector3D(2,2,2) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(cube_node17,&cube_node17);

    SceneGraphNode cube_node36 = SceneGraphNode(&root, objectType::CUBE);
    cube_node36.setTransform(Transform( QVector3D(-16.0,1.5,-32.0) , QVector3D(2,2,2) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(cube_node36,&cube_node36);

    SceneGraphNode cube_node37 = SceneGraphNode(&root, objectType::CUBE);
    cube_node37.setTransform(Transform( QVector3D(-20.0,1.5,-32.0) , QVector3D(2,2,2) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(cube_node37,&cube_node37);

    //small obstacle, but still section 3
    SceneGraphNode obstacle_1 = SceneGraphNode(&root, objectType::CUBE);
    obstacle_1.setTransform(Transform( QVector3D(-16.0,4.0,-32.0) , QVector3D(1,1,1) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(obstacle_1,&obstacle_1);

    SceneGraphNode cube_node38 = SceneGraphNode(&root, objectType::CUBE);
    cube_node38.setTransform(Transform( QVector3D(-12.0,1.5,-32.0) , QVector3D(2,2,2) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(cube_node38,&cube_node38);
    SceneGraphNode cube_node39 = SceneGraphNode(&root, objectType::CUBE);
    cube_node39.setTransform(Transform( QVector3D(-8.0,1.5,-32.0) , QVector3D(2,2,2) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(cube_node39,&cube_node39);
    SceneGraphNode cube_node40 = SceneGraphNode(&root, objectType::CUBE);
    cube_node40.setTransform(Transform( QVector3D(-4.0,1.5,-32.0) , QVector3D(2,2,2) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(cube_node40,&cube_node40);

    //ending
    SceneGraphNode cube_ending = SceneGraphNode(&root, objectType::TRIGGER);
    cube_ending.setTransform(Transform( QVector3D(-4.0,3.5,-32.0) , QVector3D(2,2,2) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(cube_ending,&cube_ending);

    //Groupe 4
    SceneGraphNode cube_node22 = SceneGraphNode(&root, objectType::CUBE);
    cube_node22.setTransform(Transform( QVector3D(0.0,-0.5,-8.0) , QVector3D(2,0,2) , QQuaternion(0.0,0.0,0.0,0.0) ));
    scene.AddNode(cube_node22,&cube_node22);
    SceneGraphNode cube_node23 = SceneGraphNode(&root, objectType::CUBE);
    cube_node23.setTransform(Transform( QVector3D(0.0,-0.5,-12.0) , QVector3D(2,0,2) , QQuaternion(0.0,0.0,0.0,0.0) ));
    scene.AddNode(cube_node23,&cube_node23);
    SceneGraphNode cube_node24 = SceneGraphNode(&root, objectType::CUBE);
    cube_node24.setTransform(Transform( QVector3D(0.0,-0.5,-16.0) , QVector3D(2,0,2) , QQuaternion(0.0,0.0,0.0,0.0) ));
    scene.AddNode(cube_node24,&cube_node24);

    SceneGraphNode cube_node25 = SceneGraphNode(&root, objectType::CUBE);
    cube_node25.setTransform(Transform( QVector3D(-8.0,1.5,-16.0) , QVector3D(2*sqrt(2),0,2) , QQuaternion::fromEulerAngles(0.0,0.0,-45.0) ));
    scene.AddNode(cube_node25,&cube_node25);
    SceneGraphNode cube_node26 = SceneGraphNode(&root, objectType::CUBE);
    cube_node26.setTransform(Transform( QVector3D(-4.0,-0.5,-16.0) , QVector3D(2,0,2) , QQuaternion(0.0,0.0,0.0,0.0) ));
    scene.AddNode(cube_node26,&cube_node26);
    SceneGraphNode cube_node27 = SceneGraphNode(&root, objectType::CUBE);
    cube_node27.setTransform(Transform( QVector3D(4.0, 1.5,-8.0) , QVector3D(2*sqrt(2),0,2) , QQuaternion::fromEulerAngles(0.0,0.0,45.0) ));
    scene.AddNode(cube_node27,&cube_node27);

    //death plane
    SceneGraphNode death_plane = SceneGraphNode(&root, objectType::TRIGGER);
    death_plane.setTransform(Transform( QVector3D(0.0,-5.0,0.0) , QVector3D(40,0.5,60) , QQuaternion(0.0,.0,0.0,0.0) ));
    scene.AddNode(death_plane,&death_plane);
}


void MainWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Binding the textures to the matchinng buffers
    texture_ball->bind(objectType::SPHERE);
    texture_ground->bind(objectType::CUBE);

    scene.updateCurrentTime();
    scene.manageCollision();
    scene.updateForce(scene.currentTime-scene.previousTime);
    scene.displaySceneElements(&program, geometries ,projection, rotation);

}


