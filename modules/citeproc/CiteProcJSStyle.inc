<?php

/**
 * @file
 *
 * Defines a class that models a CSL version 1.0 style.
 *
 * http://gsl-nagoya-u.net/http/pub/citeproc-doc.html#data-input
 */

/**
 * Models a Style
 */
class CiteProcJSStyle {

  /**
   * Uniquely identifies this object
   *
   * @var array
   */
  private $id;

  /**
   * The CSL style as an XML string.
   *
   * @var string
   */
  private $data;

  /**
   * Create a CiteProcStyle instance.
   * @param mixed $style either a DOMDocument or string containing the XML for the style.
   */
  public function __construct($style) {
    module_load_include('inc', 'citeproc', 'CiteProcJS');
    
    $this->id = citeproc_get_id();
    
    if ($style instanceof DOMDocument) {
      $this->data = $style->saveXML();
    }
    else { //Assume it's a string...  Possibly save parsing?
      $this->data = trim($style);
    }
    
    $citeproc = CiteProcJS::getInstance();
    $citeproc->addStyle($this);
  }

  /**
   * Beware of Cloning.
   */
  public function __clone() {
    // The id doesn't change for this object when cloned, its assumed to be the exactly same since its internal data
    // can not be changed. Clone has not affect.
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
   * A JSON formatted CSL Style, for use with citeproc-js.
   *
   * @return string
   */
  public function toJSON() {
    return $this->data;
  }
}
