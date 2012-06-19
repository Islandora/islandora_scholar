<?php

/**
 * @file
 *
 * Defines a class that models citeproc-js Bibliography.
 *
 * http://gsl-nagoya-u.net/http/pub/citeproc-doc.html#data-input
 */
module_load_include('inc', 'citeproc', 'CiteProcJSCitation');
module_load_include('inc', 'citeproc', 'CiteProcJSStyle');
module_load_include('inc', 'citeproc', 'CiteProcJSAbbreviation');

/**
 * Models a Citation.
 */
class CiteProcJSBibliography {

  /**
   * Uniquely identifies this object
   *
   * @var array
   */
  private $id;

  /**
   * A array of CiteProcCitations's to include in this citation.
   *
   * @var array
   */
  private $citations;
  
  /**
   * The style CSL to apply to this bibliography.
   *
   * @var CiteProcStyle
   */
  private $style;
  /**
   * The abbreviations to apply to this bibliography.
   * 
   * @var CiteProcAbbreviation 
   */
  private $abbreviation;

  /**
   * Create a CiteProcBibliography instance.
   * 
   * @param CiteProcStyle $style
   *   The CSL style to render with. Must be CSL version 1.0.
   * @param CiteProcAbbreviation $abbreviation
   *   Any abbreviations we wan't to apply.
   */
  public function __construct(CiteProcJSStyle $style, CiteProcJSAbbreviation $abbreviation = NULL) {
    $this->id = citeproc_get_id();
  
    $this->style = $style;
    $this->abbreviation = $abbreviation;
    $this->citations = array();
  }

  /**
   * Clone this CiteProcBibliography.
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
   * Adds a CiteProcCitation, citations can have multiple items.
   *
   * @param CiteProcCitation $citation
   *   The item to add to this citation.
   */
  public function addCitation(CiteProcJSCitation $citation) {
    $citation_id = $citation->getID();
    $this->citations[$citation_id] = $citation;
  }

  /**
   * Gets the JSON repersentation for this object.
   * 
   * @return array
   *   Data that will be converted to JSON and used to render the bibliography.
   */
  public function toJSON() {
    $data = array(
      'id' => $this->id,
      'style' => $this->style->getID(),
      'abbreviation' => isset($this->abbreviation) ? $this->abbreviation->getID() : NULL,
      'items' => array()
    );
    foreach ($this->citations as $id => $citation) {
      $data['items'][$id] = $citation->toJSON();
    }
    return $data;
  }

  /**
   * Renders an bibliography.
   *
   *
   * @param CiteProcStyle $style
   *   The CSL style to render with. Must be CSL version 1.0.
   * @param CiteProcAbbreviation $abbreviation
   *   Any abbreviations we wan't to apply.
   *
   * @return string
   *   An html string where the in-text citation will be rendered into.
   */
  public function render() {
    module_load_include('inc', 'citeproc', 'CiteProcJS');
    $citeproc = CiteProcJS::getInstance();
    $citeproc->addBibliography($this);
    return '<div id="' . $this->id . '" class="citeproc-bibliography">[Page generation failure.  The bibliography processor requires a browser with Javascript enabled.]</div>';
  }
}
