<?php

/**
 * @file
 *
 * Functions for generating a batch export of citations to a single RIS file.
 */

/**
 * Gets the operations required to batch export a collection of citations to an RIS file.
 * 
 * @param array $pids 
 *   The citation objects to convert into an RIS file.
 * 
 * @return array 
 *   The operations required to generate a RIS file from the given citations.
 */
function citation_exporter_batch_get_ris_operations(array $pids) {
  $temp_file_dir = file_directory_temp();
  $temp_mods_file = file_create_filename("mods.xml", $temp_file_dir);
  $temp_pids_file = file_create_filename("pids.txt", $temp_file_dir);
  citation_exporter_batch_store_pids_in_file($temp_pids_file, $pids);
  $total = count($pids);
  $mods = new DOMDocument();
  $mods->loadXML('<modsCollection xmlns="http://www.loc.gov/mods/v3" />');
  $mods->save($temp_mods_file);
  $operations = array();
  $operations[] = array('citation_exporter_batch_combine_mods', array($temp_pids_file, $total, $temp_mods_file));
  $operations[] = array('citation_exporter_batch_convert_mods_to_ris', array($temp_mods_file));
  $operations[] = array('citation_exporter_batch_redirect', array($_GET['q'], array($temp_mods_file, $temp_pids_file)));
  return $operations;
}

function citation_exporter_batch_combine_mods($pids_filename, $total, $mods_filename, array &$context) {
  if (empty($context['sandbox'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['total'] = $total;
  }
  list($pids, $remaining) = citation_exporter_batch_get_num_pids_from_file($pids_filename, 5);
  _citation_exporter_batch_combine_mods($mods_filename, $pids);
  $context['sandbox']['progress'] = $context['sandbox']['total'] - $remaining;
  if ($context['sandbox']['progress'] != $context['sandbox']['total']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['total'];
  }
}

function _citation_exporter_batch_combine_mods($filename, array $pids) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  $mods = new DOMDocument();
  $mods->load($filename);
  foreach ($pids as $pid) {
    $item = new Fedora_Item($pid);
    if ($item->exists() && isset($item->datastreams['MODS'])) {
      $datastream = $item->get_datastream_dissemination('MODS');
      $datastream = trim($datastream);
      if (!empty($mods)) {
        $doc = new DOMDocument();
        $doc->loadXML($datastream);
        $node = $mods->importNode($doc->documentElement, TRUE);
        $mods->documentElement->appendChild($node);
      }
    }
  }
  $mods->save($filename);
}

/**
 * Converts the given MODS file to RIS using bibutils
 * 
 * @param string $filename
 *   The name of the MODS file to convert to RIS.
 * @param array $context
 *   The batch processes context.
 */
function citation_exporter_batch_convert_mods_to_ris($mods_filename, array &$context) {
  module_load_include('inc', 'bibutils', 'Bibutils');
  $temp_file_dir = file_directory_temp();
  $ris_filename = file_create_filename("ris_export.ris", $temp_file_dir);
  if (Bibutils::Convert($mods_filename, 'MODS', $ris_filename, 'RIS')) {
    $context['results']['filename'] = $ris_filename;
    $context['results']['mime_type'] = 'application/x-Research-Info-Systems';
    $context['results']['download_filename'] = 'export.ris';
  }
}