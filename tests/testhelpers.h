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

#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include <contact.h>

#include <QtCore/QObject>
#include <QtTest/QtTest>
#include <kcalcore/recurrence.h>
#include <kcalcore/event.h>
#include <kcalcore/todo.h>
#include <kcalcore/journal.h>
#include <kabc/addressee.h>

#include <kolabformat.h>

Q_DECLARE_METATYPE(Kolab::Duration);
Q_DECLARE_METATYPE(Kolab::cDateTime);
Q_DECLARE_METATYPE(std::vector<Kolab::cDateTime>);
Q_DECLARE_METATYPE(Kolab::Event);
Q_DECLARE_METATYPE(std::vector<Kolab::Event>);
Q_DECLARE_METATYPE(Kolab::Todo);
Q_DECLARE_METATYPE(Kolab::Journal);
Q_DECLARE_METATYPE(Kolab::Contact);
Q_DECLARE_METATYPE(Kolab::Period);
Q_DECLARE_METATYPE(std::vector<Kolab::FreebusyPeriod>);

Q_DECLARE_METATYPE(KCalCore::Event);
Q_DECLARE_METATYPE(KCalCore::Todo);
Q_DECLARE_METATYPE(KCalCore::Journal);

#if KDEPIMLIBS_VERSION_MAJOR <= 4
#if KDEPIMLIBS_VERSION_MINOR <= 11
#if KDEPIMLIBS_VERSION_PATCH < 52
Q_DECLARE_METATYPE(KCalCore::Duration);
#endif
#endif
#endif

namespace QTest {
    
    template<>
    char *toString(const Kolab::cDateTime &dt)
    {
        QByteArray ba = "Kolab::cDateTime(";
        ba += QByteArray::number(dt.year()) + ", " + QByteArray::number(dt.month())+ ", " + QByteArray::number(dt.day()) + ", ";
        ba += QByteArray::number(dt.hour()) + ", " + QByteArray::number(dt.minute()) + ", " + QByteArray::number(dt.second())+ ", ";
        ba += QString(dt.isUTC()?QString("UTC"):QString("TZ: "+QString::fromStdString(dt.timezone()))).toAscii();
        ba += ")";
        return qstrdup(ba.data());
    }
    
    template<>
    char *toString(const KDateTime &dt)
    {
        QByteArray ba = "KDateTime(";
        ba += dt.toString().toAscii();
        ba += dt.timeZone().name();
        ba += ")";
        return qstrdup(ba.data());
    }

    template<>
    char *toString(const KCalCore::Attendee &at)
    {
        QByteArray ba = "Attendee(";
        ba += at.name().toAscii() + ", ";
        ba += at.email().toAscii() + ", ";
        ba += QString::number(at.role()) + ", ";
        ba += QString::number(at.status()) + ", ";
        ba += QString::number(at.RSVP()) + ", ";
        ba += at.delegate().toAscii() + ", ";
        ba += at.delegator().toAscii() + ", ";
        ba += at.uid().toAscii() + ", ";
        ba += ")";
        return qstrdup(ba.data());
    }


    template<>
    char *toString(const QList<int> &l)
    {
        QByteArray ba = "QList<int>(";
        foreach(int i, l) {
            ba += QString::number(i) + ", ";
        }
        ba += ")";
        return qstrdup(ba.data());
    }

    template<>
    char *toString(const QList<KCalCore::RecurrenceRule::WDayPos> &l)
    {
        QByteArray ba = "QList<int>(";
        foreach(const KCalCore::RecurrenceRule::WDayPos &i, l) {
            ba += QString::number(i.pos()) + " ";
            ba += QString::number(i.day()) + ", ";
        }
        ba += ")";
        return qstrdup(ba.data());
    }

    template<>
    char *toString(const KCalCore::DateList &l)
    {
        QByteArray ba = "KCalCore::DateList(";
        foreach(const QDate &i, l) {
            ba += i.toString();
        }
        ba += ")";
        return qstrdup(ba.data());
    }

    template<>
    char *toString(const KCalCore::DateTimeList &l)
    {
        QByteArray ba = "KCalCore::DateTimeList(";
        foreach(const KDateTime &i, l) {
            ba += toString(i);
        }
        ba += ")";
        return qstrdup(ba.data());
    }

    template<>
    char *toString(const KCalCore::Recurrence &at)
    {
        at.dump();
        KCalCore::RecurrenceRule *r = at.defaultRRule();
        QByteArray ba;
        if (!r) {
            ba += "Recurrence( )";
        } else {
            Q_ASSERT(r);
            Q_ASSERT(at.rRules().size() == 1);

            ba += "Recurrence(";
            ba += QString::number(r->recurrenceType()) + "\n";
            ba += QString::number(r->frequency()) + "\n";
            ba += QString::number(r->duration()) + "\n";
            ba += QByteArray(toString(r->startDt())) + "\n";
            ba += QByteArray(toString(r->endDt())) + "\n";
            ba += QByteArray(toString(r->bySeconds())) + "\n";
            ba += QByteArray(toString(r->byMinutes())) + "\n";
            ba += QByteArray(toString(r->byHours())) + "\n";
            ba += QByteArray(toString(r->byDays())) + "\n";
            ba += QByteArray(toString(r->byMonthDays())) + "\n";
            ba += QByteArray(toString(r->byYearDays())) + "\n";
            ba += QByteArray(toString(r->byMonths())) + "\n";
            ba += ")\n";
            ba += QByteArray(toString(at.exDates())) + "\n";
            ba += QByteArray(toString(at.exDateTimes())) + "\n";
            ba += QByteArray(toString(at.rDates())) + "\n";
            ba += QByteArray(toString(at.rDateTimes())) + "\n";

        }
        return qstrdup(ba.data());
    }

    template<>
    char *toString(const Kolab::RecurrenceRule &at)
    {
        QByteArray ba;
        ba += "KolabRecurrenceRule(";
        ba += QString::number(at.weekStart()) + "\n";
        ba += QString::number(at.frequency()) + "\n";
        ba += QString::number(at.interval()) + "\n";
        ba += QString::number(at.count()) + "\n";
        ba += QByteArray(toString(at.end())) + "\n";
        ba += QByteArray(toString(at.bysecond())) + "\n";
        ba += QByteArray(toString(at.byminute())) + "\n";
        ba += QByteArray(toString(at.byhour())) + "\n";
        ba += QByteArray(toString(at.byday())) + "\n";
        ba += QByteArray(toString(at.bymonthday())) + "\n";
        ba += QByteArray(toString(at.byyearday())) + "\n";
        ba += QByteArray(toString(at.byweekno())) + "\n";
        ba += QByteArray(toString(at.bymonth())) + "\n";
        ba += ")";
        return qstrdup(ba.data());
    }

    template<>
    char *toString(const KCalCore::Duration &d)
    {
        QByteArray ba;
        ba += "KCalCore::Duration(";
        ba += QString::number(d.isDaily()) + ", ";
        ba += QString::number(d.value()) + " ";
        ba += ")";
        return qstrdup(ba.data());
    }

    template<>
    char *toString(const Kolab::ContactReference &a)
    {
        QByteArray ba = "Kolab::ContactReference(";
        ba += QString::fromStdString(a.email()).toAscii()+ ", ";
        ba += QString::fromStdString(a.name()).toAscii()+ ", ";
        ba += QString::fromStdString(a.uid()).toAscii();
        ba += ")";
        return qstrdup(ba.data());
    }

    template<>
    char *toString(const std::vector<Kolab::ContactReference> &v)
    {
        QByteArray ba = "vector<Kolab::ContactReference>(";
        for (std::size_t i = 0; i < v.size(); i++) {
                ba += QByteArray(toString(v.at(i)))+ "\n";
            }
            ba += ")";
            return qstrdup(ba.data());
    }

    template<>
    char *toString(const Kolab::Attendee &a)
    {
        QByteArray ba = "Kolab::Attendee(";
        ba += QString::fromStdString(a.contact().email()).toAscii() + "\n";
        ba += QString::fromStdString(a.contact().name()).toAscii()+ "\n";
        ba += QByteArray::number(a.partStat()) + "\n";
        ba += QByteArray::number(a.role())  + "\n";
        ba += QByteArray::number(a.rsvp())  + "\n";
        ba += QString::fromStdString(a.contact().uid()).toAscii()+"\n";
        ba += QByteArray(toString(a.delegatedTo()))+"\n";
        ba += QByteArray(toString(a.delegatedFrom()))+ "\n";
        ba += QByteArray::number(a.cutype())+ "\n";
        ba += ")";
        return qstrdup(ba.data());
    }

    template<>
    char *toString(const std::vector<Kolab::Attendee> &v)
    {
        QByteArray ba = "vector<Kolab::Attendee>(";
        for (std::size_t i = 0; i < v.size(); i++) {
            ba += QByteArray(toString(v.at(i)))+ "\n";
            ba += QByteArray("#######################")+ "\n";
        }
        ba += ")";
        return qstrdup(ba.data());
    }

    template<>
    char *toString(const Kolab::CustomProperty &a)
    {
        QByteArray ba = "Kolab::CustomProperty(";
        ba += QString::fromStdString(a.identifier).toAscii()+ ", ";
        ba += QString::fromStdString(a.value).toAscii();
        ba += ")";
        return qstrdup(ba.data());
    }

    template<>
    char *toString(const std::vector<Kolab::CustomProperty> &v)
    {
        QByteArray ba = "vector<Kolab::CustomProperty>(";
        for (std::size_t i = 0; i < v.size(); i++) {
            ba += QByteArray(toString(v.at(i)))+ "\n";
        }
        ba += ")";
        return qstrdup(ba.data());
    }

    template<>
    char *toString(const Kolab::Period &p)
    {
        QByteArray ba = "Kolab::Period(";
        ba += QByteArray(toString(p.start))+ "\n";
        ba += QByteArray(toString(p.end))+ "\n";
        ba += ")";
        return qstrdup(ba.data());
    }

    template<>
    char *toString(const std::vector<Kolab::Period> &v)
    {
        QByteArray ba = "vector<Kolab::Period>(";
        for (std::size_t i = 0; i < v.size(); i++) {
            ba += QByteArray(toString(v.at(i)))+ "\n";
        }
        ba += ")";
        return qstrdup(ba.data());
    }

    template<>
    char *toString(const Kolab::FreebusyPeriod &p)
    {
        QByteArray ba = "Kolab::FreebusyPeriod(";
        ba += QString::number(p.type())+ "\n";
        ba += QString::fromStdString(p.eventUid())+ "\n";
        ba += QString::fromStdString(p.eventLocation())+ "\n";
        ba += QString::fromStdString(p.eventSummary())+ "\n";
        ba += QByteArray(toString(p.periods()))+ "\n";
        ba += ")";
        return qstrdup(ba.data());
    }

    template<>
    char *toString(const Kolab::Duration &p)
    {
        QByteArray ba = "Kolab::Duration";
        ba += p.isNegative() ? "-": "+";
        ba += "(";
        ba += QString::number(p.weeks())+ ", ";
        ba += QString::number(p.days())+ ", ";
        ba += QString::number(p.hours())+ ", ";
        ba += QString::number(p.minutes())+ ", ";
        ba += QString::number(p.seconds());
        ba += ")";
        return qstrdup(ba.data());
    }

}

#endif
