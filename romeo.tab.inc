<?php

class RomeoView {
  public static function get_tabs($pid) {
    $tabset = array();

    if (variable_get('islandora_scholar_romeo_enable', FALSE)) {
      $issn = self::get_issn($pid);
      if ($issn) {
        $tabset += array(
          'romeo_tab' => array(
            '#type' => 'tabpage',
            '#title' => t('RoMEO'),
            '#content' => self::get_romeo_markup($issn),
          ),
        );
      }
    }

    return $tabset;
  }

  protected static function get_issn($pid) {
    $cache_id = "islandora_scholar_issn_cache_$pid";
    //Try to get the issn of the object
    if ($issn = cache_get($cache_id)) {
      return $issn->data;
    }
    else {
      $issn = FALSE;

      //TODO: Try to get the ISSN from Solr
      if (!issn) {

      }

      //Try to get the ISSN from the object in Fedora Proper.
      if (!$issn) {
        module_load_include('inc', 'fedora_repository', 'api/fedora_item');
        $item = new Fedora_Item($pid);

        //Try to get the ISSN from a MODS stream.
        if (!$issn && array_key_exists('MODS', $item->datastreams)) {
          $mods_doc = DOMDocument::loadXML($item->get_datastream_dissemination('MODS'));
          $mods_xpath = new DOMXPath($mods_doc);
          $mods_xpath->registerNamespace('m', 'http://www.loc.gov/mods/v3');

          $issn_results = $mods_xpath->query('//m:identifier[@type="issn" and normalize-space(text())]');
          if (count($issn_results) > 0) {
            $issn = $issn_results->item(0)->textContent;
          }
        }
      }

      //Cache even if not successful...  It's still meaningful that we're not
      //  able to find one.
      $cache_time = variable_get('islandora_scholar_issn_cache_time', '2');
      cache_set($cache_id, $issn, 'cache', time() + ($cache_time * 86400));

      return $issn;
    }
  }

  protected static function get_romeo_markup($issn) {
    $cache_id = "islandora_scholar_romeo_cache_$issn";
    if ($romeo_cache = cache_get($cache_id)) {
      return $romeo_cache->data;
    }
    else {
      $params = array(
        'issn' => $issn,
      );
      if ($key = variable_get('islandora_scholar_romeo_key', '')) {
        $params['ak'] = $key;
      }

      $ch = curl_init(url(variable_get('islandora_scholar_romeo_url', 'http://www.sherpa.ac.uk/romeo/api29.php'), array(
        'query' => $params,
      )));
      curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
      $romeo_content = trim(curl_exec($ch));
      curl_close($ch);

      try {
        $processor = new XsltProcessor();
      } catch (Exception $e) {
        drupal_set_message(t('@error', array('@error' => $e->getMessage())), 'error');
        return;
      }

      $xsl = new DOMDocument();
      $xsl->load(drupal_get_path('module', 'islandora_scholar') . '/xsl/romeo.xsl');
      $input = new DOMDocument();
      if ($romeo_content) {
        $input->loadXML($romeo_content);
        $xsl = $processor->importStylesheet($xsl);
        $xml_output = $processor->transformToDoc($input);
        $romeo_markup = $xml_output->saveXML();
        $cache_time = variable_get('islandora_scholar_romeo_cache_time', '2');
        cache_set($cache_id, $romeo_markup, 'cache', time() + ($cache_time * 86400));
        return $romeo_markup;
      }
      else {
        drupal_set_message(t('Could not retrieve RoMEO information for ISSN: @issn!', array('@issn' => $issn)), 'warning');
        return '';
      }
    }
  }
}