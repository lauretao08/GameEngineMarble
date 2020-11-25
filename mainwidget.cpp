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

    freeCamera=true;
}

MainWidget::~MainWidget()
{
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    delete texture_ground;
    delete texture_ball;
    delete geometries;
    doneCurrent();


}

//! [0]
void MainWidget::mousePressEvent(QMouseEvent *e)
{
    // Save mouse press position
    mousePressPosition = QVector2D(e->localPos());
}

void MainWidget::mouseReleaseEvent(QMouseEvent *e)
{
    // Mouse release position - mouse press position
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
//! [0]

//! [1]
void MainWidget::timerEvent(QTimerEvent *)
{
    //if(freeCamera){
        // Decrease angular speed (friction)
        angularSpeed *= 0.9;

        // Stop rotation when speed goes below threshold
        if (angularSpeed < 0.01) {
            angularSpeed = 0.0;
        } else {
            // Update rotation
            rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

            // Request an update
            update();
        }
    /*
    }else{
        float p,y,r;
        rotation.getEulerAngles(&p,&y,&r);

        rotation = QQuaternion::fromEulerAngles(0.0,y+1.0,0.0);
        update();
    }
    */
}
//! [1]

void MainWidget::keyPressEvent(QKeyEvent *event){
    QVector3D input(0.0f,0.0f,0.0f);
    switch(event->key()){
    case Qt::Key_Up :
        input+=QVector3D(0.0f,0.0f,0.1f);
        break;

    case Qt::Key_Down :
        input+=QVector3D(0.0f,0.0f,-0.1f);
        break;

    case Qt::Key_Left :
        input+=QVector3D(0.1f,0.0f,0.0f);
        break;

    case Qt::Key_Right :
        input+=QVector3D(-0.1f,0.0f,0.0f);
        break;

    case Qt::Key_Z :
        input+=QVector3D(0.0f,0.0f,0.1f);
        break;

    case Qt::Key_S :
        input+=QVector3D(0.0f,0.0f,-0.1f);
        break;

    case Qt::Key_Q :
        input+=QVector3D(0.1f,0.0f,0.0f);
        break;

    case Qt::Key_D :
        input+=QVector3D(-0.1f,0.0f,0.0f);
        break;

    case Qt::Key_Shift  :
        input+=QVector3D(0.0f,0.1f,0.0f);
        //projection.translate(0,0.1,0);
        break;

    case Qt::Key_Space  :
        input+=QVector3D(0.0f,-0.1f,0.0f);
        //projection.translate(0,-0.1,0);
        break;
        /*
    case Qt::Key_C :{
        if(freeCamera){
            rotation = QQuaternion::fromEulerAngles(-45.0,0,0);
        }else{
            angularSpeed=0.0;
        }
        freeCamera=!freeCamera;
        break;
    }*/

    default:
        break;
    }

    projection.translate(input);
    update();
}


void MainWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    initShaders();
    initTextures();

//! [2]
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);
//! [2]

    geometries = new GeometryEngine;

    initScene();
    // Use QBasicTimer because its faster than QTimer
    timer.start(12, this);


}

//! [3]
void MainWidget::initShaders()
{
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
//! [3]

//! [4]
void MainWidget::initTextures()
{

    texture_ground = new QOpenGLTexture(QImage(":/texture/sol.png").mirrored());
    texture_ground->setMinificationFilter(QOpenGLTexture::Nearest);
    texture_ground->setMagnificationFilter(QOpenGLTexture::Linear);
    texture_ground->setWrapMode(QOpenGLTexture::Repeat);

    texture_ball = new QOpenGLTexture(QImage(":/texture/balldimpled.png").mirrored());
    texture_ball->setMinificationFilter(QOpenGLTexture::Nearest);
    texture_ball->setMagnificationFilter(QOpenGLTexture::Linear);
    texture_ball->setWrapMode(QOpenGLTexture::Repeat);


    //load heightmap
    /*
    heightMap = new QOpenGLTexture(QImage(":/heightmap-1024x1024.png"));

    grassTex = new QOpenGLTexture(QImage(":/grass.png"));
    rockTex = new QOpenGLTexture(QImage(":/rock.png"));
    snowTex = new QOpenGLTexture(QImage(":/snowrocks.png"));
    */

}
//! [4]

void MainWidget::initScene(){
    //Making the graph scene

    scene = SceneGraph();

    SceneGraphNode root = SceneGraphNode();
    root.setTransform(Transform( QVector3D(0.0,0.0,0.0) , QVector3D(1.0,1.0,1.0) , QQuaternion(0.0,0.0,0.0,0.0) ));
    scene.AddRoot(root,&root); //Item 0 on scene

    SceneGraphNode sphere_node = SceneGraphNode(&root, objectType::SPHERE);
    sphere_node.setTransform(Transform( QVector3D(0.0,2.0,0.0) , QVector3D(1.0,1.0,1.0) , QQuaternion(0.0,0.0,0.0,0.0) ));
    scene.AddNode(sphere_node,&sphere_node); //Item 1 on Scene

    SceneGraphNode cube_node = SceneGraphNode(&root, objectType::CUBE);
    cube_node.setTransform(Transform( QVector3D(0.0,0.0,0.0) , QVector3D(2,0.5,2) , QQuaternion(0.0,0.0,0.0,0.0) ));
    scene.AddNode(cube_node,&cube_node); //Item 2
}
//! [5]
void MainWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 1.0, zFar = 15.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);

}
//! [5]

void MainWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Binding the textures to the matchinng buffers
    texture_ball->bind(objectType::SPHERE);
    texture_ground->bind(objectType::CUBE);

/*

    float p,y,r;
    rotation.getEulerAngles(&p,&y,&r);


    QQuaternion r2 = QQuaternion::fromEulerAngles(0.0,y+7.0,0.0);
    QQuaternion r3 = QQuaternion::fromEulerAngles(0.0,y+3.0,0.0);

    scene.addRotation(0,rotation);
    scene.addRotation(1,r2);
    scene.addRotation(2,r3);
*/

    scene.displaySceneElements(&program, geometries ,projection, rotation);
}
