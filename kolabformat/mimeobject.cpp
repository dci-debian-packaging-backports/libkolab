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

#include "mimeobject.h"
#include "conversion/kcalconversion.h"
#include "conversion/kolabconversion.h"
#include "conversion/kabcconversion.h"
#include "kolabformat/kolabobject.h"
#include <QString>

namespace Kolab
{

MIMEObject::MIMEObject()
{

}

std::string MIMEObject::writeEvent(const Event &event, Version version, const std::string &productId)
{

    KCalCore::Event::Ptr KEvent = Conversion::toKCalCore(event);

    KMime::Message::Ptr msg = KolabObjectWriter().writeEvent(KEvent, version, QString::fromStdString(productId));
    msg->assemble();

    return msg->encodedContent().data();
}

Event MIMEObject::readEvent(const std::string &s)
{

    KMime::Message::Ptr msg(new KMime::Message);
    msg->setContent(QByteArray(s.c_str()));
    msg->parse();
    
    KCalCore::Event::Ptr event = KolabObjectReader(msg).getEvent();
    
    return Conversion::fromKCalCore(*event); 
}

std::string MIMEObject::writeTodo(const Todo &todo, Version version, const std::string &productId){
    KCalCore::Todo::Ptr kTodo = Conversion::toKCalCore(todo);

    KMime::Message::Ptr msg = KolabObjectWriter().writeTodo(kTodo, version, QString::fromStdString(productId));
    msg->assemble();

    return msg->encodedContent().data();
}


Todo MIMEObject::readTodo(const std::string &s){

    KMime::Message::Ptr msg(new KMime::Message);
    msg->setContent(QByteArray(s.c_str()));
    msg->parse();
    
    KCalCore::Todo::Ptr todo = KolabObjectReader(msg).getTodo();
    
    return Conversion::fromKCalCore(*todo);
}


std::string MIMEObject::writeJournal(const Journal &journal, Version version, const std::string &productId){
    KCalCore::Journal::Ptr kJournal = Conversion::toKCalCore(journal);

    KMime::Message::Ptr msg = KolabObjectWriter().writeJournal(kJournal, version, QString::fromStdString(productId));
    msg->assemble();

    return msg->encodedContent().data();
}


Journal MIMEObject::readJournal(const std::string &s){

    KMime::Message::Ptr msg(new KMime::Message);
    msg->setContent(QByteArray(s.c_str()));
    msg->parse();
    
    KCalCore::Journal::Ptr journal = KolabObjectReader(msg).getJournal();
    
    return Conversion::fromKCalCore(*journal);
}

std::string MIMEObject::writeNote(const Note &note, Version version, const std::string &productId){
    KMime::Message::Ptr kNote = Conversion::toNote(note);

    KMime::Message::Ptr msg = KolabObjectWriter().writeNote(kNote, version, QString::fromStdString(productId));
    msg->assemble();

    return msg->encodedContent().data();
}


Note MIMEObject::readNote(const std::string &s){

    KMime::Message::Ptr msg(new KMime::Message);
    msg->setContent(QByteArray(s.c_str()));
    msg->parse();
    
    KMime::Message::Ptr note = KolabObjectReader(msg).getNote();
    
    return Conversion::fromNote(note);
}

std::string MIMEObject::writeContact(const Contact &contact, Version version, const std::string &productId){
    KABC::Addressee kContact = Conversion::toKABC(contact);

    KMime::Message::Ptr msg = KolabObjectWriter().writeContact(kContact, version, QString::fromStdString(productId));
    msg->assemble();

    return msg->encodedContent().data();
}


Contact MIMEObject::readContact(const std::string &s){

    KMime::Message::Ptr msg(new KMime::Message);
    msg->setContent(QByteArray(s.c_str()));
    msg->parse();
    
    KABC::Addressee contact = KolabObjectReader(msg).getContact();
    
    return Conversion::fromKABC(contact);
}

std::string MIMEObject::writeDistlist(const DistList &distlist, Version version, const std::string &productId){
    KABC::ContactGroup kDistlist = Conversion::toKABC(distlist);

    KMime::Message::Ptr msg = KolabObjectWriter().writeDistlist(kDistlist, version, QString::fromStdString(productId));
    msg->assemble();

    return msg->encodedContent().data();
}


DistList MIMEObject::readDistlist(const std::string &s){

    KMime::Message::Ptr msg(new KMime::Message);
    msg->setContent(QByteArray(s.c_str()));
    msg->parse();
    
    KABC::ContactGroup distlist = KolabObjectReader(msg).getDistlist();
    
    return Conversion::fromKABC(distlist);
}
}

