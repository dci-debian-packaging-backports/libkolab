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

#include <QTest>
#include "mimeobjecttest.h"
#include "kolabformat/kolabobject.h"
#include "testutils.h"
#include "kolabformat/mimeobject.h"
#include <fstream>
#include <sstream>
#include <QString>
#include <ksystemtimezone.h>

void MIMEObjectTest::initTestCase()
{
    QVERIFY2(KSystemTimeZones::isTimeZoneDaemonAvailable(), "Timezone support is required for this test. Either use libcalendaring or make sure KTimeZoned is available");
}

void MIMEObjectTest::testEvent(){

    Kolab::MIMEObject mimeobject;

    std::ifstream t((TESTFILEDIR.toStdString()+"v3/event/simple.ics.mime").c_str());
    std::stringstream buffer;
    buffer << t.rdbuf();

    Kolab::Event event = mimeobject.readEvent(buffer.str());
    
    std::string message = mimeobject.writeEvent(event, Kolab::KolabV3);

    QString qMessage = QString::fromStdString(message);
    QString input = QString::fromStdString(buffer.str());
    
    normalizeMimemessage(qMessage);
    normalizeMimemessage(input);
    
    QCOMPARE(input.simplified(), qMessage.simplified());
}
/*
void MIMEObjectTest::testTodo(){

}
*/

void MIMEObjectTest::testJournal(){

    Kolab::MIMEObject mimeobject;

    std::ifstream t((TESTFILEDIR.toStdString()+"v3/journal/simple.ics.mime").c_str());
    std::stringstream buffer;
    buffer << t.rdbuf();

    Kolab::Journal journal = mimeobject.readJournal(buffer.str());
    
    std::string message = mimeobject.writeJournal(journal, Kolab::KolabV3);

    QString qMessage = QString::fromStdString(message);
    QString input = QString::fromStdString(buffer.str());
    
    normalizeMimemessage(qMessage);
    normalizeMimemessage(input);
    
    QCOMPARE(input.simplified(), qMessage.simplified());

}

void MIMEObjectTest::testNote(){

    Kolab::MIMEObject mimeobject;

    std::ifstream t((TESTFILEDIR.toStdString()+"v3/note/note.mime.mime").c_str());
    std::stringstream buffer;
    buffer << t.rdbuf();

    Kolab::Note note = mimeobject.readNote(buffer.str());
    
    std::string message = mimeobject.writeNote(note, Kolab::KolabV3);

    QString qMessage = QString::fromStdString(message);
    QString input = QString::fromStdString(buffer.str());
    
    normalizeMimemessage(qMessage);
    normalizeMimemessage(input);
    
    QCOMPARE(input.simplified(), qMessage.simplified());

}

void MIMEObjectTest::testContact(){

    Kolab::MIMEObject mimeobject;

    std::ifstream t((TESTFILEDIR.toStdString()+"v3/contacts/simple.vcf.mime").c_str());
    std::stringstream buffer;
    buffer << t.rdbuf();

    Kolab::Contact contact = mimeobject.readContact(buffer.str());
    
    std::string message = mimeobject.writeContact(contact, Kolab::KolabV3);

    QString qMessage = QString::fromStdString(message);
    QString input = QString::fromStdString(buffer.str());
    
    normalizeMimemessage(qMessage);
    normalizeMimemessage(input);
    
    QCOMPARE(input.simplified(), qMessage.simplified());

}

void MIMEObjectTest::testDistlist(){

    Kolab::MIMEObject mimeobject;

    std::ifstream t((TESTFILEDIR.toStdString()+"v3/contacts/distlist.vcf.mime").c_str());
    std::stringstream buffer;
    buffer << t.rdbuf();

    Kolab::DistList distlist = mimeobject.readDistlist(buffer.str());
    
    std::string message = mimeobject.writeDistlist(distlist, Kolab::KolabV3);

    QString qMessage = QString::fromStdString(message);
    QString input = QString::fromStdString(buffer.str());
    
    normalizeMimemessage(qMessage);
    normalizeMimemessage(input);
    
    QCOMPARE(input.simplified(), qMessage.simplified());

}
QTEST_MAIN( MIMEObjectTest )

#include "mimeobjecttest.moc"
