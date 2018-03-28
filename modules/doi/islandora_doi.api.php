<?php

/**
 * @file
 * Document interfaces/hooks exposed by this module.
 */

/**
 * Gather info for content negotiation.
 *
 * @return array
 *   An associative array mapping MIME-types to associative arrays containing:
 *    - translator: A callable implementing callback_islandora_doi_translator().
 *    - validator: A callable implementing callback_islandora_doi_validator().
 *    - weight: Integer to control order in HTTP Accept header (lower values
 *      are listed earlier in the Accept header; assumes 0 if unspecified;
 *      equal weights will be sorted unpredictably).
 *    - file: Optional location of file containing translator/validator
 *      functions (path relative to DRUPAL_ROOT).
 *
 * Note: At the time of writing, @link https//doi.org doi.org @endlink was
 * able to perform the content negotiation as explained on
 * @link https://www.crosscite.org/docs.html crosscite.org. @endlink Since we
 * are using the array keys to construct the Accept header of the HTTP
 * request, their order plays a role. So far, the requests alone were tested
 * simultaneously for @link https://www.crossref.org CrossRef, @endlink
 * @link https://datacite.org DataCite @endlink and
 * @link https://www.medra.org mEDRA. @endlink (content types
 * 'application/vnd.crossref.unixref+xml',
 * 'application/vnd.datacite.datacite+xml' and
 * 'application/vnd.medra.onixdoi+xml', respectively). It was noticed that:
 * - qvalues are not supported (see
 *   https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html)
 * - DataCite will only deliver if its content type comes first.
 * You may want to take into account those findings when extending this
 * module to support other metadata providers and set the weight accordingly.
 */
function hook_islandora_doi_content_type_info() {
  return array('text/xml' => array(
    'translator' => 'my_xml_translator',
    'validator' => 'my_xml_validator',
    'weight' => -13,
    'file' => 'path/to/module/includefile.inc',
  ));
}

/**
 * Allow other modules to alter info for content negotiation.
 *
 * @param array $info
 *   The associative array of variables being passed to islandora_doi.
 */
function hook_islandora_doi_content_type_info_alter(array &$info) {
  if (isset($info['text/xml'])) {
    unset($info['text/xml']);
  }
}

/**
 * Translate response to MODS, contained in a DOMDocument.
 *
 * @param string $data
 *   The response to be transformed.
 *
 * @return DOMDocument|bool
 *   The translated document as a DOMDocument; otherwise, boolean FALSE on
 *   failure.
 */
function callback_islandora_doi_translator(string $data) {
  // For an example, @see islandora_doi_crossref_translator().
}

/**
 * Validate response.
 *
 * @param string $id
 *   The DOI the response is supposed to represent.
 * @param string $data
 *   The response to validate.
 *
 * @return bool
 *   TRUE if the response appears to be valid; otherwise, FALSE.
 */
function callback_islandora_doi_validator(string $id, string $data) {
  // For an example, @see islandora_doi_crossref_validator().
}
