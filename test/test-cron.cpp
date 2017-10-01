/*
 * test-cron.cpp
 *
 * Copyright (c) 2017 Lix N. Paulian (lix@paulian.net)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Created on: 24 Sep 2017 (LNP)
 */

#include "test-cron.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <cmsis-plus/rtos/os.h>
#include <cmsis-plus/diag/trace.h>

#include "cronparser.h"

using namespace os;

static bool
date_to_struct (struct tm *t, char *pszDate);

static time_t
date_to_long (char *pszDate);



struct cron_pairs
{
  const char* date;
  const char* cron_def;
  const bool result;
};

struct cron_pairs cron_test_data[] =
  {
    { "20040303T221500", "L*/15 22 3 3 *", true },
    { "20040303T221400", "L*/15 22 3 3 *", false },
    { "20040303T221505", "L*/15 22 3 3 *", true },
    { "20040304T221505", "L*/15 22 3 3 *", false },

    { "20171003T221001", "L*/10 22 * * *", true },
    { "20171003T221001", "L*/10 22 * * 3", false },

    { "20040103T221401", "L30-59/15,0-29/14 22", true },
    { "20040103T224502", "L30-59/15,0-29/14 22", true },
    { "20040103T221103", "L30-59/15,0-29/14 23", false },

    { "20040103T224500", "Z */15;*/14", true},
    { "20040103T221415", "Z */15;*/14", true},
    { "20040103T223045", "Z */15 ; */14 ", true },
    { "20040103T222900", "Z */15;*/14", false},
    { "20040103T224200", "Z */15;*/14 22", true},

    { "20041231T232200", "7-37/15", true },
    { "20041231T235900", "7-37/15 ; ", false },
    { "20041231T233715", "7-37/15 ; ", true },

    { nullptr, nullptr, false },
  };
/**
 * @brief  This is a test function that exercises the cron parser.
 */
void
test_cron (void)
{
  cronparser my_cron {};

  for (struct cron_pairs* p = cron_test_data; p->date != nullptr; p++)
    {
      uint32_t timer = (uint32_t) (rtos::hrclock.now () / (SystemCoreClock / 1000000));
      bool result = my_cron.cron_check (date_to_long ((char *) p->date), (char *) p->cron_def);
      timer = (uint32_t) (rtos::hrclock.now () / (SystemCoreClock / 1000000)) - timer;

      trace::printf (
          "date: %s, cron string: \"%s\", match %s, test %s (%u us)\n", p->date,  p->cron_def,
          result ?  "yes" : "no", result == p->result ? "OK" : "failed", timer);
    }
}

static bool
date_to_struct (struct tm *t, char *pszDate)
{
  char buf[5];
  int i;

  if (t == (struct tm *) NULL)
    return false;

  strncpy (buf, pszDate, 4);
  buf[4] = '\0';
  i = atoi (buf);
  if (i <= 1900)
    t->tm_year = 1; /* too old to matter */
  else if (i > 2046)
    t->tm_year = 2046 - 1900; /* too futuristic to matter */
  else
    t->tm_year = i - 1900;

  strncpy (buf, pszDate + 4, 2);
  buf[2] = '\0';
  t->tm_mon = atoi (buf) - 1; /* 0-11 */

  strncpy (buf, pszDate + 6, 2);
  t->tm_mday = atoi (buf);

  strncpy (buf, pszDate + 9, 2);
  t->tm_hour = atoi (buf);

  strncpy (buf, pszDate + 11, 2);
  t->tm_min = atoi (buf);

  strncpy (buf, pszDate + 13, 2);
  t->tm_sec = atoi (buf);

  t->tm_wday = 0;

  t->tm_yday = 0;
  t->tm_isdst = -1;     // let mktime guess if DST is active or not (see man mktime)

#if 0
  trace::printf ("date_to_struct %04d%02d%02dT%02d%02d%02d", t->tm_year,
      t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
  trace::printf (" %s", asctime (t));
#endif

  return true; /* TODO: check errors */
}


static time_t
date_to_long (char *pszDate)
{
  struct tm tm;
  time_t t;

  if (!date_to_struct (&tm, pszDate))
    return -1;

  if (tm.tm_year < 70)
    return 0;

  t = mktime (&tm);

#if 0
  trace::printf ("date_to_long: %s %s", pszDate, ctime (&t));
#endif

  return t;
}

