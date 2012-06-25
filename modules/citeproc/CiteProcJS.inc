<?php

/**
 * @file
 *
 * Citation Class this is used to register various objects required to render citations. This class is not mean for
 * general users of this module, it will be used internally by the classes defined within this module.
 */

/**
 * Singleton, for storing the data and citation items.
 */
class CiteProcJS {

  /**
   * The default locale. This local is always loaded.
   */
  static $DEFAULT_LOCALE = 'en-US';
  /**
   * The location of the locale files relative to this module.
   */
  const LOCALE_DIRECTORY = '/data/locale/';
  /**
   * The locale's avaliable to this module.
   */
  static $LOCALES = array(
    'af-ZA' => 'locales-af-ZA.xml',
    'ar-AR' => 'locales-ar-AR.xml',
    'bg-BG' => 'locales-bg-BG.xml',
    'ca-AD' => 'locales-ca-AD.xml',
    'cs-CZ' => 'locales-cs-CZ.xml',
    'da-DK' => 'locales-da-DK.xml',
    'de-AT' => 'locales-de-AT.xml',
    'de-CH' => 'locales-de-CH.xml',
    'de-DE' => 'locales-de-DE.xml',
    'el-GR' => 'locales-el-GR.xml',
    'en-GB' => 'locales-en-GB.xml',
    'en-US' => 'locales-en-US.xml',
    'es-ES' => 'locales-es-ES.xml',
    'et-EE' => 'locales-et-EE.xml',
    'fa-IR' => 'locales-fa-IR.xml',
    'fr-FR' => 'locales-fr-FR.xml',
    'he-IL' => 'locales-he-IL.xml',
    'hu-HU' => 'locales-hu-HU.xml',
    'is-IS' => 'locales-is-IS.xml',
    'it-IT' => 'locales-it-IT.xml',
    'ja-JP' => 'locales-ja-JP.xml',
    'km-KH' => 'locales-km-KH.xml',
    'ko-KR' => 'locales-ko-KR.xml',
    'mn-MN' => 'locales-mn-MN.xml',
    'nb-NO' => 'locales-nb-NO.xml',
    'nl-NL' => 'locales-nl-NL.xml',
    'nn-NO' => 'locales-nn-NO.xml',
    'pl-PL' => 'locales-pl-PL.xml',
    'pt-BR' => 'locales-pt-BR.xml',
    'pt-PT' => 'locales-pt-PT.xml',
    'ro-RO' => 'locales-ro-RO.xml',
    'ru-RU' => 'locales-ru-RU.xml',
    'sk-SK' => 'locales-sk-SK.xml',
    'sl-SI' => 'locales-sl-SI.xml',
    'sr-RS' => 'locales-sr-RS.xml',
    'sv-SE' => 'locales-sv-SE.xml',
    'th-TH' => 'locales-th-TH.xml',
    'tr-TR' => 'locales-tr-TR.xml',
    'uk-UA' => 'locales-uk-UA.xml',
    'vi-VN' => 'locales-vi-VN.xml',
    'zh-CN' => 'locales-zh-CN.xml',
    'zh-TW' => 'locales-zh-TW.xml'
  );

  /**
   * Gets the singleton instance of this class.
   *
   * @staticvar CiteProc $instance
   * @return CiteProc
   */
  public static function getInstance() {
    static $instance = NULL;
    if ($instance === NULL) {
      $instance = new CiteProcJS();
    }
    return $instance;
  }

  /**
   * Includes all the required files for this to function.
   * 
   */
  public static function addResources() {
    static $loaded = FALSE;
    if ($loaded === FALSE) {
      global $base_url;
      $module_path = drupal_get_path('module', 'citeproc');
      // Required for a javascript error, note the type of the script.
      drupal_set_html_head("<script type='text/javascript; e4x=1' src='$base_url/$module_path/lib/citeproc-js/xmle4x.js'></script>");
      drupal_add_js($module_path . '/lib/citeproc-js/xmldom.js');
      drupal_add_js($module_path . '/lib/citeproc-js/citeproc.js');
      drupal_add_js($module_path . '/js/loadsys.js');
      drupal_add_js($module_path . '/js/runcites.js');
      drupal_add_css($module_path . '/css/citeproc.css');
      $loaded = TRUE;
    }
  }

  /**
   * Creates a CiteProc instance.
   *
   * Includes the default locale.
   */
  private function __construct() {
    $this->items = array();
    $this->addLocale(self::$DEFAULT_LOCALE);
    self::addResources();
  }

  /**
   * Stores data required by citeproc-js in the drupal settings object.
   *
   * @param array $data
   *   The data to store in the drupal settings object.
   */
  private function storeInSettings(array $data) {
    $data['refresh'] = true;
    drupal_add_js(array('citeproc' => $data), 'setting');
  }

  /**
   * Registers a CiteProcAbbreviation with the citeproc-js api.
   *
   * Note after adding a CiteProcAbbreviation, subsequent changes to the
   * CiteProcAbbreviation will have no affect on citeproc-js.
   *
   * @param CiteProcAbbreviation $abbreviation
   *   The abbreviation to register.
   */
  public function addAbbreviation(CiteProcJSAbbreviation $abbreviation) {
    $data = array(
      'abbreviation' => array(
        $item->id => $item->toJSON()
      )
    );
    $this->storeInSettings($data);
  }

  /**
   * Registers a CiteProcItem with the citeproc-js api.
   *
   * Note after adding a CiteProcItem, subsequent changes to the CiteProcItem
   * will have no on citeproc-js.
   *
   * @param CiteProcItem $item
   *   The item to register.
   */
  public function addItem(CiteProcJSItem $item) {
    $id = $item->getID();
    $json = $item->toJSON();
    $data = array('item' => array($id => $json));
    $this->storeInSettings($data);
  }

  /**
   * Adds an existing local.
   *
   * @param string $name
   *   A $key in this objects self::$LOCALES array.
   * @return boolean
   *   TRUE on success, FALSE otherwise.
   */
  public function addLocale($name) {
    if (isset(self::$LOCALES[$name])) {
      $locale = new DOMDocument();
      $filename = drupal_get_path('module', 'citeproc') . self::LOCALE_DIRECTORY . self::$LOCALES[$name];
      $locale->load($filename);
      $this->addCustomLocale($name, $locale);
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Adds a custom locale.
   *
   * @param string $name
   *   The name of the locale, used to identify it uniquely.
   * @param DOMDocument $locale
   *   The locale definition. Must be CSL version 1.0.
   */
  public function addCustomLocale($name, DOMDocument $locale) {
    $data = array(
      'locale' => array(
        $name => $locale->saveXML()
      )
    );
    $this->storeInSettings($data);
  }

  /**
   * Adds a CSL style, which can be used to format citaions.
   *
   * @param string $name
   *   The name of the style.
   * @param DOMDocument $style
   *   The CSL style definition. Must be CSL version 1.0.
   */
  public function addStyle(CiteProcJSStyle $style) {
    $data = array('style' => array($style->getID() => $style->toJSON()));
    $this->storeInSettings($data);
  }

  /**
   * This function is used by the CiteProcBibliography, and shouldn't be used by any other class.
   */
  public function addBibliography(CiteProcJSBibliography $bibliography) {
    // Only one supported for the moment.
    $id = $bibliography->getID();
    $json = $bibliography->toJSON();
    $data = array('bibliography' => array($id => $json));
    $this->storeInSettings($data);
  }
}
