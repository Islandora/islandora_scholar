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

