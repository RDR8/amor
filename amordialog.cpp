//---------------------------------------------------------------------------
//
// amordialog.cpp
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#include <qlayout.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qslider.h>
#include <qdir.h>
#include <qpainter.h>
#include <kapp.h>
#include <ksimpleconfig.h>

#include "amordialog.h"
#include "amordialog.moc"
#include "version.h"
#include <klocale.h>
#include <qvbox.h>
#include <kglobal.h>
#include <kstddirs.h>

//---------------------------------------------------------------------------
//
// Constructor
//
AmorDialog::AmorDialog()
    : KDialogBase(0, "amordlg", false, i18n("Options"), Ok|Cancel, Ok )
{
    mConfig.read();
    QVBox *mainwidget = makeVBoxMainWidget();

    QHBox *hb = new QHBox(mainwidget);

    // Theme list
    QVBox *themeBox = new QVBox(hb);
    themeBox->setSpacing(spacingHint());

    QLabel *label = new QLabel(i18n("Theme:"), themeBox);

    mThemeListBox = new QListBox(themeBox);
    connect(mThemeListBox,SIGNAL(highlighted(int)),SLOT(slotHighlighted(int)));
    mThemeListBox->setMinimumSize( fontMetrics().maxWidth()*20,  
				   fontMetrics().lineSpacing()*6 );

    mAboutEdit = new QMultiLineEdit(themeBox);
    mAboutEdit->setReadOnly(true);
    mAboutEdit->setMinimumHeight( fontMetrics().lineSpacing()*4 );

    themeBox->setStretchFactor(mThemeListBox, 4);
    themeBox->setStretchFactor(mAboutEdit, 1);

    // Animation offset
    QVBox *offsetBox = new QVBox(hb);
    offsetBox->setSpacing(spacingHint());
    label = new QLabel(i18n("Offset:"), offsetBox);

    QSlider *slider = new QSlider(-40, 40, 5, mConfig.mOffset,
                                    QSlider::Vertical, offsetBox);
    connect(slider, SIGNAL(valueChanged(int)), SLOT(slotOffset(int)));

    // Always on top
    QCheckBox *checkBox = new QCheckBox(i18n("Always on top"), mainwidget);
    connect(checkBox, SIGNAL(toggled(bool)), SLOT(slotOnTop(bool)));
    checkBox->setChecked(mConfig.mOnTop);

    checkBox = new QCheckBox(i18n("Show random tips"), mainwidget);
    connect(checkBox, SIGNAL(toggled(bool)), SLOT(slotRandomTips(bool)));
    checkBox->setChecked(mConfig.mTips);

    checkBox = new QCheckBox(i18n("Allow application tips"), mainwidget);
    connect(checkBox, SIGNAL(toggled(bool)), SLOT(slotApplicationTips(bool)));
    checkBox->setChecked(mConfig.mAppTips);

    readThemes();
}

//---------------------------------------------------------------------------
//
// Destructor
//
AmorDialog::~AmorDialog()
{
}

//---------------------------------------------------------------------------
//
// Get list of all themes
//
void AmorDialog::readThemes()
{
    QStringList files(KGlobal::dirs()->findAllResources("appdata", "*rc"));
    for (QStringList::ConstIterator it = files.begin();
	 it != files.end();
	 it++)
      addTheme(*it);
}

//---------------------------------------------------------------------------
//
// Add a single theme to the list
//
void AmorDialog::addTheme(const QString& file)
{
    KSimpleConfig config(file, true);

    config.setGroup("Config");

    QString pixmapPath = config.readEntry("PixmapPath");
    if (pixmapPath.isEmpty())
    {
        return;
    }

    pixmapPath += "/";

    if (pixmapPath[0] != '/')
    {
        // relative to config file. We add a / to indicate the dir
        pixmapPath = locate("appdata", pixmapPath);
    }

    QString description = config.readEntry("Description");
    QString about = config.readEntry("About", " ");
    QString pixmapName = config.readEntry("Icon");

    pixmapPath += pixmapName;

    QPixmap pixmap(pixmapPath);

    AmorListBoxItem *item = new AmorListBoxItem(description, pixmap);
    mThemeListBox->insertItem(item);
    mThemes.append(file);
    mThemeAbout.append(about);

    if (mConfig.mTheme == file)
    {
        mThemeListBox->setSelected(mThemeListBox->count()-1, true);
    }
}

//---------------------------------------------------------------------------
//
// User highlighted a theme
//
void AmorDialog::slotHighlighted(int index)
{
    mConfig.mTheme = *mThemes.at(index);
    mAboutEdit->setText(*mThemeAbout.at(index));
}

//---------------------------------------------------------------------------
//
// User changed offset
//
void AmorDialog::slotOffset(int off)
{
    mConfig.mOffset = off;
    emit offsetChanged(mConfig.mOffset);
}

//---------------------------------------------------------------------------
//
// User toggled always on top
//
void AmorDialog::slotOnTop(bool onTop)
{
    mConfig.mOnTop = onTop;
}

//---------------------------------------------------------------------------
//
// User toggled random tips
//
void AmorDialog::slotRandomTips(bool tips)
{
    mConfig.mTips = tips;
}

//---------------------------------------------------------------------------
//
// User toggled application tips
//
void AmorDialog::slotApplicationTips(bool tips)
{
    mConfig.mAppTips = tips;
}

//---------------------------------------------------------------------------
//
// User clicked Ok
//
void AmorDialog::slotOk()
{
    mConfig.write();
    emit changed();
    accept();
}

//---------------------------------------------------------------------------
//
// User clicked Cancel
//
void AmorDialog::slotCancel()
{
    // restore offset
    KConfig *config = kapp->config();
    emit offsetChanged(config->readNumEntry("Offset"));
    reject();
}

//===========================================================================
//
// AmorListBoxItem implements a list box items for selection of themes
//
void AmorListBoxItem::paint( QPainter *p )
{
    p->drawPixmap( 3, 0, mPixmap );
    QFontMetrics fm = p->fontMetrics();
    int yPos;                       // vertical text position
    if ( mPixmap.height() < fm.height() )
        yPos = fm.ascent() + fm.leading()/2;
    else
        yPos = mPixmap.height()/2 - fm.height()/2 + fm.ascent();
    p->drawText( mPixmap.width() + 5, yPos, text() );
}

int AmorListBoxItem::height(const QListBox *lb ) const
{
    return QMAX( mPixmap.height(), lb->fontMetrics().lineSpacing() + 1 );
}

int AmorListBoxItem::width(const QListBox *lb ) const
{
    return mPixmap.width() + lb->fontMetrics().width( text() ) + 6;
}
