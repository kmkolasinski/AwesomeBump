#ifndef FORMMATERIALINDICESMANAGER_H
#define FORMMATERIALINDICESMANAGER_H


#include "formimageprop.h"
#include "formimagebase.h"

namespace Ui {
class FormMaterialIndicesManager;
}
typedef std::map<int,QRgb>::iterator it_type;
class FormMaterialIndicesManager : public FormImageBase
{
    Q_OBJECT

public:
    FormMaterialIndicesManager(QMainWindow *parent = 0, QOpenGLWidget *qlW_ptr = 0 );

    void setImage(QImage image);


    ~FormMaterialIndicesManager();

    // Counts colors and manages material masking
    bool updateMaterials(QImage &_image);
    bool isEnabled();
    void disableMaterials();

    // just pointers to images
    FormImageProp* imagesPointers[7];


public slots:
    void changeMaterial(int index);
    void pasteFromClipboard();
    void copyToClipboard();
    void toggleMaterials(bool toggle);// enable disable materials
    void chooseMaterialByColor(QColor color);// takes a color then searches for similar in materials table
signals:
    void materialChanged();
    void imageLoaded(int width,int height);
    void materialsToggled(bool);
protected:

    bool loadFile(const QString &fileName);
    void pasteImageFromClipboard(QImage& image);





    // keep all the settings in one place
    std::map<QString,FBOImageProporties> materialIndices[7];
    std::map<int,QRgb> colorIndices;
    int lastMaterialIndex;
    Ui::FormMaterialIndicesManager *ui;
    bool bSkipUpdating;
};

#endif // FORMMATERIALINDICESMANAGER_H
