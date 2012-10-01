<?php

module_load_include('inc', 'islandora_scholar_importer', 'islandora_scholar_importer');

/**
 * RIS citation importer.
 * @author adam
 */
class EndNoteXMLImporter extends IslandoraScholarBatchImporter {
  protected $item_class = 'EndNoteXMLImportObject';

  public function get_number() {
    $endnote = new DOMDocument();
    $endnote->load($this->file->filepath);
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
          '#type' => 'file',
          '#title' => t('File of EndNote XML records to import'),
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
    if (empty($form_state['values']['file'])) {
      $file = file_save_upload('file');
      if ($file) {
        $form_state['values']['file'] = $file;
      }
    }

    return !empty($form_state['values']['file']);
  }

  public static function get_batch_info(&$form_state) {
     $file = $form_state['values']['file'];
     return $file;
  }
}

/**
 * EndNote XML import object.
 *
 * Actually does the heavy-lifting during the import.
 * @author adam
 */
class EndNoteXMLImportObject extends IslandoraScholarImportObject {
  protected $mods;

  public static function get_one(&$file) {
    $record = '';

    $endnote = new DOMDocument();
    $endnote->load($file->filepath);
    $xpath = new DOMXPath($endnote);
    $results = $xpath->query('/xml/records/record');
    $documents = array();
    if ($results->length >= 1) {
      $child = $results->item(0); // Get Record
      $record = '<xml><records>' . $endnote->saveXML($child) . '</records></xml>';
      $child->parentNode->removeChild($child); // Remove Record
      $endnote->save($file->filepath);
    }

    return (empty($record) ?
      FALSE:
      new self($record, $parent_pid)); //XXX:  Be careful with "self"?  PHP is odd.
  }

  public function get_mods() {
    if ($this->mods === NULL) {
      $enxml_file = file_create_filename('temp_en.xml', file_directory_path());
      file_put_contents($enxml_file, $this->source);
      $mods_file = file_create_filename('temp_mods.xml', file_directory_path());

      module_load_include('inc', 'bibutils', 'Bibutils');
      Bibutils::Convert($enxml_file, 'EndNoteXML', $mods_file, 'MODS');

      $this->mods = file_get_contents($mods_file);

      file_delete($enxml_file);
      file_delete($mods_file);
    }

    return $this->mods;
  }
}
