<?php

/**
 * @file
 *
 */
module_load_include('inc', 'islandora_solr', 'IslandoraSolrResults');

/**
 * Extention of IslandoraSolrResults for templating purposes.
 * This overrides the displayResults function to provide an alternate display type.
 */
class IslandoraSolrResultsCitation extends IslandoraSolrResults {
  /**
   * Function: printResults
   *
   * Description: translates a solr query result into a basic in-browser search result.
   *
   * @param Apache_Solr_Object $results
   */
  function printResults($results) {
    if (empty($results)) {
      return "no results";
    }
    $output = drupal_get_form('islandora_scholar_search_results_form', (array)$results->response->docs);
    return drupal_render($output);
  }
}
function template_preprocess_islandora_scholar_search_alt_item(array &$vars) {
  $mods_title_field = variable_get('islandora_scholar_solr_title_field', 'mods_title_mlt');
  $s_doc = & $vars['solr_doc'];
  $pid = $s_doc->PID;

  $title = $s_doc->$mods_title_field;
  if (is_array($title)) {
    $title = $title[0];
  }
  //XXX:  Shouldn't have to occur (multiple times, anyway).
  while (($decoded = htmlspecialchars_decode($title, ENT_QUOTES)) != $title) {
    $title = $decoded;
  }
  $vars['title'] = l($title, 'islandora/object/' . htmlspecialchars($pid, ENT_QUOTES, 'utf-8', FALSE));

  $vars['citation'] = scholar_citation_from_solr_doc($s_doc, $vars['style'], variable_get('islandora_scholar_mods_xml_solr_field','mods_fullxml_store'), 'js');
}

/**
 *
 * @param array $variables
 */
function template_preprocess_islandora_scholar_search_item(array &$variables) {
  $pid = $variables['pid'];
  $object = islandora_object_load($pid);
  if (isset($object) && isset($object['MODS'])) {
    $title = $object->label;
    //XXX:  Shouldn't have to occur (multiple times, anyway).
    while (($decoded = htmlspecialchars_decode($title, ENT_QUOTES)) != $title) {
      $title = $decoded;
    }
    $variables['title'] = l($title, 'islandora/object' . htmlspecialchars($pid, ENT_QUOTES, 'utf-8', FALSE));

    $style = $variables['style'];

    $cached_raw = NULL;
    $cache_id = NULL;

    if (module_exists('citeproc') && CITEPROC_DO_CACHE) {
      $cached_raw = cache_get($cache_id);
      $cache_id = CITEPROC_CACHE_PREFIX . '_' . $pid;
    }

    $bibliography = NULL;
    if (!empty($cached_raw)) {
      $bibliography = citeproc_bibliography_from_raw_array($style, $cached_raw->data);
    }
    else {
      $mods = $object['MODS']->content;
      $mods = trim($mods);
      if (!empty($mods)) {
        $doc = new DOMDocument();
        $doc->loadXML($mods);

        $bibliography = citeproc_bibliography_from_mods($style, $doc, $cache_id);
      }
    }
    $variables['citation'] = $bibliography->render();
  }
}

/**
 *
 * @param array $docs
 *   An array of objects, as received from Solr.
 */
function islandora_scholar_search_results_form(array $form, array &$form_state, array $docs) {
  form_load_include($form_state, 'inc', 'islandora_scholar', 'includes/SolrResults');
  global $user;
  $table = array(
    '#header' => array(''),
    '#theme' => THEME_SCHOLAR_SEARCH_TABLE,
    '#tree' => TRUE,
  );

  module_load_include('inc', 'csl', 'CSL');
  $style = citeproc_default_style();
  $pids = array();

  foreach ($docs as $doc) {
    $pid = $doc->PID;
    $result = '';
    $pids[] = $pid;

    $params = array(
      'solr_doc' => $doc,
      'style' => $style,
      'pid' => $pid,
    );

    // Turn the choice of getting object MODS from Fedora or Solr into a configuration option
    // Default is to get it the old way: from Fedora -- THEME_SCHOLAR_SEARCH_ITEM
    $result = (
      variable_get('islandora_scholar_theme_citation_from_solr_mods', FALSE) ?
      theme(THEME_SCHOLAR_SEARCH_ITEM_ALT, $params) :
      theme(THEME_SCHOLAR_SEARCH_ITEM, $params)
    );
    $table['rows'][] = array(
      '#pid' => $pid,
      'result' => array('#markup' => $result),
    );
  }

  return array(
    'controls' => array(
      '#type' => 'markup',
      '#prefix' => '<div class="scholar-export-controls">',
      '#suffix' => '</div>',
      'export' => array(),
      'save' => array(),
    ),
    'pager' => array(
      '#markup' => theme('pager', array(NULL, 0, 0, NULL, 5)),
    ),
    'table' => $table,
  );
}

/**
 *
 * @param array $form_state
 * @return type
 */
function _islandora_scholar_search_get_all_pids_from_query(array &$form_state) {
  $query_processor = $form_state['storage']['query'];
  $previous_limit = $query_processor->solrLimit;
  $query_processor->solrLimit = 100000;
  $fl =& $query_processor->solrParams['fl'];
  $old_fl = $fl;
  $fl = 'PID';
  $query_processor->executeQuery();
  $fl = $old_fl;
  $query_processor->solrLimit = $previous_limit;
  $results = $query_processor->solrResult;
  $pids = array();
  foreach ($results->response->docs as $doc) {
    $pids[] = $doc->PID;
  }
  return $pids;
}

/**
 * Theme's a form table for this module.
 *
 * @param array $vars
 * @return sting
 *   HTML that renders a table of settings for datastreams.
 */
function theme_islandora_scholar_search_results_table(array $vars) {
  $element = $vars['element'];
  $rows = array();
  foreach (element_children($element['rows']) as $child) {
    $setting = $element['rows'][$child];
    $fields = array();
//TODO: Make the checkboxes in the alter...
//     $fields[] = drupal_render($element['selections'][$setting['#pid']]); // First field is a checkbox
    foreach (element_children($setting) as $property) {
      $field = $setting[$property];
      $fields[] = drupal_render($field);
    }
    $rows[] = array(
      'data' => $fields,
      'class' => isset($setting['#attributes']['class']) ? $setting['#attributes']['class'] : NULL
    );
  }
  $attributes = isset($element['#id']) ? array('id' => $element['#id']) : array();
  return '<div class="scholar-search-results">' . theme('table', array(
    'header' => $element['#header'],
    'rows' => $rows,
    'attributes' => $attributes,
  )) . '</div>';
}
