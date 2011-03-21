/*
 * QEstEidUtil
 *
 * Copyright (C) 2009-2011 Jargo Kõster <jargo@innovaatik.ee>
 * Copyright (C) 2009-2011 Raul Metsma <raul@innovaatik.ee>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "DiagnosticsDialog.h"

#include <QDialogButtonBox>
#include <QFile>
#include <QProgressBar>
#include <QProgressDialog>
#include <QTextStream>
#include <QProcess>

#ifdef Q_OS_MAC
#include <Carbon/Carbon.h>
#endif

static QByteArray runProcess( const QString &program, const QStringList &arguments = QStringList() )
{
	QProcess p;
	if( arguments.isEmpty() )
		p.start( program );
	else
		p.start( program, arguments );
	p.waitForFinished();
	return p.readAll();
}

DiagnosticsDialog::DiagnosticsDialog( QWidget *parent )
:	QDialog( parent )
{
	setupUi( this );
	setAttribute( Qt::WA_DeleteOnClose, true );

	QString info;
	QTextStream s( &info );

	s << "<b>" << tr("Locale:") << "</b> ";
	QLocale::Language language = QLocale::system().language();
	s << (language == QLocale::C ? "English/United States" : QLocale::languageToString( language ) ) << "<br /><br />";

	QString package = getPackageVersion( QStringList() << "estonianidcard", false );
	QString utility = getPackageVersion( QStringList() << "qesteidutil", false );
	if ( !package.isEmpty() )
		s << "<b>" << tr("ID-card package version:") << "</b> " << package << "<br />";
	if ( !utility.isEmpty() )
		s << "<b>" << tr("ID-card utility version:") << "</b> " << utility << "<br />";

	s << "<b>" << tr("OS:") << "</b> ";
#ifdef Q_OS_LINUX
	s << runProcess( "lsb_release", QStringList() << "-s" << "-d" );
#else
	SInt32 major, minor, bugfix;
	
	if( Gestalt(gestaltSystemVersionMajor, &major) == noErr &&
			Gestalt(gestaltSystemVersionMinor, &minor) == noErr &&
			Gestalt(gestaltSystemVersionBugFix, &bugfix) == noErr )
		s << "Mac OS " << major << "." << minor << "." << bugfix;
	else
		s << "Mac OS 10.3";
#endif

	s << " (" << QSysInfo::WordSize << ")<br />";
	s << "<b>" << tr("CPU:") << "</b> " << getProcessor() << "<br /><br />";
	s << "<b>" << tr("Library paths:") << "</b> " << QCoreApplication::libraryPaths().join( ";" ) << "<br />";
	s << "<b>" << tr("Libraries") << ":</b><br />";
	s << getPackageVersion( QStringList() << "libdigidoc" << "libdigidocpp" );
#ifdef Q_OS_MAC
	s << runProcess(  "/Library/OpenSC/bin/opensc-tool", QStringList() << "-i" ) << "<br />";
#else
	s << getPackageVersion( QStringList() << "openssl" << "libpcsclite1" << "pcsc-lite" << "opensc" );
#endif
	s << "QT (" << qVersion() << ")<br />" << "<br />";

	s << "<b>" << tr("PCSC service status: ") << "</b>" << " " << (isPCSCRunning() ? tr("Running") : tr("Not running")) << "<br /><br />";

	s << "<b>" << tr("Card readers") << ":</b><br />" << getReaderInfo() << "<br />";

	QString browsers = getRegistry();
	if ( !browsers.isEmpty() )
		s << "<b>" << tr("Browsers:") << "</b><br />" << browsers << "<br /><br />";

	diagnosticsText->setHtml( info );

	buttonBox->addButton( tr( "More info" ), QDialogButtonBox::HelpRole );
}

QString DiagnosticsDialog::getRegistry( const QString & ) const
{
#ifdef Q_OS_LINUX
	return getPackageVersion( QStringList() << "chromium-browser" << "firefox" << "MozillaFirefox" );
#else
	return getPackageVersion( QStringList() << "Google Chrome" << "Firefox" << "Safari" );
#endif
}

QString DiagnosticsDialog::getPackageVersion( const QStringList &list, bool returnPackageName ) const
{
	QString ret;
#ifdef Q_OS_LINUX
	QStringList params;
	QProcess p;
	p.start( "which", QStringList() << "dpkg-query" );
	p.waitForReadyRead();
	QByteArray cmd = p.readAll();
	if ( cmd.isEmpty() )
	{
		p.start( "which", QStringList() << "rpm" );
		p.waitForReadyRead();
		cmd = p.readAll();
		if ( cmd.isEmpty() )
			return ret;
		cmd = "rpm";
		params << "-q" << "--qf" << "%{VERSION}";
	} else {
		cmd = "dpkg-query";
		params << "-W" << "-f=${Version}";
	}
	p.close();

	Q_FOREACH( const QString &package, list )
	{
		p.start( cmd, QStringList() << params << package );
		p.waitForFinished();
		if ( p.exitCode() )
			continue;
		QByteArray result = p.readAll();
		if ( !result.isEmpty() )
		{
			if ( returnPackageName )
				ret += package + " ";
			ret += result + "<BR />";
		}
		p.close();
	}
#else
	Q_FOREACH( const QString &package, list )
	{
		QStringList params = QStringList() << "read";
		if( QFile::exists( "/Applications/" + package + ".app/Contents/Info.plist" ) )
			params << "/Applications/" + package + ".app/Contents/Info" << "CFBundleShortVersionString";
		else if( QFile::exists( "/var/db/receipts/ee.sk.idcard." + package + ".plist" ) )
			params << "/var/db/receipts/ee.sk.idcard." + package << "PackageVersion";
		else if( QFile::exists( "/Library/Receipts/" + package + ".pkg/Contents/Info.plist" ) )
			params << "/Library/Receipts/" + package + ".pkg/Contents/Info" << "CFBundleShortVersionString";
		else
			continue;

		QByteArray result = runProcess( "defaults", params );
		if ( !result.isEmpty() )
		{
			if ( returnPackageName )
				ret += package + " ";
			ret += result + "<BR />";
		}
	}
#endif

	return ret;
}

QString DiagnosticsDialog::getProcessor() const
{
#ifdef Q_OS_LINUX
	return runProcess( "sh -c \"cat /proc/cpuinfo | grep -m 1 model\\ name\"" );
#else
	QString result = runProcess( "system_profiler", QStringList() << "SPHardwareDataType" );
	QRegExp reg( "Processor Name:(\\s*)(.*)\n.*Processor Speed:(\\s*)(.*)\n" );
	reg.setMinimal( true );
	return reg.indexIn( result ) != -1 ? reg.cap( 2 ) + " (" + reg.cap( 4 ) + ")" : "";
#endif
}

bool DiagnosticsDialog::isPCSCRunning() const
{
#ifdef Q_OS_LINUX
	QByteArray result = runProcess( "pidof", QStringList() << "pcscd" );
#else
	QByteArray result = runProcess( "sh -c \"ps ax | grep -v grep | grep pcscd\"" );
#endif
	return !result.trimmed().isEmpty();
}

void DiagnosticsDialog::showDetails()
{
	QProgressDialog box( tr( "Generating diagnostics\n\nPlease wait..." ), QString(), 0, 0, qApp->activeWindow() );
	box.setWindowTitle( windowTitle() );
	box.setWindowFlags( (box.windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint );
	if( QProgressBar *bar = box.findChild<QProgressBar*>() )
		bar->setVisible( false );
	box.open();
	
	QApplication::processEvents();

	QString ret;
	QByteArray cmd = runProcess( "opensc-tool", QStringList() << "-la" );
	if ( !cmd.isEmpty() )
		ret += "<b>" + tr("OpenSC tool:") + "</b><br/> " + cmd.replace( "\n", "<br />" ) + "<br />";

	QApplication::processEvents();

	cmd = runProcess( "pkcs11-tool", QStringList() << "-T" );
	if ( !cmd.isEmpty() )
		ret += "<b>" + tr("PKCS11 tool:") + "</b><br/> " + cmd.replace( "\n", "<br />" ) + "<br />";

#ifdef Q_OS_LINUX
	cmd = runProcess( "lsusb" );
#else
	cmd = runProcess( "system_profiler", QStringList() << "SPUSBDataType" );
#endif
	if ( !cmd.isEmpty() )
		ret += "<b>" + tr("USB info:") + "</b><br/> " + cmd.replace( "\n", "<br />" ) + "<br />";

	if ( !ret.isEmpty() )
		diagnosticsText->append( ret );
}
