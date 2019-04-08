<?php

/**
 * @file
 * Helper utilities for citeproc module.
 */

/**
 * Tests if the PHP version is suitable for citeproc v2.
 *
 * @return bool
 *   Returns TRUE if citeproc v2 can be used.
 */
function citeproc_v2_supported() {
  return version_compare(phpversion(), '5.6', '>=');
}

/**
 * Lookup the language to use for citeproc-php v2.
 *
 * @param string $language
 *   String returned from Drupal's $language->language.
 *
 * @return string
 *   Citeproc style language code.
 */
function citeproc_lookup_language($language) {
  // Taken from citeproc-php v1.
  $lang_bases = array(
    "af" => "af-ZA",
    "ar" => "ar-AR",
    "bg" => "bg-BG",
    "ca" => "ca-AD",
    "cs" => "cs-CZ",
    "da" => "da-DK",
    "de" => "de-DE",
    "el" => "el-GR",
    "en" => "en-US",
    "es" => "es-ES",
    "et" => "et-EE",
    "fa" => "fa-IR",
    "fi" => "fi-FI",
    "fr" => "fr-FR",
    "he" => "he-IL",
    "hu" => "hu-HU",
    "is" => "is-IS",
    "it" => "it-IT",
    "ja" => "ja-JP",
    "km" => "km-KH",
    "ko" => "ko-KR",
    "mn" => "mn-MN",
    "nb" => "nb-NO",
    "nl" => "nl-NL",
    "nn" => "nn-NO",
    "pl" => "pl-PL",
    "pt" => "pt-PT",
    "ro" => "ro-RO",
    "ru" => "ru-RU",
    "sk" => "sk-SK",
    "sl" => "sl-SI",
    "sr" => "sr-RS",
    "sv" => "sv-SE",
    "th" => "th-TH",
    "tr" => "tr-TR",
    "uk" => "uk-UA",
    "vi" => "vi-VN",
    "zh" => "zh-CN",
  );
  $citeproc_lang = $lang_bases[$language];
  drupal_alter('citeproc_language', $citeproc_lang);
  return $citeproc_lang;
}

/**
 * Call filter_xss on the leaves of an array.
 */
function citeproc_deep_array_filter_xss(&$array) {
  // @codingStandardsIgnoreStart
  $func = function ($item) use (&$func) {
    return is_array($item) ? array_map($func, $item) : filter_xss($item);
  };
  // @codingStandardsIgnoreEnd

  return array_map($func, $array);
};
