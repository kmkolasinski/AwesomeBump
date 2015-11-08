#include "camera.h"


//-----------------------------------------------------------------------------
AwesomeCamera::AwesomeCamera(){
        position		 = QVector3D(0,0,0);
        direction        = QVector3D(0,0,1);
        side_direction   = QVector3D(1,0,0);
        updown_direction = QVector3D(0,1,0);
        //------------------------------------
        key_sens   = 5.0f;
        mouse_sens = 0.3f;
        //------------------------------------
        rot_angles[0] = rot_angles[1] = rot_angles[2] = 0;
        rotM.setToIdentity(); // tworzymy macierz jednostkowa
        rot_angles[0] = 0;
        rot_angles[1] = 0;
        isFree = true; // domyslnie kamera jest wolna
        radius = 1.0;

}
AwesomeCamera::~AwesomeCamera(){
}



/**
*Odnawia pozycje kamery
*/
QMatrix4x4 AwesomeCamera::updateCamera(void){
    QMatrix4x4 unit_mat;
    unit_mat.setToIdentity();
    if(isFree){
        unit_mat.lookAt(position,
                position+direction,
                updown_direction
               );

    }else{

     unit_mat.lookAt(position - (radius)*direction,
                position - (radius)*direction + direction,
                updown_direction
               );
    }

    return unit_mat;
}
QVector3D AwesomeCamera::get_position(){
    if(isFree){
        return position;
    }else{
        return position - (radius)*direction;
    }
}

void AwesomeCamera::toggleFreeCamera(bool free){
    isFree = free;
}

void AwesomeCamera::moveForward(float speed){

       if(isFree)position+=direction*speed*key_sens;

}

void AwesomeCamera::moveBackward(float speed){

       if(isFree)position-=direction*speed*key_sens;

}

void AwesomeCamera::moveLeft(float speed){

      if(isFree)position+=side_direction*speed*key_sens;

}

void AwesomeCamera::moveRight(float speed){

      if(isFree)position-=side_direction*speed*key_sens;

}

void AwesomeCamera::moveUp(float speed){

      if(isFree)position+=updown_direction*speed*key_sens;

}

void AwesomeCamera::moveDown(float speed){

      if(isFree)position-=updown_direction*speed*key_sens;

}



void AwesomeCamera::rotateView(float z_angle,float x_angle){

//    rotM.setToIdentity();


    double cosPhi = cos(mouse_sens*(-z_angle)/180*M_PI);
    double sinPhi = sin(mouse_sens*(-z_angle)/180*M_PI);


    direction      = QVector3D(cosPhi*direction.x()+sinPhi*direction.z(),direction.y(),
                               cosPhi*direction.z()-sinPhi*direction.x());

    QMatrix4x4 rotMat;
    rotMat.setToIdentity();
    rotMat.rotate(mouse_sens*(-x_angle),QVector3D::crossProduct(direction,QVector3D(0,1,0)));
    QVector3D tmpVec = (rotMat*QVector4D(direction)).toVector3D();
    tmpVec.normalize();
    double angleTheta = QVector3D::dotProduct(tmpVec,QVector3D(0,1,0));
    if(qAbs(angleTheta) < 0.9){
        rotMat.setToIdentity();
        rotMat.rotate(mouse_sens*(-x_angle)*(1-qAbs(angleTheta)),QVector3D::crossProduct(direction,QVector3D(0,1,0)));
        QVector3D tmpVec = (rotMat*QVector4D(direction)).toVector3D();
        tmpVec.normalize();
        direction = tmpVec;
    }

    side_direction = QVector3D(cosPhi*side_direction.x()+sinPhi*side_direction.z(),0,
                               cosPhi*side_direction.z()-sinPhi*side_direction.x());

    updown_direction = QVector3D::crossProduct(direction,side_direction);



/*
    rot_angles[0] += mouse_sens*(z_angle);//przesuniecie X
    rot_angles[1] -= mouse_sens*(x_angle);//przesuniecie Y
    if(rot_angles[1] > 90) rot_angles[1] = 90;
    if(rot_angles[1] <-90) rot_angles[1] = -90;
//    przesuniecie do przodu
    direction = QVector3D(-sin(rot_angles[0]/180*M_PI),sin(rot_angles[1]/180*M_PI),cos(rot_angles[0]/180*M_PI));
//    przesuniece na boki
    side_direction = QVector3D(sin((rot_angles[0]+90)/180*M_PI),0,-cos((rot_angles[0]+90)/180*M_PI));
//    przesuwanie gora dol
    updown_direction = QVector3D::crossProduct(direction,side_direction);
*/

    direction.normalize();
    side_direction.normalize();
    updown_direction.normalize();

}

void AwesomeCamera::reset(){
    radius = 1;
    position		 = QVector3D(0,0,0);
    direction        = QVector3D(0,0,1);
    side_direction   = QVector3D(1,0,0);
    updown_direction = QVector3D(0,1,0);
}

void AwesomeCamera::mouseWheelMove(int direction){
    radius+=mouse_sens*0.0025f*direction;
    if(radius < 0.3) radius = 0.3;
}

void AwesomeCamera::setMouseSensitivity(int value){
    mouse_sens = value / 100.0;
}
