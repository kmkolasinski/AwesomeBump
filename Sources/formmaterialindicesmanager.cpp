#include "formmaterialindicesmanager.h"
#include "ui_formmaterialindicesmanager.h"

FormMaterialIndicesManager::FormMaterialIndicesManager(QMainWindow *parent, QGLWidget* qlW_ptr) :
    FormImageBase(parent),
    ui(new Ui::FormMaterialIndicesManager)
{
    ui->setupUi(this);
    imageProp.glWidget_ptr = qlW_ptr;

    connect(ui->pushButtonOpenMaterialImage,SIGNAL(released()),this,SLOT(open()));
    connect(ui->pushButtonCopyToClipboard,SIGNAL(released()),this,SLOT(copyToClipboard()));
    connect(ui->pushButtonPasteFromClipboard,SIGNAL(released()),this,SLOT(pasteFromClipboard()));
    connect(ui->checkBoxDisableMaterials,SIGNAL(toggled(bool)),this,SLOT(toggleMaterials(bool)));

    connect(ui->listWidgetMaterialIndices,SIGNAL(currentRowChanged(int)),this,SLOT(changeMaterial(int)));

    ui->groupBox->setDisabled(true);
    setAcceptDrops(true);
}

FormMaterialIndicesManager::~FormMaterialIndicesManager()
{
    qDebug() << "calling" << Q_FUNC_INFO;
    delete ui;
}

bool FormMaterialIndicesManager::isEnabled(){
    return (FBOImageProporties::currentMaterialIndeks != MATERIALS_DISABLED);
}

void FormMaterialIndicesManager::disableMaterials(){
    FBOImageProporties::currentMaterialIndeks = MATERIALS_DISABLED;
}


void FormMaterialIndicesManager::setImage(QImage _image){

    if (imageProp.glWidget_ptr->isValid()){
        // remember the last id
        int mIndex = FBOImageProporties::currentMaterialIndeks;
        if(updateMaterials(_image)){
              image    = _image;
              imageProp.init(image);
              emit materialChanged();
        }

        FBOImageProporties::currentMaterialIndeks = mIndex ;

    }
    else
        qDebug() << Q_FUNC_INFO << "Invalid context.";
}

bool FormMaterialIndicesManager::updateMaterials(QImage& image){
    bSkipUpdating = true;

    // Calculate image color map
    colorIndices.clear();
    for(int w = 0 ; w < image.width() ; w++){
        for(int h = 0 ; h < image.height() ; h++){
            QRgb pixel          = image.pixel(w,h);
            QColor bgColor = QColor(pixel);
            int indeks = bgColor.red()*255*255 + bgColor.green()*255 + bgColor.blue();
            colorIndices[indeks] = pixel;

    }}
    if(colorIndices.size() > 32){
    QMessageBox msgBox;
        msgBox.setText("Error: too much colors!");
        msgBox.setInformativeText(" Sorry, but this image does not look like a material texture.\n"
                                  " Your image contains more than 32 different colors");
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.exec();
        bSkipUpdating = false;
        return false;
    }

    typedef std::map<int,QRgb>::iterator it_type;
    ui->listWidgetMaterialIndices->clear();

    // generate materials list
    int indeks = 1;
    for(it_type iterator = colorIndices.begin(); iterator != colorIndices.end(); iterator++) {
           qDebug() << "Material index:  " << iterator->first << " Color :" << QColor(iterator->second);

           QListWidgetItem* pItem =new QListWidgetItem("Material"+QString::number(indeks++));
           QColor mColor = QColor(iterator->second);
           pItem->setForeground(mColor); // sets red text
           pItem->setBackground(mColor); // sets green background
           QColor textColor = QColor(255-mColor.red(),255-mColor.green(),255-mColor.blue());
           pItem->setTextColor(textColor);
           ui->listWidgetMaterialIndices->addItem(pItem);

    }


    qDebug() << "Updating material indices. Total indices count:" << ui->listWidgetMaterialIndices->count();
    for(int i = 0 ; i < MATERIAL_TEXTURE ; i++){
        materialIndices[i].clear();
        for(int m = 0 ; m < ui->listWidgetMaterialIndices->count() ; m++){
            QString m_name = ui->listWidgetMaterialIndices->item(m)->text();
            FBOImageProporties tmp;
            tmp.copySettings(imagesPointers[i]->imageProp);
            materialIndices[i][m_name] = tmp;
        }
    }


    lastMaterialIndex = 0;
    QString cText = ui->listWidgetMaterialIndices->item(lastMaterialIndex)->text();
    ui->listWidgetMaterialIndices->item(lastMaterialIndex)->setText(cText+" (selected material)");

    QColor bgColor = ui->listWidgetMaterialIndices->item(lastMaterialIndex)->backgroundColor();
    FBOImageProporties::currentMaterialIndeks = bgColor.red()*255*255 + bgColor.green()*255 + bgColor.blue();

    bSkipUpdating = false;



    return true;
}


void FormMaterialIndicesManager::changeMaterial(int index){
    if(bSkipUpdating) return;
    // copy current settings
    ui->listWidgetMaterialIndices->item(lastMaterialIndex)->setText("Material"+QString::number(lastMaterialIndex+1));

    QString m_name = ui->listWidgetMaterialIndices->item(lastMaterialIndex)->text();
    for(int i = 0 ; i < MATERIAL_TEXTURE ; i++){
        materialIndices[i][m_name].copySettings(imagesPointers[i]->imageProp);
    }

    lastMaterialIndex = index;

    // update current mask color
    QColor bgColor = ui->listWidgetMaterialIndices->item(lastMaterialIndex)->backgroundColor();
    FBOImageProporties::currentMaterialIndeks = bgColor.red()*255*255 + bgColor.green()*255 + bgColor.blue();

    // load different material
    m_name = ui->listWidgetMaterialIndices->item(index)->text();
    for(int i = 0 ; i < MATERIAL_TEXTURE ; i++){
        imagesPointers[i]->imageProp.copySettings(materialIndices[i][m_name]);
        imagesPointers[i]->reloadSettings();
    }
    QString cText = ui->listWidgetMaterialIndices->item(lastMaterialIndex)->text();
    ui->listWidgetMaterialIndices->item(lastMaterialIndex)->setText(cText+" (selected material)");
    emit materialChanged();
}




bool FormMaterialIndicesManager::loadFile(const QString &fileName)
{
    QFileInfo fileInfo(fileName);
    QImage _image;

    // Targa support added
    if(fileInfo.completeSuffix().compare("tga") == 0){
        TargaImage tgaImage;
        _image = tgaImage.read(fileName);
    }else{
        QImageReader loadedImage(fileName);
        _image = loadedImage.read();
    }

    if (_image.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load material image %1.").arg(QDir::toNativeSeparators(fileName)));
        return false;
    }

    qDebug() << "<FormImageProp> Open material image:" << fileName;


    (*FormImageProp::recentDir).setPath(fileName);

    int mIndex = FBOImageProporties::currentMaterialIndeks;
    if(updateMaterials(_image)){
          image    = _image;
          imageProp.init(image);
          emit materialChanged();
          FBOImageProporties::currentMaterialIndeks = mIndex;
          emit imageLoaded(image.width(),image.height());
          // repaint all materials
          if(FBOImageProporties::currentMaterialIndeks != MATERIALS_DISABLED){
              toggleMaterials(true);
          }
    }
    return true;
}

void FormMaterialIndicesManager::pasteImageFromClipboard(QImage& _image){


    int mIndex = FBOImageProporties::currentMaterialIndeks;
    if(updateMaterials(_image)){
          image    = _image;
          imageProp.init(image);
          emit materialChanged();
          FBOImageProporties::currentMaterialIndeks = mIndex;
          emit imageLoaded(image.width(),image.height());
          // repaint all materials
          if(FBOImageProporties::currentMaterialIndeks != MATERIALS_DISABLED){
              toggleMaterials(true);
          }
    }
}

void FormMaterialIndicesManager::toggleMaterials(bool toggle){

    if(toggle == false){
        FBOImageProporties::currentMaterialIndeks = MATERIALS_DISABLED; // render normaly
        emit materialChanged();
    }else{ // if material group is enabled -> replot all materials

        int lastMaterial = lastMaterialIndex;
        // repaint all materials
        for(int m = 0 ; m < ui->listWidgetMaterialIndices->count() ; m++){
            QCoreApplication::processEvents();
            changeMaterial(m);
        }
        changeMaterial(lastMaterial);
    }

    emit  materialsToggled(toggle);

}


void FormMaterialIndicesManager::chooseMaterialByColor(QColor color){
    // check if materials are enabled
    if(FBOImageProporties::currentMaterialIndeks == MATERIALS_DISABLED) return;

    bool bColorFound = false;
    // look for the color in materials
    for(int m = 0 ; m < ui->listWidgetMaterialIndices->count() ; m++){
        // get the material color
        QColor materialColor = ui->listWidgetMaterialIndices->item(m)->backgroundColor();
        int compValue = abs(materialColor.red() - color.red())
                     +  abs(materialColor.green() - color.green())
                     +  abs(materialColor.blue() - color.blue());

        // if color are same change material
        if(compValue == 0){
            QCoreApplication::processEvents();
            changeMaterial(m);
            bColorFound = true;
            break;
        }

    } // end of for

    // if color not found on the list
    if(!bColorFound){
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 "The picked color was not found in the materials table\n"
                                 "make sure you chose the correct color e.g\n"
                                 "not the sky box background.");
    }
}


void FormMaterialIndicesManager::pasteFromClipboard(){
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    if (mimeData->hasImage()) {
        qDebug() << "<FormImageProp> Image :"+
                    PostfixNames::getTextureName(imageProp.imageType)+
                    " loaded from clipboard.";
        QPixmap pixmap = qvariant_cast<QPixmap>(mimeData->imageData());
        QImage image = pixmap.toImage();
        pasteImageFromClipboard(image);

    }
}
void FormMaterialIndicesManager::copyToClipboard(){

    qDebug() << "<FormImageProp> Image :"+
                PostfixNames::getTextureName(imageProp.imageType)+
                " copied to clipboard.";

    QApplication::processEvents();
    image = imageProp.getImage();
    QApplication::clipboard()->setImage(image,QClipboard::Clipboard);
}
