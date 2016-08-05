
#include <QGLWidget>
#include <QtOpenGL>
#include <QDebug>
#include "contextwidget.h"
#include "allaboutdialog.h"
#include "ui_allaboutdialog.h"
#include "CommonObjects.h"

AllAboutDialog::AllAboutDialog(QWidget *parent, QGLWidget *surface) :
	QDialog(parent),
	ui(new Ui::allAboutDialog)
{
	ui->setupUi(this);
  	ui->groupBoxInfo->hide();

	ui->version->setText(AWESOME_BUMP_VERSION);

	QOpenGLContext *ctx = 0;
	if (surface) {
	  surface->makeCurrent();
	  ctx = surface->context()->contextHandle();
	} else
	  ctx = QOpenGLContext::currentContext();

  if (ctx == 0) 
  {
    QGLWidget *gl = new QGLWidget;
    ctx = gl->context()->contextHandle();
    gl->makeCurrent();
    gl->deleteLater(); qDebug() << ctx;
  }

  GLfloat GL_MaxAnisotropy = 0; glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &GL_MaxAnisotropy);

  QString Anisotropic = GL_MaxAnisotropy>0 ? tr("supported (max %1)").arg(GL_MaxAnisotropy) : tr("Not supported");
	QString VersionFormat = tr("OpenGL Version %1\nGLSL Version %2\n%3 - %4\n\n");
	QString Version = VersionFormat.arg(QString((const char*)glGetString(GL_VERSION)), QString((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)), QString((const char*)glGetString(GL_RENDERER)), QString((const char*)glGetString(GL_VENDOR)));
	QString BuffersFormat = tr("Color Buffer: %1 bits %2\nDepth Buffer: %4 bits\nStencil Buffer: %5 bits\nAnisotropy: %6\n\n");
	QString Buffers = BuffersFormat.arg(
    QString::number(ctx->format().redBufferSize() + ctx->format().greenBufferSize() + ctx->format().blueBufferSize() + ctx->format().alphaBufferSize()), 
    ctx->format().swapBehavior() == QSurfaceFormat::DoubleBuffer ? tr("double buffered") : 
      (ctx->format().swapBehavior() == QSurfaceFormat::TripleBuffer ? tr("triple buffered") : QString()), 
    QString::number(ctx->format().depthBufferSize()), 
    QString::number(ctx->format().stencilBufferSize()), 
    Anisotropic);

	QString ExtensionsFormat = tr("Features:\n  - Multitexture: %1\n  - Shaders: %2\n  - Buffers: %3\n  - Framebuffers: %4\n  - BlendColor: %5\n  - BlendEquation: %6\n  - BlendEquationSeparate: %7\n  - BlendSubtract: %8\n  - CompressedTextures: %9\n  - Multisample: %10\n  - StencilSeparate: %11\n  - NPOTTextures: %12\n  - NPOTTextureRepeat: %13\n  - FixedFunctionPipeline: %14");

  QOpenGLFunctions *f = ctx->functions();
	QString Extensions = ExtensionsFormat
    .arg(f->hasOpenGLFeature(QOpenGLFunctions::Multitexture))
    .arg(f->hasOpenGLFeature(QOpenGLFunctions::Shaders))
    .arg(f->hasOpenGLFeature(QOpenGLFunctions::Buffers))
    .arg(f->hasOpenGLFeature(QOpenGLFunctions::Framebuffers))
    .arg(f->hasOpenGLFeature(QOpenGLFunctions::BlendColor))
    .arg(f->hasOpenGLFeature(QOpenGLFunctions::BlendEquation))
    .arg(f->hasOpenGLFeature(QOpenGLFunctions::BlendEquationSeparate))
    .arg(f->hasOpenGLFeature(QOpenGLFunctions::BlendSubtract))
    .arg(f->hasOpenGLFeature(QOpenGLFunctions::CompressedTextures))
    .arg(f->hasOpenGLFeature(QOpenGLFunctions::Multisample))
    .arg(f->hasOpenGLFeature(QOpenGLFunctions::StencilSeparate))
    .arg(f->hasOpenGLFeature(QOpenGLFunctions::NPOTTextures))
    .arg(f->hasOpenGLFeature(QOpenGLFunctions::NPOTTextureRepeat))
    .arg(f->hasOpenGLFeature(QOpenGLFunctions::FixedFunctionPipeline));

	ui->info->setText(Version + Buffers + Extensions);
}

void AllAboutDialog::setText(const QString &t)
{
  ui->groupBoxInfo->setTitle(t);
  ui->groupBoxInfo->setVisible(!ui->labelInfo->text().isEmpty());  
}

void AllAboutDialog::setInformativeText(const QString &t)
{
  ui->labelInfo->setText(t);
  ui->groupBoxInfo->setVisible(!ui->labelInfo->text().isEmpty());  
}

void AllAboutDialog::setPixmap(const QPixmap &pixmap)
{
  ui->pixmap->setPixmap( pixmap );  
}

void AllAboutDialog::setPixmap(const QString &pixmap)
{
  ui->pixmap->setPixmap( QPixmap(pixmap) );
}

AllAboutDialog::~AllAboutDialog()
{
	delete ui;
}
