/*
 * Copyright (C) 2012  Sofia Balicka <balicka@kolabsys.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MIMEOBJECTTEST_H
#define MIMEOBJECTTEST_H
#include <QObject>

class MIMEObjectTest: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testEvent();
    void testJournal(); 
    void testNote();
    void testContact();
    void testDistlist();
};
#endif // MIMEOBJECTTEST_H

