
#include <qopengl.h>

#include <qopenglext.h>

#include <QDebug>
#include <QDateTime>
#include <QScreen>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QLibraryInfo>
#include <QApplication>

#include "contextwidget.h"
#include "allaboutdialog.h"
#include "ui_allaboutdialog.h"

#include "CommonObjects.h"

#ifndef GL_MAX_FRAMEBUFFER_WIDTH
# define GL_MAX_FRAMEBUFFER_WIDTH          0x9315
#endif
#ifndef GL_MAX_FRAMEBUFFER_HEIGHT
# define GL_MAX_FRAMEBUFFER_HEIGHT         0x9316
#endif
#ifndef GL_MAX_COMPUTE_UNIFORM_COMPONENTS
# define GL_MAX_COMPUTE_UNIFORM_COMPONENTS 0x8263
#endif
#ifndef GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS
# define GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS 0x91BC
#endif

QString qs(const QRect& d) { return QString::fromLatin1("%1 %2 %3 %4").arg(d.x()).arg(d.y()).arg(d.width()).arg(d.height()); }
QString qs(const QSize& d) { return QString::fromLatin1("%1 %2").arg(d.width()).arg(d.height()); }
QString qs(const QSizeF& d) { return QString::fromLatin1("%1 %2").arg(d.width()).arg(d.height()); }
QString qs(const void *d) { return QString::fromLatin1("%1").arg(intptr_t(d)); }

#define qn(X) QString::number(X)

AllAboutDialog::AllAboutDialog(QOpenGLContext *ctx, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::allAboutDialog)
{
	ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, [](){
        qApp->quit();
    });

    ui->infoBox->hide();
  	ui->labelInfo->hide();

	ui->version->setText(AWESOME_BUMP_VERSION);

    QString Info;

    Info += "Build-info\n" ;
    Info += "------------------------------------------------------\n" ;
    Info += "Build time: " + QStringLiteral(__DATE__) +  " " + QStringLiteral(__TIME__) + "\n";
    Info += "Qt version: " + QStringLiteral(QT_VERSION_STR) + (QLibraryInfo::isDebugBuild()?" (DEBUG)":" (RELEASE)") + ", QT_DEBUG="
#ifdef QT_DEBUG
    + "1\n";
#else
    + "0\n";
#endif
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    Info += "BYTE_ORDER: BIG_ENDIAN\n" ;
#endif
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    Info += "BYTE_ORDER: LITTLE_ENDIAN\n" ;
#endif
    Info += "WordSize: " + qn( sizeof( void * ) <<3 ) + " bytes\n";

    Info += "Qt buildDate:" + QLibraryInfo::buildDate().toString() + "\n";
    Info += "\n";

    Info += "Run-time\n" ;
    Info += "------------------------------------------------------\n" ;
    Info += "time: " + QDateTime::currentDateTime ().toString() + "\n";
    Info += "version: " + QString(qVersion()) + "\n";
    Info += "Qt PrefixPath:" + QLibraryInfo::location(QLibraryInfo::PrefixPath) + "\n";
    Info += "Qt HeadersPath:" + QLibraryInfo::location(QLibraryInfo::HeadersPath) + "\n";
    Info += "Qt LibrariesPath:" + QLibraryInfo::location(QLibraryInfo::LibrariesPath) + "\n";
    Info += "Qt BinariesPath:" + QLibraryInfo::location(QLibraryInfo::BinariesPath) + "\n";

    Info += "Qt PluginsPath:" + QLibraryInfo::location(QLibraryInfo::PluginsPath) + "\n";
    Info += "Qt ImportsPath:" + QLibraryInfo::location(QLibraryInfo::ImportsPath) + "\n";
    Info += "Qt Qml2ImportsPath:" + QLibraryInfo::location(QLibraryInfo::Qml2ImportsPath) + "\n";

    Info += "platformName:" + qApp->platformName() + "\n";
    Info += "screens: " + qn( qApp->screens().size() ) + "\n";
    const QString _orientations[] = { "PrimaryOrientation", "LandscapeOrientation", "PortraitOrientation", "InvertedLandscapeOrientation", "InvertedPortraitOrientation"
};
    foreach (QScreen* s, qApp->screens()) {
        Info += "	depth:" + qn(s->depth()) + "\n";
        Info += "	availableGeometry:" + qs(s->availableGeometry()) + "\n";
        Info += "	availableVirtualGeometry:" + qs(s->availableVirtualGeometry()) + "\n";
        Info += "	availableVirtualSize:" + qs(s->availableVirtualSize()) + "\n";
        Info += "	geometry:" + qs(s->geometry()) + "\n";
        Info += "	logicalDotsPerInch:" + qn(s->logicalDotsPerInch()) + "\n";
        Info += "	logicalDotsPerInchX:" + qn(s->logicalDotsPerInchX()) + "\n";
        Info += "	logicalDotsPerInchY:" + qn(s->logicalDotsPerInchY()) + "\n";
        Info += "	nativeOrientation:" + _orientations[s->nativeOrientation()] + "\n";
        Info += "	orientation:" + _orientations[s->orientation()] + "\n";
        Info += "	physicalDotsPerInch:" + qn(s->physicalDotsPerInch()) + "\n";
        Info += "	physicalDotsPerInchX:" + qn(s->physicalDotsPerInchX()) + "\n";
        Info += "	physicalDotsPerInchY:" + qn(s->physicalDotsPerInchY()) + "\n";
        Info += "	physicalSize:" + qs(s->physicalSize()) + "\n";
        Info += "	primaryOrientation:" + _orientations[s->primaryOrientation()] + "\n";
        Info += "	refreshRate:" + qn(s->refreshRate()) + "\n";
        Info += "	size:" + qs(s->size()) + "\n";
        Info += "	virtualGeometry:" + qs(s->virtualGeometry()) + "\n";
        Info += "	virtualSize:" + qs(s->virtualSize()) + "\n";
        Info += "	handle:" + qs(s->handle()) + "\n";
    }

    Info += "OpenGL" ;
    Info += "------------------------------------------------------" ;

    QOpenGLExtraFunctions* gl = ctx->extraFunctions();
    QSurfaceFormat glFormat = ctx->format();

    Info += "OpenGL Versions supported: " + glFormat.version().first + QStringLiteral(".") + glFormat.version().second;

    QString versionString(QLatin1String(reinterpret_cast<const char*>(gl->glGetString(GL_VERSION))));
    Info += "Driver Version string: " + versionString;

    GLfloat GL_MaxAnisotropy = 0; gl->glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &GL_MaxAnisotropy);

    QString Anisotropic = GL_MaxAnisotropy>0 ? tr("supported (max %1)").arg(GL_MaxAnisotropy) : tr("Not supported");
	QString VersionFormat = tr("OpenGL Version %1\nGLSL Version %2\n%3 - %4\n\n");
	QString Version = VersionFormat.arg(QString((const char*)gl->glGetString(GL_VERSION)), QString((const char*)gl->glGetString(GL_SHADING_LANGUAGE_VERSION)), QString((const char*)gl->glGetString(GL_RENDERER)), QString((const char*)gl->glGetString(GL_VENDOR)));
	QString BuffersFormat = tr("Color Buffer: %1 bits %2\nDepth Buffer: %4 bits\nStencil Buffer: %5 bits\nAnisotropy: %6\n\n");
	QString Buffers = BuffersFormat.arg(
    qn(glFormat.redBufferSize() + glFormat.greenBufferSize() + glFormat.blueBufferSize() + glFormat.alphaBufferSize()),
    glFormat.swapBehavior() == QSurfaceFormat::DoubleBuffer ? tr("double buffered") :
      (glFormat.swapBehavior() == QSurfaceFormat::TripleBuffer ? tr("triple buffered") : QString()),
    qn(glFormat.depthBufferSize()),
    qn(glFormat.stencilBufferSize()),
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

    /* Print general info */
    QString contextString = ctx->isOpenGLES() ? "OpenGLES" : "OpenGL";
    contextString += QString(" version ")
        + qn(ctx->format().majorVersion()) + '.'
        + qn(ctx->format().minorVersion());
    if (!ctx->isOpenGLES()
            && (ctx->format().majorVersion() > 3
                || (ctx->format().majorVersion() == 3 && ctx->format().minorVersion() >= 2))) {
        contextString += QString(" ")
            + (ctx->format().profile() == QSurfaceFormat::CompatibilityProfile ? "compatibility" : "core")
            + " profile";
    }

    auto getI = [](QOpenGLExtraFunctions* gl, GLenum p) {
        GLint v = 0;
        gl->glGetIntegerv(p, &v);
        return v;
    };

    auto getS = [=](QOpenGLExtraFunctions* gl, GLenum p) {
        return reinterpret_cast<const char*>(gl->glGetString(p));
    };

    Info += QString::asprintf("Context:    %s\n", qPrintable(contextString));
    Info += QString::asprintf("Version:    %s\n", getS(gl, GL_VERSION));
    Info += QString::asprintf("SL Version: %s\n", getS(gl, GL_SHADING_LANGUAGE_VERSION));
    Info += QString::asprintf("Vendor:     %s\n", getS(gl, GL_VENDOR));
    Info += QString::asprintf("Renderer:   %s\n", getS(gl, GL_RENDERER));

    /* Print extensions */
    QSet<QByteArray> extensions = ctx->extensions();
    QList<QByteArray> sortedExtensions;
    foreach (const QByteArray& ext, extensions)
        if (ext.size() > 0)
            sortedExtensions.append(ext);
    std::sort(sortedExtensions.begin(), sortedExtensions.end());
    Info += "Extensions:\n";
    foreach (const QByteArray& ext, sortedExtensions)
        Info += "    " + ext + "\n";

    /* Print implementation-defined limitations */
    Info += QString::asprintf("Resource limitations:\n");
    Info += QString::asprintf("  Texture limits:\n");
    Info += QString::asprintf("    1D / 2D size: %5d  GL_MAX_TEXTURE_SIZE\n", getI(gl, GL_MAX_TEXTURE_SIZE));
    Info += QString::asprintf("    3D size:      %5d  GL_MAX_3D_TEXTURE_SIZE\n", getI(gl, GL_MAX_3D_TEXTURE_SIZE));
    Info += QString::asprintf("    Cubemap size: %5d  GL_MAX_CUBE_MAP_TEXTURE_SIZE\n", getI(gl, GL_MAX_CUBE_MAP_TEXTURE_SIZE));
    Info += QString::asprintf("    Arr. layers:  %5d  GL_MAX_ARRAY_TEXTURE_LAYERS\n", getI(gl, GL_MAX_ARRAY_TEXTURE_LAYERS));
    Info += QString::asprintf("  Framebuffer object limits:\n");
    Info += QString::asprintf("    Width:        %5d  GL_MAX_FRAMEBUFFER_WIDTH\n", getI(gl, GL_MAX_FRAMEBUFFER_WIDTH));
    Info += QString::asprintf("    Height:       %5d  GL_MAX_FRAMEBUFFER_HEIGHT\n", getI(gl, GL_MAX_FRAMEBUFFER_HEIGHT));
    Info += QString::asprintf("    Color Attach.:%5d  GL_MAX_COLOR_ATTACHMENTS\n", getI(gl, GL_MAX_COLOR_ATTACHMENTS));
    Info += QString::asprintf("    Draw buffers: %5d  GL_MAX_DRAW_BUFFERS\n", getI(gl, GL_MAX_DRAW_BUFFERS));
    Info += QString::asprintf("  Maximum number of uniform components in shader stage:\n");
    Info += QString::asprintf("    Vertex:       %5d  GL_MAX_VERTEX_UNIFORM_COMPONENTS\n", getI(gl, GL_MAX_VERTEX_UNIFORM_COMPONENTS));
    Info += QString::asprintf("    Tess. Ctrl.:  %5d  GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS\n", getI(gl, GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS));
    Info += QString::asprintf("    Tess. Eval.:  %5d  GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS\n", getI(gl, GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS));
    Info += QString::asprintf("    Geometry:     %5d  GL_MAX_GEOMETRY_UNIFORM_COMPONENTS\n", getI(gl, GL_MAX_GEOMETRY_UNIFORM_COMPONENTS));
    Info += QString::asprintf("    Fragment:     %5d  GL_MAX_FRAGMENT_UNIFORM_COMPONENTS\n", getI(gl, GL_MAX_FRAGMENT_UNIFORM_COMPONENTS));
    Info += QString::asprintf("    Compute:      %5d  GL_MAX_COMPUTE_UNIFORM_COMPONENTS\n", getI(gl, GL_MAX_COMPUTE_UNIFORM_COMPONENTS));
    Info += QString::asprintf("  Maximum number of input components in shader stage:\n");
    Info += QString::asprintf("    Vertex:       %5d  4*GL_MAX_VERTEX_ATTRIBS\n", 4 * getI(gl, GL_MAX_VERTEX_ATTRIBS));
    Info += QString::asprintf("    Tess. Ctrl.:  %5d  GL_MAX_TESS_CONTROL_INPUT_COMPONENTS\n", getI(gl, GL_MAX_TESS_CONTROL_INPUT_COMPONENTS));
    Info += QString::asprintf("    Tess. Eval.:  %5d  GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS\n", getI(gl, GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS));
    Info += QString::asprintf("    Geometry:     %5d  GL_MAX_GEOMETRY_INPUT_COMPONENTS\n", getI(gl, GL_MAX_GEOMETRY_INPUT_COMPONENTS));
    Info += QString::asprintf("    Fragment:     %5d  GL_MAX_FRAGMENT_INPUT_COMPONENTS\n", getI(gl, GL_MAX_FRAGMENT_INPUT_COMPONENTS));
    //Info += QString::asprintf("    Vert.->Frag.: %5d  GL_MAX_VARYING_COMPONENTS\n", getI(gl, GL_MAX_VARYING_COMPONENTS));
    Info += QString::asprintf("  Maximum number of output components in shader stage:\n");
    Info += QString::asprintf("    Vertex:       %5d  GL_MAX_VERTEX_OUTPUT_COMPONENTS\n", getI(gl, GL_MAX_VERTEX_OUTPUT_COMPONENTS));
    //Info += QString::asprintf("    Vert.->Frag.: %5d  GL_MAX_VARYING_COMPONENTS\n", getI(gl, GL_MAX_VARYING_COMPONENTS));
    Info += QString::asprintf("    Tess. Ctrl.:  %5d  GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS\n", getI(gl, GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS));
    Info += QString::asprintf("    Tess. Eval.:  %5d  GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS\n", getI(gl, GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS));
    Info += QString::asprintf("    Geometry:     %5d  GL_MAX_GEOMETRY_OUTPUT_COMPONENTS\n", getI(gl, GL_MAX_GEOMETRY_OUTPUT_COMPONENTS));
    Info += QString::asprintf("    Fragment:     %5d  4*GL_MAX_DRAW_BUFFERS\n", 4 * getI(gl, GL_MAX_DRAW_BUFFERS));
    Info += QString::asprintf("  Maximum number of samplers in shader stage:\n");
    Info += QString::asprintf("    Vertex:       %5d  GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS\n", getI(gl, GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS));
    Info += QString::asprintf("    Tess. Ctrl.:  %5d  GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS\n", getI(gl, GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS));
    Info += QString::asprintf("    Tess. Eval.:  %5d  GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS\n", getI(gl, GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS));
    Info += QString::asprintf("    Geometry:     %5d  GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS\n", getI(gl, GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS));
    Info += QString::asprintf("    Fragment:     %5d  GL_MAX_TEXTURE_IMAGE_UNITS\n", getI(gl, GL_MAX_TEXTURE_IMAGE_UNITS));
    Info += QString::asprintf("    Compute:      %5d  GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS\n", getI(gl, GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS));
    Info += QString::asprintf("    Combined:     %5d  GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS\n", getI(gl, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS));

	ui->info->setText(Version + Buffers + Info + Extensions);
}

void AllAboutDialog::showGroupBoxInfo()
{
      	ui->infoBox->show();
}

void AllAboutDialog::setInfoText(const QString &t)
{
  ui->labelInfo->setText(t);
  ui->labelInfo->setVisible(!ui->labelInfo->text().isEmpty());
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
