#include <activelabel.h>
#include <QMouseEvent>

// --------------------------------------------------------------------
//  ActiveLabel implementation

ActiveLabel::ActiveLabel (int index, const char *title, const char *name, QWidget *parent)
  : QLabel (" "+QString(title)+" ", parent), m_index (index), m_grabbed (false)
{
  setFont(QFont("Mono Condensed"));
  setAutoFillBackground (true);
  setObjectName (QString::fromLatin1 (name));
  setFrameStyle (QFrame::Panel | QFrame::Raised);
  setBackgroundRole (QPalette::Window);
  setAlignment(Qt::AlignHCenter);
}

void ActiveLabel::mousePressEvent (QMouseEvent *e)
{
  if (! m_grabbed && e->button () == Qt::LeftButton) {
    setFrameShadow (QFrame::Sunken);
    m_grabbed = true;
  }
}

void ActiveLabel::mouseReleaseEvent (QMouseEvent *e)
{
  if (m_grabbed) {
   
    setFrameShadow (QFrame::Raised);
    m_grabbed = false;

    if (e->button () == Qt::LeftButton && rect ().contains (e->pos ())) {
      emit clicked (m_index);
    }
  }
}


