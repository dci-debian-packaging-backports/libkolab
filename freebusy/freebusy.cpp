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


#include "freebusy.h"
#include "conversion/kcalconversion.h"
#include "conversion/commonconversion.h"
#include <kcalcore/freebusy.h>
#include <kcalcore/icalformat.h>
#include <kdebug.h>
#include <quuid.h>


// namespace KCalCore {
//     struct KCalFreebusy
// {
//
// void init( const Event::List &eventList, const KDateTime &start, const KDateTime &end )
// {
//     mDtStart = start.toUtc();
//     mDtEnd = end.toUtc();
//
//   // Loops through every event in the calendar
//   Event::List::ConstIterator it;
//   for ( it = eventList.constBegin(); it != eventList.constEnd(); ++it ) {
//     Event::Ptr event = *it;
//
//     // If this event is transparent it shouldn't be in the freebusy list.
//     if ( event->transparency() == Event::Transparent ) {
//       continue;
//     }
//
//     if ( event->hasRecurrenceId() ) {
//       continue; //TODO apply special period exception (duration could be different)
//     }
//
//     const KDateTime eventStart = event->dtStart().toUtc();
//     const KDateTime eventEnd = event->dtEnd().toUtc();
//
//     if ( event->recurs() ) {
//         const KCalCore::Duration duration( eventStart, eventEnd );
//         const KCalCore::DateTimeList list = event->recurrence()->timesInInterval(start, end);
//         foreach (const KDateTime &dt, list) {
//             const KDateTime utc = dt.toUtc();
//             addLocalPeriod(utc, duration.end(utc) );
//         }
//     } else {
//         addLocalPeriod( eventStart, eventEnd );
//     }
//   }
//
// //   q->sortList();
// }
//
// bool addLocalPeriod(
//                                         const KDateTime &eventStart,
//                                         const KDateTime &eventEnd )
// {
//   KDateTime tmpStart;
//   KDateTime tmpEnd;
//
//   //Check to see if the start *or* end of the event is
//   //between the start and end of the freebusy dates.
//   if ( !( ( ( mDtStart.secsTo( eventStart ) >= 0 ) &&
//             ( eventStart.secsTo( mDtEnd ) >= 0 ) ) ||
//           ( ( mDtStart.secsTo( eventEnd ) >= 0 ) &&
//             ( eventEnd.secsTo( mDtEnd ) >= 0 ) ) ) ) {
//       qDebug() << "out of scope";
//     return false;
//   }
//
// //   qDebug() << eventStart.date().toString() << eventStart.time().toString() << mDtStart.toString();
//   if ( eventStart < mDtStart ) { //eventStart is before start
// //       qDebug() << "use start";
//     tmpStart = mDtStart;
//   } else {
//     tmpStart = eventStart;
//   }
//
//   qDebug() << eventEnd.date().toString() << eventEnd.time().toString() << mDtEnd.toString();
//   if ( eventEnd > mDtEnd ) { //event end is after dtEnd
// //     qDebug() << "use end";
//     tmpEnd = mDtEnd;
//   } else {
//     tmpEnd = eventEnd;
//   }
//
// //   qDebug() << "########## " << tmpStart.isValid();
//   Q_ASSERT(tmpStart.isValid());
//   Q_ASSERT(tmpEnd.isValid());
// //   qDebug() << tmpStart.date().toString() << tmpStart.time().toString() << tmpStart.toString();
//
//   FreeBusyPeriod p( tmpStart, tmpEnd );
//   mBusyPeriods.append( p );
//
//   return true;
// }
//
//     KDateTime mDtStart;
//     KDateTime mDtEnd;                  // end datetime
//     FreeBusyPeriod::List mBusyPeriods; // list of periods
//
// };
//
// } // Namespace







namespace Kolab {
    namespace FreebusyUtils {


Kolab::Period addLocalPeriod(  const KDateTime &eventStart, const KDateTime &eventEnd, const KDateTime &mDtStart, const KDateTime &mDtEnd)
{
  KDateTime tmpStart;
  KDateTime tmpEnd;

  //Check to see if the start *or* end of the event is
  //between the start and end of the freebusy dates.
  if ( !( ( ( mDtStart <= eventStart) &&
            ( eventStart <= mDtEnd ) ) ||
          ( ( mDtStart <= eventEnd ) &&
            ( eventEnd <= mDtEnd ) ) ) ) {
    qDebug() << "event is not within the fb range, skipping";
    return Kolab::Period();
  }

  if ( eventStart < mDtStart ) { //eventStart is before start
    tmpStart = mDtStart;
  } else {
    tmpStart = eventStart;
  }

//   qDebug() << eventEnd.date().toString() << eventEnd.time().toString() << mDtEnd.toString();
  if ( eventEnd > mDtEnd ) { //event end is after dtEnd
    tmpEnd = mDtEnd;
  } else {
    tmpEnd = eventEnd;
  }
  Q_ASSERT(tmpStart.isValid());
  Q_ASSERT(tmpEnd.isValid());
  if (tmpStart.isDateOnly()) {
    tmpStart.setTime(QTime(0,0,0,0));
  }
  if (tmpEnd.isDateOnly()) {
    tmpEnd.setTime(QTime(23,59,59,999)); //The window is inclusive
  }
  return Kolab::Period(Kolab::Conversion::fromDate(tmpStart), Kolab::Conversion::fromDate(tmpEnd));
}

Freebusy generateFreeBusy(const std::vector< Event >& events, const cDateTime& startDate, const cDateTime& endDate)
{
    QList<KCalCore::Event::Ptr> list;
    foreach (const Kolab::Event &e, events) {
        list.append(Kolab::Conversion::toKCalCore(e));
    }
    KCalCore::Person::Ptr person(new KCalCore::Person("dummyname", "dummyemail"));
    return generateFreeBusy(list, Kolab::Conversion::toDate(startDate), Kolab::Conversion::toDate(endDate), person);
}

Freebusy generateFreeBusy(const QList<KCalCore::Event::Ptr>& events, const KDateTime& startDate, const KDateTime& endDate, const KCalCore::Person::Ptr &organizer)
{
    /*
     * TODO the conversion of date-only values to date-time is only necessary because xCal doesn't allow date only. iCalendar doesn't seem to make this restriction so it looks like a bug.
     */
    KDateTime start = startDate.toUtc();
    if (start.isDateOnly()) {
        start.setTime(QTime(0,0,0,0));
    }
    KDateTime end = endDate.toUtc();
    if (end.isDateOnly()) {
        end.addDays(1);
        end.setTime(QTime(0,0,0,0)); //The window is inclusive
    }

    //TODO try to merge that with KCalCore::Freebusy
    std::vector<Kolab::FreebusyPeriod> freebusyPeriods;
    Q_FOREACH (KCalCore::Event::Ptr event, events) {    
        // If this event is transparent it shouldn't be in the freebusy list.
        if ( event->transparency() == KCalCore::Event::Transparent ) {
            continue;
        }

        if ( event->hasRecurrenceId() ) {
            continue; //TODO apply special period exception (duration could be different)
        }

        const KDateTime eventStart = event->dtStart().toUtc();
        const KDateTime eventEnd = event->dtEnd().toUtc();

        std::vector <Kolab::Period> periods;
        if ( event->recurs() ) {
            const KCalCore::Duration duration( eventStart, eventEnd );
            const KCalCore::DateTimeList list = event->recurrence()->timesInInterval(start, end);
            Q_FOREACH (const KDateTime &dt, list) {
                const KDateTime utc = dt.toUtc();
                const Kolab::Period &period = addLocalPeriod(utc, duration.end(utc), start, end);
                if (period.isValid()) {
                    periods.push_back(period);
                }
            }
        } else {
            const Kolab::Period &period = addLocalPeriod(eventStart, eventEnd, start, end);
            if (period.isValid()) {
                periods.push_back(period);
            }
        }
        if (!periods.empty()) {
            Kolab::FreebusyPeriod period;
            period.setPeriods(periods);
            //TODO get busy type from event (out-of-office, tentative)
            period.setType(Kolab::FreebusyPeriod::Busy);
            period.setEvent(Kolab::Conversion::toStdString(event->uid()), Kolab::Conversion::toStdString(event->summary()), Kolab::Conversion::toStdString(event->location()));
            freebusyPeriods.push_back(period);
        }
    }

    Kolab::Freebusy freebusy;
    
    freebusy.setStart(Kolab::Conversion::fromDate(start));
    freebusy.setEnd(Kolab::Conversion::fromDate(end));
    freebusy.setPeriods(freebusyPeriods);
    freebusy.setUid(QUuid::createUuid().toString().toStdString());
    freebusy.setTimestamp(Kolab::Conversion::fromDate(KDateTime::currentUtcDateTime()));
    if (organizer) {
        freebusy.setOrganizer(ContactReference(Kolab::ContactReference::EmailReference, Kolab::Conversion::toStdString(organizer->email()), Kolab::Conversion::toStdString(organizer->name())));
    }
    
    return freebusy;
}

Freebusy aggregateFreeBusy(const std::vector< Freebusy >& fbList, const std::string &organizerEmail, const std::string &organizerName, bool simple)
{
    std::vector <Kolab::FreebusyPeriod > periods;

    KDateTime start;
    KDateTime end;
    Q_FOREACH (const Freebusy &fb, fbList) {

        const KDateTime &tmpStart = Kolab::Conversion::toDate(fb.start());
        if (!start.isValid() || tmpStart < start) {
            start = tmpStart;
        }
        const KDateTime &tmpEnd = Kolab::Conversion::toDate(fb.end());
        if (!end.isValid() || tmpEnd > end) {
            end = tmpEnd;
        }

        Q_FOREACH (const Kolab::FreebusyPeriod &period, fb.periods()) {
            Kolab::FreebusyPeriod simplifiedPeriod;
            simplifiedPeriod.setPeriods(period.periods());
            simplifiedPeriod.setType(period.type());
            if (!simple) { //Don't copy and reset to avoid unintentional information leaking into simple lists
                simplifiedPeriod.setEvent(period.eventSummary(), period.eventUid(), period.eventLocation());
            }
            periods.push_back(simplifiedPeriod);
        }
    }
    
    Freebusy aggregateFB;

    aggregateFB.setStart(Kolab::Conversion::fromDate(start));
    aggregateFB.setEnd(Kolab::Conversion::fromDate(end));
    aggregateFB.setPeriods(periods);
    aggregateFB.setUid(QUuid::createUuid().toString().toStdString());
    aggregateFB.setTimestamp(Kolab::Conversion::fromDate(KDateTime::currentUtcDateTime()));
    aggregateFB.setOrganizer(ContactReference(Kolab::ContactReference::EmailReference, organizerEmail, organizerName));
    return aggregateFB;
}

std::string toIFB(const Kolab::Freebusy &freebusy)
{
    KCalCore::FreeBusy::Ptr fb(new KCalCore::FreeBusy(Kolab::Conversion::toDate(freebusy.start()), Kolab::Conversion::toDate(freebusy.end())));
    KCalCore::FreeBusyPeriod::List list;
    Q_FOREACH (const Kolab::FreebusyPeriod &fbPeriod, freebusy.periods()) {
        Q_FOREACH (const Kolab::Period &p, fbPeriod.periods()) {
            KCalCore::FreeBusyPeriod period(Kolab::Conversion::toDate(p.start), Kolab::Conversion::toDate(p.end));
//             period.setSummary("summary"); Doesn't even work. X-SUMMARY is read though (just not written out)
            //TODO
            list.append(period);
            
        }
    }
    fb->addPeriods(list);

    fb->setUid(QString::fromStdString(freebusy.uid()));
    fb->setOrganizer(KCalCore::Person::Ptr(new KCalCore::Person(Conversion::fromStdString(freebusy.organizer().name()), Conversion::fromStdString(freebusy.organizer().email()))));
    fb->setLastModified(Kolab::Conversion::toDate(freebusy.timestamp()));

    KCalCore::ICalFormat format;
    QString data = format.createScheduleMessage( fb, KCalCore::iTIPPublish );
    return Conversion::toStdString(data);
}

    }
}
