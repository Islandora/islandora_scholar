<?php

/**
 * @file
 * Provides export to file functionality for a collection of citations.
 */

class CitationExporter {

  /**
   * Gets the storage location of all the export information.
   */
  static protected function &GetExportTable() {
    if (!isset($_SESSION[CITATION_EXPORTER_INDEX_SESSION])) {
      $_SESSION[CITATION_EXPORTER_INDEX_SESSION] = array();
    }
    return $_SESSION[CITATION_EXPORTER_INDEX_SESSION];
  }

  /**
   * Clears the storage location for the export information.
   */
  static protected function ClearExportTable() {
    unset($_SESSION[CITATION_EXPORTER_INDEX_SESSION]);
  }

  /**
   * Prepares a file accessible by the public for download.
   *
   * @param string $filename
   *   The name of the file to download.
   * @param string $mime_type
   *   The mime type of the file to download.
   * @param string $download_filename
   *   The name of the file the client will see when they download it.
   * @param string $redirect
   *   The internal path to redirect to.
   */
  static public function PrepareToExport($filename, $mime_type, $download_filename, $redirect) {
    $table = &self::GetExportTable();
    $table['filename'] = $filename;
    $table['mime_type'] = $mime_type;
    $table['download_filename'] = $download_filename;
    $table['redirect'] = $redirect;
  }

  /**
   * Checks to see if a file is waiting for download.
   */
  static public function ReadyToExport() {
    $table = &self::GetExportTable();
    return empty($table) === FALSE;
  }

  /**
   * Downloads a file to the clients machine.
   *
   * @global string $base_url
   *   The base url for the site.
   */
  static public function Export() {
    if (self::ReadyToExport()) {
      $table = &self::GetExportTable();
      $filename = $table['filename'];
      $filepath = file_unmanaged_move($filename, 'public://', FILE_EXISTS_REPLACE);
      $mime_type = $table['mime_type'];
      $download_filename = $table['download_filename'];
      $redirect = $table['redirect'];
      self::ClearExportTable();
      if (file_exists($filepath)) {
        $file_size = filesize($filepath);
        header("Content-type: $mime_type");
        header("Content-length: $file_size");
        header("Content-Disposition: attachment; filename=\"$download_filename\"");
        header("Cache-control: protected");
        $curl_handle = curl_init();
        if ($curl_handle !== FALSE) {
          $url = file_create_url($filepath);
          curl_setopt($curl_handle, CURLOPT_SSL_VERIFYPEER, FALSE);
          curl_setopt($curl_handle, CURLOPT_SSL_VERIFYHOST, FALSE);
          // Fail on errors.
          curl_setopt($curl_handle, CURLOPT_FAILONERROR, 1);
          // Allow redirects.
          curl_setopt($curl_handle, CURLOPT_FOLLOWLOCATION, 1);
          curl_setopt($curl_handle, CURLOPT_USERAGENT, "Mozilla/4.0 pp(compatible; MSIE 5.01; Windows NT 5.0)");
          // Return into a variable.
          curl_setopt($curl_handle, CURLOPT_RETURNTRANSFER, 0);
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
  }

}
