/*
 * Copyright (C) 2012  Christian Mollekopf <mollekopf@kolabsys.com>
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

#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <qtemporaryfile.h>
#include <qprocess.h>
#include <quuid.h>
#include <kdebug.h>
#include <kolabevent.h>
#include <kmime/kmime_message.h>

#include "kolabformat/kolabobject.h"


Q_DECLARE_METATYPE(Kolab::ObjectType);
Q_DECLARE_METATYPE(Kolab::Version);

#define KCOMPARE(actual, expected) \
do {\
    if ( !(actual == expected) ) { \
        qDebug() << __FILE__ << ':' << __LINE__ << "Actual: " #actual ": " << actual << "\nExpceted: " #expected ": " << expected; \
        return false; \
    } \
    } while (0)


#endif

const QString TESTFILEDIR = QString::fromLatin1(TEST_DATA_PATH "/testfiles/");

QString getPath(const char *file) {
    return TESTFILEDIR+QString::fromLatin1(file);
}

void showDiff(const QString &expected, const QString &converted)
{
    if (expected.isEmpty() || converted.isEmpty()) {
        kWarning() << "files are emtpy";
        return;
    }
    if (expected == converted) {
        kWarning() << "contents are the same";
        return;
    }
    // QTemporaryFile expectedFile("expectedFile");
    // QTemporaryFile convertedFile("convertedFile");
    // if (expectedFile.open() && convertedFile.open()) {
    //     expectedFile.write(expected.toLatin1());
    //     convertedFile.write(converted.toLatin1());
    //     expectedFile.close();
    //     convertedFile.close();
    //     QProcess::execute("kompare", QStringList() << "-c" << expectedFile.fileName() << convertedFile.fileName());
    // } else {
    //     kWarning() << "files are not open";
    // }
    
    qDebug() << "EXPECTED: " << expected;
    qDebug() << "CONVERTED: " << converted;
}

KMime::Message::Ptr readMimeFile( const QString &fileName, bool &ok)
{
    //   qDebug() << fileName;
    QFile file( fileName );
    ok = file.open( QFile::ReadOnly );
    if (!ok) {
        kWarning() << "failed to open file: " << fileName;
        return KMime::Message::Ptr();
    }
    const QByteArray data = file.readAll();
    
    KMime::Message::Ptr msg = KMime::Message::Ptr(new KMime::Message);
    msg->setContent( data );
    msg->parse();
    
    return msg;
}


void normalizeMimemessage(QString &content)
{
    content.replace(QRegExp("\\bLibkolab-\\d.\\d.\\d\\b", Qt::CaseSensitive), "Libkolab-x.x.x");
    content.replace(QRegExp("\\bLibkolabxml-\\d.\\d.\\d\\b", Qt::CaseSensitive), "Libkolabxml-x.x.x");
    content.replace(QRegExp("\\bLibkolab-\\d.\\d\\b", Qt::CaseSensitive), "Libkolab-x.x.x");
    content.replace(QRegExp("\\bLibkolabxml-\\d.\\d\\b", Qt::CaseSensitive), "Libkolabxml-x.x.x");
    content.replace(QRegExp("<uri>cid:*@kolab.resource.akonadi</uri>", Qt::CaseSensitive, QRegExp::Wildcard), "<uri>cid:id@kolab.resource.akonadi</uri>");
    content.replace(QRegExp("<uri>mailto:*</uri>", Qt::CaseSensitive, QRegExp::Wildcard), "<uri>mailto:</uri>");
    content.replace(QRegExp("<cal-address>mailto:*</cal-address>", Qt::CaseSensitive, QRegExp::Wildcard), "<cal-address>mailto:</cal-address>");
    content.replace(QRegExp("<uri>data:*</uri>", Qt::CaseSensitive, QRegExp::Wildcard), "<uri>data:</uri>");
    content.replace(QRegExp("<last-modification-date>*</last-modification-date>", Qt::CaseSensitive, QRegExp::Wildcard), "<last-modification-date></last-modification-date>");
    content.replace(QRegExp("<timestamp>*</timestamp>", Qt::CaseSensitive, QRegExp::Wildcard), "<timestamp></timestamp>");
    content.replace(QRegExp("<x-kolab-version>*</x-kolab-version>", Qt::CaseSensitive, QRegExp::Wildcard), "<x-kolab-version></x-kolab-version>");

    content.replace(QRegExp("--nextPart\\S*", Qt::CaseSensitive), "--part");
    content.replace(QRegExp("\\bboundary=\"nextPart[^\\n]*", Qt::CaseSensitive), "boundary");
    content.replace(QRegExp("Date[^\\n]*", Qt::CaseSensitive), "Date");
}


//Normalize incidences for comparison
void normalizeIncidence( KCalCore::Incidence::Ptr incidence)
{   
    //The UID is not persistent (it's just the internal pointer), therefore we clear it
    //TODO make sure that the UID does really not need to be persistent
    foreach(KCalCore::Attendee::Ptr attendee, incidence->attendees()) {
        attendee->setUid(QString());
    }

    //FIXME even if hasDueDate can differ, it shouldn't because it breaks equality. Check why they differ in the first place.
    if ( incidence->type() == KCalCore::IncidenceBase::TypeTodo ) {
        KCalCore::Todo::Ptr todo = incidence.dynamicCast<KCalCore::Todo>();
        Q_ASSERT(todo.data());
        if ( !todo->hasDueDate() && !todo->hasStartDate() )
            todo->setAllDay( false ); // all day has no meaning if there are no start and due dates but may differ nevertheless
    }
}

template <template <typename> class Op, typename T>
static bool LexicographicalCompare( const T &_x, const T &_y )
{
    T x( _x );
    x.setId( QString() );
    T y( _y );
    y.setId( QString() );
    Op<QString> op;
    return op( x.toString(), y.toString() );
}

bool normalizePhoneNumbers( KABC::Addressee &addressee, KABC::Addressee &refAddressee )
{
    KABC::PhoneNumber::List phoneNumbers = addressee.phoneNumbers();
    KABC::PhoneNumber::List refPhoneNumbers = refAddressee.phoneNumbers();
    if ( phoneNumbers.size() != refPhoneNumbers.size() )
        return false;
    std::sort( phoneNumbers.begin(), phoneNumbers.end(), LexicographicalCompare<std::less, KABC::PhoneNumber> );
    std::sort( refPhoneNumbers.begin(), refPhoneNumbers.end(), LexicographicalCompare<std::less, KABC::PhoneNumber> );

    for ( int i = 0; i < phoneNumbers.size(); ++i ) {
        KABC::PhoneNumber phoneNumber = phoneNumbers.at( i );
        const KABC::PhoneNumber refPhoneNumber = refPhoneNumbers.at( i );
        KCOMPARE( LexicographicalCompare<std::equal_to>( phoneNumber, refPhoneNumber ), true );
        addressee.removePhoneNumber( phoneNumber );
        phoneNumber.setId( refPhoneNumber.id() );
        addressee.insertPhoneNumber( phoneNumber );
        //Make sure that both have the same sorted order
        refAddressee.removePhoneNumber( refPhoneNumber );
        refAddressee.insertPhoneNumber( refPhoneNumber );
    }
//     for ( int i = 0; i < phoneNumbers.size(); ++i ) {
//         kDebug() << "--------------------------------------";
//         kDebug() << addressee.phoneNumbers().at(i).toString();
//         kDebug() << refAddressee.phoneNumbers().at(i).toString();
//     }

    return true;
}

bool normalizeAddresses( KABC::Addressee &addressee, const KABC::Addressee &refAddressee )
{
    KABC::Address::List addresses = addressee.addresses();
    KABC::Address::List refAddresses = refAddressee.addresses();
    if ( addresses.size() != refAddresses.size() )
        return false;
    std::sort( addresses.begin(), addresses.end(), LexicographicalCompare<std::less, KABC::Address> );
    std::sort( refAddresses.begin(), refAddresses.end(), LexicographicalCompare<std::less, KABC::Address> );

    for ( int i = 0; i < addresses.size(); ++i ) {
        KABC::Address address = addresses.at( i );
        const KABC::Address refAddress = refAddresses.at( i );
        KCOMPARE( LexicographicalCompare<std::equal_to>( address, refAddress ), true );
        addressee.removeAddress( address );
        address.setId( refAddress.id() );
        addressee.insertAddress( address );
    }

    return true;
}

void normalizeContact(KABC::Addressee &addressee)
{
    KABC::Address::List addresses = addressee.addresses();
    
    foreach(KABC::Address a, addresses) {
        addressee.removeAddress(a);
        a.setPostOfficeBox(QString()); //Not supported anymore
        addressee.insertAddress(a);
    }
    addressee.setSound(KABC::Sound()); //Sound is not supported
    
    addressee.removeCustom("KOLAB", "CreationDate"); //The creation date is no longer existing

    //Attachment names are no longer required because we identify the parts by cid and no longer by name
    addressee.removeCustom("KOLAB", "LogoAttachmentName");
    addressee.removeCustom("KOLAB", "PictureAttachmentName");
    addressee.removeCustom("KOLAB", "SoundAttachmentName");
    
}

Kolab::Event createEvent(const Kolab::cDateTime &start, const Kolab::cDateTime &end)
{
    Kolab::Event event;
    event.setUid(QUuid::createUuid().toString().toStdString());
    event.setStart(start);
    event.setEnd(end);
    return event;
}
