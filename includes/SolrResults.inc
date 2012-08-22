<?php

/**
 * @file
 *
 */
module_load_include('inc', 'islandora_solr_search', 'IslandoraSolrResults');

/**
 * Extention of IslandoraSolrResults for templating purposes.
 * This overrides the displayResults function to provide an alternate display type.
 */
class IslandoraSolrResultsCitation extends IslandoraSolrResults {

  /**
   * Overrides parent function.
   *
   * @param IslandoraSolrQueryProcessor $query_processor
   * @param string $title
   * @param string $output
   */
  function displayResults(IslandoraSolrQueryProcessor $query_processor, $title = "Search Results", $output = '') {
    module_load_include('inc', 'scholar', 'SearchForm');
    drupal_set_title(t('Search Publications'));
    drupal_add_css(drupal_get_path('module', 'scholar') . '/css/Search.css');

    if (!empty($_SESSION['scholar']['search']['sort'])) {
      $sort = $_SESSION['scholar']['search']['sort'];
      $order = $_SESSION['scholar']['search']['order'];
      $query_processor->solrParams['fl'] = '*,score';
      $query_processor->solrParams['sort'] = $sort . ' ' . $order;
      $query_processor->executeQuery();
    }
    elseif ($query_processor->solrQuery === '*:*') {
      $sort = variable_get('islandora_scholar_solr_title_field', 'mods_title_mlt');
      $order = 'asc';
      $query_processor->solrParams['fl'] = '*,score';
      $query_processor->solrParams['sort'] = $sort . ' ' . $order;
      $query_processor->executeQuery();
    }

    $total = (int) $query_processor->solrResult->response->numFound;

    if ($total === 0) {
      $output .= "<h2>Search Results</h2>";
      $output .= "<div>Sorry, but your search returned no results.</div>";
      return $output;
    }
    islandora_solr_search_pager_init($total, $query_processor->solrLimit);
    $output .= '<div class="content">';
    $output .= drupal_get_form('islandora_scholar_search_results_form', $query_processor);
    $output .= theme('pager', null, $query_processor->solrLimit, 0, null, 5);
    $output .= "</div>";
    return $output;
  }

  /**
   * Function: printResults
   *
   * Description: translates a solr query result into a basic in-browser search result.
   *
   * @param Apache_Solr_Object $results
   */
  function printResults($results) {
    module_load_include('inc', 'fedora_repository', 'api/fedora_item');
    if (empty($results)) {
      return "no results";
    }
    $output = '';
    $record_start = $results->response->start;
    $limit_results = variable_get('islandora_solr_search_limit_result_fields', 0);
    module_load_include('inc', 'csl', 'CSL');
    $style = citeproc_default_style();
    foreach ($results->response->docs as $doc) {
      $output .= theme(THEME_SCHOLAR_SEARCH_ITEM, $doc->PID, $style);
    }
    return $output;
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
  while(($decoded = htmlspecialchars_decode($title, ENT_QUOTES)) != $title) {
    $title = $decoded;
  }
  $vars['title'] = l($title, 'fedora/repository/' . htmlspecialchars($pid, ENT_QUOTES, 'utf-8', FALSE));

  $vars['citation'] = scholar_citation_from_solr_doc($s_doc, $vars['style'], variable_get('islandora_scholar_mods_xml_solr_field','mods_fullxml_store'), 'js');
}

/**
 *
 * @param array $variables
 */
function template_preprocess_islandora_scholar_search_item(array &$variables) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  $pid = $variables['pid'];
  $item = new Fedora_Item($pid);
  if ($item->exists() && isset($item->datastreams['MODS'])) {
    $title = $item->objectProfile->objLabel;
    //XXX:  Shouldn't have to occur (multiple times, anyway).
    while(($decoded = htmlspecialchars_decode($title, ENT_QUOTES)) != $title) {
      $title = $decoded;
    }
    $variables['title'] = l($title, 'fedora/repository/' . htmlspecialchars($pid, ENT_QUOTES, 'utf-8', FALSE));

    $style = $variables['style'];

    $cached_raw = NULL;
    $cache_id = NULL;

    if(module_exists('citeproc') && CITEPROC_DO_CACHE) {
      $cached_raw = cache_get($cache_id);
      $cache_id = CITEPROC_CACHE_PREFIX . '_' . $pid;
    }

    $bibliography = NULL;
    if (!empty($cached_raw)) {
      $bibliography = citeproc_bibliography_from_raw_array($style, $cached_raw->data);
    }
    else {
      $mods = $item->get_datastream_dissemination('MODS');
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

function islandora_scholar_search_results_form(array &$form_state, IslandoraSolrQueryProcessor $query_processor) {
  global $user;
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  $results = $query_processor->solrResult;
  $total = (int) $results->response->numFound;
  $start = (int) $query_processor->solrStart + 1;
  $end = min(($query_processor->solrLimit + $query_processor->solrStart), $total);
  $title = t('Search Results (%start - %end of %total)', array('%start' => $start, '%end' => $end, '%total' => $total));
  $table = array(
    '#header' => array($title),
    '#theme' => THEME_SCHOLAR_SEARCH_TABLE,
    '#tree' => TRUE,
  );

  module_load_include('inc', 'csl', 'CSL');
  $style = citeproc_default_style();
  $pids = array();
  foreach ($results->response->docs as $doc) {
    $pid = $doc->PID;
    $result = '';
    $pids[] = $pid;
    // Turn the choice of getting object MODS from Fedora or Solr into a configuration option
    // Default is to get it the old way: from Fedora -- THEME_SCHOLAR_SEARCH_ITEM
    $result = (
      variable_get('islandora_scholar_theme_citation_from_solr_mods',false) ?
      theme(THEME_SCHOLAR_SEARCH_ITEM_ALT, $doc, $style) :
      theme(THEME_SCHOLAR_SEARCH_ITEM, $pid, $style)
    );
    $table['rows'][] = array(
      '#pid' => $pid,
      'result' => array('#value' => $result),
    );
  }
  /*$checkboxes = array_fill_keys($pids, '');
  $table['selections'] = array(
    '#type' => 'checkboxes',
    '#options' => $checkboxes,
  );*/
  $form_state['storage']['query'] = $query_processor;

  return array(
    'controls' => array(
      '#type' => 'markup',
      '#prefix' => '<div class="scholar-export-controls">',
      '#suffix' => '</div>',
      'export' => array(),
      'save' => array(),
    ),
    'pager' => array(
      '#value' => theme('pager', null, $query_processor->solrLimit, 0, null, 5)
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
 * @param array $element
 *   A Drupal Form Element.
 *
 * @return sting
 *   HTML that renders a table of settings for datastreams.
 */
function theme_islandora_scholar_search_results_table(array $element) {
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
  $attributes = isset($element['#id']) ? array('id' => $element['#id']) : NULL;
  return '<div class="scholar-search-results">' . theme_table($element['#header'], $rows, $attributes) . '</div>';
}
