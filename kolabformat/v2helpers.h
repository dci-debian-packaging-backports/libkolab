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

#ifndef V2HELPERS_H
#define V2HELPERS_H

#include "kolabdefinitions.h"

#include "kolabformatV2/kolabbase.h"
#include "kolabformatV2/journal.h"
#include "kolabformatV2/task.h"
#include "kolabformatV2/event.h"
#include "kolabformatV2/contact.h"
#include "kolabformatV2/distributionlist.h"
#include "kolabformatV2/note.h"
#include "mime/mimeutils.h"
#include "kolabformat/errorhandler.h"

#include <kabc/contactgroup.h>

#include <qdom.h>
#include <qbuffer.h>
#include <akonadi/notes/noteutils.h>

namespace Kolab {


/*
 * Parse XML, create KCalCore container and extract attachments
 */
template <typename KCalPtr, typename Container>
static KCalPtr fromXML(const QByteArray &xmlData, QStringList &attachments)
{
    const QDomDocument xmlDoc = KolabV2::KolabBase::loadDocument( QString::fromUtf8(xmlData) ); //TODO extract function from V2 format
    Q_ASSERT ( !xmlDoc.isNull() );
    const KCalPtr i = Container::fromXml( xmlDoc, QString() ); //For parsing we don't need the timezone, so we don't set one
    Q_ASSERT ( i );
    QDomNodeList nodes = xmlDoc.elementsByTagName("inline-attachment");
    for (int i = 0; i < nodes.size(); i++ ) {
        attachments.append(nodes.at(i).toElement().text());
    }
    return i;
}

template <typename IncidencePtr, typename Converter>
static inline IncidencePtr incidenceFromKolabImpl( const KMime::Message::Ptr &data, const QByteArray &mimetype, const QString &timezoneId )
{
    KMime::Content *xmlContent = Mime::findContentByType( data, mimetype );
    if ( !xmlContent ) {
        Warning() << "couldn't find part";
        return IncidencePtr();
    }
    const QByteArray &xmlData = xmlContent->decodedContent();
    
    QStringList attachments;
    IncidencePtr ptr = fromXML<IncidencePtr, Converter>(xmlData, attachments); //TODO do we care about timezone?
    Mime::getAttachments(ptr, attachments, data);
    
    return ptr;
}

KABC::Addressee addresseeFromKolab( const QByteArray &xmlData, const KMime::Message::Ptr &data);
KABC::Addressee addresseeFromKolab( const QByteArray &xmlData, QString &pictureAttachmentName, QString &logoAttachmentName, QString &soundAttachmentName);

KMime::Message::Ptr contactToKolabFormat(const KolabV2::Contact& contact, const QString &productId);

KABC::ContactGroup contactGroupFromKolab(const QByteArray &xmlData);

KMime::Message::Ptr distListToKolabFormat(const KolabV2::DistributionList& distList, const QString &productId);
KMime::Message::Ptr noteFromKolab(const QByteArray &xmlData, const KDateTime &creationDate);

KMime::Message::Ptr noteToKolab(const KMime::Message::Ptr& msg, const QString &productId);
QByteArray noteToKolabXML(const KMime::Message::Ptr& msg);

QStringList readLegacyDictionaryConfiguration(const QByteArray &xmlData, QString &language);

}

#endif
