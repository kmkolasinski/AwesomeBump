#ifndef CAMERA_H
#define CAMERA_H


#include <iostream>
#include <cmath>
#include <QMatrix4x4>
#include <QVector3D>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

using namespace std;
////////////////////////////////////////////////////////////////////////////////
class AwesomeCamera
{
public:
    AwesomeCamera();
    ~AwesomeCamera();

    /**
    * Zwraca macierz widoku kamery: ViewMatrix
    */
    QMatrix4x4 updateCamera(void);
    /**
    * Zmienia stan kamery na Free albo zwiazany (WSAD - nie dziala)
    */
    void toggleFreeCamera(bool free);
    bool isFreeCamera(){return isFree;}
    void mouseWheelMove(int direction);
    void reset();

    void moveForward(float speed);  // ruch do przodu - zwykle klawisz W
    void moveBackward(float speed); // ruch do tylu - klawisz S
    void moveLeft(float speed);     // ruch w lewo - zwykle klawisz A
    void moveRight(float speed);    // ruch w prawo - zwykle klawisz D
    void moveUp(float speed);       // ruch do gory
    void moveDown(float speed);     // ruch w dol
    /**
    * Obraca kamere zgodnie z wgledna zmiana polozenia kursora na ekranie.
    * @param delta_x - zmiana polozenia kursora w osi x ekranu (wyrazona w pikselach)
    * @param delta_y - zmiana polozenia kursora w osi y ekranu (wyrazona w pikselach)
    */
    void rotateView(float delta_x,float delta_y);
    /**
    * Funkcja zwraca wektor polozenia kamery w ukladzie gdzie os z jest skierowana do gory.
    */
    QVector3D get_world_position(){ return QVector3D(position.x(),-position.z(),position.y()); }
    QVector3D get_position();
    /**
    * Funkcja zwraca wektor kierunku patrzenia kamery w ukladzie gdzie os z jest skierowana do gory.
    */
    QVector3D get_world_direction(){ return QVector3D(direction.x(),-direction.z(),direction.y()); }
    QVector3D get_world_up(){ return QVector3D(updown_direction.x(),-updown_direction.z(),updown_direction.y()); }

    void setMouseSensitivity(int value);

public:
    QMatrix3x3      rotM;
    QVector3D	    position;           // pozycja obserwatora
    QVector3D	    direction;          // kierunek patrzenia
    QVector3D	    side_direction;     //kierunek chodzenia na bok
    QVector3D	    updown_direction;   //kierunek chodzenia w gora-dol
    float radius; // w przypadku, gdy isFree == false, zmienia odleglosc obiektu obserwowanego od kamery-

private:
    float           key_sens;   // czulosc klawiszy
    float           mouse_sens; // czulosc myszy
    bool isFree;  // czy mozemy poruszac kamera swobodnie za pomoca klawiszow WSAD, czy tylko obracamy kamera wokol polozenia

    float           rot_angles[3];
};
#endif // CAMERA_H
