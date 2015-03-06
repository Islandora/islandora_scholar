## Introduction

The bibliography module supports bibliography functions for the Islandora Scholar Module. The module allows users to maintain/export their own bibliography of citations.

## Requirements

This module requires the following modules/libraries:

* [Islandora](https://github.com/islandora/islandora)
* [Tuque](https://github.com/islandora/tuque)
* [csl]((https://github.com/citation-style-language)
* [citeproc](https://github.com/Islandora/citeproc-php)
* [islandora_bookmark](https://github.com/Islandora/islandora_bookmark)


## Installation

Install as usual, see [this](https://drupal.org/documentation/install/modules-themes/modules-7) for further information.

## Configuration

Islandora 7 has re-defined how we are displaying citations to the user. As such, existing citations should be updated using the provided Drush script. The command creates PDF derivatives for any existing attached PDFs as the Google PDF Viewer has been removed in favor of displaying just the PREVIEW datastream.

This can be done by running: 
`drush -u 1 islandora-scholar-update-citations`

## Troubleshooting/Issues

Having problems or solved a problem? Check out the Islandora google groups for a solution.

* [Islandora Group](https://groups.google.com/forum/?hl=en&fromgroups#!forum/islandora)
* [Islandora Dev Group](https://groups.google.com/forum/?hl=en&fromgroups#!forum/islandora-dev)

## Maintainers/Sponsors

Current maintainers:

* [Nick Ruest](https://github.com/nruest)

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
