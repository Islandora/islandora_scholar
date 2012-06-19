<?php

class CitationView {
  private static function get_mods($pid) {
    module_load_include('inc', 'fedora_repository', 'api/fedora_item');
    $item = new Fedora_Item($pid);
    $mods = trim($item->get_datastream_dissemination('MODS'));
    return $mods;
  }

  private static function get_citation($pid) {
    $style = citeproc_default_style();

    $bibliography = citeproc_bibliography_from_mods($style, self::get_mods($pid));
    $citation = $bibliography->render();
  }

  public static function get_tabs($pid) {
    return array(
      'overview' => array(
        '#title' => t('Overview'),
        '#type' => 'tabpage',
        '#tab_name' => 'overview',
        'citation' => array(
          '#title' => t('Citation'),
          '#type' => 'item',
          '#value' => self::get_citation($pid),
        ),
      ),
    );
  }
}