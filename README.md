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

### Manually Setting XPaths
Ouch! Using caution here can be useful. A small change can cause an issue that could easily go unnoticed for some time. The documentation for this can be extensive. Please use [MODS User Guidelines Version 3](https://www.loc.gov/standards/mods/userguide/index.html) for the basics. Islandora Scholar XPath Configuration is at /admin/islandora/solution_pack_config/scholar

![ISLANDORA SCHOLAR XPATH CONFIGURATION](https://user-images.githubusercontent.com/2738244/41784378-8481d2c8-760d-11e8-935e-527b6c5c02c2.png)

Clicking the "__Save Configuration__" will create drupal variable for each of these. Clicking "__Reset all XPaths to Defaults__" will delete the drupal global variables and fault back to the hard coded ones. 
- __Title__
  * Default Value: //mods:mods[1]/mods:titleInfo/mods:title
  * Context:
- __Abstract__
  * Default Value: //mods:mods[1]/mods:abstract
  * Context:
- __Call number__
  * Default Value: //mods:mods[1]/mods:classification
  * Context:
- __Collection title__
  * Default Value: //mods:mods[1]/mods:relatedItem[@type="series"]/mods:titleInfo[not(@type)]/mods:title
  * Context:
- __Container title__
  * Default Value: //mods:mods[1]/mods:relatedItem[@type="host"]/mods:titleInfo[not(@type)]/mods:title
  * Context:
- __DOI__
  * Default Value: //mods:mods[1]/mods:identifier[@type="doi"]
  * Context:
- __Edition__
  * Default Value: //mods:mods[1]/mods:originInfo/mods:edition
  * Context:
- __Event__
  * Default Value: //mods:mods[1][mods:genre[@authority="local"]="conferencePaper"]/mods:relatedItem/mods:titleInfo/mods:title
  * Context:
- __Genre__
  * Default Value: //mods:mods[1]/mods:genre
  * Context:
- __ISBN__
  * Default Value: //mods:mods[1]/mods:identifier[@type="isbn"]
  * Context:
- __Volume__
  * Default Value: //mods:mods[1]/mods:part/mods:detail[@type="volume"]/mods:number
  * Context:
- __Issue__
  * Default Value: //mods:mods[1]/mods:part/mods:detail[@type="issue"]/mods:number
  * Context:
- __Note__
  * Default Value: //mods:mods[1]/mods:note
  * Context:
- __Number__
  * Default Value: //mods:mods[1]/mods:relatedItem[@type="series"]/mods:titleInfo/mods:partNumber
  * Context:
- __Publisher__
  * Default Value: //mods:mods[1]/mods:originInfo/mods:publisher
  * Context:
- __Publisher place__
  * Default Value: //mods:mods[1]/mods:originInfo/mods:place/mods:placeTerm
  * Context:
- __URL__
  * Default Value: //mods:mods[1]/mods:location/mods:url
  * Context:
- __PubMed ID__
  * Default Value: //mods:mods[1]/mods:identifier[@type="accession"]
  * Context:
- __PMCID__
  * Default Value: //mods:mods[1]/mods:identifier[@type="pmc"]
  * Context:
- __NIH Manuscript Submission Reference Number__
  * Default Value: //mods:mods[1]/mods:identifier[@type="mid"]
  * Context:
- __Authors__
  * Default Value: //mods:mods[1]/mods:name/mods:role[mods:roleTerm = "author"]/../mods:namePart[@type="family"]
  * Context:
- __Property__
  * Default Value: //mods:mods[1][mods:genre[@authority="marcgt"][text()="conference publication"]]/mods:relatedItem/mods:titleInfo/mods:title 
  * Context:
- __Pages__
  * Default Value: //mods:mods[1]/mods:part/mods:extent[@unit="pages"]
  * Context:
- __Types__
  * Default Value: //mods:mods[1]/mods:genre[@authority="endnote"]
  * Context:
- __MARC genre type__
  * Default Value: //mods:mods[1]/mods:genre[@authority="marcgt"]
  * Context:
- __MARC genre type related__
  * Default Value: //mods:mods[1]/mods:relatedItem/mods:genre[@authority="marcgt"]
  * Context:
- __types local auth__
  * Default Value: //mods:mods[1]/mods:genre[not(@authority="marcgt" or @authority="endnote")]
  * Context:
- __Season__
  * Default Value: //mods:originInfo/mods:dateOther[@type="season"]
  * Context:
- __Date Captured__
  * Default Value: //mods:originInfo/mods:dateCaptured
  * Context:
- __Date Issued__
  * Default Value: //mods:originInfo/mods:dateIssued | //mods:mods[1]/mods:relatedItem[@type="host"]/mods:part/mods:date
  * Context:
- __Title Results__
  * Default Value: //mods:mods[1]/mods:titleInfo/mods:title
  * Context:
- __Degree Grantor__
  * Default Value: //mods:mods/mods:name[@type="corporate"][mods:role/mods:roleTerm = "Degree grantor"]/mods:namePart
  * Context:
- __Date Captured__
  * Default Value: //mods:originInfo/mods:dateCaptured
  * Context:
- __Date Issued__
  * Default Value: //mods:originInfo/mods:dateIssued | //mods:mods[1]/mods:relatedItem[@type='host']/mods:part/mods:date
  * Context:
- __Names__
  * Default Value: //mods:mods[1]/mods:name[normalize-space(mods:namePart)]
  * Context:
- __Container Author__
  * Default Value: //mods:mods[1]/mods:relatedItem[@type="host"]/mods:name
  * Context:
- __Collection Editor__
  * Default Value: //mods:mods[1]/mods:relatedItem[@type="series"]/mods:name
  * Context:
- __Origin Date__
  * Default Value: //mods:originInfo/mods:dateIssued
  * Context:
- __Part Date__
  * Default Value: //mods:part/mods:date
  * Context:
- __Related Date__
  * Default Value: //mods:relatedItem[@type="host"]//mods:date
  * Context:
- __Created Date__
  * Default Value: //mods:originInfo/mods:dateCreated
  * Context:
- __Host Title__
  * Default Value: //mods:relatedItem[@type="host"]//mods:title
  * Context:
- __ISSN__
  * Default Value: //mods:identifier[@type="issn"]
  * Context:
- __Start Page__
  * Default Value: //mods:extent[@unit="page"]/mods:start
  * Context:
- __End Page__
  * Default Value: //mods:extent[@unit="page"]/mods:end
  * Context:
- __Online Date__
  * Default Value: //mods:recordInfo/mods:recordCreationDate
  * Context:
- __Sub Title__
  * Default Value: //mods:subTitle
  * Context:
- __Embargo Date__
  * Default Value: //mods:dateOther[@type="embargo"]
  * Context:
- __Department__
  * Default Value: //mods:identifier[@type="u2"]
  * Context:
- __Given Name__
  * Default Value: mods:namePart[@type = 'given']
  * Note how this doesn't start with a double slash '//'
  * Context:
- __Family Name__
  * Default Value: mods:namePart[@type = 'family']
  * Note how this doesn't start with a double slash '//'
  * Context:
- __Role Term__
  * Default Value: mods:role/mods:roleTerm
  * Note how this doesn't start with a double slash '//'
  * Context:

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
