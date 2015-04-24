<?php
/**
 * @file
 * Batch functionality for the Islandora Scholar Embargo module.
 */

/**
 * Batch callback function which updates the embargo for a target pid.
 *
 * @param string $pid
 *   The PID of the object we are going to update.
 * @param mixed $dsid
 *   Either NULL--meaning the object should be embargoed--or an array
 *   representing a group of datastream IDs to be embargoed.
 * @param string $end
 *   Either an xsd:dateTime compliant string for when to lift the embargo, or
 *   the string 'indefinite' to require it to be manually lifted.
 * @param array $query_array
 *   An associative array where the key is the unique ID and contains:
 *   -type: The type of query, either sparql or itql.
 *   -query: The defined query string.
 *   -description: The human-readable description of the query.
 * @param array $context
 *   Context array used in the batch.
 */
function islandora_scholar_embargo_apply_batch_function($pid, $dsid, $end, $query_array, &$context) {
  module_load_include('inc', 'islandora_xacml_editor', 'includes/batch');
  if (empty($context['sandbox'])) {
    $query = new IslandoraXacmlEditorQuery($pid, $query_array);
    $context['sandbox'] = array();
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['pids'] = $query->getPids();
    $context['sandbox']['items'] = count($context['sandbox']['pids']);
    $context['results']['success_count'] = 0;
    $context['results']['skip_count'] = 0;
  }
  $targetpid = array_pop($context['sandbox']['pids']);
  $context['sandbox']['progress']++;
  $allowed_cmodels = variable_get('islandora_embargo_content_models', array('ir:citationCModel', 'ir:thesisCModel'));
  $object = islandora_object_load($targetpid);
  if (count(array_intersect($allowed_cmodels, $object->models)) > 0) {
    islandora_scholar_embargo_set_embargo($targetpid, $dsid, $end);
    $context['results']['success_count'] = $context['results']['success_count'] + 1;
  }
  else {
    $context['results']['skip_count'] = $context['results']['skip_count'] + 1;
  }
  // Make sure we don't divide by zero.
  $context['finished'] = $context['sandbox']['items'] == 0 ? 1 : $context['sandbox']['progress'] / $context['sandbox']['items'];
}

/**
 * Finished function for the update embargo batch.
 *
 * @param bool $success
 *   Whether the batch was successful or not.
 * @param array $results
 *   An array containing the results of the batch operations.
 * @param array $operations
 *   The operations array that was used in the batch.
 */
function islandora_scholar_embargo_apply_batch_finished($success, $results, $operations) {
  drupal_set_message(format_plural($results['success_count'], 'One embargo updated.', '@count embargoes updated.'));
  if ($results['skip_count'] > 0) {
    drupal_set_message(format_plural($results['skip_count'], '@count object skipped as the content model is not configured to allow embargoes.', '@count objects skipped as their content models are not configured to allow embargoes.'));
  }
}

/**
 * Batch callback function which lifts the embargo for a target pid.
 *
 * @param string $pid
 *   The PID of the object we are going to update.
 * @param mixed $ds
 *   NULL--meaning the object should be disembargoed
 *   An array representing a list of datastream IDs to be disembargoed.
 * @param array $query_array
 *   An associative array where the key is the unique ID and contains:
 *   -type: The type of query, either sparql or itql.
 *   -query: The defined query string.
 *   -description: The human-readable description of the query.
 * @param array $context
 *   Context array used in the batch.
 */
function islandora_scholar_embargo_lift_batch_function($pid, $ds, $query_array, $context) {
  module_load_include('inc', 'islandora_xacml_editor', 'includes/batch');
  if (empty($context['sandbox'])) {
    $query = new IslandoraXacmlEditorQuery($pid, $query_array);
    $context['sandbox'] = array();
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['pids'] = $query->getPids();
    $context['sandbox']['items'] = count($context['sandbox']['pids']);
    $context['results']['redirect'] = $pid;
    $context['results']['success_count'] = 0;
    $context['results']['skip_count'] = 0;
  }
  $targetpid = array_pop($context['sandbox']['pids']);
  $context['sandbox']['progress']++;
  $allowed_cmodels = variable_get('islandora_embargo_content_models', array('ir:citationCModel', 'ir:thesisCModel'));
  $object = islandora_object_load($targetpid);
  if (count(array_intersect($allowed_cmodels, $object->models)) > 0) {
    islandora_scholar_embargo_lift_embargo($targetpid, $ds);
    $context['results']['success_count'] = $context['results']['success_count'] + 1;
  }
  else {
    $context['results']['skip_count'] = $context['results']['skip_count'] + 1;
  }
  // Make sure we don't divide by zero.
  $context['finished'] = $context['sandbox']['items'] == 0 ? 1 : $context['sandbox']['progress'] / $context['sandbox']['items'];
}

/**
 * Finished function for the lift embargo batch.
 *
 * @param bool $success
 *   Whether the batch was successful or not.
 * @param array $results
 *   An array containing the results of the batch operations.
 * @param array $operations
 *   The operations array that was used in the batch.
 */
function islandora_scholar_embargo_lift_batch_finished($success, $results, $operations) {
  drupal_set_message(format_plural($results['success_count'], 'One embargo lifted.', '@count embargoes lifted.'));
  if ($results['skip_count'] > 0) {
    drupal_set_message(format_plural($results['skip_count'], '@count object skipped as the content model is not configured to allow embargoes.', '@count objects skipped as their content models are not configured to allow embargoes.'));
  }
}
