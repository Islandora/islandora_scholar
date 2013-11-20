<?php

/**
 * @file
 * Drush command/hook implementation for updating existing citation objects.
 */

/**
 * Implements hook_drush_command().
 */
function islandora_scholar_drush_command() {
  $commands = array();

  $commands['islandora-scholar-update-citations'] = array(
    'description' => dt('Update existing citations to generate PDF derivatives for facilitating new theme changes. Any existing PDF derivatives on citation objects will not be overwritten. As such, subsquent runs of this script will not overwrite existing content.'),
    'drupal dependencies' => array(
      'islandora',
      'islandora_scholar',
      'imagemagick',
    ),
    'options' => array(
      'force' => array(
        'description' => 'Whether we are forcing the creation of derivatives or not.',
      ),
    ),
    'examples' => array(
      'drush -u 1 islandora-scholar-update-citations' => dt('Updating existing citations with PDF derivatives.'),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  return $commands;
}

/**
 * Command callback to install required objects.
 */
function drush_islandora_scholar_update_citations() {
  module_load_include('inc', 'islandora', 'includes/derivatives');
  $query = <<<EOQ
SELECT ?pid FROM <#ri>
WHERE {
  ?pid <fedora-model:hasModel> <info:fedora/ir:citationCModel> ;
       <fedora-view:disseminates> ?ds .
  ?ds <fedora-view:disseminationType> <info:fedora/*/PDF> .
}
EOQ;
  $connection = islandora_get_tuque_connection();
  $results = $connection->repository->ri->sparqlQuery($query, 'unlimited');
  drush_log(dt('Processing @num results...', array('@num' => count($results))), 'notice');
  foreach ($results as $result) {
    $object = islandora_object_load($result['pid']['value']);
    $derivative_results = islandora_do_derivatives($object, array(
      'force' => drush_get_option('force', FALSE),
      'source_dsid' => 'PDF',
    ));
    $success = TRUE;
    foreach ($derivative_results as $log) {
      if (!$log['success']) {
        $success = FALSE;
        break;
      }
    }
    if ($success) {
      drush_log(dt("PDF derivative creation succeeded for @pid.", array('@pid' => $object->id)), 'success');
    }
    else {
      drush_log(dt("PDF derivative creation failed for @pid. Check the Drupal watchdog for detailed errors.", array('@pid' => $object->id)), 'error');
    }
  }
}
