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

#include "geometryengine.h"
#include "BasicIO.h"

#include <QVector2D>
#include <QVector3D>
#include <vector>
#include <QImage>
#include "time.h"
#include <iostream>
#include <fstream>
#include <cmath>

struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
};

//! [0]
GeometryEngine::GeometryEngine()
{
    wireframe = false;
    initializeOpenGLFunctions();
    QOpenGLBuffer arrayBuf1, arrayBuf2;
    QOpenGLBuffer indexBuf1(QOpenGLBuffer::IndexBuffer), indexBuf2(QOpenGLBuffer::IndexBuffer);

    arrayBuf1.create(); arrayBuf2.create();
    indexBuf1.create(); indexBuf2.create();
    arrayBufs.push_back(arrayBuf1);
    arrayBufs.push_back(arrayBuf2);
    indexBufs.push_back(indexBuf1);
    indexBufs.push_back(indexBuf2);
    IndexSize.push_back(-1);
    IndexSize.push_back(-1);


    // Initializes cube geometry and transfers it to VBOs
    initCubeGeometry(objectType::CUBE);

    //initPlaneGeometry(64);

    initSphereGeometry(objectType::SPHERE);

    std::string filename = ":/sphere2.obj";
    //initObjGeometry(objectType::SPHERE,filename);
}

GeometryEngine::~GeometryEngine()
{
    /*
    arrayBuf.destroy();
    indexBuf.destroy();
    */
    for(unsigned int i = 0; i < arrayBufs.size(); i++){
        arrayBufs[i].destroy();
        indexBufs[i].destroy();
    }

}
//! [0]

void GeometryEngine::setWireframe(bool b){
    wireframe = b;

}

bool GeometryEngine::getWireframe(){
    return wireframe;
}

/********************************************/
//InitXXXGeometry

void GeometryEngine::initCubeGeometry(int bufferID)
{
    // For cube we would need only 8 vertices but we have to
    // duplicate vertex for each face because texture coordinate
    // is different.
    VertexData vertices[] = {
        // Vertex data for face 0
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.0f, 0.0f)},  // v0
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.33f, 0.0f)}, // v1
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.0f, 0.5f)},  // v2
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v3

        // Vertex data for face 1
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D( 0.0f, 0.5f)}, // v4
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.33f, 0.5f)}, // v5
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.0f, 1.0f)},  // v6
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.33f, 1.0f)}, // v7

        // Vertex data for face 2
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.5f)}, // v8
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(1.0f, 0.5f)},  // v9
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.66f, 1.0f)}, // v10
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(1.0f, 1.0f)},  // v11

        // Vertex data for face 3
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.0f)}, // v12
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(1.0f, 0.0f)},  // v13
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(0.66f, 0.5f)}, // v14
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(1.0f, 0.5f)},  // v15

        // Vertex data for face 4
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(0.33f, 0.0f)}, // v16
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.0f)}, // v17
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v18
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.66f, 0.5f)}, // v19

        // Vertex data for face 5
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v20
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.66f, 0.5f)}, // v21
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(0.33f, 1.0f)}, // v22
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.66f, 1.0f)}  // v23
    };

    // Indices for drawing cube faces using triangle strips.
    // Triangle strips can be connected by duplicating indices
    // between the strips. If connecting strips have opposite
    // vertex order then last index of the first strip and first
    // index of the second strip needs to be duplicated. If
    // connecting strips have same vertex order then only last
    // index of the first strip needs to be duplicated.
    GLushort indices[] = {
         0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
         4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
         8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
        12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
        16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
        20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
    };

//! [1]
   IndexSize[bufferID] = 34;
    // Transfer vertex data to VBO 0
    arrayBufs[bufferID].bind();
    arrayBufs[bufferID].allocate(vertices, 24 * sizeof(VertexData));

    // Transfer index data to VBO 1
    indexBufs[bufferID].bind();
    indexBufs[bufferID].allocate(indices, 34 * sizeof(GLushort));
    //! [1]
}

void GeometryEngine::initSphereGeometry(int bufferID){
    const int strip_size  = 18; //Alpha Phy
    const int strip_count = 18; //Beta  Theta

    //Defining the Vertices
    VertexData t_vertices[((strip_size+1)*(strip_count+1))-2];
    int t_vertices_it = 0;
    for(int out_strip_it=0;out_strip_it<=strip_count;out_strip_it++){
        float theta = 2.0f *M_PI * (float)out_strip_it/(float)strip_count;
        for(int in_strip_it=0;in_strip_it<=strip_size;in_strip_it++){
            float phy = (M_PI/2.0f) - M_PI*((float)in_strip_it/(float)strip_size);
            if(out_strip_it==strip_count &&(in_strip_it==0 || in_strip_it==strip_size)){

            }else{
                t_vertices[t_vertices_it].position={QVector3D(
                                                    cos(phy)*cos(theta),
                                                    cos(phy)*sin(theta),
                                                    sin(phy))};
                t_vertices[t_vertices_it].texCoord={QVector2D(
                                                    theta/((float)2*M_PI),   //u  about Theta
                                                    (phy/M_PI)-0.5f)}; //V about Phy
                t_vertices_it++;
            }
        }
    }
    //std::cout<<"Vertices size = "<<t_vertices_it<<std::endl;

    //Defining the indices
    GLushort t_indices[(strip_size+1)*(strip_count+1)*2-2];
    int t_indices_it = 0;

    //for(int i_strip=0;i_strip<=4;i_strip++){
    for(int i_strip=0;i_strip<strip_count;i_strip++){
        t_indices[t_indices_it]=i_strip*(1+strip_size);
        t_indices_it++;

        for(int lil_i=0;lil_i<=strip_size;lil_i++){
            t_indices[t_indices_it]=lil_i+(i_strip*(1+strip_size));
            t_indices_it++;
            if(lil_i==strip_size-1){//If we are on the end of a strip

                if(i_strip==strip_count-1){//If we are on the last strip
                    t_indices[t_indices_it]=lil_i+1+(i_strip*(1+strip_size));
                }else{
                    t_indices[t_indices_it]=lil_i+1+(i_strip*(1+strip_size));
                }
                t_indices_it++;
            }else{
                if(i_strip==strip_count-1){//If we are on the last strip
                    t_indices[t_indices_it]=lil_i+strip_size+1+(i_strip*(1+strip_size));
                }else{
                    t_indices[t_indices_it]=lil_i+strip_size+2+(i_strip*(1+strip_size));
                }
                t_indices_it++;
            }
        }
        t_indices[t_indices_it]=t_indices[t_indices_it-1];
        t_indices_it++;
    }

    //std::cout<<"Indice size = "<<t_indices_it<<std::endl;
    this->IndexSize[bufferID] = t_indices_it; // sauvegarder size pour render
    arrayBufs[bufferID].bind();
    arrayBufs[bufferID].allocate(t_vertices, t_vertices_it * sizeof(VertexData));

    indexBufs[bufferID].bind();
    indexBufs[bufferID].allocate(t_indices, t_indices_it * sizeof(GLushort));


}

void GeometryEngine::initObjGeometry(int bufferID,std::string filename)
{
    std::vector<QVector3D> vertices;
    std::vector<std::vector<GLushort>> indices;
    OBJIO::open(filename,vertices,indices);

    int size_vertices = vertices.size();
    int size_indices = indices.size() * 3;

    VertexData t_vertices[vertices.size()];
    GLushort * t_indices = new GLushort[indices.size() * 3];

    for(size_t i = 0; i < vertices.size(); i++){
        t_vertices[i].position = vertices[i];
        QVector3D p = vertices[i];
        p.normalize();
        t_vertices[i].texCoord[0] = 0.5 +(atan2(p.z(),p.x()))/(2.0*M_PI);
        t_vertices[i].texCoord[1] = 0.5 -(asin(p.y()))/M_PI;

        if(t_vertices[i].texCoord[0]<0.01 || t_vertices[i].texCoord[0]>0.99){
            std::cout << t_vertices[i].position.x() <<"/"<< t_vertices[i].position.y() <<"/"<< t_vertices[i].position.z() << "->"
                      << t_vertices[i].texCoord[0] <<"/"<< t_vertices[i].texCoord[1] << std::endl;
        }

        //std::cout << t_vertices[i].position.x() <<"/"<< t_vertices[i].position.y() <<"/"<< t_vertices[i].position.z() << "->" << t_vertices[i].texCoord[0] <<"/"<< t_vertices[i].texCoord[1] << std::endl;

        if(t_vertices[i].texCoord[0]>1 || t_vertices[i].texCoord[0]<0){

            std::cout << "Correcting texCoord 0, value =" << t_vertices[i].texCoord[0] << std::endl;
            t_vertices[i].texCoord[0]=1;
        }
        if(t_vertices[i].texCoord[1]>1 || t_vertices[i].texCoord[1]<0){
            //t_vertices[i].texCoord[1]=0;
            std::cout << "Correcting texCoord 1" << std::endl;
        }
    }
    for(size_t i = 0; i < indices.size(); i++){
        for(size_t j = 0; j < indices[i].size(); j++){
            t_indices[i * 3 + j] = indices[i][j];
        }
    }

    this->IndexSize[bufferID] = size_indices; // sauvegarder size pour render
    arrayBufs[bufferID].bind();
    arrayBufs[bufferID].allocate(t_vertices, size_vertices * sizeof(VertexData));

    indexBufs[bufferID].bind();
    indexBufs[bufferID].allocate(t_indices, size_indices * sizeof(GLushort));
}

/********************************************/
//DrawXXXGeometries

void GeometryEngine::drawGeometry(int bufferID,QOpenGLShaderProgram *program)
{
    // Tell OpenGL which VBOs to use
    arrayBufs[bufferID].bind();
    indexBufs[bufferID].bind();

    program->setUniformValue("texture", bufferID);
    quintptr offset = 0;	// Offset for position

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);// Offset for texture coordinate

    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    if(wireframe){
        glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);
    }else{
        glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);

    }
    glDrawElements(GL_TRIANGLE_STRIP, IndexSize[bufferID], GL_UNSIGNED_SHORT, 0);
}

void GeometryEngine::drawCubeGeometry(int bufferID,QOpenGLShaderProgram *program)
{
    if(bufferID!=objectType::CUBE){
        printf("[GeometryEngine::drawCubeGeometry] : Be careful you may be using the wrong buffer, texture may also be off \n");
    }
    // Tell OpenGL which VBOs to use
    arrayBufs[bufferID].bind();
    indexBufs[bufferID].bind();

    program->setUniformValue("texture", bufferID);
    quintptr offset = 0;	// Offset for position

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    glDrawElements(GL_TRIANGLE_STRIP, IndexSize[bufferID], GL_UNSIGNED_SHORT, 0);
}

void GeometryEngine::drawObjGeometry(int bufferID,QOpenGLShaderProgram *program)
{
    if(bufferID!=objectType::SPHERE){
        printf("[GeometryEngine::drawObjGeometry] : Be careful you may be using the wrong buffer, texture may also be off \n");
    }
    // Tell OpenGL which VBOs to use
    arrayBufs[bufferID].bind();
    indexBufs[bufferID].bind();

    program->setUniformValue("texture", bufferID);
    quintptr offset = 0;	// Offset for position

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    glDrawElements(GL_TRIANGLE_STRIP, IndexSize[bufferID], GL_UNSIGNED_SHORT, 0);

}

/*********************************************/
//Trash Zone (Depreciated)

void GeometryEngine::initPlaneGeometry(int size)
{
    printf("[GeometryEngine::initPlaneGeometry] WARNING : Deprecated");
    //Read height map
    //QImage heightMap;
    /*QImage heightMap(":/heightmap-1024x1024.png");

    int hHeight = heightMap.height()/size;
    int hWidth = heightMap.width()/size;
    printf("HEIGHT : %d, WIDTH : %d\n",heightMap.height(),heightMap.width());*/



    srand(time(NULL));

    //VertexData vertices[size*size];
    VertexData tempData = {QVector3D(0,0,0),QVector2D(0,0)};
    std::vector<VertexData> vertices(size*size,tempData);

    float xPos=-1.0f;
    float yPos=-1.0f;

    float z=0.0f;

    for(int i=0;i<size;i++){
        for(int j=0;j<size;j++){
          //z = float(rand())/float((RAND_MAX)) * 0.2f;
          //z = (float)qGray(heightMap.pixel(i*hHeight,j*hWidth))/255.0f*0.2f;
          vertices[(i*size)+j] = {QVector3D(xPos+(i*2.0f/(float)(size-1.0f)), yPos+(j*2.0f/(float)(size-1.0f)),  z), QVector2D((i/(float)(size-1.0f)), (j/(float)(size-1.0f)))};

        }
    }

    //GLushort indices[(size*size*2)-4];
    std::vector<GLushort> indices((size*size*2)-4,0);

    int ind=0;
    for(int k=0;k<size-1;k++){
        if(k!=0 ){
            indices[ind]=((k)*size);
            ind++;
        }

        for(int l=0;l<size;l++){

            indices[ind]=(k*size)+l;
            indices[ind+1]=((k+1)*size)+l;
            ind+=2;

        }
        //add doubles on end
        if(k!=size-2){
            indices[ind]=((k+1)*size)+size-1;
            ind++;
        }


    }

    VertexData* bufVertices = &vertices[0];
    GLushort* bufIndices = &indices[0];



    // Transfer vertex data to VBO 0
    arrayBuf.bind();
    //arrayBuf.allocate(bufVertices, 256 * sizeof(VertexData));
    arrayBuf.allocate(bufVertices, size*size * sizeof(VertexData));


    // Transfer index data to VBO 1
    indexBuf.bind();
    indexBuf.allocate(bufIndices, ((size*size*2)-4) * sizeof(GLushort));


}

void GeometryEngine::drawPlaneGeometry(QOpenGLShaderProgram *program, int size){
    printf("[GeometryEngine::drawPlaneGeometry] WARNING : Deprecated");
    // Tell OpenGL which VBOs to use
    arrayBuf.bind();
    indexBuf.bind();

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLE_STRIP, ((size*size*2)-4), GL_UNSIGNED_SHORT, 0);
}
