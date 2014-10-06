<?php

/**
 * @file
 * Theme hooks.
 */

/**
 * Implements hook_preprocess().
 */
function islandora_entities_preprocess_islandora_citation(array &$variables) {
  module_load_include('inc', 'islandora', 'includes/metadata');
  $object = $variables['object'];
  $object_pid = $object->id;
  if ($object['PREVIEW'] && $object['PDF']) {
    $variables['preview'] = "/islandora/object/$object_pid/datastream/PREVIEW/view";
    $variables['pdf'] = "/islandora/object/$object_pid/datastream/PDF/view";
  }
  $variables['metadata'] = islandora_retrieve_metadata_markup($object, TRUE);
}

/**
 * Implements hook_preprocess().
 */
function islandora_entities_preprocess_islandora_thesis(array &$variables) {
  module_load_include('inc', 'islandora', 'includes/metadata');
  $object = $variables['object'];
  $object_pid = $object->id;
  if ($object['PREVIEW'] && $object['PDF']) {
    $variables['preview'] = "/islandora/object/$object_pid/datastream/PREVIEW/view";
    $variables['pdf'] = "/islandora/object/$object_pid/datastream/PDF/view";
  }
  $variables['metadata'] = islandora_retrieve_metadata_markup($object, TRUE);
}
