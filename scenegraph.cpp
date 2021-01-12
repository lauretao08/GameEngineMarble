#include "scenegraph.h"
#include <iostream>
#include <QVector3D>


SceneGraph::SceneGraph(){
    rootdefined = false;
    currentTime = GetCurrentTime();
    updateCurrentTime();
}

void SceneGraph::AddRoot(SceneGraphNode root, SceneGraphNode *addrRoot){
    if(rootdefined){
        printf("[SceneGraph::AddRoot] WARNING : Redefining scene root");
    }
    this->root=root;
    graph.push_back(root);
    this->addrRoot = addrRoot;
    addrGraph.push_back(addrRoot);
    rootdefined = true;
}

void SceneGraph::AddNode(SceneGraphNode node, SceneGraphNode *addrNode){
    graph.push_back(node);
    addrGraph.push_back(addrNode);
}


SceneGraphNode SceneGraph::getNode(int objectID){
    return graph[objectID];
}

void SceneGraph::addForce(int objectID, QVector3D force){

    graph[objectID].velocity+=force;

}

void SceneGraph::addTranslation(int objectID,Translation translation){
    Transform t = graph[objectID].getTransform();
    t.addTranslation(translation);
    graph[objectID].setTransform(t);
}

void SceneGraph::addRotation(int objectID,Rotation rotation){
    Transform t = graph[objectID].getTransform();
    t.setRotation(rotation);
    graph[objectID].setTransform(t);
}

void SceneGraph::setVelocity(int objectID, QVector3D velocity)
{
    graph[objectID].velocity=velocity;
}

void SceneGraph::addVelocity(int objectID, QVector3D velocity)
{
    graph[objectID].velocity+=velocity;
}


void SceneGraph::displaySceneElements(QOpenGLShaderProgram *program, GeometryEngine *geometries, QMatrix4x4 projection, Rotation rotation){
    if(graph.size()<1){
        std::cout << "[SceneGraph::displaySceneElements] ERROR : Empty Graph !" << std::endl;
    }
    if(!rootdefined){
        std::cout << "[SceneGraph::displaySceneElements] ERROR : Undefined Graph Root !" << std::endl;
    }
    SceneGraphNode current = root;
    SceneGraphNode *currentAddr = addrRoot;
    std::vector<int> currentOrderedChildren;
    currentOrderedChildren.push_back(0);

    bool ongoing=true;

    QMatrix4x4 matrix;

    /* Calcul dt for update force */
    /*previousTime = currentTime;
    currentTime = GetCurrentTime();
    float delta_t = currentTime - previousTime;
    std::cout<<"PrviousTime : "<<previousTime<<"   CurrentTime : "<<currentTime<<"   delta_t : "<<delta_t<<std::endl;
    if(delta_t >0.15f) delta_t = 0.15f; //lock down dt*/


    while(ongoing){
        matrix.setToIdentity();
        matrix.translate(0.0, 0.0, -5.0);
        matrix.rotate(rotation);

        matrix.translate(current.getTransform().getTranslation());
        matrix.rotate(current.getTransform().getRotation());
        matrix.scale(current.getTransform().getScaling());

        //******** UpdateForce ******
        //updateForce(matrix, current, delta_t);

        // Set modelview-projection matrix
        program->setUniformValue("mvp_matrix", projection * matrix);


        if(current.isDrawable()){
            geometries->drawGeometry(current.getType(),program);
        }

        for(unsigned int i=0;i<graph.size();i++){
            //std::cout <<"current addr : " << currentAddr << ", graph["<< i<<"].getParent() : " << graph[i].getParent() << std::endl;
            if(graph[i].getParent()==currentAddr){
                currentOrderedChildren.push_back(i);
            }
        }

        currentOrderedChildren.erase(currentOrderedChildren.begin());
        //then remove current from remaining nodes to render, and pick the next one
        if(currentOrderedChildren.size()>=1){
            current = graph[currentOrderedChildren[0]];
            currentAddr = addrGraph[currentOrderedChildren[0]];
        }else{
            ongoing=false;
        }

    }
}

void SceneGraph::updateForce(float delta_t){
    /* à Mettre à jour les position / velocity
        p = p + v * dt;
        v = v + friction * dt + gravity * dt;
    */
    //matrix.translate(0.0,0.0,0.0);
    delta_t/=1000.0;
    QVector3D delta_p = (getNode(MAIN_NODE_ID).velocity + getNode(MAIN_NODE_ID).gravity) * delta_t;
    //std::cout<<"Delta p  : ("<<delta_p.x()<<","<<delta_p.y()<<","<<delta_p.z()<<")"<<std::endl;
    //std::cout<<"Velocity : ("<< getNode(MAIN_NODE_ID).velocity.x()<<","<< getNode(MAIN_NODE_ID).velocity.y()<<","<< getNode(MAIN_NODE_ID).velocity.z()<<")"<<std::endl;
    //std::cout<<delta_t<<std::endl;
    //matrix.translate(delta_p);
    //getNode(MAIN_NODE_ID).getTransform().addTranslation(delta_p);
    addTranslation(MAIN_NODE_ID,delta_p);

   //**** Mettre à jour la vitesse ****
   if(getNode(MAIN_NODE_ID).velocity.lengthSquared()>VELOCITY_THRESHOLD){
       QVector3D friction = FRICTION_STRENGTH * getNode(MAIN_NODE_ID).velocity;
       addVelocity(MAIN_NODE_ID,friction * delta_t);
   }else{
       setVelocity(MAIN_NODE_ID,QVector3D(0.0,0.0,0.0));
   }



}

void SceneGraph::manageCollision(){
    std::vector<int> collisions;
    for(int i=0;i<graph.size();i++){
        Translation ContactPoint=Translation(0.0,0.0,0.0);
        if(isColliding(MAIN_NODE_ID,i,&ContactPoint)){
            collisions.push_back(i);
            //std::cout<<"Contact : ("<< ContactPoint.x() <<","<<ContactPoint.y() <<","<<ContactPoint.z() <<")"<<std::endl;

            //ContactPoint-Centre => Direction de propulsion
            QVector3D force = ContactPoint-getNode(MAIN_NODE_ID).getTransform().getTranslation();
            //Rayon de la sphere - Norme de la direction => Puissance souhaité
            float puissance =(float)getNode(MAIN_NODE_ID).getTransform().getScaling().x() - (float)force.length();
            force.normalize();
            addTranslation(MAIN_NODE_ID,-force*puissance);
            force*=(float)puissance * BOUNCE_MODIFIER;
            std::cout<<"Force : ("<< force.x() <<","<<force.y() <<","<<force.z() <<")"<<std::endl;

            addForce(MAIN_NODE_ID,-force);
        }
    }
}

bool SceneGraph::isColliding(int id_a,int id_b,Translation *contactPoint){
    if(!getNode(id_a).isCollidable() || !getNode(id_b).isCollidable()){
        return false;
    }
    if(id_a == id_b){
        return false;
    }
    SceneGraphNode SGN_a = getNode(id_a);
    SceneGraphNode SGN_b = getNode(id_b);
    if(SGN_a.getType()==objectType::SPHERE){
        if(SGN_b.getType()==objectType::SPHERE){
            //Sphere X Sphere collision

            //std::cout<<"<"<<id_a<<","<<id_b<<"> SphereXSphere collision"<<std::endl;
            //Distance between the two centers < 2 radius
            Translation center_A = SGN_a.getTransform().getTranslation();
            Translation center_B = SGN_b.getTransform().getTranslation();
            double C = sqrt( pow( center_A.x()- center_B.x(), 2 ) +
                         pow( center_A.y()- center_B.y(), 2 ) +
                         pow( center_A.z()- center_B.z(), 2 ) );
            double R =  SGN_a.getTransform().getScaling().x() + SGN_b.getTransform().getScaling().x();
            //std::cout <<"C = "<<C<<"/R = "<<R<<std::endl;
            if(C<R){
                *contactPoint=center_B-center_A; //NOT COMPLETE
            }
            return (C<R);

        }else if(SGN_b.getType()==objectType::CUBE){
            //Sphere X Cube

            //std::cout<<"<"<<id_a<<","<<id_b<<"> SphereXCube collision"<<std::endl;

            Translation closestPoint = SGN_b.getTransform().getTranslation();
            QVector3D direction = SGN_a.getTransform().getTranslation() - closestPoint;
            QMatrix3x3 rotation = SGN_b.getTransform().getRotationAsMatrix();
            for (int i = 0; i < 3; ++i) { //Going direction by direction
                QVector3D axis(rotation(i,0),rotation(i,1),rotation(i,2));
                float distance = QVector3D::dotProduct(direction, axis);
                //std::cout<<"Distance"<<distance<<std::endl;

                if (distance > SGN_b.getTransform().getTranslation()[i] + SGN_b.getTransform().getScaling()[i]) {
                    distance = SGN_b.getTransform().getTranslation()[i] + SGN_b.getTransform().getScaling()[i];
                }
                if (distance < -(SGN_b.getTransform().getTranslation()[i] + SGN_b.getTransform().getScaling()[i])) {
                    distance = -(SGN_b.getTransform().getTranslation()[i] + SGN_b.getTransform().getScaling()[i]);
                }

                closestPoint = closestPoint + (axis * distance);
                //std::cout <<"ClosestPoint"<< closestPoint.x() <<","<<closestPoint.y()<<","<<closestPoint.z()<<std::endl;
            }
            float distSq = QVector3D::dotProduct(SGN_a.getTransform().getTranslation() - closestPoint,SGN_a.getTransform().getTranslation() - closestPoint);  //Dot product of himself
            float radiusSq = SGN_a.getTransform().getScaling().x() * SGN_a.getTransform().getScaling().x();
            //std::cout <<"Dist = "<<distSq<<"/Rad = "<<radiusSq<<std::endl;

            if(distSq < radiusSq){
                *contactPoint=closestPoint;
            }
            return distSq < radiusSq;
        }
    }else if(SGN_a.getType()==objectType::CUBE){
        if(SGN_b.getType()==objectType::SPHERE){
            //Cube X Sphere
            //std::cout<<"<"<<id_a<<","<<id_b<<"> CubeXSphere collision"<<std::endl;

            Translation closestPoint = SGN_a.getTransform().getTranslation();
            QVector3D direction = SGN_b.getTransform().getTranslation() - closestPoint;
            QMatrix3x3 rotation = SGN_a.getTransform().getRotationAsMatrix();
            for (int i = 0; i < 3; ++i) { //Going direction by direction
                QVector3D axis(rotation(i,0),rotation(i,1),rotation(i,2));
                float distance = QVector3D::dotProduct(direction, axis);
                //std::cout<<"Distance"<<distance<<std::endl;

                if (distance > SGN_a.getTransform().getTranslation()[i] + SGN_a.getTransform().getScaling()[i]) {
                    distance = SGN_a.getTransform().getTranslation()[i] + SGN_a.getTransform().getScaling()[i];
                }
                if (distance < -(SGN_a.getTransform().getTranslation()[i] + SGN_a.getTransform().getScaling()[i])) {
                    distance = -(SGN_a.getTransform().getTranslation()[i] + SGN_a.getTransform().getScaling()[i]);
                }

                closestPoint = closestPoint + (axis * distance);
                //std::cout <<"ClosestPoint"<< closestPoint.x() <<","<<closestPoint.y()<<","<<closestPoint.z()<<std::endl;
            }
            float distSq = QVector3D::dotProduct(SGN_b.getTransform().getTranslation() - closestPoint,SGN_b.getTransform().getTranslation() - closestPoint);  //Dot product of himself
            float radiusSq = SGN_b.getTransform().getScaling().x() * SGN_b.getTransform().getScaling().x();
            //std::cout <<"Dist = "<<distSq<<"/Rad = "<<radiusSq<<std::endl;
            return distSq < radiusSq;

        }else if(SGN_b.getType()==objectType::CUBE){
            //Cube X Cube
            //std::cout<<"<"<<id_a<<","<<id_b<<"> CubeXCube collision"<<std::endl;
            //Todo
        }
    }
    return false;
}
