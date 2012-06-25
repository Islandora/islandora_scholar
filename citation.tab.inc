<?php

class CitationView {
  /**
   * Covenience method.
   *
   * Get the MODS withwhich to create a citation.
   *
   * @param $pid string
   *   A Fedora PID to get the MODS from.
   * @return string
   *   A string containing a MODS XML data.
   */
  protected static function get_mods($pid) {
    module_load_include('inc', 'fedora_repository', 'api/fedora_item');
    $item = new Fedora_Item($pid);
    $mods = trim($item->get_datastream_dissemination('MODS'));
    return $mods;
  }

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
  protected static function get_citation($pid) {
    $bibliography = citeproc_bibliography_from_mods(
        citeproc_default_style(),
        self::get_mods($pid)
    );
    return $bibliography->render();
  }

  /**
   * Get a tabset to merge into the set of Islandora tabs.
   *
   * @param $pid string
   *   A string containing the PID for which we're to render a view.
   * @return array
   *   A tabset (to be merged with existing tabs).
   */
  public static function get_tabs($pid) {
    return array(
      'overview' => array(
        '#title' => t('Overview'),
        '#type' => 'tabpage',
        '#tab_name' => 'overview',
        'citation' => array(
          '#title' => t('Citation'),
          '#type' => 'item',
          '#value' => self::get_citation($pid),
        ),
        'document' => array(
          '#title' => t('Document'),
          '#type' => 'item',
          'viewer' => self::get_viewer($pid),
          'add_remove' => self::add_remove_form($pid),
        ),
      ),
    );
  }

  protected static function get_viewer($pid) {
    module_load_include('inc', 'fedora_repository', 'api/fedora_item');
    $item = new Fedora_Item($pid);
    $to_return = array('#value' => '');

    $viewers = module_invoke_all('islandora_scholar_viewer', $item);
    drupal_alter('islandora_scholar_viewer', $viewers, $item);
    uasort($viewers, 'element_sort');

    while(($viewer = array_shift($viewers)) && empty($to_return['#value'])) {
      $to_return = array(
        '#value' => call_user_func_array($viewer['#callback'], $viewer['#callback arguments']));
    }

    return $to_return;
  }

  protected static function add_remove_form($pid) {
    module_load_include('inc', 'fedora_repository', 'api/fedora_item');
    $item = new Fedora_Item($pid);
    if(user_access('ingest new fedora objects' || $is_owner)) {
      $has_pdf = array_key_exists('PDF', $item->datastreams);
      return array(
        '#title' => ($has_pdf ?
          t('Modify Current Document'):
          t('Add Document')),
        '#value' => ($has_pdf ?
          drupal_get_form('islandora_scholar_modify_form', $pid):
          drupal_get_form('islandora_scholar_upload_form', $pid)),
      );
    }
    else {
      return array();
    }
  }
}

function islandora_scholar_modify_form($form_state, $pid) {
  return array(
    'pid' => array( //Store the PID.
      '#type' => 'value',
      '#value' => $pid,
    ),
    'download' => array(
      '#type' => 'item',
      '#title' => t('Download'),
      '#value' => l(t('PDF'), "fedora/repository/object_download/$pid/PDF"),
    ),
    'delete_document' => array(
      '#type' => 'submit',
      '#value' => t('Delete Document/PDF'),
    ),
  );
}

function islandora_scholar_modify_form_submit($form, $form_state) {
  //TODO:  Remove the PDF (and derivatives) if the button is clicked.
  dpm('Nothing doing...  Implement the deletion.');
}

function islandora_scholar_upload_form($form_state, $pid) {
  return array(
    array(
      '#type' => 'fieldset',
      '#title' => t('Upload Document'),
      'pid' => array(
        '#type' => 'value',
        '#value' => $pid,
      ),
      'file' => array(
        '#type' => 'file', //Might want to make this use an AJAX uploader? (upload_element?)
        '#title' => 'PDF file',
      ),
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Upload'),
      ),
    ),
  );
}

function islandora_scholar_upload_form_submit($form, $form_state) {
  //TODO:  Add the PDF (and derivatives?) on submission.
  dpm('Nothing doing...  Implement the addition.');
}

function islandora_scholar_islandora_scholar_viewer($item) {
  $to_return = array();
//   if (array_key_exists('SWF', $item->datastreams)) {
//     $to_return += array(
//       'flexpaper' => array(
//         '#callback' => 'swf',
//         '#callback arguments' => array("fedora/repository/{$item->pid}/SWF"),
//         '#weight' => -1,
//       ),
//     );
//   }

  if (array_key_exists('PDF', $item->datastreams)) {
    $to_return += array(
//       'flexpaper_temp_swf' => array(
//         '#callback' => 'islandora_scholar_transformed_pdf_viewer',
//         '#callback arguments' => array($item),
//         '#weight' => 0,
//       ),
      'google' => array(
        '#callback' => 'islandora_scholar_google_pdf_viewer',
        '#callback arguments' => array($item->pid),
        '#weight' => 1,
      ),
    );
  }

  return $to_return;
}

/* TODO:  Finish implementing...
function islandora_scholar_transformed_pdf_viewer($item) {
  $to_return = '';

  //Check if we've got a SWF-transformed copy newer than the PDF...
  $path = 'islandora_scholar_swfs/' . rawurlencode("{$item->pid}/SWF");
  if ($swf_filename = file_create_path($path) && !file_exists($swf_filename)) {
    //If we do not, get the PDF from Fedora.
    $pdf_filename = file_create_filename(rawurlencode("{$item->pid}/PDF"), file_directory_temp());
    file_put_contents($pdf_filename, $item->get_datastream_dissemination('PDF'));

    //And transform the PDF to a swf...
    $cmd = variable_get('islandora_scholar_pdf2swf', '/usr/local/bin/pdf2swf');
    proc_open(escapeshellcmd("$cmd "));
  }

  //Return markup to show the SWF.
  return $to_return;
}*/

function islandora_scholar_google_pdf_viewer($pid) {
  global $base_url;

  $base = '!base_url/fedora/repository/!pid/!dsid/preview.pdf';
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

  return '<iframe ' . drupal_attributes($iframe_attrs) . '></iframe>';
}