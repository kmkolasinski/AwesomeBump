#ifndef FORMIMAGEPROP_H
#define FORMIMAGEPROP_H

#include <QWidget>
#include <QImage>
#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QImageReader>
#include <QStandardPaths>

#include "CommonObjects.h"

namespace Ui {
class FormImageProp;
}

class FormImageProp : public QWidget
{
    Q_OBJECT

public:
    explicit FormImageProp(QWidget *parent = 0, QGLWidget* qlW_ptr = 0);
    void saveFileToDir(const QString &dir);
    void saveImageToDir(const QString &dir,QImage& image);
    void setImage(QImage image);
    void setImageName(QString name);
    void setPtrToGLWidget(QGLWidget* ptr){ imageProp.glWidget_ptr = ptr;  }
    FBOImageProporties* getImageProporties(){return &imageProp;}

    QString getImageName();

    void hideHNGroupBox();
    void hideNHGroupBox();
    void hideBMGroupBox();
    void hideSpecularGroupBox();
    void hideNormalStepBar();
    void hideSSAOBar();

    void setSpecularControlChecked();
    void reloadSettings();
    ~FormImageProp();
public slots:
    void open();
    void save();
    void updateGuiSpinBoxes(int);
    void updateSlidersOnRelease();
    void updateGuiCheckBoxes();
    void setRemoveShading(bool);
    void setRemoveShadingGaussIter(int);

    void applyHeightToNormalConversion();
    void applyNormalToHeightConversion();
    void applyBaseConversionConversion();
    void applyRecalculateOcclusion();
signals:
    void imageChanged();
    void conversionHeightToNormalApplied();
    void conversionNormalToHeightApplied();
    void conversionBaseConversionApplied();
    void recalculateOcclusion();
protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
private:
    bool loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);

    bool bLoading;
    Ui::FormImageProp *ui;
    QImage image;
    QString imageName;
    FBOImageProporties imageProp;

public:
    static QDir* recentDir;
};

#endif // FORMIMAGEPROP_H
