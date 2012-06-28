<?php

/**
 * @file
 *
 * Functions for generating a batch export of citations to a single RTF file.
 */

/**
 * Get the batch operations required to generate an RTF file.
 * 
 * @param array $pids
 *   The citation to export to the RTF file.
 * @param sting $style 
 *   The style to apply to the citation before exporting to the RTF.
 * 
 * @return array
 *   The batch operations to create the RTF file.
 */
function citation_exporter_batch_get_rtf_operations(array $pids, $style) {
  $temp_file_dir = file_directory_temp();
  $temp_text_file = file_create_filename("rtf_export.txt", $temp_file_dir);
  $temp_pids_file = file_create_filename("pids.txt", $temp_file_dir);
  citation_exporter_batch_store_pids_in_file($temp_pids_file, $pids);
  $total = count($pids);
  $operations = array();
  $header_text = variable_get('fedora_repository_title', 'Fedora Repository');
  $operations[] = array('citation_exporter_batch_add_header_to_text_file', array($temp_text_file, $header_text));
  $operations[] = array('citation_exporter_batch_append_citations_to_text_file', array($temp_pids_file, $total, $temp_text_file, $style));
  $operations[] = array('citation_exporter_batch_convert_txt_to_rtf', array($temp_text_file));
  $operations[] = array('citation_exporter_batch_redirect', array($_GET['q'], array($temp_text_file, $temp_pids_file)));
  return $operations;
}

function citation_exporter_batch_add_header_to_text_file($file_name, $header, array &$context) {
  $file = fopen($file_name, 'wb');
  fwrite($file, "<h2>$header</h2><br/>");
  fclose($file);
}

/**
 *
 * 
 * @param string $pids_filename
 * @param string $text_filename
 * @param string $style 
 */
function citation_exporter_batch_append_citations_to_text_file($pids_filename, $total, $text_filename, $style, array &$context) {
  if (empty($context['sandbox'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['total'] = $total;
  }
  list($pids, $remaining) = citation_exporter_batch_get_num_pids_from_file($pids_filename, 5);
  _citation_exporter_batch_append_citations_to_text_file($pids, $text_filename, $style);
  $context['sandbox']['progress'] = $context['sandbox']['total'] - $remaining;
  if ($context['sandbox']['progress'] != $context['sandbox']['total']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['total'];
  }
}

/**
 * Appends a citation generated from citeproc-php to the provided text file.
 * 
 * @param string $filename
 *   The name of the text file to append the citation to.
 * @param string $style
 *   The name of the style used to render the citation.
 * @param type $pid
 *   The object from whom the metadata for the citation is taken.
 * @param array $context 
 *   The batch context.
 */
function _citation_exporter_batch_append_citations_to_text_file(array $pids, $filename, $style) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  module_load_include('php', 'citeproc', 'generators/converter');
  module_load_include('php', 'CiteProc', 'lib/citeproc-php/CiteProc');
  module_load_include('inc', 'csl', 'CSL');
  $style = CSL::Get($style);
  $citeproc = new citeproc($style->saveXML());
  $file = fopen($filename, 'ab');
  foreach ($pids as $pid) {
    $item = new Fedora_Item($pid);
    if ($item->exists() && isset($item->datastreams['MODS'])) {
      $mods = $item->get_datastream_dissemination('MODS');
      $mods = trim($mods);
      if (!empty($mods)) {
        $json = _citeproc_array_to_object(convert_mods_to_citeproc_jsons($mods));
        $text = $citeproc->render($json, 'bibliography');
        if ($file) {
          fwrite($file, $text . "<br/><br/>"); // I know the <br/> look out of place but its cause this is an intermediate document and the rtf class will replace them with newlines.
        }
      }
    }
  }
  fclose($file);
}

/**
 * Converts TXT file to an RTF File.
 * 
 * @param string $style
 *   The CSL style to format the JSON citations with.
 * @param array $context 
 *   The batch context.
 */
function citation_exporter_batch_convert_txt_to_rtf($filename, array &$context) {
  module_load_include('php', 'citation_exporter', 'class_rtf');
  $text = '';
  $file = fopen($filename, 'rb');
  while (!feof($file)) {
    $text .= fread($file, 4096);
  }
  fclose($file);
  $data = json_decode($json);
  $rtf = new rtf();
  $rtf->addText($text);
  /* File */
  $temp_file_dir = file_directory_temp();
  $temp_file_name = file_create_filename("export.rtf", $temp_file_dir);
  $file = fopen($temp_file_name, 'wb');
  if ($file !== FALSE) {
    $document = $rtf->getDocument();
    fwrite($file, $document);
    fclose($file);
  }
  /* Batch Values */
  $context['results']['filename'] = $temp_file_name;
  $context['results']['download_filename'] = 'export.rtf';
  $context['results']['mime_type'] = 'application/rtf';
}
