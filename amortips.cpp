//---------------------------------------------------------------------------
//
// amortips.cpp
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#include <stdlib.h>
#include <kapp.h>
#include "amortips.h"
#include <kglobal.h>
#include <kstddirs.h>
#include <klocale.h>
#include <kdebug.h>
#include <qfile.h>
#include <qtextstream.h>

//---------------------------------------------------------------------------
//
AmorTips::AmorTips()
{
}

//---------------------------------------------------------------------------
//
// Set the file containing tips.  This reads all tips into memory at the
// moment - need to make more efficient.
//
bool AmorTips::setFile(const QString& file)
{
    bool rv = false;

    QString path( locate("appdata", file) );
    if(path.length() && read(path))
        rv = true;

    rv |= readKTips();

    return rv;
}

//---------------------------------------------------------------------------
//
// Clear all tips from memory
//
void AmorTips::reset()
{
    mTips.clear();
}

//---------------------------------------------------------------------------
//
// Get a tip randomly from the list
//
QString AmorTips::tip()
{
    if (mTips.count())
    {
        QString tip = *mTips.at(kapp->random() % mTips.count());
		return i18n(tip.utf8());
    }

    return i18n("No tip");
}

//---------------------------------------------------------------------------
//
// Read the tips from ktip's file
//
bool AmorTips::readKTips()
{
    QString fname;

    fname = locate("data", QString("kdewizard/tips"));

    if (fname.isEmpty())
	return false;

    QFile f(fname);
    if (f.open(IO_ReadOnly))
    {
	QTextStream ts(&f);

	QString line, tag, tip;
	bool inTip = false;
	while (!ts.eof())
	{
	    line = ts.readLine();
	    tag = line.stripWhiteSpace().lower();

	    if (tag == "<html>")
	    {
		inTip = true;
		tip = QString::null;
		continue;
	    }

	    if (inTip)
	    {
		if (tag == "</html>")
		{
		    mTips.append(tip);
		    inTip = false;
		}
		else
		    tip.append(line).append("\n");
	    }

	}

	f.close();
    }

    return true;
}

//---------------------------------------------------------------------------
//
// Read all tips from the specified file.
//
bool AmorTips::read(const QString& path)
{
    QFile file(path);

    if (file.open(IO_ReadOnly))
    {
        while (!file.atEnd())
        {
            readTip(file);
        }

	return true;
    }

    return false;
}

//---------------------------------------------------------------------------
//
// Read a single tip.
//
bool AmorTips::readTip(QFile &file)
{
    char buffer[1024] = "";
    QString tip;

    while (!file.atEnd() && buffer[0] != '%')
    {
        file.readLine(buffer, 1024);
        if (buffer[0] != '%')
        {
            tip += QString::fromUtf8(buffer);
        }
    }

    if (!tip.isEmpty())
    {
        if (tip[tip.length()-1] == '\n')
        {
            tip.truncate(tip.length()-1);
        }
        mTips.append(tip);
        return true;
    }

    return false;
}

