<?php

/**
 * @file
 * Defines the PMImporter.
 */

class PMImporter extends ScholarBatchImporter {
  protected $itemClass = 'PMIDImportObject';

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
    return array(
      'fs' => array(
        '#type' => 'fieldset',
        '#title' => t('Pubmed Batch Importer'),
        '#description' => t("Give either a list of Pubmed IDs in a file, or enter the list in the textarea. In either case, the items in the list should be separated by either whitespace or commas (or some combination thereof)."),
        'file' => array(
          '#type' => 'managed_file',
          '#title' => t('File of Pubmed IDs'),
          '#description' => t('A file listing Pubmed IDs (".txt" extension).'),
          '#upload_validators' => array(
            'file_validate_extensions' => array('txt'),
          ),
        ),
        'textarea' => array(
          '#type' => 'textarea',
          '#title' => t('Pubmed IDs'),
          '#description' => t('A list Pubmed IDs.'),
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

  /**
   * Determine if we're ready to run our batch process.
   *
   * @see IslandoraBatchImporter::readyForBatch()
   */
  public static function readyForBatch(array &$form_state) {
    $text = trim($form_state['values']['textarea']);
    $file = file_load($form_state['values']['file']);
    if ($file) {
      $text .= ' ' . trim(file_get_contents($file->uri));
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
 * Pubmed ID import object.
 *
 * Actually does the heavy-lifting during the import.
 * @author adam
 */
class PMIDImportObject extends IslandoraImportObject {
  protected $mods;

  /**
   * Get an item from the source.
   *
   * @see IslandoraImportObject::getOne()
   */
  public static function getOne(&$ids) {
    $id = array_shift($ids);

    $pubmed_url = 'http://www.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi';
    $url = url($pubmed_url, array(
             'query' => array(
               'db' => 'pubmed',
               'retmode' => 'xml',
               'id' => $id,
             )));
    $response = drupal_http_request($url);
    if ($response->data) {
      $pubmed_xml = simplexml_load_string($response->data);
      if ($id == self::getPMID($pubmed_xml)) {
        return new self($response->data);
      }
      else {
        return FALSE;
      }
    }
  }

  /**
   * Get the PMID from the given PubMed Document.
   */
  protected static function getPMID(SimpleXMLElement $xml) {
    $pmid = $xml->xpath('//PubmedArticle/MedlineCitation/PMID');
    return (string) array_shift($pmid);
  }

  /**
   * Normalize some fields.
   */
  protected static function correctTransform(DOMDocument $doc) {
    $xpath = new DOMXPath($doc);
    $xpath->registerNamespace('mods', 'http://www.loc.gov/mods/v3');

    // Correct dateIssued/dateCaptured
    $results = $xpath->query('//mods:dateIssued | //mods:dateCaptured');
    $count = $results->length;
    for ($i = 0; $i < $count; $i++) {
      $date = $results->item($i);
      self::normalizeDate($date, $xpath);
    }

    // Add the record creation date.
    $results = $xpath->query('//mods:recordInfo/mods:recordCreationDate');
    $count = $results->length;
    for ($i = 0; $i < $count; $i++) {
      $date = $results->item($i);
      $date->nodeValue = gmdate("Y-m-d\TH:i:s\Z");
    }

    return $xpath->document;
  }

  /**
   * Normalizes dates used in PubMed, only supports PubDate and ArticleDate.
   *
   * Torn from CoH.
   * Note: Can't normalize <MedlineDate> values.
   *
   * @param DOMElement $date
   *   The element containing the PubMed date.
   * @param DOMXPath $xpath
   *   The xpath used to fetch the date.
   */
  protected static function normalizeDate(DOMElement $date, DOMXPath $xpath) {
    $months = array(
      'Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct',
      'Nov', 'Dec');
    $iso = $xpath->evaluate('count(mods:MedlineDate) = 0', $date);
    if ($iso) {
      $date->setAttribute('encoding', 'iso8601');
      $y = $xpath->evaluate('normalize-space(mods:Year/text())', $date);
      $m = $xpath->evaluate('normalize-space(mods:Month/text())', $date);
      $d = $xpath->evaluate('normalize-space(mods:Day/text())', $date);
    }
    else {
      $string = $xpath->evaluate('normalize-space(mods:MedlineDate/text())', $date);
      if (preg_match('/^[0-9]{4}-[0-9]{2}-[0-9]{2}$/', $string)) {
        list($y, $m, $d) = explode('-', $string);
      }
      elseif (preg_match('/^[0-9]{4}-[0-9]{2}$/', $string)) {
        list($y, $m) = explode('-', $string);
      }
      elseif (preg_match('/^[0-9]{4}$/', $string)) {
        $y = $string;
      }
      elseif (preg_match('/^[0-9]{4}-[0-9]{4}$/', $string)) {
        $y = substr($string, 0, 4);
      }
      elseif (preg_match('/^[0-9]{4} [A-Za-z]{3}-[A-Za-z]{3}$/', $string)) {
        $y = substr($string, 0, 4);
        $m = substr($string, 5, 3);
      }
      elseif (preg_match('/^[0-9]{4} [A-Za-z]{3}$/', $string)) {
        $y = substr($string, 0, 4);
        $m = substr($string, 5, 3);
      }
      elseif (preg_match('/^[0-9]{4}-[A-Za-z]{3}$/', $string)) {
        $y = substr($string, 0, 4);
        $m = substr($string, 5, 3);
      }
    }
    $key = array_search(strtolower($m), array_map('strtolower', $months));
    if ($key !== FALSE) {
      $m = $key + 1;
    }
    $values = array(&$y, &$m, &$d);
    foreach ($values as &$val) {
      // Pad mth and day with 0.
      $val = (strlen($val) < 2 && $val) ? str_pad($val, 2, "0", STR_PAD_LEFT) : $val;
    }
    if ($y) {
      $string = $y;
      if ($m) {
        $string .= '-' . $m;
        if ($d) {
          $string .= '-' . $d;
        }
      }
    }
    $date->nodeValue = $string;
  }

  /**
   * Generates a MODS document repersenting the imported data.
   *
   * @see IslandoraImportObject::getMODS()
   */
  public function getMODS() {
    if ($this->mods === NULL) {
      $xsl = new DOMDocument();
      $xsl->load(drupal_get_path('module', 'pmid_importer') . '/xsl/pubmed_to_mods.xslt');
      $pm_xml = new DOMDocument();
      $pm_xml->loadXML($this->source);

      $xslt = new XSLTProcessor();
      $xslt->importStylesheet($xsl);

      $pm_xml = $xslt->transformToDoc($pm_xml->documentElement);

      $pm_xml = self::correctTransform($pm_xml);

      $this->mods = $pm_xml->saveXML();
    }

    return $this->mods;
  }
}
