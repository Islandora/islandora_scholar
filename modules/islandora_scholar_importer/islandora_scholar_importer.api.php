<?php

/**
 * Hook to register importers.
 *
 * @return array
 *   An associative array mapping unique machine names to associative arrays
 *   containing the following keys:
 *   - type: The extension of the file.
 *   - module: The module to which the file belongs.
 *   - file: A file to load before attempting to import.
 *   - title: A translated title,
 *   - class: A string containing the class name for your importer.
 *   The first three (type, module and file) correspond to the parameters for
 *   module_load_include.  Title is used as the label, and the class is one
 *   which extends IslandoraScholarBatchImporter (defined in
 *   islandora_scholar_importer.inc; just implementing the abstract methods and
 *   using your custom "item class" should suffice)
 */
function hook_islandora_scholar_importer() {
  return array(
    'my_awesome_importer' => array(
      'type' => 'inc',
      'module' => 'my_awesome_module',
      'file' => 'importer',
      'title' => t('My Awesome Importer Module'),
      'class' => 'MAIM',
    ),
  );
}
