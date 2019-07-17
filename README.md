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
Ouch! Using caution here can be useful. A small change can cause an issue that could easily go unnoticed for some time. The documentation for this can be extensive. Please use [MODS User Guidelines Version 3](https://www.loc.gov/standards/mods/userguide/index.html) for the basics. Islandora Scholar XPath Configuration is at /admin/islandora/solution_pack_config/scholar/xpaths

Islandora-Collaboration-Group: [Description and breakdown of XPaths and Keys](https://github.com/Islandora-Collaboration-Group/icg_csv_import/blob/master/README.md#xpaths-and-keys), [Additional information at Github/Islandora Scholar](https://github.com/Islandora/islandora_scholar/)

Additional XPath Configurations:
- Google Scholar Search XPaths: admin/islandora/solution_pack_config/scholar
- Library Catalog Search XPaths Configuration Page:  'admin/islandora/solution_pack_config/scholar#edit-library-catalog-search

![ISLANDORA SCHOLAR XPATH CONFIGURATION](https://user-images.githubusercontent.com/11573234/48782673-b3362400-ecac-11e8-869c-3928c43df253.PNG)

Clicking the "__Save Configuration__" will create drupal variable for each of these. Clicking "__Reset all XPaths to Defaults__" will delete the drupal global variables and fault back to the hard coded ones. Using the demo object will not modify the object, this is for identifying what MODS values are read with the specified XPath.

#### GENERAL XPATH CONFIGURATIONS
These XPaths are shared within most of the submodules. To see the MODS value after editing an entry, tab to the next field.

- __Title__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:titleInfo/mods:title
  * XPath to use for title.


- __Subtitle__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]//mods:subTitle
  * XPath for Coins submodule to construct the first entry in an array of subtitle entries.


- __Abstract__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:abstract
  * XPath to use for the abstract.


- __Family Name__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:name[mods:role/mods:roleTerm = "author"]/mods:namePart[@type = "family"]
  * XPath to use for the family in the bibutils submodule.
  * XML of the mods:name to the processed format:


- __Given Name__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:name[mods:role/mods:roleTerm = "author"]//mods:namePart[@type = 'given']
  * XPath to use for the given.


- __Authors__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:name/mods:role[mods:roleTerm = "author"]/../mods:namePart[@type="family"]
  * XPath to use for authors in the bibliography submodule for display.


- __Created Date__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:originInfo/mods:dateCreated
  * XPath to use for the created_date.

#### GOOGLE SCHOLAR SUBMODULE XPATH CONFIGURATIONS
These XPaths are shared within most of the submodules. To see the MODS value after editing an entry, tab to the next field.

- __Degree Grantor__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:name[@type="corporate"][mods:role/mods:roleTerm = "Degree grantor"]/mods:namePart
  * XPath to use for degree grantor.


- __Genre__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:genre
  * XPath to use for genre.


- __Host Title__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:relatedItem[@type="host"]//mods:title
  * XPath to use for the host_title.


- __ISSN__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:identifier[@type="issn"]
  * XPath to use for the issn.


- __Issue__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:part/mods:detail[@type="issue"]/mods:number
  * XPath to use for the issue title, name or number.


- __Online Date__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:recordInfo/mods:recordCreationDate
  * XPath to use for the online_date.

#### COINS SUBMODULE XPATH CONFIGURATIONS
These XPaths are shared within most of the submodules. To see the MODS value after editing an entry, tab to the next field.

- __Department__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:identifier[@type="u2"]
  * XPath to use for the department.


- __Related Date__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:relatedItem[@type="host"]//mods:date
  * XPath to use for the related_date.


- __Start Page__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]//mods:extent[@unit="page"]/mods:start
  * XPath to use for the start_page.


- __Embargo Date__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:dateOther[@type="embargo"]
  * XPath to use for the embargo_date.


- __End Page__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]//mods:extent[@unit="page"]/mods:end
  * XPath to use for the end_page.


- __Origin Date__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:originInfo/mods:dateIssued
  * XPath to use for the origin_date.


- __Part Date__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:part/mods:date
  * XPath to use for the part_date.


- __Volume__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:part/mods:detail[@type="volume"]/mods:number
  * XPath to use for volume number / title.

#### CITEPROC SUBMODULE XPATH CONFIGURATIONS
These XPaths are shared within most of the submodules. To see the MODS value after editing an entry, tab to the next field.

- __Subtitle used for Citeproc JSON__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: ../mods:subTitle
  * XPath to use for subtitle. Default is a relative path from the title (declared above as Title with a default value: //mods:mods[1]/mods:titleInfo/mods:title). If there is a value it will display as `title: subtitle`


- __Nonsort used for Citeproc JSON__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: ../mods:nonSort
  * XPath to use for title's nonsort values. The defaults are relative path from the title (declared above). If there is a value it will display as `non_sort title`


- __Call number__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:classification
  * XPath to use for the call number.


- __Series Title__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:relatedItem[@type="series"]/mods:titleInfo[not(@type)]/mods:title
  * XPath to use for series title.


- __Container title__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:relatedItem[@type="host"]/mods:titleInfo[not(@type)]/mods:title
  * XPath to use for the container search.


- __DOI__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:identifier[@type="doi"]
  * XPath to use for DOI.


- __Edition__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:originInfo/mods:edition
  * The XPath to use edition.


- __Event Title__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1][mods:genre[@authority="local"]="conferencePaper"]/mods:relatedItem/mods:titleInfo/mods:title
  * XPath to use for event type.


- __Event Place__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1][mods:genre[@authority="marcgt"][text()="conference publication"]]/mods:originInfo/mods:place/mods:placeTerm
  * XPath to use for event place property. Gets the event-place property for the Citation.


- __Event__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1][mods:genre[@authority="local"][text()="conferencePaper"]]/mods:originInfo/mods:place/mods:placeTerm
  * XPath to use for event place. Gets the event-place property for the Citation. If the value of `Event Place` is empty Citeproc will default to this.


- __ISBN__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:identifier[@type="isbn"]
  * XPath to use for ISBN number.


- __Note__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:note
  * XPath to use for notes.


- __Number__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:relatedItem[@type="series"]/mods:titleInfo/mods:partNumber
  * XPath to use for series part number.


- __Publisher__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:originInfo/mods:publisher
  * XPath to use for Publisher.


- __Publisher place__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:originInfo/mods:place/mods:placeTerm
  * XPath to use for publisher-place.


- __URL__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:location/mods:url
  * XPath to use for URL.


- __Accession Number__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:identifier[@type="accession"]
  * XPath to use for the accession number.


- __PMCID__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:identifier[@type="pmc"]
  * XPath to use for the PubMed Central reference number (PMCID).


- __NIH Manuscript Submission Reference Number__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:identifier[@type="mid"]
  * XPath to use for NIH Manuscript Submission Reference Number (NIHMSID).


- __Property__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1][mods:genre[@authority="marcgt"][text()="conference publication"]]/mods:relatedItem/mods:titleInfo/mods:title
  * XPath to use for property.


- __Pages__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:part/mods:extent[@unit="pages"]
  * XPath to use for pages.


- __Endnote authority genre types__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:genre[@authority="endnote"]
  * XPath to use for types like "Journal Article, ".


- __MARC authority genre types__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:genre[@authority="marcgt"]
  * XPath to use for type_marcgt.


- __→ MARC genre type (relative child): chapter titles in a book__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:relatedItem[@type='host']/mods:titleInfo/mods:title
  * Relative child XPath to use for type_marcgt titles (Book uses this for title or defaults back to the book title).


- __MARC genre type related__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:relatedItem/mods:genre[@authority="marcgt"]
  * XPath to use for type_marcgt_related.


- __types local auth__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:genre[not(@authority="marcgt" or @authority="endnote")]
  * XPath to use for types_local_auth.


- __Season__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:originInfo/mods:dateOther[@type="season"]
  * XPath to use for season.


- __Date Issued__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:originInfo/mods:dateIssued | //mods:mods[1]/mods:relatedItem[@type="host"]/mods:part/mods:date
  * XPath to use for date_issued.


- __Date Captured__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:originInfo/mods:dateCaptured
  * XPath to use for the date_captured.


- __Names__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:name[normalize-space(mods:namePart)]
  * XPath to use for the names.


- __Container Author__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:relatedItem[@type="host"]/mods:name
  * XPath to use for the container-author.


- __Series Editor__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]/mods:relatedItem[@type="series"]/mods:name
  * XPath to use for the series editor.


- __Role Term__
  * Modify to refresh value (PID for demo object is required).
  * Default Value: //mods:mods[1]//mods:role/mods:roleTerm
  * XPath to use for the role_term.

## Upgrade notices

### 7.x-1.3

As of Islandora 7.x-1.3, PDF datastreams of citationCModel objects are expected to have derivatives (PREVIEW, TN, optionally FULL_TEXT). Existing citations that are missing PDF-derived datastreams can be updated using the provided Drush script, which will generate the derivatives.

This can be done by running:
`drush -u 1 islandora-scholar-update-citations`

### 7.x-1.14

The TCPDF library was previously directly included inside the `citation_exporter` module; it has now been removed so that it can be managed properly via the Drupal Libraries module. If the `citation_exporter` submodule is installed when updating to Islandora 7.x-1.14, it is imperative that the TCPDF library is installed in the site libraries folder for citation exports to continue to properly function.

Check the `README.md` for the `citation_exporter` submodule for details on downloading and installing the TCPDF library.

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

* [Don Richards](https://github.com/DonRichards)

## Development

If you would like to contribute to this module, please check out [CONTRIBUTING.md](CONTRIBUTING.md). In addition, we have helpful [Documentation for Developers](https://github.com/Islandora/islandora/wiki#wiki-documentation-for-developers) info, as well as our [Developers](http://islandora.ca/developers) section on the [Islandora.ca](http://islandora.ca) site.

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
