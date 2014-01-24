<?php

/**
 * @file
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

  $queries = $_GET;
  unset($queries['q']);
  $operations[] = array('citation_exporter_batch_append_citations_to_text_file', array(
                    $temp_pids_file, $total, $temp_text_file, $style));
  $operations[] = array('citation_exporter_batch_convert_txt_to_rtf', array($temp_text_file));
  $operations[] = array('citation_exporter_batch_redirect', array(
                    url(request_path(), array('query' => $queries)),
                    array($temp_text_file, $temp_pids_file)));
  return $operations;
}

/**
 * Add the header to the given text file.
 */
function citation_exporter_batch_add_header_to_text_file($file_name, $header, array &$context) {
  $file = fopen($file_name, 'wb');
  fwrite($file, "<h2>$header</h2><br/>");
  fclose($file);
}

/**
 * Append citations to the given textfile.
 *
 * @param string $pids_filename
 *   File containing PID.
 * @param string $text_filename
 *   File citations will be appended to.
 * @param string $style
 *   The CSL style to use when rendering the citations.
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
 * @param array $pids
 *   The objects from whom the metadata for the citation is taken.
 * @param string $filename
 *   The name of the text file to append the citation to.
 * @param string $style
 *   The name of the style used to render the citation.
 */
function _citation_exporter_batch_append_citations_to_text_file(array $pids, $filename, $style) {
  module_load_include('inc', 'citeproc', 'includes/converter');
  module_load_include('php', 'CiteProc', 'lib/citeproc-php/CiteProc');
  module_load_include('inc', 'csl', 'includes/csl');
  $style = CSL::Get($style);
  $citeproc = new citeproc($style->saveXML());
  $file = fopen($filename, 'ab');
  foreach ($pids as $pid) {
    $object = islandora_object_load($pid);
    if (isset($object) && isset($object['MODS']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $object['MODS'])) {
      $mods = trim($object['MODS']->content);
      if (!empty($mods)) {
        $json = _citeproc_array_to_object(convert_mods_to_citeproc_jsons($mods));
        $text = $citeproc->render($json, 'bibliography');
        if ($file) {
          // I know the <br/> look out of place but its cause this is an
          // intermediate document and the rtf class will replace them with
          // newlines.
          fwrite($file, $text . "<br/><br/>");
        }
      }
    }
  }
  fclose($file);
}

/**
 * Converts TXT file to an RTF File.
 *
 * @param string $filename
 *   The file to convert.
 * @param array $context
 *   The batch context.
 */
function citation_exporter_batch_convert_txt_to_rtf($filename, array &$context) {
  module_load_include('inc', 'citation_exporter', 'includes/rtf');
  $text = '';
  $file = fopen($filename, 'rb');
  while (!feof($file)) {
    $text .= fread($file, 4096);
  }
  fclose($file);
  $rtf = new RTF();
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
