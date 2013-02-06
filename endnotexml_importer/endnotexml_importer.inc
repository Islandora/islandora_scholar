<?php

module_load_include('inc', 'islandora_importer');

/**
 * RIS citation importer.
 * @author adam
 */
class EndNoteXMLImporter extends IslandoraBatchImporter {
  protected $item_class = 'EndNoteXMLImportObject';

  public function get_number() {
    $endnote = new DOMDocument();
    $endnote->load($this->file->uri);
    $xpath = new DOMXPath($endnote);
    $results = $xpath->query('/xml/records/record');
    return $results?
      $results->length:
      0;
  }

  public static function get_form(&$form_state) {
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

  public static function ready_for_batch(&$form_state) {
    return !empty($form_state['values']['file']);
  }

  public static function get_batch_info(&$form_state) {
     $file = file_load($form_state['values']['file']);
     return $file;
  }
}

/**
 * EndNote XML import object.
 *
 * Actually does the heavy-lifting during the import.
 * @author adam
 */
class EndNoteXMLImportObject extends IslandoraImportObject {
  protected $mods;

  public static function get_one(&$file) {
    $record = '';

    $endnote = new DOMDocument();
    $endnote->load($file->uri);
    $xpath = new DOMXPath($endnote);
    $results = $xpath->query('/xml/records/record');
    $documents = array();
    if ($results->length >= 1) {
      $child = $results->item(0); // Get Record
      $record = '<xml><records>' . $endnote->saveXML($child) . '</records></xml>';
      $child->parentNode->removeChild($child); // Remove Record
      $endnote->save($file->uri);
      file_save($file);
    }

    return (empty($record) ?
      FALSE:
      new self($record)); //XXX:  Be careful with "self"?  PHP is odd.
  }

  public function get_mods() {
    if ($this->mods === NULL) {
      $enxml_file = file_save_data($this->source, "temp://temp_en.xml");
      $mods_file = drupal_tempnam('temp://', 'temp_mods.xml');

      module_load_include('inc', 'bibutils', 'Bibutils');
      Bibutils::Convert(
        drupal_realpath($enxml_file->uri),
        'EndNoteXML',
        drupal_realpath($mods_file),
        'MODS'
      );

      $this->mods = file_get_contents($mods_file);

      file_delete($enxml_file);
      unlink($mods_file);
    }

    return $this->mods;
  }
}
