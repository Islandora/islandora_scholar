<?php

/**
 * @file
 * Defines RISImporter.
 */

class RISImporter extends ScholarBatchImporter {
  protected $itemClass = 'RISImportObject';

  /**
   * Get the number of items to import from $this->file.
   *
   * @see IslandoraBatchImporter::getNumber()
   */
  public function getNumber() {
    $count = 0;
    $file = fopen($this->file->uri, 'r');
    while (($line = fgets($file)) !== FALSE) {
      if (trim($line) == 'ER  -') {
        $count++;
      }
    }
    fclose($file);
    return $count;
  }

  /**
   * Get the form for this importer.
   *
   * @see IslandoraBatchImporter::getForm()
   */
  public static function getForm(array &$form_state) {
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

  /**
   * Determine if we're ready to run our batch process.
   *
   * @see IslandoraBatchImporter::readyForBatch()
   */
  public static function readyForBatch(array &$form_state) {
    return !empty($form_state['values']['file']);
  }

  /**
   * Get the required info to pass into the file parameter of the constructor.
   *
   * @see IslandoraBatchImporter::getBatchInfo()
   */
  public static function getBatchInfo(array &$form_state) {
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

  /**
   * Get an item from the source.
   *
   * @see IslandoraImportObject::getOne()
   */
  public static function getOne(&$file) {
    $record = '';

    // Get the first Record from the RIS File.
    $fp = fopen($file->uri, 'r');
    while (($line = fgets($fp)) !== FALSE) {
      $trimed_line = trim($line);
      // Ignore blank lines.
      if (!empty($trimed_line)) {
        // Store the line in outgoing record.
        $record .= $line;
        if ($trimed_line == 'ER  -') {
          break;
        }
      }
    }

    // Put the remaining text into a temp file.
    $temp_filename = file_create_filename("temp.ris", file_directory_temp());
    $temp_file = fopen($temp_filename, 'w');
    while (!feof($fp)) {
      fwrite($temp_file, fread($fp, 1024));
    }
    fclose($fp);
    fclose($temp_file);

    // Replace the RIS file with the shorter temp file.
    file_unmanaged_move($temp_filename, $file->uri, FILE_EXISTS_REPLACE);

    $record = trim($record);
    return (empty($record) ? FALSE : new self($record));
  }

  /**
   * Generates a MODS document repersenting the imported data.
   *
   * @see IslandoraImportObject::getMODS()
   */
  public function getMODS() {
    if ($this->mods === NULL) {
      $ris_file = file_create_filename('temp.ris', file_directory_temp());
      file_put_contents($ris_file, $this->source);
      $mods_file = file_create_filename('temp_mods.xml', file_directory_temp());

      module_load_include('inc', 'bibutils', 'includes/utilities');
      Bibutils::Convert($ris_file, 'RIS', $mods_file, 'MODS');

      $this->mods = file_get_contents($mods_file);

      drupal_unlink($ris_file);
      drupal_unlink($mods_file);
    }

    return $this->mods;
  }
}
