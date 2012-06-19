<?php

/**
 * @file
 *
 * Defines a class that models citeproc-js Items.
 *
 * http://gsl-nagoya-u.net/http/pub/citeproc-doc.html#data-input
 */

/**
 * Models a citeproc-js item data structure. This must be registered with the CiteProc singleton to be included
 * on the rendered page.
 */
class CiteProcJSItem {

  /**
   * A unique identifier for this object.
   *
   * @var hash
   */
  private $id;

  /**
   * Valid name CSL variable fields.
   *
   * @var array
   */
  static $NAMES = array(
    'author',
    // use for series editor
    'collection-editor',
    'composer',
    // use for book author when citing a book chapter
    'container-author',
    'editor',
    'editorial-director',
    'interviewer',
    'original-author',
    'recipient',
    'translator'
  );

  /**
   * Valid date CSL variable fields.
   *
   * @var array
   */
  static $DATES = array(
    'accessed',
    'container',
    'event-date',
    'issued',
    'original-date',
    'submitted',
  );

  /**
   * Valid number CSL variable fields.
   *
   * @var array
   */
  static $NUMBERS = array(
    "edition",
    "issue",
    "number",
    "number-of-volumes",
    "volume",
  );

  /**
   * Valid simple CSL variable fields.
   *
   * @var array
   */
  static $VARIABLES = array(
    "abstract",
    // notes made by a reader about the content of the resource
    "annote",
    // the name of the archive
    "archive",
    // the location within an archival collection (for example, box and folder)
    "archive_location",
    // the place of the archive
    "archive-place",
    // issuing authority (for patents) or judicial authority (such as court
    // for legal cases)
    "authority",
    "call-number",
    "chapter-number",
    // the number used for the in-text citation mark in numeric styles
    "citation-number",
    // the label used for the in-text citation mark in label styles
    "citation-label",
    // collection number; for example, series number
    "collection-number",
    // the tertiary title for the cited item; for example, a series title
    "collection-title",
    // the secondary title for the cited item (book title for book chapters,
    // journal title for articles, etc.).
    "container-title",
    "DOI",
    // an edition description
    "edition",
    // the name or title of a related event such as a conference or hearing
    "event",
    // the location or place for the related event
    "event-place",
    // The number of a preceding note containing the first reference to this
    // item. Relevant only for note-based styles, and null for first references.
    "first-reference-note-number",
    "genre",
    "ISBN",
    // the issue number for the container publication
    "issue",
    // For legislation and patents; scope of geographic relevance for a document.
    "jurisdiction",
    "keyword",
    // a description to locate an item within some larger container or
    // collection; a volume or issue number is a kind of locator, for example.
    "locator",
    // medium description (DVD, CD, etc.)
    "medium",
    // a short inline note, often used to refer to additional details of the resource
    "note",
    // a document number; useful for reports and such
    "number",
    // refers to the number of pages in a book or other document
    "number-of-pages",
    // refers to the number of items in multi-volume books and such
    "number-of-volumes",
    // the name of the original publisher
    "original-publisher",
    // the place of the original publisher
    "original-publisher-place",
    // title of a related original version; often useful in cases of translation
    "original-title",
    // the range of pages an item covers in a containing item
    "page",
    // the first page of an item within a containing item
    "page-first",
    // the name of the publisher
    "publisher",
    // the place of the publisher
    "publisher-place",
    // for related referenced resources; used for legal case histories, but
    // may be relevant for other contexts.
    "references",
    // a section description (for newspapers, etc.)
    "section",
    // the (typically publication) status of an item; for example "forthcoming"
    "status",
    // the primary title for the cited item
    "title",
    "URL",
    "version",
    // volume number for the container periodical
    "volume",
    // The year suffix for author-date styles; e.g. the 'a' in '1999a'.
    "year-suffix",
  );

  /**
   *
   * @var array
   */
  private $data;

  /**
   * Create a CiteProcItem.
   */
  public function __construct() {
    $this->id = citeproc_get_id();
    $this->data = array();
  }

  /**
   * Clone this CiteProcItem.
   */
  public function __clone() {
    $this->id = citeproc_get_id(); // Preserve uniqueness.
  }

  /**
   * Gets this objects identifier.
   *
   * @return hash
   *   This objects ID.
   */
  public function getID() {
    return $this->id;
  }

  /**
   * Filters an array down to the provided fields.
   *
   * @param array $array
   *   The array to filter.
   * @param array $fields
   *   The fields to keep.
   *
   * @return array
   *   The filtered array.
   */
  private function filterFields(array &$array, array &$fields) {
    foreach ($array as $key => $value) {
      if (array_search($key, $fields) === FALSE) {
        unset($array[$key]);
      }
    }
    return $array;
  }

  /**
   * Adds a name type CSL variable field.
   *
   * @param string $field
   *   One of the self::$NAMES,
   * @param array $name
   *   An array that conforms to the following layout.
   *    array(
   *      'family' => string,
   *      'given' => string,
   *      'literal' => string,
   *      'parse-names' => string, either 'true' or 'false',
   *      'suffix' => string,
   *      'comma-suffix' => string, either 'true' or 'false',
   *      'dropping-particle' => string,
   *      'non-dropping-particle' => string,
   *      'static-ordering' => if specificed it should be the integer value 1
   *    )
   *
   */
  public function addName($field, array $name) {
    if (array_search($field, self::$NAMES) !== FALSE) {
      $fields = array(
        'family',
        'given',
        'literal',
        'parse-names',
        'suffix',
        'comma-suffix',
        'dropping-particle',
        'non-dropping-particle',
        'static-ordering'
      );
      $this->filterFields($name, $fields);
      $this->data[$field] = isset($this->data[$field]) ? $this->data[$field] : array(); // Initalize.
      $this->data[$field] = array_merge($this->data[$field], $name);
    }
  }

  /**
   * Sets a date type CSL variable field.
   *
   * @todo finish documenting
   *
   * @param string $field
   *   One of the self::$NAMES,
   */
  public function setDate($field, array $date) {
    if (array_search($field, self::$DATES) !== FALSE) {
      $fields = array(
        'date-parts',
        'literal',
        'season',
        'circa'
      );
      $this->filterFields($date, $fields);
      $this->data[$field] = isset($this->data[$field]) ? $this->data[$field] : array(); // Initalize.
      $this->data[$field] = array_merge($this->data[$field], $date);
    }
  }

  /**
   * Sets a number type CSL variable field.
   *
   * @param string $field
   *   One of the self::$NAMES,
   */
  public function setNumber($field, $number) {
    if (array_search($field, self::$NUMBERS) !== FALSE) {
      $this->data[$field] = $value;
    }
  }

  /**
   * Sets a simple CSL variable field.
   *
   * @param string $field
   *   A simple CSL variable field.
   * @param string $value
   *   The value for the given field.
   */
  public function setVariable($field, $value) {
    if (array_search($field, self::$VARIABLES) !== FALSE) {
      $this->data[$field] = $value;
    }
  }

  /**
   * Merges a raw repersentation of a CiteProcItem with this, overriding an previously set values.
   * 
   * @param array $raw 
   */
  public function set($raw) {
    $this->data = array_merge($this->data, $raw);
  }

  /**
   * Gets the JSON repersentation of this object.
   *
   * @return string
   *   The JSON repersentation of this object.
   */
  public function toJSON() {
    $data = $this->data;
    $data['id'] = $this->id;
    foreach ($data as $key => $value) {
      if (empty($value)) {
        unset($data[$key]);
      }
    }
    return $data;
  }
}
