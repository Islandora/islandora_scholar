# Islandora Scholar [![Build Status](https://travis-ci.org/Islandora/islandora_scholar.png?branch=7.x)](https://travis-ci.org/Islandora/islandora_scholar)

## Introduction

Islandora Scholar is a module designed to help Islandora function as an Institutional Repository. It provides a framework for handling Citation and Thesis objects, and a variety of additional features for working with citations.

### It will soon include:
* Citation Content Model and Thesis Content Model (and associated default forms)
* Display formatted citations based on Drupal fields and RDF data model based.
* (optional) links on Citation and Thesis objects to search for them in Google Scholar or your local discovery layer
* Ability to embargo objects or datastreams permanently or with a time limit (see Scholar Embargo submodule)
* Optimized Google Scholar microdata in meta tags


## Requirements

This module requires Drupal 8|9 and the following modules/libraries:

* ...


## Installation

...


## Configuration

...


## Documentation

Further documentation for this module is available at [our wiki](https://islandora.github.io/documentation).


## Complementary Modules

* Embargo Modules
  * [IP Embargo](https://github.com/mjordan/ip_range_access): will restrict viewing of an object to users within a defined set of IP address ranges.
  * [Embargo](https://github.com/discoverygarden/embargoes): Embargo by date on a node.
* Mediated Submission Portal: this module is intended to extend Islandora's submission and approval process. Example [module](https://www.drupal.org/docs/8/core/modules/workflows/overview). A fork still needs to be done.
* Migrating from 7.x or Bepress via [Islandora Workbench](https://github.com/mjordan/islandora_workbench)

## Troubleshooting/Issues

* ...


## Maintainers/Sponsors

Current maintainers:

* [Don Richards](https://github.com/DonRichards)


## Development

If you would like to contribute to this module, please check out [CONTRIBUTING.md](CONTRIBUTING.md). In addition, we have helpful [Documentation for Developers](https://github.com/Islandora/islandora/wiki#wiki-documentation-for-developers) info, as well as our [Developers](http://islandora.ca/developers) section on the [Islandora.ca](http://islandora.ca) site.

### Important considerations for Module development.

* Is a single module (can "require" other modules but no embedded submodules)
* When creating anything that may be impacted by switching themes, the module must determine the current default theme and associated changes with that theme and/or include a function that can be ran to add to the current default them.
* Avoid relying on importing or merging template YML files.
* Please review [Drupal's Coding Standards](https://www.drupal.org/docs/develop/standards/coding-standards) if (at all) possible.
* Object/Node level view example can be found [here](https://miro.com/app/board/uXjVOd9c3Bw=/?fromRedirect=1)

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
