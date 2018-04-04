
# Islandora DOI

## Introduction

Support code for Digital Object Identifiers, to create citation objects.

## Requirements

This module requires the following modules/libraries:

* [Islandora](https://github.com/islandora/islandora)
* [Islandora Scholar](https://github.com/islandora/islandora_scholar)

## Installation

Install as usual, see [this](https://drupal.org/documentation/install/modules-themes/modules-7) for further information.

## Configuration

By default, no configuration is required.

If you wish to use the legacy CrossREF OpenURL endpoint, you must enable it and enter your "OpenURL PID" at `admin/islandora/solution_pack_config/scholar/islandora_doi`.

It is possible to change the URL against which content negotiation requests are made by changing the "DOI URL supporting content negotiation" URL at `admin/islandora/solution_pack_config/scholar/islandora_doi`.

## Maintainers/Sponsors

Current maintainers:

* [Bryan Brown](https://github.com/bryjbrown)

## Development

If you would like to contribute to this module, please check out our helpful [Documentation for Developers](https://github.com/Islandora/islandora/wiki#wiki-documentation-for-developers) info, as well as our [Developers](http://islandora.ca/developers) section on the Islandora.ca site.

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
