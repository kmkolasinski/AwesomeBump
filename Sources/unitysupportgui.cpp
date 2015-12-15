#include "unitysupportgui.h"
#include "ui_unitysupportgui.h"

#define MGS(str) {QMessageBox mgs(QMessageBox::Icon::Warning,tr("Warning"),str); mgs.exec();}
#define IMGS(str) {QMessageBox mgs(QMessageBox::Icon::Information,tr("Information"),str); mgs.exec();}
#define delete_(heapAlloc) if(heapAlloc != nullptr) delete heapAlloc;

UnitySupportGui::UnitySupportGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UnitySupportGui)
{
    ui->setupUi(this);
    image = new QImage();
}

UnitySupportGui::~UnitySupportGui()
{
    delete_(ui);
    delete_(image);
}

void UnitySupportGui::loadImage()
{
    delete_(image);
    if ( !(image = new QImage(filename,"PNG"))->isNull())
    {
        if(corrupted = !image->allGray()) {MGS("ERROR: The image is not grayscaled!"); return;}
    }

    else {
        MGS("ERROR: Failed to load image. Please reload.");
        corrupted = true;
        return;
    }
    IMGS(tr("Image successfully loaded"));
    reset();
}

void UnitySupportGui::on_buttonLoad_clicked()
{
    filename = QFileDialog::getOpenFileName(this,tr("Load Image"), "C:/", tr("Image Files (*.png)"));
    loadImage();
}

void UnitySupportGui::dragEnterEvent(QDragEnterEvent* event)
{
    if(event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }

    else event->ignore();
}

void UnitySupportGui::dropEvent(QDropEvent * event)
{
    filename = QString(event->mimeData()->urls().at(0).toLocalFile());
    loadImage();
}


// see addToSmoothness
void UnitySupportGui::addToMetallic(int to_apply)
{
    for (int y=0; y<image->height();y++)
    {
        for(int x = 0; x<image->width();x++)
        {
            QRgb* line_out = (QRgb *)image->scanLine(y);

            line_out += x;
            int appliedValue = qRed(*line_out)+to_apply;

            //Supposed to be a grayscaled Image;
            *line_out = qRgba(appliedValue,appliedValue,appliedValue,qAlpha(*line_out));
        }
    }
}


// compute changes. TODO: implement functionality to do this on the gpu
void UnitySupportGui::addToSmoothness(int to_apply)
{
    for (int y=0; y<image->height();y++)
    {
        for(int x = 0; x<image->width();x++)
        {
            QRgb* line_out = (QRgb *)image->scanLine(y);

            line_out += x;
            int appliedValue = qAlpha(*line_out)+to_apply;
            int c = qGreen(*line_out);

            //Supposed to be a grayscaled Image;
            *line_out = qRgba(c,c,c,appliedValue);
        }
    }
}


/*
inline int UnitySupportGui::clamp(int min, int max, int value)
{
    return std::max(min,std::min(max,value));
}
*/

//find min/max and set the value of the spinbox
void UnitySupportGui::on_cbPreventMetallic_toggled(bool checked)
{
    if(corrupted)
    {
        return;
    }

    if(checked)
    {
        int max = 255-determineMax(QString("metallic"));
        int min = -determineMin(QString("metallic"));
        ui->sbMetallic->setMaximum(max);
        ui->sbMetallic->setMinimum(min);
    }

    else {ui->sbMetallic->setMaximum(255); ui->sbMetallic->setMinimum(-255);}
}

// same as above
void UnitySupportGui::on_cbPreventSmoothness_toggled(bool checked)
{
    if(corrupted)
    {
        return;
    }


    if(checked)
    {
        int max = 255-determineMax(QString("smoothness"));
        int min = -determineMin(QString("smoothness"));
        qDebug() << max << min;
        ui->sbSmoothness->setMaximum(max);
        ui->sbSmoothness->setMinimum(min);
    }

    else {ui->sbSmoothness->setMaximum(255); ui->sbSmoothness->setMinimum(-255);}
}

// The min value of the whole picture. If you want to prevent artifacts you cant substract more than the min value.
int UnitySupportGui::determineMin(const QString& str) const
{
    unsigned char min= (str == "metallic") ? qRed(image->pixel(0,0)) : qAlpha(image->pixel(0,0));

    if(min > 0)
    {
        for(int y = 0; y < image->height();y++)
        {
            for(int x = 0; x<image->width();x++)
            {
                QRgb* line = (QRgb*) image->scanLine(y);
                line += x;
                if(str == "metallic")
                {
                    unsigned char red = qRed(*line);
                    if(red < min)
                    {
                        min = red;
                    }
                }

                else if (str=="smoothness")
                {
                    unsigned char alpha = qAlpha(*line);
                    if(alpha < min)
                    {
                        min = alpha;
                    }
                }
            }
        }
    }

    qDebug() << "Minimum found: " << min << str;

    return min;
}

// same as above
int UnitySupportGui::determineMax(const QString& str) const
{
    unsigned char max= str == "metallic" ? qRed(image->pixel(0,0)) : qAlpha(image->pixel(0,0));
    if(max < 255)
    {

        for(int y = 0; y < image->height();y++)
        {
            for(int x = 0; x<image->width();x++)
            {
                QRgb* line = (QRgb*) image->scanLine(y);
                line += x;
                if(str == "metallic")
                {
                    unsigned char red = qRed(*line);
                    if(red > max)
                    {
                        max = red;
                    }
                }

                else
                {
                    unsigned char alpha = qAlpha(*line);
                    if(alpha > max)
                    {
                        max = alpha;
                    }
                }
            }
        }
    }
    qDebug() << "Maximum found: " << max << str;
    return max;
}

void UnitySupportGui::reset()
{
    breset = true;
    ui->cbPreventMetallic->setChecked(false);
    ui->cbPreventSmoothness->setChecked(false);
    ui->sbMetallic->setMinimum(-255);
    ui->sbMetallic->setMaximum(255);
    ui->sbMetallic->setValue(0);
    ui->sbSmoothness->setMinimum(-255);
    ui->sbSmoothness->setMaximum(255);
    ui->sbSmoothness->setValue(0);
    preSmoothness = 0;
    preMetallic = 0;
    breset = false;
}

void UnitySupportGui::on_sbSmoothness_editingFinished()
{
    int value = ui->sbSmoothness->value();

    if(value == preSmoothness)
    {
        return;
    }

    ui->cbPreventSmoothness->setDisabled(true);

    if(corrupted && !breset)
    {
        MGS(tr("Your image is corrupted. Please reload."));
        return;
    }

    int to_apply = value-preSmoothness;

    addToSmoothness(to_apply);
    preSmoothness = value;
    saveImage();

}

void UnitySupportGui::saveImage()
{
    image->save(filename,"PNG");
}

void UnitySupportGui::on_sbMetallic_editingFinished()
{
    int value = ui->sbMetallic->value();
    //Nothing has changed. Could happen due to a Qt bug.
    if(value == preMetallic)
    {
        return;
    }

    ui->cbPreventMetallic->setDisabled(true);

    if(corrupted && !breset)
    {
        MGS(tr("Your image is corrupted. Please reload."));
        return;
    }


    int to_apply = value-preMetallic;
    addToMetallic(to_apply);
    preMetallic = value;
    saveImage();

}
