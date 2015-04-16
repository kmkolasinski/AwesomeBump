#ifndef DIALOGSHORTCUTS_H
#define DIALOGSHORTCUTS_H

#include <QDialog>

namespace Ui {
class DialogShortcuts;
}

class DialogShortcuts : public QDialog
{
    Q_OBJECT

public:
    explicit DialogShortcuts(QWidget *parent = 0);
    ~DialogShortcuts();

private:
    Ui::DialogShortcuts *ui;
};

#endif // DIALOGSHORTCUTS_H
