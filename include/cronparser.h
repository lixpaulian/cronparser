/*
 * cronparser.h
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
 * Created on: 9 Oct 2017 (LNP)
 */

#ifndef CRONPARSER_H_
#define CRONPARSER_H_

#if defined (__cplusplus)

class cronparser
{

  // --------------------------------------------------------------------

public:
  cronparser (void);

  ~cronparser () = default;

  bool
  cron_check (time_t time_in, char* ps);

  // --------------------------------------------------------------------

private:

  int
  cron_eol (char* pc);

  char*
  cron_parse_entry (char* ps);

  char*
  cron_parse_list (char* ps, char* pbuf, uint8_t min, uint8_t max);

  char*
  cron_parse_element (char* ps, char* pbuf, uint8_t min, uint8_t max);

  char*
  cron_parse_number (char* ps, uint8_t* pn);

  char cron_minutes_[60];       // 0 - 59
  char cron_hours_[24];         // 0 - 23
  char cron_mdays_[31 + 1];     // 1 - 31
  char cron_months_[12 + 1];    // 1 - 12
  char cron_wdays_[7];          // must be 0-6, not 0-7 as in crontab

};

#endif /* (__cplusplus) */

#endif /* CRONPARSER_H_ */
