<?php

/**
 * @file
 *
 * Defines a class that models citeproc-js Citation.
 *
 * http://gsl-nagoya-u.net/http/pub/citeproc-doc.html#data-input
 */
module_load_include('inc', 'citeproc', 'CiteProcJSItem');
module_load_include('inc', 'citeproc', 'CiteProcJSStyle');
module_load_include('inc', 'citeproc', 'CiteProcJSAbbreviation');

/**
 * Models a Citation.
 */
class CiteProcJSCitation {

  /**
   * Uniquely identifies this object
   *
   * @var array
   */
  private $id;

  /**
   * A array of CiteProcItem's to include in this citation.
   *
   * @var array
   */
  private $items;

  /**
   * Create a CiteProcCitation instance.
   */
  public function __construct() {
    $this->id = citeproc_get_id();
    $this->items = array();
  }

  /**
   * Clone this CiteProcCitation.
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
   * Adds a CiteProcItem to this citation, citations can have multiple items.
   *
   * @param CiteProcItem $item
   *   The item to add to this citation.
   * @param array $config
   *   An array that conforms to the following layout.
   *      locator: a string identifying a page number or other pinpoint location or range within the resource;
   *      label: a label type, indicating whether the locator is to a page, a chapter, or other subdivision of
   *             the target resource. Valid labels are defined in the  CSL specification.
   *      suppress-author: if true, author names will not be included in the citation output for this cite;
   *      author-only: if true, only the author name will be included in the citation output for this cite -- this
   *                   optional parameter provides a means for certain demanding styles that require the processor
   *                   output to be divided between the main text and a footnote.
   *      prefix: a string to print before this cite item;
   *      suffix: a string to print after this cite item.
   */
  public function addItem(CiteProcJSItem $item, array $config = array()) {
    module_load_include('inc', 'citeproc', 'CiteProcJS');
    $item_id = $item->getID();
    $data = array('id' => $item_id);
    $this->items[] = array_merge($data, $config);
    $citeproc = CiteProcJS::getInstance();
    $citeproc->addItem($item);
  }

  /**
   * Creates a JSON repersentation of this object.
   */
  public function toJSON() {
    return array(
      'citationItems' => $this->items,
      'properties' => array(
        'noteIndex' => 0
      )
    );
  }

  /**
   * Renders an in-text citation. If this Citation is going to be part of a bibliography.
   *
   * If you tell it to render without a bibili
   *
   * @param CiteProcStyle $style
   *   The CSL style to render with. Must be CSL version 1.0.
   * @param CiteProcAbbreviation $abbreviation
   *   Any abbreviations we wan't to apply.
   * @param int $note_index
   *   Indicates the footnote number in which the citation is located within the document.
   *   Citations within the main text of the document have a noteIndex of zero.
   *
   * @return string
   *   An html string where the in-text citation will be rendered into.
   */
  public function render(CiteProcJSStyle $style, CiteProcJSAbbreviation $abbreviation = NULL, $note_index = 0) {
    /**
     * @todo Create render object determine how to deal with the difference of interfaces
     * This will only get populated if CiteProcBibliography::render() gets called.
     */
    return '<span id="' . $this->id . '" class="citeproc-in-text"></span>';
  }
}
