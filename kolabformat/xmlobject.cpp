/*
 * Copyright (C) 2012  Christian Mollekopf <mollekopf@kolabsys.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "xmlobject.h"
#include "v2helpers.h"
#include "kolabformatV2/event.h"
#include "conversion/kcalconversion.h"
#include "conversion/kolabconversion.h"
#include "conversion/commonconversion.h"
#include "conversion/kabcconversion.h"
#include <QUuid>

namespace Kolab {

static QString createUuid()
{
    const QString uuid = QUuid::createUuid().toString();
    return uuid.mid(1, uuid.size()-2);
}

XMLObject::XMLObject()
{

}

std::string XMLObject::getSerializedUID() const
{
    return mWrittenUID;
}
    
std::vector< std::string > XMLObject::getAttachments() const
{
    return mAttachments;
}

std::string XMLObject::writeEvent(const Event &event, Version version, const std::string& productId)
{
    mWrittenUID.clear();
    if (version == KolabV2) {
        const KCalCore::Event::Ptr i = Conversion::toKCalCore(event);
        if (!i) {
            Critical() << "invalid incidence";
            return std::string();
        }
        if (i->uid().isEmpty()) {
            i->setUid(createUuid());
        }
        mWrittenUID = Conversion::toStdString(i->uid());
        //The timezone is used for created and last modified dates
        const QString &xml = KolabV2::Event::eventToXML(i, QLatin1String("UTC"));
        return Conversion::toStdString(xml);
    }
    const std::string result = Kolab::writeEvent(event, productId);
    mWrittenUID = Kolab::getSerializedUID();
    return result;
}

Event XMLObject::readEvent(const std::string& s, Version version)
{
    if (version == KolabV2) {
        QStringList attachments;
        const KCalCore::Event::Ptr event = Kolab::fromXML<KCalCore::Event::Ptr, KolabV2::Event>(QString::fromUtf8(s.c_str()).toUtf8(), attachments);
        if (!event || Kolab::ErrorHandler::errorOccured()) {
            Critical() << "failed to read xml";
            return Event();
        }
        mAttachments.clear();
        foreach (const QString &attachment, attachments) {
            mAttachments.push_back(Conversion::toStdString(attachment));
        }
        return Conversion::fromKCalCore(*event);
    }
    return Kolab::readEvent(s, false);
}

std::string XMLObject::writeTodo(const Todo &event, Version version, const std::string& productId)
{
    mWrittenUID.clear();
    if (version == KolabV2) {
        const KCalCore::Todo::Ptr i = Conversion::toKCalCore(event);
        if (!i) {
            Critical() << "invalid incidence";
            return std::string();
        }
        if (i->uid().isEmpty()) {
            i->setUid(createUuid());
        }
        mWrittenUID = Conversion::toStdString(i->uid());
        //The timezone is used for created and last modified dates
        const QString &xml = KolabV2::Task::taskToXML(i, QLatin1String("UTC"));
        return Conversion::toStdString(xml);
    }
    const std::string result = Kolab::writeTodo(event, productId);
    mWrittenUID = Kolab::getSerializedUID();
    return result;
}

Todo XMLObject::readTodo(const std::string& s, Version version)
{
    if (version == KolabV2) {
        QStringList attachments;
        const KCalCore::Todo::Ptr event = Kolab::fromXML<KCalCore::Todo::Ptr, KolabV2::Task>(QString::fromUtf8(s.c_str()).toUtf8(), attachments);
        if (!event || Kolab::ErrorHandler::errorOccured()) {
            Error() << "failed to read xml";
            return Todo();
        }
        mAttachments.clear();
        foreach (const QString &attachment, attachments) {
            mAttachments.push_back(Conversion::toStdString(attachment));
        }
        return Conversion::fromKCalCore(*event);
    }
    return Kolab::readTodo(s, false);
}

std::string XMLObject::writeJournal(const Journal &event, Version version, const std::string& productId)
{
    mWrittenUID.clear();
    if (version == KolabV2) {
        const KCalCore::Journal::Ptr i = Conversion::toKCalCore(event);
        if (!i) {
            Critical() << "invalid journal";
            return std::string();
        }
        if (i->uid().isEmpty()) {
            i->setUid(createUuid());
        }
        mWrittenUID = Conversion::toStdString(i->uid());
        //The timezone is used for created and last modified dates
        const QString &xml = KolabV2::Journal::journalToXML(i, QLatin1String("UTC"));
        return Conversion::toStdString(xml);
    }
    const std::string result = Kolab::writeJournal(event, productId);
    mWrittenUID = Kolab::getSerializedUID();
    return result;
}

Journal XMLObject::readJournal(const std::string& s, Version version)
{
    if (version == KolabV2) {
        QStringList attachments;
        const KCalCore::Journal::Ptr event = Kolab::fromXML<KCalCore::Journal::Ptr, KolabV2::Journal>(QString::fromUtf8(s.c_str()).toUtf8(), attachments);
        if (!event || Kolab::ErrorHandler::errorOccured()) {
            Critical() << "failed to read xml";
            return Journal();
        }
        mAttachments.clear();
        foreach (const QString &attachment, attachments) {
            mAttachments.push_back(Conversion::toStdString(attachment));
        }
        return Conversion::fromKCalCore(*event);
    }
    return Kolab::readJournal(s, false);
}

std::string XMLObject::writeFreebusy(const Freebusy &event, Version version, const std::string& productId)
{
    mWrittenUID.clear();
    if (version != KolabV3) {
        Critical() << "only v3 implementation available";
        return std::string();
    }
    const std::string result = Kolab::writeFreebusy(event, productId);
    mWrittenUID = Kolab::getSerializedUID();
    return result;
}

Freebusy XMLObject::readFreebusy(const std::string& s, Version version)
{
    if (version != KolabV3) {
        Critical() << "only v3 implementation available";
        return Freebusy();
    }
    return Kolab::readFreebusy(s, false);
}

std::string XMLObject::logoAttachmentName() const
{
    return mLogoAttachmentName;
}

std::string XMLObject::pictureAttachmentName() const
{
    return mPictureAttachmentName;
}

std::string XMLObject::soundAttachmentName() const
{
    return mSoundAttachmentName;
}

Contact XMLObject::readContact(const std::string& s, Version version)
{
    if (version == KolabV2) {        
        const QByteArray xmlData(s.c_str(), s.size());
        QString pictureAttachmentName;
        QString logoAttachmentName;
        QString soundAttachmentName;
        const KABC::Addressee addressee = addresseeFromKolab(xmlData, pictureAttachmentName, logoAttachmentName, soundAttachmentName);
        mPictureAttachmentName = Conversion::toStdString(pictureAttachmentName);
        mLogoAttachmentName = Conversion::toStdString(logoAttachmentName);
        mSoundAttachmentName = Conversion::toStdString(soundAttachmentName);
        return Conversion::fromKABC(addressee);
    }
    return Kolab::readContact(s, false);
}

std::string XMLObject::writeContact(const Contact &contact, Version version, const std::string& productId)
{
    mWrittenUID.clear();
    if (version == KolabV2) {
        //FIXME attachment names are hardcoded for now
        KABC::Addressee addressee = Conversion::toKABC(contact);
        if (addressee.uid().isEmpty()) {
            addressee.setUid(createUuid());
        }
        mWrittenUID = Conversion::toStdString(addressee.uid());
        const KolabV2::Contact contact(&addressee);
        return Conversion::toStdString(contact.saveXML());
    }
    const std::string result = Kolab::writeContact(contact, productId);
    mWrittenUID = Kolab::getSerializedUID();
    return result;
}

DistList XMLObject::readDistlist(const std::string& s, Version version)
{
    if (version == KolabV2) {        
        const QByteArray xmlData(s.c_str(), s.size());
        const KABC::ContactGroup contactGroup = contactGroupFromKolab(xmlData);
        return Conversion::fromKABC(contactGroup);
    }
    return Kolab::readDistlist(s, false);
}

std::string XMLObject::writeDistlist(const DistList &distlist, Version version, const std::string& productId)
{
    mWrittenUID.clear();
    if (version == KolabV2) {
        KABC::ContactGroup contactGroup = Conversion::toKABC(distlist);
        if (contactGroup.id().isEmpty()) {
            contactGroup.setId(createUuid());
        }
        mWrittenUID = Conversion::toStdString(contactGroup.id());
        const KolabV2::DistributionList d(&contactGroup);
        return Conversion::toStdString(d.saveXML());
    }
    const std::string result = Kolab::writeDistlist(distlist, productId);
    mWrittenUID = Kolab::getSerializedUID();
    return result;
}

Note XMLObject::readNote(const std::string& s, Version version)
{
    if (version == KolabV2) {
        const KMime::Message::Ptr msg = noteFromKolab(QByteArray(s.c_str(), s.length()), KDateTime());
        if (!msg || Kolab::ErrorHandler::errorOccured()) {
            Critical() << "failed to read xml";
            return Note();
        }
        return Conversion::fromNote(msg);
    }
    return Kolab::readNote(s, false);
}

std::string XMLObject::writeNote(const Note &note, Version version, const std::string& productId)
{
    mWrittenUID.clear();
    if (version == KolabV2) {
        Note noteWithUID = note;
        if (noteWithUID.uid().empty()) {
            noteWithUID.setUid(Conversion::toStdString(createUuid()));
        }
        mWrittenUID = noteWithUID.uid();
        const KMime::Message::Ptr n = Conversion::toNote(noteWithUID);
        const QByteArray &xml = noteToKolabXML(n);
        return std::string(xml.constData());
    }
    const std::string result = Kolab::writeNote(note, productId);
    mWrittenUID = Kolab::getSerializedUID();
    return result;
}

Configuration XMLObject::readConfiguration(const std::string& s, Version version)
{
    if (version == KolabV2) {
        QString lang;
        const QStringList dict = readLegacyDictionaryConfiguration(QByteArray(s.c_str(), s.length()), lang);
        if (lang.isEmpty()) {
            Critical() << "not a dictionary or not a v2 configuration object";
            return Kolab::Configuration();
        }
        std::vector<std::string> entries;
        foreach (const QString e, dict) {
            entries.push_back(Conversion::toStdString(e));
        }
        Kolab::Dictionary dictionary(Conversion::toStdString(lang));
        dictionary.setEntries(entries);
        return Configuration(dictionary);
    }
    return Kolab::readConfiguration(s, false);
}

std::string XMLObject::writeConfiguration(const Configuration &configuration, Version version, const std::string& productId)
{
    mWrittenUID.clear();
    if (version != KolabV3) {
        Critical() << "only v3 implementation available";
        return std::string();
    }
    const std::string result = Kolab::writeConfiguration(configuration, productId);
    mWrittenUID = Kolab::getSerializedUID();
    return result;
}

File XMLObject::readFile(const std::string& s, Version version)
{
    if (version == KolabV2) {
        Critical() << "only v3 implementation available";
        return File();
    }
    return Kolab::readFile(s, false);
}

std::string XMLObject::writeFile(const File &file, Version version, const std::string& productId)
{
    mWrittenUID.clear();
    if (version != KolabV3) {
        Critical() << "only v3 implementation available";
        return std::string();
    }
    const std::string result = Kolab::writeFile(file, productId);
    mWrittenUID = Kolab::getSerializedUID();
    return result;
}

    
};
