<?php

module_load_include('inc', 'islandora_scholar_importer');

/**
 * Zip file importer.
 * @author adam
 */
class ZipBatchImporter extends IslandoraScholarBatchImporter {
  protected $item_class = 'ZipBatchImportObject';

  public function get_number() {
    return count($this->file['object_info']);
  }

  public static function get_form(&$form_state) {
    // Get the list of valid content models.
    $parent = islandora_object_load($form_state['storage']['parent_pid']);
    module_load_include('ingest.inc', 'islandora', 'includes/islandora');
    $parent_info = islandora_ingest_get_information($parent);

    $options = array();
    foreach ($parent_info['models'] as $pid => $info) {
      $options[$pid] = $info['name'];
    }

    return array(
      'fs' => array(
        '#type' => 'fieldset',
        '#title' => t('Zip Batch Importer'),
        '#description' => t('Select the file containing the assets and ' .
          'metadata to import.  Assets and metadata will be matched together' .
          ' based on the portion of the filename without the extension, ' .
          'so my_file.xml and my_file.pdf will be combined into a single object.'),
        'file' => array(
          '#type' => 'managed_file',
          '#title' => t('Zip file of files to import'),
          '#upload_validators' => array(
            'file_validate_extensions' => array('zip')
          ),
        ),
        'content_model' => array(
          '#type' => 'select',
          '#title' => t('Content Model'),
          '#description' => t('The content model to assign to the imported ' .
            'objects.'),
          '#options' => $options,
        ),
        'submit' => array(
          '#type' => 'submit',
          '#value' => t('Import'),
        ),
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
     $info = array(
       'file' => file_load($form_state['values']['file']),
       'content_model' => $form_state['values']['content_model'],
       'object_info' => self::get_ingest_info(file_load($form_state['values']['file'])),
       'processed_objects' => array(),
     );
     return $info;
  }

  protected static function get_ingest_info($file) {
    $zip = new ZipArchive();
    if (($error = $zip->open(drupal_realpath($file->uri))) !== TRUE) {
      dsm($error);
      return;
    }

    $count = $zip->numFiles;

    $info = array();
    for ($i = 0; $i < $count; $i++) {
      $name = $zip->getNameIndex($i);
      $path_info = pathinfo($name);

      $object_key = $path_info['dirname'] . '-_-' . $path_info['filename'];
      $datastream_key = strtolower($path_info['extension']);
      $info[$object_key][$datastream_key] = $name;
    }

    // XXX: We don't actually want to introduce any changes.
    //$zip->unchangeAll();
    $zip->close();

    return $info;
  }
}

/**
 * ZIP import object.
 *
 * Actually does the heavy-lifting during the import.
 * @author adam
 */
class ZipBatchImportObject extends IslandoraScholarImportObject {
  protected $mods;

  public static function get_one(&$info) {
    $record = array(
      'file' => $info['file'],
      'content_model' => $info['content_model'],
    );
    $key = key($info['object_info']);
    $record['object_info'] = $info['processed_objects'][$key] = $info['object_info'][$key];
    unset($info['object_info'][$key]);

    return (empty($record) ?
      FALSE:
      new self($record)); //XXX:  Be careful with "self"?  PHP is odd.
  }

  protected function get_datastreams(&$errors = NULL, &$files = NULL) {
    $to_return = parent::get_datastreams($errors, $files);

    module_load_include('inc', 'islandora', 'includes/mime.detect');
    $mime_detect = new MimeDetect();

    module_load_include('inc', 'islandora', 'includes/datastream');
    $dsids = islandora_get_dsids($this->source['content_model']);

    foreach ($this->source['object_info'] as $datastream => $name) {
      if ($datastream == 'xml') {
        // XML streams are handled via the parent implementation, so let's go
        //  to the next item.
        continue;
      }

      // Determine which stream this should be...  Uses the first matching, as
      //  as received from the DC-COMPOSITE.
      $mimetype = $mime_detect->getMimetype($name);
      
      $dsid = 'OBJ';
      foreach ($dsids as $ds => $mimes) {
        if (in_array($mimetype, $mimes)) {
          $dsid = $ds;
          break;
        }
      }

      $zip = new ZipArchive();
      $zip->open(drupal_realpath($this->source['file']->uri));
      if ($zip->extractTo(drupal_realpath('public://'), $name)) {
        $file = new stdClass();
        $file->uri = "public://$name";
        $files[] = $file;

        $to_return += array(
          $dsid => array(
            'dsid' => $dsid,
            'label' => "$dsid datastream",
            'mimetype' => $mimetype,
            'datastream_file' => "public://$name",
            'control_group' => 'M',
          ),
        );
      }
      $zip->close();
    }

    return $to_return;
  }

  /**
   * Run an XSLT, and return the results.
   *
   * @param array $info
   *   An associative array of parameters, containing:
   *   - input: The input XML in a string.
   *   - xsl: The path to an XSLT file.
   *
   * @return string
   *   The transformed XML, as a string.
   */
  protected static function run_xsl_transform($info) {
    $xsl = new DOMDocument();
    $xsl->load($info['xsl']);

    $input = new DOMDocument();
    $input->loadXML($info['input']);

    $processor = new XSLTProcessor();
    $processor->importStylesheet($xsl);

    // XXX: Suppressing warnings regarding unregistered prefixes.
    return @$processor->transformToXML($input);
  }

  public function get_mods() {
    if ($this->mods === NULL) {
      // If we have an XML stream which is:
      if (isset($this->source['object_info']['xml'])) {
        $zip = new ZipArchive();
        $zip->open(drupal_realpath($this->source['file']->uri));
        $xml = $zip->getFromName($this->source['object_info']['xml']);
        $zip->close();

        if ($xml) {
          $s_xml = new SimpleXMLElement($xml);
      //   MODS, set
          if ($s_xml->getName() == 'mods') {
            $this->mods = $xml;
          }
      //   MARCXML, transform to MODS and set
          elseif ($s_xml->getName() == 'record') {
            $this->mods = self::run_xsl_transform(array(
              'input' => $xml,
              'xsl' => drupal_get_path('module', 'zip_importer') . '/xsl/MARC21slim2MODS3-4.xsl',
            ));
          }
      //   DC, transform to MODS and set
          elseif ($s_xml->getName() == 'dc') {
            $this->mods = self::run_xsl_transform(array(
              'input' => $xml,
              'xsl' => drupal_get_path('module', 'zip_importer') . '/xsl/simpleDC2MODS.xsl',
            ));
          }
      //   Unrecognized format...
          else {
            // Do nothing?
          }
        }
      }
      
      if (empty($this->mods)) {
      // If we do not yet have any XML, create a dumb MODS with just the
      //   title, and set it...
        $title = pathinfo(reset($this->source['object_info']), PATHINFO_FILENAME);
        
        $this->mods = <<<EOXML
<mods xmlns:mods="http://www.loc.gov/mods/v3" xmlns="http://www.loc.gov/mods/v3">
  <titleInfo>
    <title>$title</title>
  </titleInfo>
</mods>
EOXML;
      }
    }

    return $this->mods;
  }

  public function get_dc() {
    if ($this->dc === NULL) {
      // If we have an XML stream which is DC, set it.
      if (isset($this->source['object_info']['xml'])) {
        $zip = new ZipArchive();
        $zip->open(drupal_realpath($this->source['file']->uri));
        $xml = $zip->getFromName($this->source['object_info']['xml']);
        $zip->close();

        if ($xml) {
          $s_xml = new SimpleXMLElement($xml);
          if ($s_xml->getName() == 'dc') {
            $this->dc = $xml;
          }
        }
      } 
      // Otherwise, call the parent implementation (transform from MODS).
      if (empty($this->dc)) {
        parent::get_dc();
      }
    }

    return $this->dc;
  }
}

