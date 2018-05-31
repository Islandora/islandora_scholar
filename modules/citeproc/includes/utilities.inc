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
 * Sets and returns default backend based on the PHP version of the installation.
 *
 * @return null|string
 *   String with the backend name or NULL if already set.
 */
function citeproc_default_backend() {
  if (variable_get('citeproc_backend', NULL) !== NULL) {
    return NULL;
  }
  $backend = !citeproc_v2_supported() ? CITEPROC_BACKEND_V1 : CITEPROC_BACKEND_V2;
  return $backend;
}

/**
 * Lookup the language to use for citeproc-php v2.
 *
 * @param string $language
 *   String returned from Drupal's $language->language.
 * @return string
 *   Citeproc style language code.
 */
function citeproc_lookup_language($language) {
  // Taken from citeproc-php v1.
  // https://github.com/Islandora/citeproc-php/blob/master/CiteProc.php#L1545-L1585
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