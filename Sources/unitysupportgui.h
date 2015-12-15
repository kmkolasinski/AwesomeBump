#ifndef UNITYSUPPORTGUI_H
#define UNITYSUPPORTGUI_H

#include <QWidget>
#include <QImage>
#include <QFileDialog>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDropEvent>
#include <QUrl>
#include <QDebug>


//This class is experimental. It's just for quickly changing Smoothness and Metallic values in realtime, when working on materials in unity.
// It's useful to me, but if you don't want it, just delete this.
// I need to change the overall GUI theme so that it is in sync with the other tabs.
namespace Ui {
class UnitySupportGui;
}

class UnitySupportGui : public QWidget
{
    Q_OBJECT

public:
    explicit UnitySupportGui(QWidget *parent = 0);
    ~UnitySupportGui();

private slots:
    void on_buttonLoad_clicked();

    void on_cbPreventMetallic_toggled(bool checked);

    void on_cbPreventSmoothness_toggled(bool checked);

    int determineMax(const QString&) const;
    int determineMin(const QString&) const;


    void on_sbSmoothness_editingFinished();

    void on_sbMetallic_editingFinished();


private:
    Ui::UnitySupportGui *ui;
    QImage* image;
    bool corrupted = true;
    QString filename;
    bool breset = false;

    int preMetallic;
    int preSmoothness;
    void addToMetallic(int to_apply);
    void addToSmoothness(int to_apply);
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent*);

    void reset();
    void loadImage();
    void saveImage();
};

#endif // UNITYSUPPORTGUI_H
