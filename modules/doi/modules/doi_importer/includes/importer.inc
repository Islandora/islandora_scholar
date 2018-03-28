<?php

/**
 * @file
 * Defines a class for importing DOI citations.
 */

class DOIImporter extends IslandoraBatchImporter {

  protected $itemClass = 'DOIImportObject';

  /**
   * Get the number of items to import from $this->file.
   *
   * @see IslandoraBatchImporter::getNumber()
   */
  public function getNumber() {
    // "file" is perhaps a misnomer.
    return count($this->file);
  }

  /**
   * Get the form for this importer.
   *
   * @see IslandoraBatchImporter::getForm()
   */
  public static function getForm(array &$form_state) {
    form_load_include($form_state, 'inc', 'islandora_doi', 'includes/utilities');
    $openurl_pid = variable_get('islandora_doi_openurl_pid', ISLANDORA_DOI_OPENURL_PID_DEFAULT);
    $legacy_mode = islandora_doi_use_legacy_mode();
    if ($legacy_mode && $openurl_pid == ISLANDORA_DOI_OPENURL_PID_DEFAULT) {
      drupal_set_message(t('OpenURL PID email address has not been configured, please enter a valid email address in the <a href="@doi-admin-page">Scholar DOI configuration form</a> or use the non-legacy mode.', array('@doi-admin-page' => '/admin/islandora/solution_pack_config/scholar/islandora_doi')), 'error');
    }
    else {
      return array(
        'fs' => array(
          '#type' => 'fieldset',
          '#title' => t('DOI Batch Journal Importer'),
          '#description' => t('Provide either a list of Digital Object Identifiers in a file, or enter DOIs in the textarea below. In either case, the DOIs in the list should be separated by either whitespace or commas (or some combination thereof). NOTE: Presently, only journal (articles) from CrossREF are supported.'),
          'file' => array(
            '#type' => 'managed_file',
            '#title' => t('File of DOIs'),
            '#description' => t('A file listing Digital Object Identifiers (with ".txt" extension).'),
            '#upload_validators' => array(
              'file_validate_extensions' => array('txt'),
            ),
          ),
          'textarea' => array(
            '#type' => 'textarea',
            '#title' => t('DOIs'),
            '#description' => t('Provide a list Digital Object Identifiers (DOI) - eg. 10.1016/j.jorganchem.2011.11.018, 10.1016/j.tiv.2011.10.017, 10.1111/j.1540-4560.2012.01733.x'),
          ),
          'ids' => array(
            '#type' => 'value',
            '#value' => array(),
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
   * Determine if we're ready to run our batch process.
   *
   * @see IslandoraBatchImporter::readyForBatch()
   */
  public static function readyForBatch(array &$form_state) {
    $text = trim($form_state['values']['textarea']);
    $file = file_load($form_state['values']['file']);
    if ($file) {
      $file_dois = trim(file_get_contents(drupal_realpath($file->uri)));
      if ($text) {
        $text .= ' ' . $file_dois;
      }
      else {
        $text = $file_dois;
      }
      file_delete($file);
      unset($form_state['values']['file']);
    }

    if ($text) {
      $form_state['values']['ids'] = array_unique(preg_split("/[\s,]+/", $text));
    }

    return !empty($form_state['values']['ids']);
  }

  /**
   * Get the required info to pass into the file parameter of the constructor.
   *
   * @see IslandoraBatchImporter::getBatchInfo()
   */
  public static function getBatchInfo(array &$form_state) {
    $ids = $form_state['values']['ids'];
    return $ids;
  }
}

/**
 * DOI import object.
 *
 * Actually does the heavy-lifting during the import.
 * @author adam
 */
class DOIImportObject extends IslandoraImportObject {

  /**
   * Generated MODS Document.
   *
   * @var string
   */
  protected $mods;

  /**
   * Source content type.
   *
   * @var string
   */
  protected $sourceContentType;

  /**
   * Get an item from the source.
   *
   * @see IslandoraImportObject::getOne()
   */
  public static function getOne(&$ids) {
    $id = array_shift($ids);

    module_load_include('inc', 'islandora_doi', 'includes/utilities');
    $source_array = islandora_doi_get_source($id);
    if ($source_array === FALSE) {
      return FALSE;
    }
    list($source_string, $content_type) = $source_array;
    $toreturn = new self($source_string);
    $toreturn->sourceContentType = $content_type;
    return $toreturn;
  }

  /**
   * Generates a MODS document representing the imported data.
   *
   * @see IslandoraImportObject::getMODS()
   */
  public function getMODS() {
    if ($this->mods === NULL) {
      // @todo Implement book support.
      module_load_include('inc', 'islandora_doi', 'includes/utilities');
      $content_type = $this->sourceContentType;
      // If no content type is set, we assume that the object was there before
      // the module got updated to retrieve data via content negotiation, i.e.
      // the object source contains CrossRef metadata.
      if (!isset($content_type)) {
        $content_type = 'application/vnd.crossref.unixref+xml';
      }
      $this->mods = islandora_doi_translate($this->source, $content_type)->saveXML();
    }

    return $this->mods;
  }
}
