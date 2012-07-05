<?php

module_load_include('inc', 'islandora_scholar_importer', 'islandora_scholar_importer');

/**
 * RIS citation importer.
 * @author adam
 */
class RISImporter extends IslandoraScholarBatchImporter {
  protected $item_class = 'RISImportObject';

  public function get_number() {
    $count = 0;
    $file = fopen($this->file->filepath, 'r');
    while (($line = fgets($file)) !== false) {
      if (trim($line) == 'ER  -') {
        $count++;
      }
    }
    fclose($file);
    return $count;
  }

  public static function ready_for_batch(&$form_state) {
    if (empty($form_state['values']['file'])) {
      $file = file_save_upload('file');
      if ($file) {
        $form_state['values']['file'] = $file;
      }
    }

    dsm($form_state, 'ready');

    return !empty($form_state['values']['file']);
  }

  public static function get_batch_info(&$form_state) {
     $file = $form_state['values']['file'];
     return $file;
  }

  public static function get_form(&$form_state) {
    return array(
      'fs' => array(
        '#type' => 'fieldset',
        '#title' => t('RIS Batch Importer'),
        'file' => array(
          '#type' => 'file',
          '#title' => t('File of RIS records to import'),
        ),
        'submit' => array(
          '#type' => 'submit',
          '#value' => t('Import'),
        ),
      ),
      '#attributes' => array(
        'enctype' => 'multipart/form-data',
      ),
    );
  }
}

/**
 * RIS import object.
 *
 * Actually does the heavy-lifting during the import.
 * @author adam
 */
class RISImportObject extends IslandoraScholarImportObject {
  public static function get_one(&$file) {
    $record = '';

    /**
     * Get the first Record from the RIS File.
     */
    $fp = fopen($file->filepath, 'r');
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
    file_move($temp_filename, $file->filepath, FILE_EXISTS_REPLACE);

    $record = trim($record);
    return (empty($record) ?
      FALSE:
      new self($record, $parent_pid)); //XXX:  Be careful with "self"?  PHP is odd.
  }

  protected $mods;

  protected function __construct($source) {
    $ris_file = file_create_filename('temp.ris', file_directory_path());
    file_put_contents($ris_file, $source);
    $mods_file = file_create_filename('temp_mods.xml', file_directory_path());

    module_load_include('inc', 'bibutils', 'Bibutils');
    Bibutils::Convert($ris_file, 'RIS', $mods_file, 'MODS');

    $this->mods = file_get_contents($mods_file);

    //Extract title...
    $this->title = '';

    file_delete($ris_file);
    file_delete($mods_file);
  }

  public function get_title() {
    if (empty($this->title)) {
      $mods_doc = DOMDocument::loadXML($this->mods);
      $mods_xpath = new DOMXPath($mods_doc);
      $mods_xpath->registerNamespace('m', 'http://www.loc.gov/mods/v3');

      $this->title = $mods_xpath->evaluate('string(//m:mods/m:titleInfo/m:title/text())');
    }

    return $this->title;
  }

  protected function get_mods() {
    return $this->mods;
  }
}
