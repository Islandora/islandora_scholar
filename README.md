# Islandora Scholar [![Build Status](https://travis-ci.org/Islandora/islandora_scholar.png?branch=7.x)](https://travis-ci.org/Islandora/islandora_scholar)

## Introduction

Islandora Scholar is a suite of modules designed to help Islandora function as an Institutional Repository. It provides a framework for handling Citation and Thesis objects, and a variety of additional features for working with citations. It includes:
* Citation Content Model and Thesis Content Model (and associated default forms)
* Display formatted citations based on MODS data based on user-uploaded CSL files
* (optional) tab on Citation objects containing the self-archiving policies of the associated periodical, based on Sherpa/RoMEO
* (optional) links on Citation and Thesis objects to search for them in Google Scholar or your local discovery layer
* Importers and populators to create objects based on DOIs, PMIDs, Endnote XML, or RIS files (see associated submodules)
* Ability to embargo objects or datastreams permanently or with a time limit (see Scholar Embargo submodule)
* Google Scholar-ready microdata in meta tags (see Islandora Google Scholar submodule)


## Requirements

This module requires the following modules/libraries:

* [Islandora](https://github.com/islandora/islandora)
* [Tuque](https://github.com/islandora/tuque)
* [Islandora Solr](https://github.com/Islandora/islandora_solr_search)
* [Bibutils](https://github.com/Islandora/islandora_scholar/tree/7.x/modules/bibutils) (included in /modules)
* [Citeproc](https://github.com/Islandora/islandora_scholar/tree/7.x/modules/citeproc) (included in /modules)
* [CSL](https://github.com/Islandora/islandora_scholar/tree/7.x/modules/csl) (included in /modules)


The Citeproc module depends on the [citeproc-php](https://github.com/Islandora/citeproc-php) library, which should be installed in the `sites/all/libraries` directory, such that the main `CiteProc.php` file is located at `sites/all/libraries/citeproc-php/CiteProc.php`. More information is available in [Citeproc's README.md file](https://github.com/Islandora/islandora_scholar/blob/7.x/modules/citeproc/README.md).

The Bibutils module depends on Bibutils, a command-line tool which must be installed on the server. More information is available in [Bibutils' README.md file](https://github.com/Islandora/islandora_scholar/tree/7.x/modules/bibutils).


## Installation

Install as usual, see [this](https://drupal.org/documentation/install/modules-themes/modules-7) for further information. This module requires new Fedora objects. If enabling through Drush, use the administrative user (e.g. `drush en -u 1 islandora_scholar`).

## Configuration

Configure Islandora Scholar at __Administration » Islandora » Solution pack configuation » Scholar__ (`admin/islandora/solution_pack_config/scholar`).
Further documentation of the available options is available at [our wiki](https://wiki.duraspace.org/display/ISLANDORA/Islandora+Scholar).

## Upgrade notice
As of Islandora 7.x-1.3, PDF datastreams of citationCModel objects are expected to have derivatives (PREVIEW, TN, optionally FULL_TEXT). Existing citations that are missing PDF-derived datastreams can be updated using the provided Drush script, which will generate the derivatives.

This can be done by running:
`drush -u 1 islandora-scholar-update-citations`

## Documentation

Further documentation for this module is available at [our wiki](https://wiki.duraspace.org/display/ISLANDORA/Islandora+Scholar).

## Complementary Modules

Aside from the submodules bundled with Scholar, several other modules can be useful for creating an institutional repository:

* [Entities Solution Pack](https://github.com/Islandora/islandora_solution_pack_entities) - create and manage objects representing people (scholars) and organizations (departments)
* [Islandora Badges](https://github.com/Islandora/islandora_badges) - display citation counts for objects, pulling from a variety of citation-tracking APIs
* [Islandora Usage Stats](https://github.com/Islandora/islandora_usage_stats) - keeps track of views/downloads of Islandora objects.


## Troubleshooting/Issues

Having problems or solved a problem? Check out the Islandora google groups for a solution.

* [Islandora Group](https://groups.google.com/forum/?hl=en&fromgroups#!forum/islandora)
* [Islandora Dev Group](https://groups.google.com/forum/?hl=en&fromgroups#!forum/islandora-dev)

## Maintainers/Sponsors

Current maintainers:

* [Bryan Brown](https://github.com/bryjbrown)

## Development

If you would like to contribute to this module, please check out [CONTRIBUTING.md](CONTRIBUTING.md). In addition, we have helpful [Documentation for Developers](https://github.com/Islandora/islandora/wiki#wiki-documentation-for-developers) info, as well as our [Developers](http://islandora.ca/developers) section on the [Islandora.ca](http://islandora.ca) site.

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
