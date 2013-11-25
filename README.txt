CONTENTS OF THIS FILE
---------------------

 * summary
 * requirements
 * installation
 * configuration
 * customization
 * troubleshooting
 * faq
 * contact
 * sponsors


SUMMARY
-------

Islandora Scholar

Based on the UPEI scholar module.

REQUIREMENTS
------------

The following Drupal modules are required:
 * islandora

INSTALLATION
------------


CONFIGURATION
-------------
Islandora 7 has re-defined how we are displaying citations to the user. As such,
existing citations should be updated using the provided Drush script. The
command creates PDF derivatives for any existing attached PDFs as the Google
PDF Viewer has been removed in favor of displaying just the PREVIEW datastream.
This can be done by running:
  drush -u 1 islandora-scholar-update-citations

CUSTOMIZATION
-------------


TROUBLESHOOTING
---------------


F.A.Q.
------


CONTACT
-------


SPONSORS
--------
