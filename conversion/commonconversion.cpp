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

#include "commonconversion.h"
#include "timezoneconverter.h"
#include <kolabformat/errorhandler.h>

#include <iostream>
#include <KTimeZone>
#include <KSystemTimeZones>
#include <QUrl>
#include <QTimeZone>

namespace Kolab {
    namespace Conversion {

KDateTime::Spec getTimeSpec(bool isUtc, const std::string& timezone)
{
    if (isUtc) { //UTC
        return KDateTime::Spec(KDateTime::UTC);
    }
    if (timezone.empty()) { //Floating
        return  KDateTime::Spec(KDateTime::ClockTime);
    }

    //Convert non-olson timezones if necessary
    const QString normalizedTz = TimezoneConverter::normalizeTimezone(QString::fromStdString(timezone));
    if (!QTimeZone::isTimeZoneIdAvailable(normalizedTz.toLatin1())) {
        Warning() << "invalid timezone: " << QString::fromStdString(timezone) << ", assuming floating time";
        return  KDateTime::Spec(KDateTime::ClockTime);
    }
    //FIXME convert this to a proper KTimeZone
    
    return KDateTime::Spec(KSystemTimeZones::zone(normalizedTz));
}

KDateTime toDate(const Kolab::cDateTime &dt)
{
    KDateTime date;
    if (!dt.isValid()) { //We rely on this codepath, so it's not an error
        //         qDebug() << "invalid datetime converted";
        return KDateTime();
    }
    if (dt.isDateOnly()) { //Date only
        date.setDateOnly(true);
        date.setDate(QDate(dt.year(), dt.month(), dt.day()));
        date.setTimeSpec(KDateTime::Spec(KDateTime::ClockTime));
    } else {
        date.setDate(QDate(dt.year(), dt.month(), dt.day()));
        date.setTime(QTime(dt.hour(), dt.minute(), dt.second()));
        date.setTimeSpec(getTimeSpec(dt.isUTC(), dt.timezone()));
    }
    Q_ASSERT(date.timeSpec().isValid());
    Q_ASSERT(date.isValid());
    return date;
}

cDateTime fromDate(const KDateTime &dt)
{
    if (!dt.isValid()) {
        //         qDebug() << "invalid datetime converted";
        return cDateTime();
    }
    cDateTime date;
    if (dt.isDateOnly()) { //Date only
        const QDate &d = dt.date();
        date.setDate(d.year(), d.month(), d.day());
    } else {
        const QDate &d = dt.date();
        date.setDate(d.year(), d.month(), d.day());
        const QTime &t = dt.time();
        date.setTime(t.hour(), t.minute(), t.second());
        if (dt.timeType() == KDateTime::UTC) { //UTC
            date.setUTC(true);
        } else if (dt.timeType() == KDateTime::OffsetFromUTC) {
            const KDateTime utcDate = dt.toUtc();
            const QDate &d = utcDate.date();
            date.setDate(d.year(), d.month(), d.day());
            const QTime &t = utcDate.time();
            date.setTime(t.hour(), t.minute(), t.second());
            date.setUTC(true);
        } else if (dt.timeType() == KDateTime::TimeZone) { //Timezone
            //TODO handle local timezone?
            //Convert non-olson timezones if necessary
            const QString timezone = TimezoneConverter::normalizeTimezone(dt.timeZone().name());
            if (!timezone.isEmpty()) {
                date.setTimezone(toStdString(timezone));
            } else {
                Warning() << "invalid timezone: " << dt.timeZone().name() << ", assuming floating time";
                return date;
            }
        } else if (dt.timeType() != KDateTime::ClockTime) {
            Error() << "invalid timespec, assuming floating time. Type: " << dt.timeType() << "dt: " << dt.toString();
            return date;
        }
    }
    Q_ASSERT(date.isValid());
    return date;
}

QStringList toStringList(const std::vector<std::string> &l)
{
    QStringList list;
    foreach(const std::string &s, l) {
        list.append(Conversion::fromStdString(s));
    }
    return list;
}

std::vector<std::string> fromStringList(const QStringList &l)
{
    std::vector<std::string> list;
    foreach(const QString &s, l) {
        list.push_back(toStdString(s));
    }
    return list;
}

QUrl toMailto(const std::string &email, const std::string &name)
{
    std::string mailto;
    if (!name.empty()) {
        mailto.append(name);
    }
    mailto.append("<");
    mailto.append(email);
    mailto.append(">");
    return QUrl(QString::fromStdString(std::string("mailto:")+mailto));
}

std::string fromMailto(const QUrl &mailtoUri, std::string &name)
{
    const QPair<std::string, std::string> pair = fromMailto(toStdString(mailtoUri.toString()));
    name = pair.second;
    return pair.first;
}

QPair<std::string, std::string> fromMailto(const std::string &mailto)
{
    const std::string &decoded = toStdString(QUrl::fromPercentEncoding(QByteArray(mailto.c_str())));
    if (decoded.substr(0, 7).compare("mailto:")) {
        // WARNING("no mailto address");
        // std::cout << decoded << std::endl;
        return qMakePair(decoded, std::string());
    }
    std::size_t begin = decoded.find('<',7);
    if (begin == std::string::npos) {
        WARNING("no mailto address");
        std::cout << decoded << std::endl;
        return qMakePair(decoded, std::string());
    }
    std::size_t end = decoded.find('>', begin);
    if (end == std::string::npos) {
        WARNING("no mailto address");
        std::cout << decoded << std::endl;
        return qMakePair(decoded, std::string());
    }
    const std::string name = decoded.substr(7, begin-7);
    const std::string email = decoded.substr(begin+1, end-begin-1);
    return qMakePair(email, name);
}

    }
}

