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
   * @param $pid string
   *   A string containing the PID for which we're to return markup for a citation.
   * @return string
   *   A string containing HTML markup.
   */
  public static function get_citation($mods) {
    $bibliography = citeproc_bibliography_from_mods(
        citeproc_default_style(),
        $mods
    );
    return $bibliography->render();
  }

  /**
   * Retrieves the viewer.
   *
   * @param type $object
   * @return type
   */
  public static function get_viewer($object) {
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

function islandora_scholar_islandora_scholar_viewer($object) {
  global $user;
  $to_return = array();

  if (isset($object['PDF'])) {
    $access_pdf = TRUE;

    module_load_include('inc', 'islandora_xacml_api', 'IslandoraXacml');
    if ($access_pdf && class_exists('IslandoraXacml')) {
      $xacml = new IslandoraXacml($object->id);
      $access_pdf = $access_pdf && (($user->uid === 1) || $xacml->datastreamRule->hasPermission($user->name, $user->roles, NULL, 'PDF'));
    }

    if ($access_pdf) {
      $to_return += array(
        'google' => array(
          '#callback' => 'islandora_scholar_google_pdf_viewer',
          '#callback arguments' => array($object->id),
          '#weight' => 1,
        ),
      );
    }
  }

  return $to_return;
}

function islandora_scholar_google_pdf_viewer($pid) {
  global $base_url;

  $base = '!base_url/islandora/objects/!pid/datastreams/!dsid/view';
  if (variable_get('islandora_scholar_google_direct_to_fedora', FALSE)) {
    $base = '!fedora_base/objects/!pid/datastreams/!dsid/content';
  }

  $iframe_attrs = array(
    'src' => url('http://docs.google.com/viewer', array('query' => array(
      'url' =>  t($base, array(
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
      '#value' => l(t('PDF'), "islandora/object/$pid/datastreams/PDF/download"),
      '#weight' => 1,
    ),
  );
}
