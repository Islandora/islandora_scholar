<?php

/**
 * @file
 *
 * Functions for exporting citations from the Bibliography.
 */

/**
 * Export many citations in a particular format and redirect the user to another page.
 */
function islandora_bibliography_export() {
  $export = isset($_SESSION['bibliography']['export']) ? $_SESSION['bibliography']['export'] : NULL;
  $filename = isset($export['filename']) ? $export['filename'] : NULL;
  $mime_type = isset($export['mime_type']) ? $export['mime_type'] : NULL;
  $download_filename = isset($export['download_filename']) ? $export['download_filename'] : $filename;
  $redirect = isset($export['redirect']) ? $export['redirect'] : NULL;
  unset($_SESSION['bibliography']['export']);
  if (file_exists($filename)) {
    $file_size = filesize($filename);
    header("Content-type: $mime_type");
    header("Content-length: $file_size");
    header("Content-Disposition: attachment; filename=\"$download_filename\"");
    header("Cache-control: private");
    $curl_handle = curl_init();
    if ($curl_handle !== FALSE) {
      global $base_url;
      $url = $base_url . '/' . $filename;
      curl_setopt($curl_handle, CURLOPT_SSL_VERIFYPEER, FALSE);
      curl_setopt($curl_handle, CURLOPT_SSL_VERIFYHOST, FALSE);
      curl_setopt($curl_handle, CURLOPT_FAILONERROR, 1); // Fail on errors
      curl_setopt($curl_handle, CURLOPT_FOLLOWLOCATION, 1); // allow redirects
      curl_setopt($curl_handle, CURLOPT_USERAGENT, "Mozilla/4.0 pp(compatible; MSIE 5.01; Windows NT 5.0)");
      curl_setopt($curl_handle, CURLOPT_RETURNTRANSFER, 0); // return into a variable
      curl_setopt($curl_handle, CURLOPT_URL, $url);
    }
    curl_exec($curl_handle);
    curl_close($curl_handle);
  }
  else {
    drupal_goto($redirect);
    exit();
  }
}

/**
 *
 * @param array $pids
 * @param array $context
 *
function _islandora_bibliography_batch_combine_mods($filename, $pid, array &$context) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  $mods = new DOMDocument();
  $mods->load($filename);
  $item = new Fedora_Item($pid);
  if ($item->exists() && isset($item->datastreams['MODS'])) {
    $datastream = $item->get_datastream_dissemination('MODS');
    $datastream = trim($datastream);
    if (!empty($mods)) {
      $doc = new DOMDocument();
      $doc->loadXML($datastream);
      $node = $mods->importNode($doc->documentElement, TRUE);
      $mods->documentElement->appendChild($node);
      $mods->save($filename);
    }
  }
}

/**
 *
 * @param array $pids
 * @param string $redirect
 * @param array $context
 *
function _islandora_bibliography_convert_mods_to_ris($filename, array &$context) {
  $context['results']['filename'] = bibutils_mods_file_to_ris_document($filename);
  $context['results']['mime_type'] = 'application/x-Research-Info-Systems';
  $context['results']['download_filename'] = 'export.ris';
}

/**
 *
 * @param type $redirect
 * @param array $context
 *
function _islandora_bibliography_redirect($redirect, array &$context) {
  $context['results']['redirect'] = $redirect;
}

/**
 *
 * @param type $pid
 *
function _islandora_bibliography_batch_convert_mods_to_json($pid, array &$context) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  module_load_include('php', 'citeproc', 'generators/converter');
  $item = new Fedora_Item($pid);
  if ($item->exists() && isset($item->datastreams['MODS'])) {
    $mods = $item->get_datastream_dissemination('MODS');
    $mods = trim($mods);
    if (!empty($mods)) {
      $context['results']['data'][] = convert_mods_to_citeproc_jsons($mods);
    }
  }
}

function _islandora_bibliography_convert_json_to_rtf($style, array &$context) {
  module_load_include('php', 'islandora_bibliography', 'class_rtf');
  module_load_include('php', 'CiteProc', 'lib/citeproc-php/CiteProc');
  /* Style *
  $csl = new DomDocument();
  $csl->load(drupal_get_path('module', 'CiteProc') . '/data/' . $style);
  /* Citations *
  $citeproc = new citeproc($csl->saveXML());
  $rtf = new rtf();
  foreach ($context['results']['data'] as $citation) {
    $citation = (object) $citation;
    $citation = json_decode(json_encode($citation)); // HACKKKKK!
    $text = $citeproc->render($citation, 'bibliography');
    $rtf->addText($text . '<br/>');
  }
  /* File *
  $temp_file_dir = file_directory_temp();
  $temp_file_name = file_create_filename("export.rtf", $temp_file_dir);
  $file = fopen($temp_file_name, 'w');
  if ($file !== FALSE) {
    $document = $rtf->getDocument();
    fwrite($file, $document);
    fclose($file);
  }
  /* Batch Values *
  $context['results']['filename'] = $temp_file_name;
  $context['results']['download_filename'] = 'export.rtf';
  $context['results']['mime_type'] = 'application/rtf';
}

function _islandora_bibliography_convert_rtf_to_pdf(array &$context) {
  $output = array();
  $return_value = FALSE;
  $filename = $context['results']['filename'];
  exec('soffice.bin --headless -convert-to pdf ' . $filename, $output, $return_value);
  $filename = preg_replace('/\.rtf$/', '.pdf', $context['results']['filename']);
  $context['results']['filename'] = $filename;
  $context['results']['download_filename'] = 'export.pdf';
  $context['results']['mime_type'] = 'application/pdf';
}

/**
 *
 * @param boolean $successful
 * @param array $results
 * @param array $operations
 *
function _islandora_bibliography_export_citations($successful, array $results, array $operations) {
  $_SESSION['bibliography']['export']['filename'] = $results['filename'];
  $_SESSION['bibliography']['export']['download_filename'] = $results['download_filename'];
  $_SESSION['bibliography']['export']['mime_type'] = $results['mime_type'];
  $_SESSION['bibliography']['export']['redirect'] = $results['redirect'];
}

/**
 *
 * @param array $pids
 * @param type $style
 * @param string $file_format
 *   Either pdf, rtf, ris, if null display to screen.
 *
function islandora_bibliography_export_citations(array $pids, $style, $file_format = NULL) {
  $operations = array();
  switch ($file_format) {
    case 'RIS':
      $temp_file_dir = file_directory_temp();
      $temp_file = file_create_filename("mods.xml", $temp_file_dir);
      $mods = new DOMDocument();
      $mods->loadXML('<modsCollection xmlns="http://www.loc.gov/mods/v3" />');
      $mods->save($temp_file);
      foreach ($pids as $pid) {
        $operations[] = array('_islandora_bibliography_batch_combine_mods', array($temp_file, $pid));
      }
      $operations[] = array('_islandora_bibliography_convert_mods_to_ris', array($temp_file));
      $operations[] = array('_islandora_bibliography_redirect', array($_GET['q']));
      break;
    case 'RTF':
      foreach ($pids as $pid) {
        $operations[] = array('_islandora_bibliography_batch_convert_mods_to_json', array($pid));
      }
      $operations[] = array('_islandora_bibliography_convert_json_to_rtf', array($style));
      $operations[] = array('_islandora_bibliography_redirect', array($_GET['q']));
      break;
    case 'PDF':
      foreach ($pids as $pid) {
        $operations[] = array('_islandora_bibliography_batch_convert_mods_to_json', array($pid));
      }
      $operations[] = array('_islandora_bibliography_convert_json_to_rtf', array($style));
      $operations[] = array('_islandora_bibliography_convert_rtf_to_pdf', array());
      $operations[] = array('_islandora_bibliography_redirect', array($_GET['q']));
      break;
  }
  if (count($operations) > 0) {
    $batch = array(
      'title' => t('Exporting File'),
      'operations' => $operations,
      'finished' => '_islandora_bibliography_export_citations'
    );
    batch_set($batch);
  }
}*/