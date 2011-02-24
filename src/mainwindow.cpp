/*
 * QEstEidUtil
 *
 * Copyright (C) 2009,2010 Jargo Kõster <jargo@innovaatik.ee>
 * Copyright (C) 2009,2010 Raul Metsma <raul@innovaatik.ee>
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

#include "mainwindow.h"

#include "jsextender.h"
#include "jscardmanager.h"

#include <QApplication>
#include <QMenuBar>
#include <QWebFrame>
#include <QTranslator>

MainWindow::MainWindow( QWidget *parent )
:	QWebView( parent )
,	menu( 0 )
{
	setWindowFlags( Qt::Window|Qt::CustomizeWindowHint|Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint );
	page()->mainFrame()->setScrollBarPolicy( Qt::Horizontal, Qt::ScrollBarAlwaysOff );
	page()->mainFrame()->setScrollBarPolicy( Qt::Vertical, Qt::ScrollBarAlwaysOff );
	setContextMenuPolicy(Qt::PreventContextMenu);
	setFixedSize( 585, 535 );

	appTranslator = new QTranslator( this );
	qtTranslator = new QTranslator( this );
	commonTranslator = new QTranslator( this );
	qApp->installTranslator( appTranslator );
	qApp->installTranslator( qtTranslator );
	qApp->installTranslator( commonTranslator );

	m_jsExtender = new JsExtender( this );

#if defined(Q_OS_MAC)
	bar = new QMenuBar;
	menu = bar->addMenu( tr("&File") );
	pref = menu->addAction( tr("Settings"), m_jsExtender, SLOT(showSettings()) );
	about = menu->addAction( tr("Info"), m_jsExtender, SLOT(showAbout()) );
	close = menu->addAction( tr("Close"), qApp, SLOT(quit()) );
	pref->setMenuRole( QAction::PreferencesRole );
	about->setMenuRole( QAction::AboutRole );
	close->setShortcut( Qt::CTRL + Qt::Key_W );
#endif
	jsEsteidCard = new JsEsteidCard( this );
	jsCardManager = new JsCardManager( jsEsteidCard );

	connect(jsCardManager, SIGNAL(cardEvent(QString, int)),
		m_jsExtender, SLOT(jsCall(QString, int)));
	connect(jsCardManager, SIGNAL(cardError(QString, QString)),
		m_jsExtender, SLOT(jsCall(QString, QString)));
	connect(jsEsteidCard, SIGNAL(cardError(QString, QString)),
		m_jsExtender, SLOT(jsCall(QString, QString)));

 	m_jsExtender->registerObject("esteidData", jsEsteidCard);
	m_jsExtender->registerObject("cardManager", jsCardManager);

	load(QUrl("qrc:/html/index.html"));
}

MainWindow::~MainWindow()
{
#ifdef Q_OS_MAC
	delete bar;
#endif
}

void MainWindow::raiseAndRead()
{
	raise();
	activateWindow();
	page()->mainFrame()->evaluateJavaScript( "selectReader()" );
}

void MainWindow::retranslate( const QString &lang )
{
	appTranslator->load( ":/translations/" + lang );
	qtTranslator->load( ":/translations/qt_" + lang );
	commonTranslator->load( ":/translations/common_" + lang );
	setWindowTitle( tr("ID-card utility") );
#ifdef Q_OS_MAC
	if ( menu )
	{
		menu->setTitle( tr("&File") );
		pref->setText( tr("Settings") );
		about->setText( tr("Info") );
		close->setText( tr("Close") );
	}
#endif
}
