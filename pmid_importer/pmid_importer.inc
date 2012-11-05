<?php

module_load_include('inc', 'islandora_scholar_importer');

/**
 * PMID citation importer.
 * @author adam
 */
class PMImporter extends IslandoraScholarBatchImporter {
  protected $item_class = 'PMIDImportObject';

  public function get_number() {
    // "file" is perhaps a misnomer.
    return count($this->file);
  }

  public static function get_form(&$form_state) {
    return array(
      'fs' => array(
        '#type' => 'fieldset',
        '#title' => t('Pubmed Batch Importer'),
        '#description' => t('Give either a list of Pubmed IDs in a file, or ' .
          'enter the list in the textarea.  In either case, the items in the ' .
          'list should be separated by either whitespace or commas (or some ' .
          'combination thereof).'),
        'file' => array(
          '#type' => 'file',
          '#title' => t('File of Pubmed IDs'),
          '#description' => t('A file listing Pubmed IDs.'),
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

  public static function ready_for_batch(&$form_state) {
    $text = trim($form_state['values']['textarea']);
    $file = file_save_upload('file');
    if ($file) {
      $text .= ' ' . trim(file_get_contents($file->filepath));
      file_delete($file->filepath);
    }

    if ($text) {
      $form_state['values']['ids'] = array_unique(preg_split("/[\s,]+/", $text));
    }

    return !empty($form_state['values']['ids']);
  }

  public static function get_batch_info(&$form_state) {
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
class PMIDImportObject extends IslandoraScholarImportObject {
  protected $mods;

  public static function get_one(&$ids) {
    $id = array_shift($ids);

    $pubmed_url = 'http://www.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi';
    $url = url($pubmed_url, array('query' => array(
      'db' => 'pubmed',
      'retmode' => 'xml',
      'id' => $id,
    )));
    $response = drupal_http_request($url);
    if ($response->data) {
      $pubmed_xml = simplexml_load_string($response->data);
      if ($id == self::get_pmid($pubmed_xml)) {
        return new self($response->data);
      }
      else {
        return FALSE;
      }
    }
  }

  protected static function get_pmid(SimpleXMLElement $xml) {
    return (string) array_shift($xml->xpath('//PubmedArticle/MedlineCitation/PMID'));
  }

  /**
   * Normalize some fields.
   */
  protected static function correct_transform(DOMDocument $doc) {
    $xpath = new DOMXPath($doc);
    $xpath->registerNamespace('mods', 'http://www.loc.gov/mods/v3');

    // Correct dateIssued/dateCaptured
    $results = $xpath->query('//mods:dateIssued | //mods:dateCaptured');
    $count = $results->length;
    for ($i = 0; $i < $count; $i++) {
      $date = $results->item($i);
      self::normalize_date($date, $xpath);
    }

    // Add the record creation date.
    $results = $xpath->query('//mods:recordInfo/mods:recordCreationDate');
    $count = $results->length;
    for ($i = 0; $i < $count; $i++) {
      $date = $results->item($i);
      $date_time = new DateTime();
      $date->nodeValue = $date_time->format(DateTime::ISO8601) . 'Z';
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
   */
  protected static function normalize_date(DOMElement $date, DOMXPath $xpath) {
    $months = array('Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec');
    $iso = $xpath->evaluate('count(mods:MedlineDate) = 0', $date);
    if ($iso) {
      $date->setAttribute('encoding', 'iso8601');
      $y = $xpath->evaluate('normalize-space(mods:Year/text())', $date);
      $m = $xpath->evaluate('normalize-space(mods:Month/text())', $date);
      $d = $xpath->evaluate('normalize-space(mods:Day/text())', $date);
    }
    else {
      $string = $xpath->evaluate('normalize-space(mods:MedlineDate/text())', $date);
      if(preg_match('/^[0-9]{4}-[0-9]{2}-[0-9]{2}$/', $string)){
          list($y, $m, $d) = explode('-', $string);
      }
      elseif(preg_match('/^[0-9]{4}-[0-9]{2}$/', $string)){
          list($y, $m) = explode('-', $string);
      }
      elseif(preg_match('/^[0-9]{4}$/', $string)){
          $y = $string;
      }
      elseif(preg_match('/^[0-9]{4}-[0-9]{4}$/', $string)){
          $y = substr($string, 0, 4);
      }
      elseif(preg_match('/^[0-9]{4} [A-Za-z]{3}-[A-Za-z]{3}$/', $string)){
          $y = substr($string, 0, 4);
          $m = substr($string, 5, 3);
      }
      elseif(preg_match('/^[0-9]{4} [A-Za-z]{3}$/', $string)){
          $y = substr($string, 0, 4);
          $m = substr($string, 5, 3);
      }
      elseif(preg_match('/^[0-9]{4}-[A-Za-z]{3}$/', $string)){
          $y = substr($string, 0, 4);
          $m = substr($string, 5, 3);
      }
    }
    $key = array_search(strtolower($m), array_map('strtolower',$months));
    if($key !== FALSE) {
      $m = $key + 1;
    }
    $values = array(&$y, &$m, &$d);
    foreach ($values as &$val) { // pad mth and day with 0
      $val = (strlen($val) < 2 && $val) ? str_pad($val,2,"0", STR_PAD_LEFT) : $val;
    }
    if($y) {
      $string = $y;
      if($m) {
        $string .= '-' . $m;
        if($d) {
          $string .= '-' . $d;
        }
      }
    }
    $date->nodeValue = $string;
  }

  /**
   * Implementation of abstract method.
   */
  public function get_mods() {
    if ($this->mods === NULL) {
      $xsl = new DOMDocument();
      $xsl->load(drupal_get_path('module', 'pmid_importer') . '/xsl/pubmed_to_mods.xslt');
      $pm_xml = DOMDocument::loadXML($this->source);

      $xslt = new XSLTProcessor();
      $xslt->importStylesheet($xsl);

      $pm_xml = $xslt->transformToDoc($pm_xml->documentElement); 
      
      $pm_xml = self::correct_transform($pm_xml);

      $this->mods = $pm_xml->saveXML();
    }

    return $this->mods;
  }
}
