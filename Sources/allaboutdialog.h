#ifndef _ALLABOUTDIALOG_H_
#define _ALLABOUTDIALOG_H_

#include <QDialog>

namespace Ui {
class allAboutDialog;
}

class QGLWidget;

class AllAboutDialog : public QDialog
{
	Q_OBJECT

public:
	explicit AllAboutDialog(QWidget *parent = 0, QGLWidget *surface = 0);
	~AllAboutDialog();

  void setPixmap(const QPixmap &pixmap);
  void setPixmap(const QString &pixmap);
  void showGroupBoxInfo();
  void setInfoText(const QString &t);

private:
	Ui::allAboutDialog *ui;
};

#endif // _ALLABOUTDIALOG_H_
