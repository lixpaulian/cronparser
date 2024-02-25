# cronparser

This is a simple implementation of a `cron` parser. It is intended mainly for embedded applications, to schedule various events. It was originally developed in C by Liviu Ionescu.
 
The content of the input string is a ';' separated list of CRON definitions, like: `L30-59/15,0-29/14 1`. The first letter defines whether local (L) or UTC (Z) time is used, the default being local time.
 
The other fields are:
* minute (0-59)
* hour (0-23)
* day of month (1-31)
* month (1-12)
* day of week (0-7, 0 or 7 is Sunday)
 
Regular UNIX syntax is accepted, i.e. a comma separated list of ranges. Ranges accept step definitions while '*' is considered a full range.
 
Given a date (`time_t` format) and a cron string, the parser returns `true` if the two parameters match, or `false` otherwise.
 
## Version
* 2.2.0 (19 July 2020)

## License
* MIT

## Package
The class is provided as an xPack (for more details on xPacks see https://xpack.github.io). It can be installed in an Eclipse based project using either `xpm` or the attached script (however, the include and source paths must be manually added to the project in Eclipse). Of course, it can be installed without using the xPacks tools, either from the repository or manually, but then updating it later might be more difficult.

Note thathe xPacks project evolved with the time. Initially it was based on shell scripts, but the current version is based on a set of utilities, in partcular `xpm` plus a json description file. This project supports both versions, therefore you will still find the `xpacks-helper.sh` script in the `scripts` subdirectory, as well as the `package.json` file for `xpm`. However, the script based version is deprecated and will not be supported in the future.

## Dependencies
The library dependends very lightly on the __trace__ functionality found in the µOS++ package (https://github.com/micro-os-plus/micro-os-plus-iii), but this can be easily replaced.

## Tests
A separate directory `test` is included that contains a short test program which iterates a list of dates and cron strings to test for matches. You can freely add other dates/cron string pairs in the table.

