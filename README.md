[![GitHub package.json version](https://img.shields.io/github/package-json/v/lixpaulian/cronparser)](https://github.com/lixpaulian/cronparser/blob/xpack/package.json)

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
The class is provided as an **xPack** (for more details on xPacks see https://xpack.github.io). It can be installed in a project using either `xpm` or the attached script. Of course, it can be installed without using the xPacks tools, either by linking the class as a Git submodule or by copying it in your project, but then updating it later might be more difficult.

Note that the xPacks project evolved with the time. Initially it was based on shell scripts, but the current version is based on a set of utilities, `xpm` and a JSON description file. You will still find the `xpacks-helper.sh` script in the `scripts` subdirectory, but it is not recommened as it is deprecated and will not be supported in the future. Instead use the procedure described below.

To install the package using `xpm` you must make sure that you have already `nodejs` and `xpm` installed on your computer (see also [xPack install](https://xpack.github.io/install/)). Then, in your project directory issue the command

```sh
cd my-project
xpm init # Add a package.json if not already present
xpm install github:lixpaulian/cronparser#v2.2.0 --save-dev --copy
```

Note: Without `--copy`, the default is to create a link to a read-only instance of the package in the `xpm` central store.

## Dependencies
The library dependends very lightly on the __trace__ functionality found in the µOS++ package (https://github.com/micro-os-plus/micro-os-plus-iii), but this can be easily replaced.

## Tests
A separate directory `test` is included that contains a short test program which iterates a list of dates and cron strings to test for matches. You can freely add other dates/cron string pairs in the table.

