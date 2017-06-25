#ifndef ACTIVELABEL_H
#define ACTIVELABEL_H

#include <QLabel>
#include <QIcon>

/**
 *  @brief A lightweight button class
 *
 *  The lightweight buttons are implemented from labels that
 *  have a "push-down" behaviour. The only signal emitted is
 *  "clicked" with an integer value that can be assigned to
 *  the button.
 */
class ActiveLabel : public QLabel
{
	Q_OBJECT 
	
	public:
	  ActiveLabel (QWidget *parent);
	  ActiveLabel (int index, const char *title, const char *name = "button", QWidget *parent = 0);
	  ActiveLabel (int index, const QPixmap &icon, const char *name = "button", QWidget *parent = 0);
	
	  virtual void mousePressEvent (QMouseEvent *e);
	  virtual void mouseReleaseEvent (QMouseEvent *e);

    public slots:

	signals: 
	  void clicked (int);
	
	private:
	  int m_index;
	  bool m_grabbed;
};

#endif // ACTIVELABEL_H

