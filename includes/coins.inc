<?php

/**
 * @file
 * Coins.inc
 */

/**
 * Class to parse relevant data from MODS, initially used to
 * construct the COINS url but has found use elsewhere.
 */
class Coins {

  public $pid = NULL;
  public $mods = NULL;
  public $type = NULL;

  public $issn, $volume, $issue, $date, $start_page, $title, $sub_title,
    $journal_title, $abstract, $names, $authors, $contributors, $reverse_names,
    $topic, $note, $embargo_date, $genre, $family_names, $full_names,
    $username, $department;

  /**
   * Create the object properties from the MODS datastream
   *
   * @param type $pid
   * The PID of the object that you want to extract the MODS data from
   *
   * @return type
   */
  function __construct($object) {
    $this->pid = $object->id;
    $xmlstr = $object['MODS']->content;

    if ($xmlstr == NULL || strlen($xmlstr) < 5) {
      drupal_set_message(t('No MODS datastream found!'), 'warning');
      return " ";
    }
    $xml = new SimpleXMLElement($xmlstr);
    $xml->registerXPathNamespace('mods', 'http://www.loc.gov/mods/v3');

    $this->mods = $xml;

    $full_names = array();
    $names = array();
    $authors = array();
    $contributors = array();
    $family_names = array();
    $reverse = array();
    $i = 0;
    foreach ($xml->name as $name) {
      foreach ($name->namePart as $name_part) {
        if ((string) $name_part != '') {
          $np = (string) rtrim($name_part, '.'); //Strip periods off the end.

          $name = '';
          $full_name = '';
          $reverse_given = '';
          $reverse_family = '';

          if ($name_part['type'] == 'given') {
            $name .= (string) substr($np, 0, 1) . '. ';
            $full_name .= (string) $name_part;
            $reverse_given .= (string) rtrim($name_part, '.');
          }
          if ($name_part['type'] == 'family') {
            $name .= $np;
            $family_names[$i] = $np;
            $full_name .= ' ' . $np;
            $reverse_family .= (string) rtrim($name_part, '.');
          }

          $names[$i] = $name;
          $full_names[$i] = $full_name;
          $reverse[$i] = $reverse_family . ', ' . $reverse_given . '.';
        }
      }
      if (!empty($names[$i])) {
        $role = (string) @$name->role->roleTerm;
        if ($role == 'aut') {
          $role = 'author';
        }
        if ($role == 'author') {
          $authors[] = $names[$i];
        }
        else {
          $contributors[] = $names[$i];
        }

        if (!empty($role)) {
          $names[$i] .= ' (' . $role . ')';
        }
      }
      $i++;
    }
    $this->names = $names;
    $this->full_names = $full_names;
    $this->authors = $authors;
    $this->contributors = $contributors;
    $this->family_names = $family_names;

    $this->reverse_names = $reverse;

    $issn = $xml->xpath('//mods:identifier[@type="issn"]');
    $this->issn = str_replace(array("&", "=", ",", ";"), array('', '', '', ''), (string) $issn[0]);

    $this->date = $xml->part->date;
    if ($this->date == NULL) {
      $this->date = $xml->originInfo->dateCreated;
    }

    $volume = $xml->xpath('//mods:part/mods:detail[@type="volume"]/mods:number');
    if ($volume) {
      $this->volume = $volume[0];
    }

    $issue = $xml->xpath('//mods:part/mods:detail[@type="issue"]/mods:number');
    if ($issue) {
      $this->issue = $issue[0];
    }

    $start_page = $xml->xpath('//mods:extent[@unit="page"]/mods:start');
    if ($start_page) {
      $this->start_page = $start_page[0];
    }

    $title = $xml->xpath('//mods:title');
    if ($title) {
      $this->title = $title[0];
    }

    $sub_title = $xml->xpath('//mods:subTitle');
    if ($sub_title) {
      $this->sub_title = $sub_title[0];
    }

    $journal_title = $xml->xpath('//mods:relatedItem[@type="host"]//mods:title');
    if ($journal_title) {
      $this->journal_title = $journal_title[0];
    }

    $abstract = $xml->xpath('//mods:abstract');
    if ($abstract) {
      $this->abstract = $abstract[0];
    }

    $topic = array();
    foreach ($xml->subject as $subject) {
      $topic[] = $subject->topic;
    }
    $this->topic = $topic;

    $notes = array();
    foreach ($xml->note as $note) {
      $notes[] = $note;
    }
    $this->note = $notes;

    $embargo_date = $xml->xpath('//mods:dateOther[@type="embargo"]');
    if ($embargo_date) {
      $this->embargo_date = $embargo_date[0];
    }

    $genre = $xml->xpath('//mods:genre');
    if ($genre) {
      $this->genre = $genre;
    }

    $username = $xml->xpath('//mods:identifier[@type="u1"]');
    if ($username) {
      $this->username = $username;
    }

    $department = $xml->xpath('//mods:identifier[@type="u2"]');
    if ($department) {
      $this->department = $department;
    }
  }

  /**
   * Function to return citation details in a table-ready form
   *
   * @param string $type
   *   Type of reference that is being used. Default value is citation but
   * thesis and book are also valid. This parameter will change the values
   * that are returned when the render_view function is called
   *
   * @return type
   *   An array containing the headers and rows ready to put into a table describing
   * the reference
   */
  function render_view($type = 'citation') {
    global $base_url;
    $admin = user_access('administer scholarly research');
    $fedora_url = variable_get('fedora_base_url', 'http://localhost:8080/fedora');

    $headers = array('', '');
    $rows = array();

    $full_name = $this->topic;
    if (!empty($this->names)) {
      $names = implode(', ', $this->names);
    }
    else {
      $names = t('');
    }

    if (!empty($this->authors)) {
      $authors = implode(', ', $this->authors);
    }
    else {
      $authors = t('');
    }

    if (!empty($this->contributors)) {
      $contributors = implode(', ', $this->contributors);
    }
    else {
      $contributors = t('');
    }

    if (!empty($this->topic)) {
      $topics = implode($this->topic, ', ');
    }
    else {
      $topics = t('');
    }

    $genre = $this->genre;
    if (stripos((string) $genre[0], 'book') !== FALSE) {
      $type = 'book';
    }

    if ($this->embargo_date != NULL) {
      $embargo_date = date('r', tstamptotime((string) $this->embargo_date));
    }
    else {
      $embargo_date = NULL;
    }

    if (((string) $this->title != NULL) || $admin) {
      $rows[] = array('Title', $this->title);
    }

    if ($type == 'thesis') {
      if ($authors != NULL || $admin) {
        $rows[] = array('Authors', rtrim($authors, ', '));
      }
      if ($contributors != NULL || $admin) {
        $rows[] = array('Contributors', rtrim($contributors, ', '));
      }
      $rows[] = array('URL', l(t('Download PDF'), 'fedora/repository/' . $this->pid . '/PDF/PDF'));
    }
    else {
      if ($names != NULL || $admin) {
        $rows[] = array('Authors', rtrim($names, ', '));
      }
    }

    if (((string) $this->abstract != NULL) || $admin) {
      $rows[] = array('Abstract', $this->abstract);
    }

    if ($type == 'citation') {
      if (((string) $this->journal_title != NULL) || $admin) {
        $rows[] = array('Journal', $this->journal_title);
      }
    }

    if (((string) $this->date != NULL) || $admin) {
      $rows[] = array('Date', $this->date);
    }

    if ($type == 'citation') {
      if (((string) $this->volume != NULL) || $admin) {
        $rows[] = array('Volume', $this->volume);
      }

      if (((string) $this->issue != NULL) || $admin) {
        $rows[] = array('Issue', $this->issue);
      }

      if (((string) $this->start_page != NULL) || $admin) {
        $rows[] = array('Start page', $this->start_page);
      }

      if (((string) $this->issn != NULL) || $admin) {
        $rows[] = array('ISSN', $this->issn);
      }

      if (((string) $topics != NULL) || $admin) {
        $rows[] = array('Subjects', rtrim($topics, ', '));
      }

      if (((string) $this->note != NULL) || $admin) {
        $rows[] = array('Note', $this->note);
      }
    }

    if ($embargo_date != NULL) {
      $rows[] = array('Embargo date', $embargo_date);
    }

    return array(
      'headers' => $headers,
      'rows' => $rows,
    );
  }

}

/**
 * Generate a table containing the contents of the MODS stream (if one exists).
 *
 * Uses the Coins class to perform extraction.
 *
 * @param AbstractFedoraObject $object
 *   An object containing a MODS stream.
 *
 * @return array
 *   A renderable array.
 */
function islandora_scholar_details($object) {
  if (isset($object['MODS'])) {
    $coins = new Coins($object);
    extract($coins->render_view());
    return array(
      '#type' => 'item',
      '#title' => t('MODS Metadata'),
      '#markup' => theme('table', array(
        'header' => $headers,
        'rows' => $rows
      )),
    );
  }
}

