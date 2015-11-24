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

#include "mimeutils.h"
#include <quuid.h>
#include <QtCore/qfile.h>
#include <qdom.h>
#include <kdebug.h>
#include <kabc/addressee.h>
#include "kolabformat/kolabdefinitions.h"
#include "kolabformat/errorhandler.h"
#include "libkolab-version.h"

namespace Kolab {
    namespace Mime {

KMime::Content* findContentByType(const KMime::Message::Ptr &data, const QByteArray &type)
{
    if (type.isEmpty()) {
        Error() << "Empty type";
        return 0;
    }
    Q_ASSERT(!data->contents().isEmpty());
    Q_FOREACH(KMime::Content *c, data->contents()) {
//         qDebug() << c->contentType()->mimeType() << type;
        if (c->contentType()->mimeType() ==  type) {
            return c;
        }
    }
    return 0;
}

KMime::Content* findContentByName(const KMime::Message::Ptr &data, const QString &name, QByteArray &type)
{
    Q_ASSERT(!data->contents().isEmpty());
    Q_FOREACH(KMime::Content *c, data->contents()) {
//         qDebug() << "searching: " << c->contentType()->name().toUtf8();
        if ( c->contentType()->name() == name ) {
            type = c->contentType()->mimeType();
            return c;
        }
    }
    return 0;
}

KMime::Content* findContentById(const KMime::Message::Ptr &data, const QByteArray &id, QByteArray &type, QString &name)
{
    if (id.isEmpty()) {
        Error() << "looking for empty cid";
        return 0;
    }
    Q_ASSERT(!data->contents().isEmpty());
    Q_FOREACH(KMime::Content *c, data->contents()) {
//         kDebug() << "searching: " << c->contentID()->identifier();
        if ( c->contentID()->identifier() == id ) {
            type = c->contentType()->mimeType();
            name = c->contentType()->name();
            return c;
        }
    }
    return 0;
}

QList<QByteArray> getContentMimeTypeList(const KMime::Message::Ptr& data)
{
    QList<QByteArray> typeList;
    Q_ASSERT(!data->contents().isEmpty());
    Q_FOREACH(KMime::Content *c, data->contents()) {
        typeList.append(c->contentType()->mimeType());
    }
    return typeList;
}

QString fromCid(const QString &cid)
{
    if (cid.left(4) != QString::fromLatin1("cid:")) { //Don't set if not a cid, happens when serializing format v2
        return QString();
    }
    return cid.right(cid.size()-4);
}

KMime::Message::Ptr createMessage(const KCalCore::Incidence::Ptr &incidencePtr, const QString &mimetype, const QString &xKolabType, const QByteArray &xml, bool v3, const QString &productId)
{
    KMime::Message::Ptr message = createMessage( xKolabType, v3, productId );
    if (!incidencePtr) {
        Error() << "invalid incidence passed  in";
        message->assemble();
        return message;
    }
    if ( incidencePtr->organizer() && !incidencePtr->organizer()->email().isEmpty()) {
        message->from()->addAddress( incidencePtr->organizer()->email().toUtf8(), incidencePtr->organizer()->name() );
    }
    message->subject()->fromUnicodeString( incidencePtr->uid(), "utf-8" );
    
    KMime::Content *content = createMainPart( mimetype, xml );
    message->addContent( content );
    
    Q_FOREACH (KCalCore::Attachment::Ptr attachment, incidencePtr->attachments()) {
        //Serialize the attachment as attachment with uri, referencing the created mime-part
        if (v3 && !attachment->uri().contains("cid:")) {
            //onyl by url, skip
            continue;
        }
        message->addContent( createAttachmentPart(fromCid(attachment->uri()).toLatin1(), attachment->mimeType(), attachment->label(), attachment->decodedData() ) );
    }
    
    message->assemble();
    return message;
}

KMime::Message::Ptr createMessage(const KABC::Addressee &contact, const QString &mimetype, const QString &xKolabType, const QByteArray &xml, bool v3, const QString &prodid)
{
    KMime::Message::Ptr message = Mime::createMessage( xKolabType, v3, prodid );
    message->subject()->fromUnicodeString( contact.uid(), "utf-8" );
    message->from()->fromUnicodeString( contact.fullEmail(), "utf-8" );
    
    KMime::Content* content = Mime::createMainPart( mimetype, xml );
    message->addContent( content );

// TODO add pictures as separate mimeparts
//     if ( !contact.picture().isNull() ) {
//         QByteArray pic;
//         QBuffer buffer(&pic);
//         buffer.open(QIODevice::WriteOnly);
//         contact.picture().save(&buffer, "PNG");
//         buffer.close();
//         
//         content = Mime::createAttachmentPart(QByteArray(), "image/png", "kolab-picture.png", pic );
//         message->addContent(content);
//     }
//     
//     if ( !contact.logo().isNull() ) {
//         QByteArray pic;
//         QBuffer buffer(&pic);
//         buffer.open(QIODevice::WriteOnly);
//         contact.logo().save(&buffer, "PNG");
//         buffer.close();
//         
//         content = Mime::createAttachmentPart(QByteArray(), "image/png", "kolab-logo.png", pic );
//         message->addContent(content);
//     }
//     
//     if ( !contact.sound().isEmpty() ) {
//         content = Mime::createAttachmentPart(QByteArray(), "audio/unknown", "sound", contact.sound() );
//         message->addContent(content);
//     }
    
    message->assemble();
    return message;
}

KMime::Message::Ptr createMessage(const QString &subject, const QString &mimetype, const QString &xKolabType, const QByteArray &xml, bool v3, const QString &prodid)
{
    KMime::Message::Ptr message = createMessage( xKolabType, v3, prodid );
    if (!subject.isEmpty()) {
        message->subject()->fromUnicodeString( subject, "utf-8" );
    }
    
    KMime::Content *content = createMainPart( mimetype, xml );
    message->addContent( content );
    
    message->assemble();
    return message;
}

KMime::Content* createExplanationPart(bool v3)
{
    KMime::Content *content = new KMime::Content();
    content->contentType()->setMimeType( "text/plain" );
    content->contentType()->setCharset( "us-ascii" );
    content->contentTransferEncoding()->setEncoding( KMime::Headers::CE7Bit );
    if (v3) {
        content->setBody( "This is a Kolab Groupware object.\n"
        "To view this object you will need an email client that can understand the Kolab Groupware format.\n"
        "For a list of such email clients please visit\n"
        "http://www.kolab.org/get-kolab\n" );
    } else {
        content->setBody( "This is a Kolab Groupware object.\n"
        "To view this object you will need an email client that can understand the Kolab Groupware format.\n"
        "For a list of such email clients please visit\n"
        "http://www.kolab.org/get-kolab\n" );
    }
    return content;
}


KMime::Message::Ptr createMessage(const QString& xKolabType, bool v3, const QString &prodid)
{
    KMime::Message::Ptr message( new KMime::Message );
    message->date()->setDateTime( KDateTime::currentUtcDateTime() );
    KMime::Headers::Generic *h = new KMime::Headers::Generic( X_KOLAB_TYPE_HEADER, message.get(), xKolabType, "utf-8" );
    message->appendHeader( h );
    if (v3) {
        KMime::Headers::Generic *vh = new KMime::Headers::Generic( X_KOLAB_MIME_VERSION_HEADER, message.get(), KOLAB_VERSION_V3, "utf-8" );
        message->appendHeader( vh );
    }
    message->userAgent()->from7BitString( prodid.toLatin1() );
    message->contentType()->setMimeType( "multipart/mixed" );
    message->contentType()->setBoundary( KMime::multiPartBoundary() );
    
    message->addContent( createExplanationPart(v3) );
    return message;
}


KMime::Content* createMainPart(const QString& mimeType, const QByteArray& decodedContent)
{
    KMime::Content* content = new KMime::Content();
    content->contentType()->setMimeType( mimeType.toLatin1() );
    content->contentType()->setName( KOLAB_OBJECT_FILENAME, "us-ascii" );
    content->contentTransferEncoding()->setEncoding( KMime::Headers::CEquPr );
    content->contentDisposition()->setDisposition( KMime::Headers::CDattachment );
    content->contentDisposition()->setFilename( KOLAB_OBJECT_FILENAME );
    content->setBody( decodedContent );
    return content;
}

KMime::Content* createAttachmentPart(const QByteArray& cid, const QString& mimeType, const QString& fileName, const QByteArray& decodedContent)
{
    KMime::Content* content = new KMime::Content();
    if (!cid.isEmpty()) {
        content->contentID()->setIdentifier( cid );
    }
    content->contentType()->setMimeType( mimeType.toLatin1() );
    content->contentType()->setName( fileName, "utf-8" );
    content->contentTransferEncoding()->setEncoding( KMime::Headers::CEbase64 );
    content->contentDisposition()->setDisposition( KMime::Headers::CDattachment );
    content->contentDisposition()->setFilename( fileName );
    content->setBody( decodedContent );
    return content;
}

void getAttachments(KCalCore::Incidence::Ptr incidence, const QStringList &attachments, const KMime::Message::Ptr &mimeData)
{
    if (!incidence) {
        Error() << "Invalid incidence";
        return;
    }
//     kDebug() << "getting " << attachments.size() << "attachments";
//     kDebug() << mimeData->encodedContent();
    foreach (const QString &name, attachments) {
        QByteArray type;
        KMime::Content *content = findContentByName(mimeData, name, type);
        if (!content) { // guard against malformed events with non-existent attachments
            Warning() << "could not find attachment: "<< name.toUtf8() << type;
            continue;
        }
        const QByteArray c = content->decodedContent().toBase64();
//         Debug() << c;
        KCalCore::Attachment::Ptr attachment( new KCalCore::Attachment( c, QString::fromLatin1( type ) ) );
        attachment->setLabel( name );
        incidence->addAttachment(attachment);
        Debug() << "ATTACHMENT NAME" << name << type;
    }
}

void getAttachmentsById(KCalCore::Incidence::Ptr incidence, const KMime::Message::Ptr &mimeData)
{
    if (!incidence) {
        Error() << "Invalid incidence";
        return;
    }
//     kDebug() << "getting " << attachments.size() << "attachments";
//     kDebug() << mimeData->encodedContent();

    foreach(KCalCore::Attachment::Ptr attachment, incidence->attachments()) {
        Debug() << attachment->uri();
        if (!attachment->uri().contains("cid:")) {
            continue;
        }
        //It's a referenced attachmant, extract it
        QByteArray type;
        QString name;
        KMime::Content *content = findContentById(mimeData, fromCid(attachment->uri()).toLatin1(), type, name);
        if (!content) { // guard against malformed events with non-existent attachments
            Error() << "could not find attachment: "<< name << type;
            continue;
        }
        attachment->setUri(QString());
        attachment->setData(content->decodedContent().toBase64());
        attachment->setMimeType(type);
        attachment->setLabel(name);
    }
}



}; //Namespace
}; //Namespace
