<?php

/**
 * @file
 * Utility functions.
 */

/**
 * Check if we should use legacy mode.
 *
 * @return bool
 *   TRUE, if we should use legacy mode to retrieve the metadata, FALSE
 *   otherwise.
 */
function islandora_doi_use_legacy_mode() {
  // Get current OpenURL PID.
  $openurl_pid = variable_get('islandora_doi_openurl_pid', ISLANDORA_DOI_OPENURL_PID_DEFAULT);
  // Define the default for 'islandora_doi_use_legacy_mode' in dependence
  // of whether or not the OpenURL PID is set. This is not to force upgraders
  // to use the new mode.
  // Note that we do not use booleans, since checkboxes return integers.
  $use_legacy_mode_default = ($openurl_pid !== ISLANDORA_DOI_OPENURL_PID_DEFAULT) ? 1 : 0;
  // Get the legacy mode status.
  $use_legacy_mode = variable_get('islandora_doi_use_legacy_mode', $use_legacy_mode_default);
  // Return the appropriate boolean.
  return ($use_legacy_mode === 1);
}

/**
 * Implements hook_islandora_doi_content_type_info().
 */
function islandora_doi_islandora_doi_content_type_info() {
  $info = array();
  $crossref_type = 'application/vnd.crossref.unixref+xml';
  $crossref_info = array(
    'translator' => 'islandora_doi_crossref_translator',
    'validator' => 'islandora_doi_crossref_validator',
  );
  $info[$crossref_type] = $crossref_info;
  return $info;
}

/**
 * Wrap a module_invoke_all() call.
 */
function islandora_doi_get_content_type_info() {
  $info = module_invoke_all('islandora_doi_content_type_info');
  drupal_alter('islandora_doi_content_type_info', $info);
  return $info;
}

/**
 * Helper function to sort content type info.
 */
function _islandora_doi_info_compare($a, $b) {
  if (!isset($a['weight'])) {
    $a['weight'] = 0;
  }
  if (!isset($b['weight'])) {
    $b['weight'] = 0;
  }
  if ($a['weight'] == $b['weight']) {
    return 0;
  }
  return ($a['weight'] < $b['weight']) ? -1 : 1;
}

/**
 * Get HTTP Access header string.
 *
 * @return string
 *   The string to set as HTTP Access header.
 */
function islandora_doi_get_http_access() {
  $info = islandora_doi_get_content_type_info();
  uasort($info, '_islandora_doi_info_compare');
  return implode(', ', array_keys($info));
}

/**
 * Get the DOI name for the given url.
 *
 * @param string $doi_url
 *   A DOI url, or a DOI name with or without the "doi:" prefix. If the $url
 *   is a CrossRef ID passed in as doi:10.1016/j.tiv.2011.10.017 or
 *   10.1111/eve.12339 it will not get changed by running it through parse_url.
 *
 * @return string
 *   A string containing the DOI name.
 */
function islandora_doi_get_doi_name_from_url($doi_url) {
  // Allows for DOI to be entered in the following formats: 10.1111/eva.12339,
  // http://dx.doi.org/10.1111/eva.12339, http://doi.org/10.1111/eva.12339,
  // or doi:10.1111/eva.12339. Removing whitespace before parsing the url and
  // then removing any trailing "/" from the returned path.
  $doi_name = trim(parse_url(trim($doi_url), PHP_URL_PATH), '/');
  return $doi_name;
}

/**
 * Check to see if supplied OpenURL PID works.
 *
 * @param string $identifier
 *   An identifier to test
 *
 * @return bool
 *   TRUE if identifier is valid, FALSE if invalid.
 */
function islandora_doi_openurl_pid_valid($identifier) {
  $openurl = variable_get('islandora_doi_openurl', ISLANDORA_DOI_OPENURL_DEFAULT);
  $url = url($openurl, array(
           'query' => array(
             'noredirect' => 'true',
             'pid' => $identifier,
             'format' => 'unixref',
             // DOI recommended by CrossRef for testing.
             'id' => 'doi:10.5555/12345678',
           ),
  ));

  $response = drupal_http_request($url);
  $headers = array_change_key_case($response->headers);
  $response_content_type = $headers['content-type'];
  if ($response_content_type == 'text/xml;charset=UTF-8') {
    return TRUE;
  }

  return FALSE;
}

/**
 * Perform a request to metadata provider for the given ID.
 *
 * @param string $id_or_url
 *   A DOI to lookop. May be provided with or without the "doi:" prefix. Can
 *   also pass a full DOI url (i.e, http://doi.org/10.1111/eva.12339 ).
 *
 * @return object
 *   An object as provided by drupal_http_request().
 */
function islandora_doi_perform_request($id_or_url) {
  // Allows for $id to pass a DOI url string or the DOI name.
  $id = islandora_doi_get_doi_name_from_url($id_or_url);
  $options = array();
  if (islandora_doi_use_legacy_mode()) {
    $openurl = variable_get('islandora_doi_openurl', ISLANDORA_DOI_OPENURL_DEFAULT);
    $openurl_pid = variable_get('islandora_doi_openurl_pid', ISLANDORA_DOI_OPENURL_PID_DEFAULT);
    $url = url($openurl, array(
             'query' => array(
               'noredirect' => 'true',
               'pid' => $openurl_pid,
               'format' => 'unixref',
               'id' => ((strpos($id, 'doi:') === 0) ? $id : 'doi:' . $id),
             ),
    ));
  }
  else {
    $doiurl = variable_get('islandora_doi_doiurl', ISLANDORA_DOI_DOIURL_DEFAULT);
    if (strpos($id, 'doi:') === 0) {
      $id = substr($id, 4);
    }
    $url = url($doiurl . '/' . $id);
    $options = array(
      'headers' => array(
        'Accept' => islandora_doi_get_http_access(),
      ),
      'max_redirects' => 1,
    );
  }

  return drupal_http_request($url, $options);
}

/**
 * Implements callback_islandora_doi_translator() for CrossRef data.
 */
function islandora_doi_crossref_translator($crossref_data) {
  // Parse the source into a DOMDocument.
  $crossref_xml = new DOMDocument("1.0");
  if (!$crossref_xml->loadXML($crossref_data)) {
    return FALSE;
  }
  // Create MODS XML.
  $mods = new DOMDocument('1.0');
  $mods->loadXML('<mods xmlns="http://www.loc.gov/mods/v3" xmlns:mods="http://www.loc.gov/mods/v3" xmlns:xlink="http://www.w3.org/1999/xlink"/>');

  // @todo Implement book support.

  // Add metadata for journal articles.
  foreach ($crossref_xml->getElementsByTagName('journal') as $journal) {
    $genre = $mods->createElement('genre');
    $genre->nodeValue = 'article';
    $mods->firstChild->appendChild($genre);

    $relateditem = $mods->createElement('relatedItem');
    $mods->firstChild->appendChild($relateditem);
    $relateditem_attribute = $mods->createAttribute('type');
    $relateditem_attribute->value = 'host';
    $relateditem->appendChild($relateditem_attribute);

    foreach ($journal->getElementsByTagName('journal_metadata') as $metadata) {
      foreach ($metadata->childNodes as $child) {
        switch ($child->nodeName) {
          case 'full_title':
            $titleinfo = $mods->createElement('titleInfo');
            $relateditem->appendChild($titleinfo);
            $title = $mods->createElement('title');
            $titleinfo->appendChild($title);
            if (!is_null($child->firstChild)) {
              $title_text = $mods->importNode($child->firstChild);
              $title->appendChild($title_text);
            }
            break;

          case 'abbrev_title':
            $titleinfo = $mods->createElement('titleInfo');
            $titleinfo_attribute = $mods->createAttribute('type');
            $titleinfo_attribute->value = 'abbreviated';
            $titleinfo->appendChild($titleinfo_attribute);
            $relateditem->appendChild($titleinfo);
            $title = $mods->createElement('title');
            $titleinfo->appendChild($title);
            if (!is_null($child->firstChild)) {
              $title_text = $mods->importNode($child->firstChild);
              $title->appendChild($title_text);
            }
            break;

          case 'issn':
            $identifier = $mods->createElement('identifier');
            $identifier_attribute = $mods->createAttribute('type');
            $identifier_attribute->value = 'issn';
            $identifier->appendChild($identifier_attribute);
            $relateditem->appendChild($identifier);
            if (!is_null($child->firstChild)) {
              $identifier_text = $mods->importNode($child->firstChild);
              $identifier->appendChild($identifier_text);
            }
            break;
        }
      }
    }
    foreach ($journal->getElementsByTagName('journal_issue') as $issue) {
      if ($issue->hasChildNodes()) {
        foreach ($issue->childNodes as $child) {
          switch ($child->nodeName) {
            case 'publication_date':
              if ($child->hasChildNodes()) {
                foreach ($child->childNodes as $date_part) {
                  switch ($date_part->nodeName) {
                    case 'month':
                      $pubdate_month = $date_part->nodeValue;
                      break;

                    case 'year':
                      $pubdate_year = $date_part->nodeValue;
                      break;

                    case 'day':
                      $pubdate_day = $date_part->nodeValue;
                      break;
                  }
                }
              }
              break;

            case 'journal_volume':
              if ($child->hasChildNodes()) {
                foreach ($child->childNodes as $volume) {
                  switch ($volume->nodeName) {
                    case 'volume':
                      if (!is_null($volume->firstChild)) {
                        $volume_number = $mods->importNode($volume->firstChild);
                      }
                      break;
                  }
                }
              }
              break;

            case 'issue':
              if (!is_null($child->firstChild)) {
                $issue_number = $mods->importNode($child->firstChild);
              }
              break;
          }
        }
      }
    }
    foreach ($journal->getElementsByTagName('journal_article') as $article) {
      if ($article->hasAttributes()) {
        foreach ($article->attributes as $att) {
          switch ($att->name) {
            case 'language':
              $language = $mods->createElement('language');
              $mods->firstChild->appendChild($language);
              $languageterm = $mods->createElement('languageTerm');
              $language->appendChild($languageterm);
              $language_text = $mods->createTextNode($att->value);
              $languageterm->appendChild($language_text);
              if (strlen($att->value) == 2) {
                $type_attribute = $mods->createAttribute('type');
                $type_attribute->value = 'code';
                $languageterm->appendChild($type_attribute);
                $authority_attribute = $mods->createAttribute('authority');
                $authority_attribute->value = 'rfc4646';
                $languageterm->appendChild($authority_attribute);
              }
              break;
          }
        }
      }
      if ($article->hasChildNodes()) {
        foreach ($article->childNodes as $child) {
          switch ($child->nodeName) {
            case 'titles':
              $titleinfo = $mods->createElement('titleInfo');
              $mods->firstChild->appendChild($titleinfo);
              foreach ($child->childNodes as $grandchild) {
                switch ($grandchild->nodeName) {
                  case 'title':
                    if (!is_null($grandchild->firstChild)) {
                      $article_title_text = islandora_doi_filter_html($grandchild);
                      $article_title = $mods->createElement('title', $article_title_text);
                      $titleinfo->appendChild($article_title);
                    }
                    break;

                  case 'subtitle':
                    if (!is_null($grandchild->firstChild)) {
                      $article_subtitle_text = islandora_doi_filter_html($grandchild);
                      $article_subtitle = $mods->createElement('subTitle', $article_subtitle_text);
                      $titleinfo->appendChild($article_subtitle);
                    }
                    break;

                  case 'original_language_title':
                    $translated_titleinfo = $mods->createElement('titleInfo');
                    $mods->firstChild->appendChild($translated_titleinfo);
                    $titleinfo_attribute = $mods->createAttribute('type');
                    $titleinfo_attribute->value = 'translated';
                    $translated_titleinfo->appendChild($titleinfo_attribute);
                    foreach ($grandchild->attributes as $att) {
                      switch ($att->nodeName) {
                        case 'language':
                          $titleinfo_attribute = $mods->createAttribute('lang');
                          $titleinfo_attribute->value = $att->nodeValue;
                          $translated_titleinfo->appendChild($titleinfo_attribute);
                          break;
                      }
                    }
                    if (!is_null($grandchild->firstChild)) {
                      $translated_title_text = islandora_doi_filter_html($grandchild);
                      $translated_title = $mods->createElement('title', $translated_title_text);
                      $translated_titleinfo->appendChild($translated_title);
                    }
                    break;
                }
              }
              break;

            case 'contributors':
              foreach ($child->getElementsByTagName('organization') as $organization) {
                $name = $mods->createElement('name');
                $mods->firstChild->appendChild($name);
                $name_attribute = $mods->createAttribute('type');
                $name_attribute->value = 'corporate';
                $name->appendChild($name_attribute);
                $namepart = $mods->createElement('namePart');
                $name->appendChild($namepart);
                if (!is_null($organization->firstChild)) {
                  $namepart_text = $mods->importNode($organization->firstChild);
                  $namepart->appendChild($namepart_text);
                }
              }

              foreach ($child->getElementsByTagName('person_name') as $person) {
                $name = $mods->createElement('name');
                $mods->firstChild->appendChild($name);
                $name_attribute = $mods->createAttribute('type');
                $name_attribute->value = 'personal';
                $name->appendChild($name_attribute);

                foreach ($person->childNodes as $person_part) {
                  switch ($person_part->nodeName) {
                    case 'given_name':
                      $namepart = $mods->createElement('namePart');
                      $name->appendChild($namepart);
                      $namepart_attribute = $mods->createAttribute('type');
                      $namepart_attribute->value = 'given';
                      $namepart->appendChild($namepart_attribute);
                      if (!is_null($person_part->firstChild)) {
                        $given_name_part = $mods->importNode($person_part->firstChild);
                        $namepart->appendChild($given_name_part);
                      }
                      break;

                    case 'surname':
                      $namepart = $mods->createElement('namePart');
                      $name->appendChild($namepart);
                      $namepart_attribute = $mods->createAttribute('type');
                      $namepart_attribute->value = 'family';
                      $namepart->appendChild($namepart_attribute);
                      if (!is_null($person_part->firstChild)) {
                        $surname_part = $mods->importNode($person_part->firstChild);
                        $namepart->appendChild($surname_part);
                      }
                      break;

                    case 'suffix':
                      $terms_of_address = $mods->createElement('namePart');
                      $name->appendChild($terms_of_address);
                      if (!is_null($person_part->firstChild)) {
                        $terms_of_address_text = $mods->importNode($person_part->firstChild);
                        $terms_of_address->appendChild($terms_of_address_text);
                      }
                      $terms_of_address_attribute = $mods->createAttribute('type');
                      $terms_of_address_attribute->value = 'termsOfAddress';
                      $terms_of_address->appendChild($terms_of_address_attribute);
                      break;

                    case 'affiliation':
                      $affiliation = $mods->createElement('affiliation');
                      $name->appendChild($affiliation);
                      if (!is_null($person_part->firstChild)) {
                        $affiliation_text = $mods->importNode($person_part->firstChild);
                        $affiliation->appendChild($affiliation_text);
                      }
                      break;
                  }
                }

                foreach ($person->attributes as $att) {
                  switch ($att->nodeName) {
                    case 'contributor_role':
                      $role = $mods->createElement('role');
                      $name->appendChild($role);
                      $roleterm = $mods->createElement('roleTerm');
                      $role->appendChild($roleterm);
                      $roleterm_text = $mods->createTextNode(strtolower($att->nodeValue));
                      $roleterm->appendChild($roleterm_text);
                      $roles = array('author', 'editor', 'translator');
                      if (in_array(strtolower($att->nodeValue), $roles)) {
                        $roleterm_attribute = $mods->createAttribute('authority');
                        $roleterm_attribute->value = 'marcrelator';
                        $roleterm->appendChild($roleterm_attribute);
                        $roleterm_attribute = $mods->createAttribute('type');
                        $roleterm_attribute->value = 'text';
                        $roleterm->appendChild($roleterm_attribute);
                      }
                      break;

                    case 'sequence':
                      if (strcmp('first', strtolower($att->nodeValue)) == 0) {
                        $name_attribute = $mods->createAttribute('usage');
                        $name_attribute->value = 'primary';
                        $name->appendChild($name_attribute);
                      }
                  }
                }
              }
              break;

            case 'publication_date':
              // Note that this overwrites the publication date that came
              // from the journal metadata above.
              if ($child->hasChildNodes()) {
                foreach ($child->childNodes as $date_part) {
                  switch ($date_part->nodeName) {
                    case 'month':
                      $pubdate_month = $date_part->nodeValue;
                      break;

                    case 'year':
                      $pubdate_year = $date_part->nodeValue;
                      break;

                    case 'day':
                      $pubdate_day = $date_part->nodeValue;
                      break;
                  }
                }
              }
              break;

            case 'pages':
              if ($child->hasChildNodes()) {
                foreach ($child->childNodes as $page_part) {
                  switch ($page_part->nodeName) {
                    case 'first_page':
                      if (!is_null($page_part->firstChild)) {
                        $first_page = $mods->importNode($page_part->firstChild);
                      }
                      break;

                    case 'last_page':
                      if (!is_null($page_part->firstChild)) {
                        $last_page = $mods->importNode($page_part->firstChild);
                      }
                      break;

                    case 'other_pages':
                      if (!is_null($page_part->firstChild)) {
                        $other_pages = $mods->importNode($page_part->firstChild);
                      }
                      break;
                  }
                }
              }
              break;

            case 'publisher_item':
              // @todo
              break;

            case 'crossmark':
              // @todo
              break;

            case 'doi_data':
              if ($child->hasChildNodes()) {
                foreach ($child->childNodes as $identifier) {
                  switch ($identifier->nodeName) {
                    case 'doi':
                      if (!is_null($identifier->firstChild)) {
                        $doi = $mods->createElement('identifier');
                        $mods->firstChild->appendChild($doi);
                        $doi_attribute = $mods->createAttribute('type');
                        $doi_attribute->value = 'doi';
                        $doi->appendChild($doi_attribute);
                        $doi_text = $mods->importNode($identifier->firstChild);
                        $doi->appendChild($doi_text);
                      }
                      break;

                    case 'resource':
                      if (!is_null($identifier->firstChild)) {
                        $uri = $mods->createElement('identifier');
                        $mods->firstChild->appendChild($uri);
                        $uri_attribute = $mods->createAttribute('type');
                        $uri_attribute->value = 'uri';
                        $uri->appendChild($uri_attribute);
                        $uri_text = $mods->importNode($identifier->firstChild);
                        $uri->appendChild($uri_text);
                      }
                      break;
                  }
                }
              }
              break;

            case 'citation_list':
              // @todo
              break;

            case 'component_list':
              // @todo
              break;
          }
        }
      }
    }
    // Add the publication date if it was read in one of the two places above.
    $origininfo = $mods->createElement('originInfo');
    $mods->firstChild->appendChild($origininfo);

    if (!empty($pubdate_year)) {
      $date_value = $pubdate_year;
      if (!empty($pubdate_month)) {
        if (strlen($pubdate_month) == 1) {
          $pubdate_month = "0" . $pubdate_month;
        }
        $date_value .= "-" . $pubdate_month;
      }
      if (!empty($pubdate_day)) {
        if (strlen($pubdate_day) == 1) {
          $pubdate_day = "0" . $pubdate_day;
        }
        $date_value .= "-" . $pubdate_day;
      }
      $dateissued = $mods->createElement('dateIssued');
      $origininfo->appendChild($dateissued);
      $dateissued->nodeValue = $date_value;
    }

    // Add the page, volume, and issue data if it was read above.
    if (!empty($volume_number) || !empty($issue_number) || !empty($other_pages) || !empty($last_page) || !empty($first_page) || !empty($pubdate_year) || !empty($pubdate_month) || !empty($pubdate_day)) {
      $part = $mods->createElement('part');
      $mods->firstChild->appendChild($part);

      if (!empty($last_page) || !empty($first_page) || !empty($other_pages)) {
        $extent = $mods->createElement('extent');
        $part->appendChild($extent);
        $extent_attribute = $mods->createAttribute('unit');
        $extent_attribute->value = 'page';
        $extent->appendChild($extent_attribute);

        if (!empty($other_pages)) {
          $list = $mods->createElement('list');
          $list->appendChild($other_pages);
          $extent->appendChild($list);
        }
        if (!empty($first_page)) {
          $start = $mods->createElement('start');
          $start->appendChild($first_page);
          $extent->appendChild($start);
        }
        if (!empty($last_page)) {
          $end = $mods->createElement('end');
          $end->appendChild($last_page);
          $extent->appendChild($end);
        }
      }

      if (!empty($volume_number)) {
        $volume = $mods->createElement('detail');
        $part->appendChild($volume);
        $volume_attribute = $mods->createAttribute('type');
        $volume_attribute->value = 'volume';
        $volume->appendChild($volume_attribute);
        $number = $mods->createElement('number');
        $volume->appendChild($number);
        $number->appendChild($volume_number);
      }
      if (!empty($issue_number)) {
        $issue = $mods->createElement('detail');
        $part->appendChild($issue);
        $issue_attribute = $mods->createAttribute('type');
        $issue_attribute->value = 'issue';
        $issue->appendChild($issue_attribute);
        $number = $mods->createElement('number');
        $issue->appendChild($number);
        $number->appendChild($issue_number);
      }

      if (!empty($date_value)) {
        $date = $mods->createElement('date');
        $date->nodeValue = $date_value;
        $part->appendChild($date);
      }
    }
    // Return after first instance.
    return $mods;
  }
  return FALSE;
}

/**
 * Implements callback_islandora_doi_validator() for CrossRef data.
 */
function islandora_doi_crossref_validator($id, $crossref_data) {
  $doi = islandora_doi_get_doi_name_from_url($id);
  $crossref_xml = simplexml_load_string($crossref_data);
  if ($crossref_xml->doi_record->crossref->error) {
    // This should only occur in legacy mode.
    drupal_set_message(t("CrossREF reports the DOI '@doi' as erroneous.", array(
        '@doi' => $doi,
      )), 'error');
    return FALSE;
  }
  // Totally don't support books so let's not attempt to make records that
  // are actually books!
  if (!$crossref_xml->doi_record->crossref->journal) {
    drupal_set_message(t("Only the 'Journal' type is currently supported for DOI importing (offending DOI: '@doi').", array(
        '@doi' => $doi,
      )), 'error');
    return FALSE;
  }

  return TRUE;
}

/**
 * Produce MODS from the given metadata provider response data.
 *
 * @param string $data
 *   The data response of the metadata provider.
 * @param string $content_type
 *   The content type of the given data.
 *
 * @return DOMDocument|bool
 *   A DOMDocument containing the constructed MDOS, or FALSE on failure.
 */
function islandora_doi_translate($data, $content_type) {
  $info = islandora_doi_get_content_type_info();
  if (!array_key_exists($content_type, $info)) {
    // Warning message: Translator not yet implemented.
    watchdog('Islandora DOI', "The requested translator for content '@content_type' has not yet been implemented. Returning FALSE...", array(
      '@content_type' => $content_type,
    ));
    trigger_error(t("The requested translator for content '@content_type' has not yet been implemented. Returning FALSE...", array(
      '@content_type' => check_plain($content_type),
    )));
    return FALSE;
  }
  if (!isset($info[$content_type]['translator'])) {
    return FALSE;
  }
  if (isset($info[$content_type]['file'])) {
    require_once DRUPAL_ROOT . '/' . $info[$content_type]['file'];
  }
  return $info[$content_type]['translator']($data);
}

/**
 * Produce MODS from the given metadata provider XML.
 *
 * @deprecated As of release 7.x-1.11, this function remains only for backwards
 *   compatibilty. New code should use islandora_doi_translate() instead, which
 *   takes unparsed data to convert. Update your code, since this function
 *   might be removed.
 *
 * @param mixed $doi_xml
 *   XML containing the parsed response from the metadata provider. Should be
 *   passed as a DOMDocument, but also works with a SimpleXMLElement.
 * @param string $content_type
 *   Optional parameter specifying the content type to translate (defaults to
 *   'application/vnd.crossref.unixref+xml', i.e. CrossRef data).
 *
 * @return DOMDocument|bool
 *   A DOMDocument containing the constructed MODS, or FALSE on failure.
 *
 * @throws Exception
 *   If passed a SimpleXMLElement that can't be converted to DOMDocument.
 *
 * @throws InvalidArgumentException
 *   If passed something other than DOMDocument or SimpleXMLElement.
 */
function islandora_doi_transform_to_mods($doi_xml, $content_type = 'application/vnd.crossref.unixref+xml') {
  module_load_include('inc', 'islandora', 'includes/utilities');
  $deprecated_message = islandora_deprecated('7.x-1.11', t('Use @newfunction() instead.', array('@newfunction' => 'islandora_doi_translate')));
  trigger_error(filter_xss($deprecated_message), E_USER_DEPRECATED);
  // Make sure doi_xml is a DOMDocument. Otherwise show a warning and, if it
  // is a SimpleXMLElement, convert it.
  if ($doi_xml instanceof SimpleXMLElement) {
    // Warning message: Converting from SimpleXMLElement.
    watchdog('Islandora DOI', 'islandora_doi_transform_to_mods should be passed a DOMDocument but received a SimpleXMLElement. Attempting to convert the SimpleXMLElement to a DOMDocument. Code should be updated to call islandora_doi_transform_to_mods with a DOMDocument instead.');
    trigger_error(t('islandora_doi_transform_to_mods should be passed a DOMDocument but received a SimpleXMLElement. Attempting to convert the SimpleXMLElement to a DOMDocument. Code should be updated to call islandora_doi_transform_to_mods with a DOMDocument instead.'));
    $dom_xml = new DOMDocument('1.0');
    if (!$dom_xmle = dom_import_simplexml($doi_xml)) {
      // Error message: Failed to convert from SimpleXMLElement.
      throw new Exception('Failed to convert SimpleXMLElement to DOMDocument');
    }
    $dom_xmle = $dom_xml->importNode($dom_xmle, TRUE);
    $dom_xml->appendChild($dom_xmle);
    $doi_xml = $dom_xml;
  }
  elseif (!($doi_xml instanceof DOMDocument)) {
    // Error message: Not supplied with a DOMDocument.
    throw new InvalidArgumentException('islandora_doi_transform_to_mods should take a DOMDocument');
  }

  return islandora_doi_translate($doi_xml->saveXML(), $content_type);
}

/**
 * Get the content type and source string for the given DOI.
 *
 * @param string $id
 *   A DOI.
 *
 * @return array|bool
 *   A bi-valued array, where the first value is the source data string as
 *   returned by the metadata provider and the second value is the content
 *   type, or FALSE if either an error occured or the source data is invalid
 *   according to the validator.
 */
function islandora_doi_get_source($id) {
  $info = islandora_doi_get_content_type_info();
  $response = islandora_doi_perform_request($id);
  $headers = array_change_key_case($response->headers);
  if (islandora_doi_use_legacy_mode()) {
    // In legacy mode, we only queried CrossRef.
    if (!$response->data ||
      strpos($headers['content-type'], 'text/html') !== FALSE ||
      strpos($response->data, "Malformed DOI") !== FALSE) {
      drupal_set_message(t("'@doi' does not seem to be a valid CrossREF DOI.", array(
        '@doi' => islandora_doi_get_doi_name_from_url($id),
      )), 'error');
      return FALSE;
    }
    $content_type = 'application/vnd.crossref.unixref+xml';
  }
  else {
    // In non-legacy mode, the response may come from any metadata provider
    // or @link https://doi.org doi.org @endlink itself.
    if (!$response->data ||
      !isset($response->code) || $response->code != 200 || isset($response->error) ||
      !array_key_exists($headers['content-type'], $info)) {
      if (isset($response->error)) {
        if (isset($response->code) && $response->code == 406) {
          // We should always find ourselves here when the DOI exists but we
          // requested content types the metadata provider does not support.
          // Unfortunately, not all providers behave as expected, which
          // explains the slightly more convoluted construction below.
          drupal_set_message(t("It seems that the metadata provider for '@doi' has no content we can handle.", array(
            '@doi' => islandora_doi_get_doi_name_from_url($id),
          )), 'error');
        }
        elseif (isset($response->code) && $response->code == 404) {
          // We would expect this to be the response for invalid DOIs only,
          // but this is not the case...
          if (isset($response->redirect_url) &&
            preg_match('/^https?:\/\/www.doi.org/', $response->redirect_url) !== 1) {
            // We know that, e.g., DataCite will return 404 if the first content
            // type in the Accept header differs from
            // 'application/vnd.datacite.datacite+xml'. It seems that truly
            // invalid DOIs get redirected to https://www.doi.org when
            // https://doi.org is queried.
            drupal_set_message(t("'@doi' does not appear to be a valid DOI (or the metadata provider for this DOI has no content we can handle).", array(
              '@doi' => islandora_doi_get_doi_name_from_url($id),
            )), 'error');
          }
          else {
            drupal_set_message(t("'@doi' does not appear to be a valid DOI.", array(
              '@doi' => islandora_doi_get_doi_name_from_url($id),
            )), 'error');
          }
        }
      }
      elseif (isset($response->code) && $response->code >= 300 && $response->code <= 308 &&
        isset($response->redirect_url)) {
        // The metadata provider tries to redirect us, in all likelyhood to
        // the publisher's website. We treat this the same as 406 above.
        drupal_set_message(t("It seems that the metadata provider for '@doi' has no content we can handle.", array(
          '@doi' => islandora_doi_get_doi_name_from_url($id),
        )), 'error');
      }
      return FALSE;
    }
    $content_type = $headers['content-type'];
  }
  if (array_key_exists($content_type, $info) && isset($info[$content_type]['validator'])) {
    if (isset($info[$content_type]['file'])) {
      require_once DRUPAL_ROOT . '/' . $info[$content_type]['file'];
    }
    if ($info[$content_type]['validator']($id, $response->data) !== TRUE) {
      // No error message here, since the validator should take care of that.
      return FALSE;
    }
  }

  return array($response->data, $content_type);
}

/**
 * Get MODS for the given DOI.
 *
 * @param string $id
 *   A DOI.
 *
 * @return DOMDocument|bool
 *   A DOMDocument containing a MODS document, or FALSE if either an error
 *   occurred or nothing could be transformed.
 */
function islandora_doi_get_mods($id) {
  $source_array = islandora_doi_get_source($id);
  if ($source_array === FALSE) {
    return FALSE;
  }
  list($source_string, $content_type) = $source_array;

  return islandora_doi_translate($source_string, $content_type);
}

/**
 * Filter HTML tags from imported XML metadata.
 *
 * @param DOMNode $node
 *   The node containing metadata that should be converted into a string,
 *   possibly containing markup.
 *
 * @return string 
 *   The node's inner content with markup escaped or stripped.
 */
function islandora_doi_filter_html($node, $action = NULL) {
  if (!$node) {
    return "";
  }
  else {
    $text  = '';
    foreach ($node->childNodes as $child) {
      $text .= $child->ownerDocument->saveXML($child);
    }

    $text = preg_replace('/\n\s+/', ' ', $text);

    if (!$action) {
      $action = variable_get('islandora_doi_handle_imported_metadata', 'strip');
    }

    switch ($action) {
      case 'strip':
        $text = strip_tags($text);
        break;

      case 'encode':
        $text = htmlspecialchars($text, ENT_NOQUOTES, 'UTF-8', FALSE);
        break;

      default:
        drupal_set_message(t('@var is not a recognized option for handling HTML in imported DOI metadata.', array('@var' => $action)), 'error');
        break;
    }
  }

  return $text;
}
