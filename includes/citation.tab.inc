<?php

/**
 * @file
 * Module used to create a citation view tab.
 */

class CitationView {
  /**
   * Covenience method.
   *
   * Get a rendered citation.
   *
   * @param string $mods
   *   A MODS document.
   *
   * @return string
   *   A string containing HTML markup.
   */
  public static function getCitation($mods) {
    return citeproc_bibliography_from_mods(
      citeproc_default_style(),
      $mods
    );
  }

  /**
   * Retrieves a viewer for rendering the object as a citation.
   *
   * Retrieves the viewer.
   *
   * @param AbstractObject $object
   *   The object to view.
   *
   * @return array
   *   A renderable array
   */
  public static function getViewer(AbstractObject $object) {
    $to_return = array();

    $viewers = module_invoke_all('islandora_scholar_viewer', $object);
    drupal_alter('islandora_scholar_viewer', $viewers, $object);
    uasort($viewers, 'element_sort');

    while (($viewer = array_shift($viewers)) && empty($to_return)) {
      $to_return = call_user_func_array($viewer['#callback'], $viewer['#callback arguments']);
    }

    return $to_return;
  }
}

/**
 * Implements hook_islandora_scholar_viewer().
 */
function islandora_scholar_islandora_scholar_viewer(AbstractObject $object) {
  $to_return = array();

  if (isset($object['PDF']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $object['PDF'])) {
    $to_return += array(
      'google' => array(
        '#callback' => 'islandora_scholar_google_pdf_viewer',
        '#callback arguments' => array($object->id),
        '#weight' => 1,
      ),
    );
  }

  return $to_return;
}

/**
 * Renders the given object's PDF datastream in an iframe using Google docs.
 *
 * Also provides a download link for the PDF datastream.
 *
 * The PDF datastream is rendered directly from Fedora so this will only work
 * for PDF datastreams which are viewable by anonymous users.
 *
 * @param string $pid
 *   The PID of the object.
 *
 * @return array
 *   A renderable array.
 */
function islandora_scholar_google_pdf_viewer($pid) {
  global $base_url;

  $base = '!base_url/islandora/object/!pid/datastream/!dsid/view';
  if (variable_get('islandora_scholar_google_direct_to_fedora', FALSE)) {
    $base = '!fedora_base/objects/!pid/datastreams/!dsid/content';
  }

  $iframe_attrs = array(
    'src' => url('http://docs.google.com/viewer', array(
             'query' => array(
               'url' => format_string($base, array(
                        '!base_url' => $base_url,
                        '!fedora_base' => variable_get('fedora_base_url', 'http://localhost:8080/fedora'),
                        '!pid' => $pid,
                        '!dsid' => 'PDF',
                      )),
               'embedded' => 'true',
             ))),
    'style' => 'width: 600px; height: 500px;',
    'frameborder' => '0',
  );

  return array(
    'viewer' => array(
      '#markup' => '<iframe ' . drupal_attributes($iframe_attrs) . '></iframe>',
      '#weight' => 0,
    ),
    'download' => array(
      '#type' => 'item',
      '#title' => t('Download'),
      '#markup' => l(t('PDF'), "islandora/object/$pid/datastream/PDF/download"),
      '#weight' => 1,
    ),
  );
}
