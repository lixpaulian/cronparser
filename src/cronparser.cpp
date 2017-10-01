/*
 * cronparser.cpp
 *
 *
 * (c) 2015, 2017 Lix N. Paulian (lix@paulian.net)
 *
 * Based on previous work by Liviu Ionescu (see copyright notice below).
 * The copyright notice from the original work applies also to the modified
 * work.
 *
 * Created on: 28 Mar 2015 (LNP)
 * Moved to C++: 1 Oct 2017 (LNP)
 *
 * Version 2.0
 *
 */

/*-
 * Copyright (c) 2005-2007 Liviu Ionescu <ilg@livius.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * CRON.LIB - Simple CRON Parser Package
 *
 * This is a simple implementation of a CRON parser. It is intended to
 * embedded applications, to schedule various events.
 *
 * The content of the input string is a ';' separated list of CRON
 * definitions, like: "L30-59/15,0-29/14 1". The first letter defines
 * whether local (L) or UTC (Z) time is used, the default being local time.
 *
 * The other fields are:
 * - minute (0-59)
 * - hour (0-23)
 * - day of month (1-31)
 * - month (1-12)
 * - day of week (0-6, 0=Sunday)
 *
 * Regular UNIX syntax is accepted, i.e. a comma separated list of ranges.
 * Ranges accept step definition. '*' is considered a full range.
 *
 * Given a date (time_t format) and a cron string, the parser returns true
 * if the two parameters match, or false otherwise.
 *
 */

#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>
#include <cmsis-plus/diag/trace.h>

#include "cronparser.h"

// set to true to enable debug messages
#define DEBUG_CRON_PARSER false

using namespace os;

/**
 * @brief Constructor.
 */
cronparser::cronparser (void)
{
  trace::printf ("%s @%p\n", __func__, this);
}

/**
 * @brief Check if a date matches a cron string.
 * @param time_in: Unix time (a date).
 * @param ps: cron string.
 * @return true if there the input parameters match, false otherwise.
 */
bool
cronparser::cron_check (time_t time_in, char* ps)
{
  struct tm* tm;
  int local;
  int result = false;

  if (ps == nullptr)
    return false;

  while (isspace (*(uint8_t *) ps))
    ps++;

  if (*ps == 'Z')
    {
      local = false;
      ps++;
    }
  else
    {
      local = true;
      if (*ps == 'L')
        {
          ps++;
        }
    }

  // split time into components
  if (local)
    tm = localtime (&time_in);
  else
    tm = gmtime (&time_in);

  tm->tm_sec = 0;

  // clear all flag arrays, shortcut to = FALSE
  memset (cron_minutes_, 0, sizeof(cron_minutes_));
  memset (cron_hours_, 0, sizeof(cron_hours_));
  memset (cron_mdays_, 0, sizeof(cron_mdays_));
  memset (cron_months_, 0, sizeof(cron_months_));
  memset (cron_wdays_, 0, sizeof(cron_wdays_));

  // iterate through all components
  while ((ps = cron_parse_entry (ps)) != nullptr && *ps != 0)
    ;

  if (cron_minutes_[tm->tm_min] && cron_hours_[tm->tm_hour]
      && cron_mdays_[tm->tm_mday] && cron_months_[tm->tm_mon + 1]
      && cron_wdays_[tm->tm_wday])
    result = true;

  return result;
}

/**
 * @brief Check for an end of cron string.
 * @param pc: pointer on the cron string.
 * @return true if the pointer reached the end of the cron string, false
 *      otherwise.
 */
int
cronparser::cron_eol (char* pc)
{
  char c;
  c = *pc;

  if (c == ';' || c == '\0')
    return true;
  else
    return false;
}

/**
 * @brief Parse a cron string and update the internal structures.
 * @param ps: pointer on the cron string.
 * @return Pointer on the next cron string or null pointer if at end.
 */
char*
cronparser::cron_parse_entry (char* ps)
{
  // process cron fields and store in static arrays
  ps = cron_parse_list (ps, cron_minutes_, 0, 59);
  ps = cron_parse_list (ps, cron_hours_, 0, 23);
  ps = cron_parse_list (ps, cron_mdays_, 1, 31);
  ps = cron_parse_list (ps, cron_months_, 1, 12);
  ps = cron_parse_list (ps, cron_wdays_, 0, 6); // 0-6 not 0-7 as in crontab

  if (ps == nullptr)
    return ps;

  // skip to end of line, may be delimiter or end of string
  while (!cron_eol (ps))
    ++ps;

  // was it delimiter?
  if (*ps == ';')
    ps++;       // skip it

  while (isspace (*(uint8_t *) ps))
    ps++;

  return ps;
}

/**
 * @brief Parse a cron string and update a list (mins, hours, days, ... etc.).
 * @param ps: pointer on the cron string.
 * @param pbuf: pointer on a list (mins, hours, ... etc.).
 * @param min: minimum value in the list.
 * @param max: maximum value in the list.
 * @return Pointer on the cron string to the next element to parse, or null
 *      if at end.
 */
char*
cronparser::cron_parse_list (char* ps, char* pbuf, unsigned char min,
                             unsigned char max)
{
  unsigned char i;

  if (ps == nullptr)
    return ps;

#if DEBUG_CRON_PARSER == true
  trace::printf("cron_parse_list() '%s' min=%d, max=%d\n", ps, min, max);
#endif

  while (isspace (*(uint8_t *) ps))
    ps++;       // skip over leading spaces

  if (!cron_eol (ps))
    {
      while (true)
        {
          ps = cron_parse_element (ps, pbuf, min, max);
          if (ps == nullptr)
            return ps;

          if (*ps != ',')
            break;
          ps++;
        };
      if (!isspace (*(uint8_t *) ps) && !cron_eol (ps))
        {
          ++ps; // jump over ERR !!!
        }
    }
  else
    {
      // if none defined, same as '*'
      for (i = min; i <= max; ++i)
        pbuf[i] = true;
#if DEBUG_CRON_PARSER == true
      trace::printf("cron_buf[%d-%d]=true\n", min, max);
#endif
    }
  return ps;
}

/**
 * @brief Parse an element of a cron string.
 * @param ps: pointer on the element to parse.
 * @param pbuf: pointer on a list (minutes, hours, ... etc.).
 * @param min: minimum value to be expected for this element.
 * @param max: maximum value to be expected for this element.
 * @return Pointer on the next element, or null pointer of at end of the cron
 *      string.
 */
char*
cronparser::cron_parse_element (char* ps, char* pbuf, unsigned char min,
                                unsigned char max)
{
  unsigned char i, from, to, step;

  if (ps == nullptr)
    return ps;

#if DEBUG_CRON_PARSER == true
  trace::printf("cron_parse_element() '%s'\n", ps);
#endif

  while (isspace (*(uint8_t *) ps))
    ++ps;

  if (*ps == '*')
    {
      ++ps;
      if (*ps == '/')
        {
          ++ps;
          ps = cron_parse_number (ps, &step);
        }
      else
        {
          step = 1;
        }
      for (i = min; i <= max; i += step)
        {
#if DEBUG_CRON_PARSER == true
          trace::printf("cron_buf[%d]=true\n", i);
#endif
          pbuf[i] = true;
        }
    }
  else if (isdigit (*(uint8_t *) ps))
    {
      ps = cron_parse_number (ps, &from);
      if (*ps != '-')
        {
          // single value, mark its position into buffer
          if (from > max)
            from = max;
#if DEBUG_CRON_PARSER == true
          trace::printf("cron_buf[%d]=true\n", from);
#endif
          pbuf[from] = true;
        }
      else
        {
          // range, parse upper limit
          ps++;
          ps = cron_parse_number (ps, &to);
          if (*ps == '/')
            {
              ++ps;
              ps = cron_parse_number (ps, &step);
            }
          else
            {
              step = 1;
            }

          if (from < min)
            from = min; // 'from' cannot be lower than min
          if (to > max)
            to = max;   //'to' cannot be higher than max
          if (from > max)
            from = max; // but 'from' cannot be higher than max too

          for (i = from; i <= to; i += step)
            {
#if DEBUG_CRON_PARSER == true
              trace::printf("cron_buf[%d]=true\n", i);
#endif
              pbuf[i] = true;
            }
        }
    }
  else
    {
#if DEBUG_CRON_PARSER == true
      trace::printf("unknown char '%c'\n", *ps);
#endif
      return nullptr;
    }

  return ps;
}

/**
 * @brief Simple ASCII to integer conversion (for small numbers only).
 * @param ps: pointer on the number to convert.
 * @param pn: pointer on the location where the result must be returned.
 * @return Pointer behind the converted number, if null then at the end of the
 *      string.
 */
char*
cronparser::cron_parse_number (char* ps, unsigned char* pn)
{
  unsigned char n;

  for (n = 0; isdigit (*(uint8_t *) ps); ++ps)
    {
      n *= 10;
      n += (*ps - '0'); // parse and convert to number
    }

#if DEBUG_CRON_PARSER == true
  trace::printf("cron_parse_number() returned %d\n", n);
#endif

  if (pn != nullptr)
    *pn = n;    // store result

  return ps;
}
