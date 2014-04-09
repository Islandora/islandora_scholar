<?php

/**
 * @file
 * Defines a class for importing DOI citations.
 */

class DOIImporter extends ScholarBatchImporter {

  protected $itemClass = 'DOIImportObject';

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
        '#title' => t('DOI Batch Journal Importer'),
        '#description' => t('Provide either a list of Digital Object Identifiers in a file, or enter DOIs in the textarea below. In either case, the DOIs in the list should be separated by either whitespace or commas (or some combination thereof). NOTE: Presently, only journal (articles) are supported.'),
        'file' => array(
          '#type' => 'managed_file',
          '#title' => t('File of DOIs'),
          '#description' => t('A file listing Digital Object Identifiers (with ".txt" extension).'),
          '#upload_validators' => array(
            'file_validate_extensions' => array('txt'),
          ),
        ),
        'textarea' => array(
          '#type' => 'textarea',
          '#title' => t('DOIs'),
          '#description' => t('Provide a list Digital Object Identifiers (DOI) - eg. 10.1016/j.jorganchem.2011.11.018, 10.1016/j.tiv.2011.10.017, 10.1111/j.1540-4560.2012.01733.x'),
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
      $text .= ' ' . trim(file_get_contents(drupal_realpath($file->uri)));
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
 * DOI import object.
 *
 * Actually does the heavy-lifting during the import.
 * @author adam
 */
class DOIImportObject extends IslandoraImportObject {

  /**
   * Generated MODS Document.
   *
   * @var SimpleXMLElement
   */
  protected $mods;

  /**
   * Get an item from the source.
   *
   * @see IslandoraImportObject::getOne()
   */
  public static function getOne(&$ids) {
    $id = array_shift($ids);

    $openurl = variable_get('doi_importer_openurl', 'http://www.crossref.org/openurl');
    $openurl_pid = variable_get('doi_importer_openurl_pid', 'user@example.com');
    $url = url($openurl, array(
             'query' => array(
               'noredirect' => 'true',
               'pid' => $openurl_pid,
               'format' => 'unixref',
               'id' => ((strpos($id, 'doi:') === 0) ? $id : 'doi:' . $id),
             )));

    $response = drupal_http_request($url);
    $headers = array_change_key_case($response->headers);
    if ($response->data &&
      strpos($headers['content-type'], 'text/html') === FALSE &&
      strpos($response->data, "Malformed DOI") === FALSE) {

      $crossref_xml = simplexml_load_string($response->data);
      if ($crossref_xml->doi_record->crossref->error) {
        return FALSE;
      }
      else {
        return new self($response->data);
      }
    }
    else {
      return FALSE;
    }
  }

  /**
   * Generates a MODS document repersenting the imported data.
   *
   * @see IslandoraImportObject::getMODS()
   */
  public function getMODS() {
    if ($this->mods === NULL) {
      $mods = new SimpleXMLElement('<mods xmlns="http://www.loc.gov/mods/v3" xmlns:mods="http://www.loc.gov/mods/v3" xmlns:xlink="http://www.w3.org/1999/xlink"/>');
      // @todo Implement book support.
      $doi_xml = simplexml_load_string($this->source);
      foreach ($doi_xml->doi_record as $record) {
        foreach ($record->crossref as $crossref) {
          foreach ($crossref->journal as $journal) {
            foreach ($journal->journal_metadata as $metadata) {
              $relateditem = $mods->addChild('relatedItem');
              $relateditem->addAttribute('type', 'host');
              foreach ($metadata->attributes() as $name => $item) {
                switch ((string) $name) {
                  case 'language':
                    $language = (string) $item[0];
                }
              }
              foreach ($metadata->children() as $name => $item) {
                switch ((string) $name) {
                  case 'full_title':
                    $titleinfo = $relateditem->addChild('titleInfo');
                    $titleinfo->addChild('title', (string) $item);
                    break;

                  case 'abbrev_title':
                    $titleinfo = $relateditem->addChild('titleInfo');
                    $titleinfo->addAttribute('type', 'abbreviated');
                    $titleinfo->addChild('title', (string) $item);
                    break;

                  case 'issn':
                    $issn = $relateditem->addChild('identifier', (string) $item);
                    $issn->addAttribute('type', 'issn');
                    break;

                  case 'coden':
                    break;

                  case 'doi_data':
                    break;

                }
              }
            }
            foreach ($journal->journal_issue as $issue) {
              foreach ($issue->children() as $name => $item) {
                switch ((string) $name) {
                  case 'contributors':
                    break;

                  case 'publication_date':
                    $pubdate_year = (string) $item[0]->year[0];
                    $pubdate_month = (string) $item[0]->month[0];
                    $pubdate_day = (string) $item[0]->day[0];
                    break;

                  case 'journal_volume':
                    $volume_number = (string) $item[0]->volume[0];
                    break;

                  case 'issue':
                    $issue_number = (string) $item[0];
                    break;

                  case 'special_numbering':
                    break;

                  case 'doi_data':
                    break;

                }
              }
            }
            foreach ($journal->journal_article as $article) {
              foreach ($article->attributes() as $name => $item) {
                switch ((string) $name) {
                  case 'publication_type':
                    // Options: full_text, abstract_only, bibliographic_record
                    // doesn't seem to matter here.
                    break;

                  case 'language':
                    $language = $mods->addChild('language');
                    $languageterm = $language->addChild('languageTerm', (string) $item[0]);
                    if (strlen((string) $item[0]) == 2) {
                      $languageterm->addAttribute('type', 'code');
                      $languageterm->addAttribute('authority', 'rfc4646');
                    }
                    break;

                }
              }
              foreach ($article->children() as $name => $item) {
                switch ((string) $name) {
                  case 'titles':
                    $title_info = $mods->addChild('titleInfo');
                    $title_info->addChild('title', (string) $item->title[0]);
                    if ($item->subtitle) {
                      $title_info->addChild('subTitle', (string) $item->subtitle[0]);
                    }
                    if ($item->original_language_title) {
                      // XXX: doesn't seem to actually add the original language
                      // title... creates the element, with attributes, but no
                      // content?
                      $title_info_translated = $mods->addChild('titleInfo');
                      $title_info_translated->addAttribute('type', 'translated');
                      $title_info_translated->addAttribute('lang', (string) $item->original_language_title['language']);
                    }
                    break;

                  case 'contributors':
                    foreach ($item->children() as $type => $contributor) {
                      switch ((string) $type) {
                        case 'person_name':
                          $role = strtolower((string) $contributor['contributor_role']);
                          $sequence = (string) $contributor['sequence'];
                          $given_name = (string) $contributor->given_name;
                          $surname = (string) $contributor->surname;
                          $suffix = (string) $contributor->suffix;
                          $affiliation = (string) $contributor->affiliation;

                          $mods_name = $mods->addChild('name');
                          $mods_name->addAttribute('type', 'personal');

                          $given_name_part = $mods_name->addChild('namePart', $given_name);
                          $given_name_part->addAttribute('type', 'given');
                          $surname_part = $mods_name->addChild('namePart', $surname);
                          $surname_part->addAttribute('type', 'family');

                          if ($sequence == 'first') {
                            $mods_name->addAttribute('usage', 'primary');
                          }

                          if ($role) {
                            $name_role = $mods_name->addChild('role');
                            $name_role->addChild('roleTerm', $role);
                            $roles = array('author', 'editor', 'translator');
                            if (in_array($role, $roles)) {
                              $name_role->roleTerm->addAttribute('authority', 'marcrelator');
                              $name_role->roleTerm->addAttribute('type', 'text');
                            }
                          }

                          if ($affiliation) {
                            $mods_name->addChild('affiliation', $affiliation);
                          }

                          if ($suffix) {
                            $terms_of_address = $mods_name->addChild('namePart', $suffix);
                            $terms_of_address->addAttribute('type', 'termsOfAddress');
                          }

                          break;

                        case 'organization':
                          $role = strtolower((string) $contributor['contributor_role']);
                          $sequence = (string) $contributor['sequence'];
                          $mods_name = $mods->addChild('name');
                          $mods_name->addAttribute('type', 'corporate');
                          $mods_name->addChild('namePart', (string) $contributor);
                          break;

                      }
                    }
                    break;

                  // Genre element was not getting populated and it is needed
                  // for proper CSL display. If we start pulling anything other
                  // than journal articles using DOIs, then this will need to
                  // be revisited. Journal article will work when piped through
                  // CSL converter.
                  case 'genre':
                    $genre = 'journal article';
                    $mods_genre = $mods->addChild('genre', $genre);
                    break;

                  case 'publication_date':
                    $pubdate_year = (string) $item[0]->year[0];
                    $pubdate_month = (string) $item[0]->month[0];
                    $pubdate_day = (string) $item[0]->day[0];
                    break;

                  case 'pages':
                    $first_page = (string) $item[0]->first_page[0];
                    $last_page = (string) $item[0]->last_page[0];
                    $other_pages = (string) $item[0]->other_pages[0];
                    break;

                  case 'publisher_item':
                    // @todo.
                    break;

                  case 'crossmark':
                    // @todo.
                    break;

                  case 'doi_data':
                    foreach ($item->doi as $doi) {
                      $mods_identifier = $mods->addChild('identifier', (string) $doi);
                      $mods_identifier->addAttribute('type', 'doi');
                    }
                    foreach ($item->resource as $resource) {
                      $mods_identifier = $mods->addChild('identifier');
                      $mods_identifier->{0} = (string) $resource;
                      $mods_identifier->addAttribute('type', 'uri');
                    }
                    break;

                  case 'citation_list':
                    // @todo.
                    break;

                  case 'component_list':
                    // @todo.
                    break;
                }
              }
            }

            // We should populated the dateIssued element otherwise CSL does
            // not display a date.
            $origin_info = $mods->addChild('originInfo');
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
              $origin_info->addChild('dateIssued', $date_value);
            }

            if (!empty($volume_number) || !empty($issue_number) || !empty($other_pages) || !empty($last_page) || !empty($first_page) || !empty($pubdate_year) || !empty($pubdate_month) || !empty($pubdate_day)) {
              $part = $mods->addChild('part');

              if (!empty($last_page) || !empty($first_page) || !empty($other_pages)) {
                $extent = $part->addChild('extent');
                $extent->addAttribute('unit', 'page');
                if (!empty($other_pages)) {
                  $extent->addChild('list', $other_pages);
                }
                if (!empty($last_page) || !empty($first_page)) {
                  $extent->addChild('start', $first_page);
                  $extent->addChild('end', $last_page);
                }
              }

              if (!empty($volume_number)) {
                $volume = $part->addChild('detail');
                $volume->addAttribute('type', 'volume');
                $volume->addChild('number', $volume_number);
              }
              if (!empty($issue_number)) {
                $issue = $part->addChild('detail');
                $issue->addAttribute('type', 'issue');
                $issue->addChild('number', $issue_number);
              }
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
                $part->addChild('date', $date_value);
              }

            }

            // Return after first instance.
            $this->mods = $mods->asXML();
          }
        }
      }
    }

    return $this->mods;
  }
}
