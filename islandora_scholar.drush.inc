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
  $commands['islandora-scholar-fix-doi-html-in-mods'] = array(
    'description' => dt('Update existing citations with bad MODS caused by DOI importer failing to deal with embedded markup.'),
    'drupal dependencies' => array(
      'islandora',
      'islandora_scholar',
    ),
    'options' => array(
      'action' => array(
        'description' => 'Whether to strip (remove tags) or encode (into &lt; and &gt;) HTML entities. Can be "strip" or "encode".',
      ),
      'audit' => array(
        'description' => 'List the affected PIDs, but do not perform any modifications.',
      ),
      'dry-run' => array(
        'description' => 'List the affected PIDs and the modifications that would result.',
      ),
      'modify-my-data' => array(
        'description' => 'Update the affected datastreams.',
      ),
    ),
    'examples' => array(
      'drush -u 1 islandora-scholar-fix-doi-html-in-mods --audit' => dt('List objects found to have unruly HTML in MODS'),
      'drush -u 1 islandora-scholar-fix-doi-html-in-mods --action=strip --dry-run' => dt('Show what changes would result from stripping HTML from MODS, but do not modify data.'),
      'drush -u 1 islandora-scholar-fix-doi-html-in-mods --action=encode --modify-my-data' => dt('Encode HTML within MODS, modifying the datastreams in place and printing changes to drush output.'),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  return $commands;
}

/**
 * Command callback to update citations with PDF derivatives.
 */
function drush_islandora_scholar_update_citations() {
  batch_set(islandora_scholar_citation_update_create_batch());
  drush_backend_batch_process();
}

/**
 * Constructs a batch used to update things via Drush.
 */
function islandora_scholar_citation_update_create_batch() {
  return array(
    'operations' => array(
      array('islandora_scholar_update_citation_batch_operation', array()),
    ),
    'title' => t('Updating PDF derivatives for citations...'),
    'init_message' => t('Preparing to update derivatives.'),
    'progress_message' => t('Time elapsed: @elapsed <br/>Estimated time remaining @estimate.'),
    'error_message' => t('An error has occurred.'),
    'file' => drupal_get_path('module', 'islandora_scholar') . '/islandora_scholar.drush.inc',
  );
}

/**
 * Constructs and performs the citation batch operation.
 *
 * @param array $context
 *   The context of the Drupal batch.
 */
function islandora_scholar_update_citation_batch_operation(&$context) {
  $citation_update = 10;
  $query = <<<EOQ
SELECT ?pid FROM <#ri>
WHERE {
  ?pid <fedora-model:hasModel> <info:fedora/ir:citationCModel> ;
       <fedora-view:disseminates> ?ds .
  ?ds <fedora-view:disseminationType> <info:fedora/*/PDF> .
}
EOQ;
  $connection = islandora_get_tuque_connection();

  $sandbox = &$context['sandbox'];
  if (!isset($sandbox['offset'])) {
    $sparql_count = $connection->repository->ri->countQuery($query, 'sparql');
    $sandbox['offset'] = 0;
    $sandbox['total'] = $sparql_count;
    if ($sandbox['total'] === 0) {
      return;
    }
  }

  $context['message'] = t('Processing results @start to @end.', array(
    '@start' => $sandbox['offset'],
    '@end' => min($sandbox['offset'] + $citation_update, $sandbox['total']),
  ));

  $offset_start = $sandbox['offset'];
  $query .= "
  LIMIT $citation_update
  OFFSET $offset_start
  ";
  module_load_include('inc', 'islandora', 'includes/derivatives');
  $results = $connection->repository->ri->sparqlQuery($query);
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
  $sandbox['offset'] += $citation_update;
  $context['finished'] = $sandbox['offset'] / $sandbox['total'];
}


/**
 * Command callback to update citations with HTML in MODS.
 */
function drush_islandora_scholar_fix_doi_html_in_mods() {
  $action = drush_get_option('action', variable_get('islandora_doi_handle_imported_metadata', FALSE));
  $audit = drush_get_option('audit', FALSE);
  $dry_run = drush_get_option('dry-run', FALSE);
  $modify_my_data = drush_get_option('modify-my-data', FALSE);

  // Require one and only one of --audit, --dry-run, --encode.
  if (!($audit || $dry_run || $modify_my_data)) {
    drush_print("This function requires one option of the following: --audit, --dry-run, --modify-my-data.");
    drush_print("USAGE:");
    drush_invoke('help', array('islandora-scholar-fix-doi-html-in-mods'));
    exit;
  }
  if (($audit && $dry_run) || (($audit || $dry_run) && $modify_my_data)) {
    drush_print("This function requires ONLY one option of the following: --audit, --dry-run, --modify-my-data.");
    drush_print("USAGE:");
    drush_invoke('help', array('islandora-scholar-fix-doi-html-in-mods'));
    exit;
  }

  // Require an action if --dry-run or --modify-my-data is selected.
  if (($modify_my_data OR $dry_run) AND (!in_array($action, array('strip', 'encode')))) {
    drush_print("This function requires an action option, since the GUI variable of how
     to handle DOI imports is not set. Please use `--action=encode` or
     `--action=strip`. For more information use `drush help islandora-scholar-fix-doi-html-in-mods`.");
    exit;
  }

  $method = ($modify_my_data ? 'modify' : ($dry_run ? 'dry-run' : 'audit'));

  // Confirm if modifying.
  if ($modify_my_data) {
    $warnings = dt("WARNING: You are about to alter datastream content in place.\n");
    $warnings .= ($action == 'strip') ?
      dt("You have chosen to strip errant HTML tags - this will result in a loss of some markup information.\n") :
      dt("You have chosen to encode errant HTML tags - this will by default result in HTML source tags being displayed to the user.\n");
    $warnings .= dt("Are you sure you wish to continue? \n");
    if (!drush_confirm($warnings, 2)) {
      drush_user_abort(dt('Exiting, no datastreams changed.'));
      exit;
    }
  }

  batch_set(islandora_scholar_fix_doi_html_in_mods_create_batch($action, $method));
  drush_backend_batch_process();
}

/**
 * Batch info for drush command to update citations with HTML in MODS.
 */
function islandora_scholar_fix_doi_html_in_mods_create_batch($action, $method) {
  return array(
    'operations' => array(
      array(
        'islandora_scholar_fix_doi_html_in_mods_batch_operation',
        array($action, $method),
      ),
    ),
    'title' => t('Updating MODS for citations...'),
    'init_message' => t('Preparing to update MODS.'),
    'progress_message' => t('Time elapsed: @elapsed <br/>Estimated time remaining @estimate.'),
    'error_message' => t('An error has occurred.'),
    'file' => drupal_get_path('module', 'islandora_scholar') . '/islandora_scholar.drush.inc',
  );
}

/**
 * Batch operations for drush command to update citations with HTML in MODS.
 */
function islandora_scholar_fix_doi_html_in_mods_batch_operation($action, $method, &$context) {
  $batch_size = 10;
  /* The islandora_doi_crossref_translator, which introduced the bug that this
  fixes, is part of the DOI importer, which is hard-coded to
  import only ir:citationCModel objects. If the repository manager has
  changed the content models of imported objects, they are responsible for
  finding and fixing any objects that need it.
  */
  $query = <<<EOQ
SELECT ?pid FROM <#ri>
WHERE {
  ?pid <fedora-model:hasModel> <info:fedora/ir:citationCModel> ;
       <fedora-view:disseminates> ?ds .
  ?ds <fedora-view:disseminationType> <info:fedora/*/MODS> .
}
ORDER BY ?pid
EOQ;
  $connection = islandora_get_tuque_connection();

  $sandbox = &$context['sandbox'];
  if (!isset($sandbox['offset'])) {
    $sparql_count = $connection->repository->ri->countQuery($query, 'sparql');
    $sandbox['offset'] = 0;
    $sandbox['total'] = $sparql_count;
    if ($sandbox['total'] === 0) {
      return;
    }
  }

  $context['message'] = t('Processing results @start to @end.', array(
    '@start' => $sandbox['offset'],
    '@end' => min($sandbox['offset'] + $batch_size, $sandbox['total']),
  ));

  $offset_start = $sandbox['offset'];
  $query .= "
  LIMIT $batch_size
  OFFSET $offset_start
  ";
  $results = $connection->repository->ri->sparqlQuery($query);
  module_load_include('inc', 'islandora_doi', 'includes/utilities');
  foreach ($results as $result) {
    $object = islandora_object_load($result['pid']['value']);
    $namespace = 'http://www.loc.gov/mods/v3';
    $doc = new DOMDocument();
    $doc->loadXML($object['MODS']->content);
    $needs_work = FALSE;
    /* The crossref metadata spec dictates that 'Face markup' can only be
    present in title, subtitle, original_language_title, and
    unstructured_citation (not implemented in the doi translator).
    src: https://support.crossref.org/hc/en-us/articles/214532023
    */
    foreach (array('title', 'subTitle') as $local_name) {
      $dubious_nodes = $doc->getElementsByTagNameNS($namespace, $local_name);
      foreach ($dubious_nodes as $node) {
        if ($node->childNodes->length > 1) {
          $needs_work = TRUE;
        }
      }
    }
    if ($needs_work) {
      if ($method == 'audit') {
        drush_print($object->id);
        continue;
      }
      drush_print("\n===========  {$object->id}. ========================================\n");
      foreach (array('title', 'subTitle') as $local_name) {
        $dubious_nodes = $doc->getElementsByTagNameNS($namespace, $local_name);
        foreach ($dubious_nodes as $node) {
          $child_count = $node->childNodes->length;
          if ($child_count > 1) {
            $old_value = $node->ownerDocument->saveXML($node);
            $sanitized_content = new DOMText(islandora_doi_filter_html($node, $action));
            for ($i = 0; $i < $child_count; $i++) {
              $node->removeChild($node->childNodes->item(0));
            }
            $node->appendChild($sanitized_content);
            $new_value = $node->ownerDocument->saveXML($node);

            drush_print("OLD VALUE:\n$old_value");
            drush_print("NEW VALUE:\n$new_value");
          }
        }
      }
      drush_print("==========================================================================\n");
      if ($method == 'modify') {
        $object['MODS']->content = $doc->saveXML();
      }
    }
  }
  $sandbox['offset'] += $batch_size;
  $context['finished'] = $sandbox['offset'] / $sandbox['total'];
}
