<?php

/**
 * @file
 * Defines the EndNoteXMLImporter.
 */

class EndNoteXMLImporter extends ScholarBatchImporter {
  protected $itemClass = 'EndNoteXMLImportObject';

  /**
   * Get the number of items to import from $this->file.
   *
   * @see IslandoraBatchImporter::getNumber()
   */
  public function getNumber() {
    $endnote = new DOMDocument();
    $endnote->load($this->file->uri);
    $xpath = new DOMXPath($endnote);
    $results = $xpath->query('/xml/records/record');
    return $results ? $results->length : 0;
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
        '#title' => t('EndNote XML Batch Importer'),
        'file' => array(
          '#type' => 'managed_file',
          '#title' => t('File of EndNote XML records to import'),
          '#upload_validators' => array(
            'file_validate_extensions' => array('xml'),
          ),
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
 * EndNote XML import object.
 *
 * Actually does the Heavy-lifting during the import.
 * @author adam
 */
class EndNoteXMLImportObject extends IslandoraImportObject {
  protected $mods;

  /**
   * Get an item from the source.
   *
   * @see IslandoraImportObject::getOne()
   */
  public static function getOne(&$file) {
    $record = '';

    $endnote = new DOMDocument();
    $endnote->load($file->uri);
    $xpath = new DOMXPath($endnote);
    $results = $xpath->query('/xml/records/record');
    $documents = array();
    if ($results->length >= 1) {
      // Get Record.
      $child = $results->item(0);
      $record = '<xml><records>' . $endnote->saveXML($child) . '</records></xml>';
      // Remove Record.
      $child->parentNode->removeChild($child);
      $endnote->save($file->uri);
      file_save($file);
    }
    return (empty($record) ? FALSE : new self($record));
  }

  /**
   * Generates a MODS document repersenting the imported data.
   *
   * @see IslandoraImportObject::getMODS()
   */
  public function getMODS() {
    if ($this->mods === NULL) {
      $enxml_file = file_save_data($this->source, "temporary://temp_en.xml");
      $mods_file = drupal_tempnam('temporary://', 'temp_mods.xml');

      module_load_include('inc', 'bibutils', 'includes/utilities');
      Bibutils::Convert(
        drupal_realpath($enxml_file->uri),
        'EndNoteXML',
        drupal_realpath($mods_file),
        'MODS'
      );

      $this->mods = file_get_contents($mods_file);

      file_delete($enxml_file);
      drupal_unlink($mods_file);
    }

    return $this->mods;
  }
}
