# cronparser

This is a simple implementation of a `cron` parser. It is intended mainly for embedded applications, to schedule various events. It was originally developed in C by Liviu Ionescu.
 
The content of the input string is a ';' separated list of CRON definitions, like: `L30-59/15,0-29/14 1`. The first letter defines whether local (L) or UTC (Z) time is used, the default being local time.
 
The other fields are:
* minute (0-59)
* hour (0-23)
* day of month (1-31)
* month (1-12)
* day of week (0-6, 0=Sunday)
 
Regular UNIX syntax is accepted, i.e. a comma separated list of ranges. Ranges accept step definitions while '*' is considered a full range.
 
Given a date (`time_t` format) and a cron string, the parser returns `true` if the two parameters match, or `false` otherwise.
 
## Version
* 2.0 (1 Oct. 2017)

## License
* MIT

## Package
The library is provided as an XPACK and can be installed in an Eclipse based project using the attached script (however, the `include` and `source` paths must be manually added to the project in Eclipse). For more details on XPACKs see https://github.com/xpacks. The installation script requires the helper scripts that can be found at https://github.com/xpacks/scripts.

## Dependencies
The library dependends very lightly on the __trace__ functionality found in the uOS++ package (https://github.com/micro-os-plus/micro-os-plus-iii), but this can be easily replaced.

## Tests
A separate directory `test` is included that contains a short test program which iterates a list of dates and cron strings to test for matches. You can freely add other dates/cron strings pairs in the table.

