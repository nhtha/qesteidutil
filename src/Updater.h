/*
 * QEstEidUtil
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

#pragma once

#include <QtWidgets/QDialog>

class QEventLoop;
class UpdaterPrivate;
class Updater: public QDialog
{
	Q_OBJECT
public:
	explicit Updater(const QString &reader, QWidget *parent = 0);
	~Updater();
	int exec();

Q_SIGNALS:
	void log(const QString &msg);
	void send(const QVariantHash &data);
	void signReq(QEventLoop *loop, const QByteArray &in);

private:
	void process(const QByteArray &data);
	void sign(QEventLoop *loop, const QByteArray &in);

	UpdaterPrivate *d;
};
