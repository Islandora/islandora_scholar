<?php

module_load_include('inc', 'islandora_importer');

/**
 * RIS citation importer.
 * @author adam
 */
class RISImporter extends IslandoraBatchImporter {
  protected $item_class = 'RISImportObject';

  public function get_number() {
    $count = 0;
    $file = fopen($this->file->uri, 'r');
    while (($line = fgets($file)) !== false) {
      if (trim($line) == 'ER  -') {
        $count++;
      }
    }
    fclose($file);
    return $count;
  }

  public static function get_form(&$form_state) {
    return array(
      'fs' => array(
        '#type' => 'fieldset',
        '#title' => t('RIS Batch Importer'),
        'file' => array(
          '#type' => 'managed_file',
          '#title' => t('File of RIS records to import (".ris" extension).'),
          '#upload_validators' => array(
            'file_validate_extensions' => array('ris'),
          ),
        ),
        'submit' => array(
          '#type' => 'submit',
          '#value' => t('Import'),
        ),
      ),
    );
  }

  public static function ready_for_batch(&$form_state) {
    return !empty($form_state['values']['file']);
  }

  public static function get_batch_info(&$form_state) {
     $file = file_load($form_state['values']['file']);
     return $file;
  }
}

/**
 * RIS import object.
 *
 * Actually does the heavy-lifting during the import.
 * @author adam
 */
class RISImportObject extends IslandoraImportObject {
  protected $mods;

  public static function get_one(&$file) {
    $record = '';

    /**
     * Get the first Record from the RIS File.
     */
    $fp = fopen($file->uri, 'r');
    while (($line = fgets($fp)) !== false) {
      $trimed_line = trim($line);
      if (!empty($trimed_line)) { // Ignore blank lines
        $record .= $line; // Store the line in outgoing record
        if ($trimed_line == 'ER  -') {
          break;
        }
      }
    }

    /**
     * Put the remaining text into a temp file.
     */
    $temp_filename = file_create_filename("temp.ris", file_directory_temp());
    $temp_file = fopen($temp_filename, 'w');
    while (!feof($fp)) {
      fwrite($temp_file, fread($fp, 1024));
    }
    fclose($fp);
    fclose($temp_file);

    /**
     * Replace the RIS file with the shorter temp file.
     */
    file_unmanaged_move($temp_filename, $file->uri, FILE_EXISTS_REPLACE);

    $record = trim($record);
    return (empty($record) ?
      FALSE:
      new self($record)); //XXX:  Be careful with "self"?  PHP is odd.
  }

  public function get_mods() {
    if ($this->mods === NULL) {
      $ris_file = file_create_filename('temp.ris', file_directory_temp());
      file_put_contents($ris_file, $this->source);
      $mods_file = file_create_filename('temp_mods.xml', file_directory_temp());

      module_load_include('inc', 'bibutils', 'Bibutils');
      Bibutils::Convert($ris_file, 'RIS', $mods_file, 'MODS');

      $this->mods = file_get_contents($mods_file);

      file_delete($ris_file);
      file_delete($mods_file);
    }

    return $this->mods;
  }
}
