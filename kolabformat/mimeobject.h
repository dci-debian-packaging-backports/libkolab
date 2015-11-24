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

#ifndef MIMEOBJECT_H
#define MIMEOBJECT_H

#ifndef SWIG
#include "kolab_export.h"
#else
/* No export/import SWIG interface files */
#define KOLAB_EXPORT
#endif

#include <kolabformat.h>
#include "kolabdefinitions.h"


namespace Kolab
{

class KOLAB_EXPORT MIMEObject
{
public:
    MIMEObject();

    std::string writeEvent(const Kolab::Event  &event, Version version, const std::string &productId = std::string());
    Kolab::Event readEvent(const std::string &s);

    std::string writeTodo(const Kolab::Todo &todo, Version version, const std::string &productId = std::string());
    Kolab::Todo readTodo(const std::string &s);

    std::string writeJournal(const Kolab::Journal &journal, Version version, const std::string &productId = std::string());
    Kolab::Journal readJournal(const std::string &s);

    std::string writeNote(const Kolab::Note &note, Version version, const std::string &productId = std::string());
    Kolab::Note readNote(const std::string &s);

    std::string writeContact(const Kolab::Contact &contact, Version version, const std::string &productId = std::string());
    Kolab::Contact readContact(const std::string &s);

    std::string writeDistlist(const Kolab::DistList &distlist, Version version, const std::string &productId = std::string());
    Kolab::DistList readDistlist(const std::string &s);

};
}
#endif  
