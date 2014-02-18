<?php

/**
 * @file
 * Utitlity functions.
 */

/**
 * Returns view parameters.
 *
 * @param AbstractObject $object
 *   object representing Fedora Object
 *
 * @return array
 *   array of values to construct view
 */
function islandora_scholar_get_view(AbstractObject $object) {
  module_load_include('inc', 'islandora_scholar', 'includes/citation.tab');
  module_load_include('inc', 'islandora_scholar', 'includes/coins');
  $i = 1;
  $display = array(
    'citation' => array(
      '#weight' => $i++,
    ),
    'preview' => array(
      '#weight' => $i++,
    ),
    'pdf_download' => array(
      '#weight' => $i++,
    ),
    'metadata' => array(
      '#type' => 'fieldset',
      '#title' => t('Metadata'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#weight' => $i++,
      'value' => islandora_scholar_details($object),
    ),
  );
  // @TODO: Let Solr metadata and other Islandora metadata viewers interact
  // with this.
  if (isset($object['MODS']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $object['MODS'])) {
    $display['citation']['#markup'] = citeproc_bibliography_from_mods(
        citeproc_default_style(), $object['MODS']->content
    );

    if (variable_get('islandora_scholar_google_scholar_search_enabled', FALSE)) {
      // Google Scholar Search.
      $mods_xml = simplexml_load_string($object['MODS']->content);
      $mods_xml->registerXPathNamespace('mods', 'http://www.loc.gov/mods/v3');

      // Search for doi for search term.  If exists use it instead of default.
      $primary_search = $mods_xml->xpath("" . variable_get('islandora_scholar_google_scholar_primary_search_xpath', NULL));
      if ($primary_search) {
        $search_term = (string) reset($primary_search);
      }
      else {
        // Default search term is the title of the citation.
        $default_search = $mods_xml->xpath("" . variable_get('islandora_scholar_google_scholar_default_search_xpath', "$object->label"));
        $search_term = (string) reset($default_search);
      }

      $display['google_scholar_search'] = array(
        '#type' => 'item',
        '#markup' => l(t('Search for this publication on Google Scholar'), "http://scholar.google.ca/scholar?q=\"$search_term\""),
        '#weight' => 0,
      );
    }
  }
  else {
    $display['citation']['#markup'] = t('Unable to load MODS.');
  }

  if (isset($object['PREVIEW']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $object['PREVIEW'])) {
    $image = theme('image', array(
      'path' => "islandora/object/$object->id/datastream/PREVIEW/view",
    ));
    if (isset($object['PDF']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $object['PDF'])) {
      $display['preview'] = array(
        '#markup' => l($image, "islandora/object/$object->id/datastream/PDF/view", array(
          'html' => TRUE,
        )),
        '#weight' => $display['preview']['#weight'],
      );
    }
    else {
      $display['preview'] = array(
        '#theme' => 'image',
        '#path' => "islandora/object/$object/datastream/PREVIEW/view",
        '#weight' => $display['preview']['#weight'],
      );
    }
  }

  if (isset($object['PDF']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $object['PDF'])) {
    $display['pdf_download'] = array(
      '#type' => 'item',
      '#title' => t('Download'),
      '#markup' => l(t('PDF'), "islandora/object/$object->id/datastream/PDF/download/citation.pdf"),
      '#weight' => $display['pdf_download']['#weight'],
    );
  }

  return array('citation.tab' => $display);
}

/**
 * Gets steps required for object ingest.
 *
 * @return array
 *   values required for upload form
 */
function islandora_scholar_get_ingest_steps() {
  return array(
    'islandora_scholar_file_upload' => array(
      'weight' => 10,
      'type' => 'form',
      'form_id' => 'islandora_scholar_pdf_upload_form',
      'module' => 'islandora_scholar',
      'file' => 'includes/pdf_upload.form.inc',
    ),
  );
}

/**
 * Returns values required for Islandora derivative hook.
 *
 * @return array
 *   values required for Islandora derivative hook
 */
function islandora_scholar_get_derivatives() {
  $derivatives = array();
  $derivatives[] = array(
    'source_dsid' => 'PDF',
    'destination_dsid' => 'TN',
    'weight' => '0',
    'function' => array(
      'islandora_scholar_add_tn_derivative',
    ),
    'file' => drupal_get_path('module', 'islandora_scholar') . '/includes/derivatives.inc',
  );
  $derivatives[] = array(
    'source_dsid' => 'PDF',
    'destination_dsid' => 'PREVIEW',
    'weight' => '1',
    'function' => array(
      'islandora_scholar_add_preview_derivative',
    ),
    'file' => drupal_get_path('module', 'islandora_scholar') . '/includes/derivatives.inc',
  );
  if (variable_get('islandora_scholar_create_fulltext', TRUE)) {
    $derivatives[] = array(
      'source_dsid' => 'PDF',
      'destination_dsid' => 'FULL_TEXT',
      'weight' => '2',
      'function' => array(
        'islandora_scholar_add_fulltext_derivative',
      ),
      'file' => drupal_get_path('module', 'islandora_scholar') . '/includes/derivatives.inc',
    );
  }
  return $derivatives;
}
