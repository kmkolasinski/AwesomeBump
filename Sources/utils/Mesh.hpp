/* 
 * File:   Mesh.hpp
 * Author: mkk
 * Klasa ta stanowi konetener dla klasy Actor. Sluzy do wczytywania obiektow prosto z plikow (np. obj, 3ds itp - obslugiwane przez assimp) 
 */

#ifndef Mesh_H
#define	Mesh_H


#include <QtOpenGL>
#include <QOpenGLFunctions_4_0_Core>

#include <QString>
#include <QDebug>
#include <QVector>
#include <QVector3D>
#include <iostream>
#include "qopenglerrorcheck.h"
#include "utils/tinyobj/tiny_obj_loader.h"
using namespace std;

class Mesh : public QOpenGLFunctions_4_0_Core {
public:

	/**
    * @brief Loads the OBJ mesh from folder "dir" and name "name": eq. Mesh("models/","cube.obj")
    * @param dir - mesh location
    * @param name - mesh name
	*/
    Mesh(QString dir, QString name);

	/**
    * @brief Draw the mesh if bLoaded is true otherwise does nothing
	*/
    void drawMesh(bool bUseArrays = false);
    /**
     * @brief isLoaded returns true if mesh was succsesfully loaded from file,
     * otherwise returns false.
     */
    inline bool isLoaded(){return bLoaded;}
    inline QString& getMeshLog(){return mesh_log;}
    // Cleanings
    virtual ~Mesh();    

    void initializeMesh();

    // utils
    QVector3D centre_of_mass; // it helps to aling mesh to the center of the screen
    float     radius;         // contains maxiumum distance from centre of mass to some vertex
private:       
    bool hasCommonEdge(int i, int j);
    void calculateTangents();


    QString mesh_path;
    bool bLoaded;


    // arrays
    QVector<QVector3D> gl_vertices;
    QVector<QVector3D> gl_normals;
    QVector<QVector3D> gl_smoothed_normals;
    QVector<QVector3D> gl_texcoords;
    QVector<QVector3D> gl_tangents;
    QVector<QVector3D> gl_bitangents;

    unsigned int mesh_vbos[6]; // VBO indices
    QString mesh_log;
};



#endif	/* Mesh_H */

