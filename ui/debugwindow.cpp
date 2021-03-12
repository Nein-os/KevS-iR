/* Copyright (C) 2021 Krosny Kevin
 *
 */
/* This file is part of KevS.

    KevS is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    KevS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "debugwindow.h"

#include <QGridLayout>
#include <QLabel>

DebugWindow::DebugWindow(QWidget *parent) : QWidget(parent)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_TranslucentBackground);
	base_layout = new QGridLayout(parent);

	header[0] = new QLabel("Lap:", parent);
	header[1] = new QLabel("Time:", parent);
	header[2] = new QLabel("Fuel:", parent);
	header[3] = new QLabel("Ignore:", parent);
	for (int i = 0; i < AMOUNT_HEADER_DEBUG; i++) {
		header[i]->setFont(QFont("Arial", 25, 25));
		base_layout->addWidget(header[i], i, 0);
	}
	for (int i = 0; i < AMOUNT_DATA_DEBUG; i++) {
		data[i] = new QLabel(parent);
		data[i]->setFont(QFont("Arial", 25, 10));
		base_layout->addWidget(data[i], i, 1);
	}

	setWindowTitle("Debug Window - KevS");
	setLayout(base_layout);
}

void DebugWindow::setNewData(int lap, float laptime, float verbrauch, bool ignorelap)
{
	data[0]->setText(QString("%1").arg(lap));
	data[1]->setText(QString("%1").arg(laptime));
	data[2]->setText(QString("%1").arg(verbrauch));
	data[3]->setText(QString("%1").arg(ignorelap));
}

DebugWindow::~DebugWindow() {}
