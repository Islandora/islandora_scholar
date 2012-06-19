<?php

/**
 * @file
 *
 * Defines a class that models citeproc-js Abbreviations.
 */

/**
 * Models a citeproc-js abbreviation data structure. This must be registered with the CiteProc singleton to be included
 * on the rendered page.
 */
class CiteProcJSAbbreviation {

  /**
   * A unique identifier for this object.
   *
   * @var hash
   */
  private $id;

  /**
   * Abbreviations for the container-title CSL variable.
   *
   * A array where the key repersents the full string, and the value repersents the abbreviation.
   *
   * @var array
   */
  private $containerTitle;

  /**
   * Abbreviations for the collection-title CSL variable.
   *
   * A array where the key repersents the full string, and the value repersents the abbreviation.
   *
   * @var array
   */
  private $collectionTitle;

  /**
   * Abbreviations for the authority CSL variable.
   *
   * A array where the key repersents the full string, and the value repersents the abbreviation.
   *
   * @var array
   */
  private $authority;

  /**
   * Abbreviations for the institution CSL variable.
   *
   * A array where the key repersents the full string, and the value repersents the abbreviation.
   *
   * @var array
   */
  private $institution;

  /**
   * Abbreviations for the title CSL variable.
   *
   * A array where the key repersents the full string, and the value repersents the abbreviation.
   *
   * @var array
   */
  private $title;

  /**
   * Abbreviations for the publisher CSL variable.
   *
   * A array where the key repersents the full string, and the value repersents the abbreviation.
   *
   * @var array
   */
  private $publisher;

  /**
   * Abbreviations for the publisher-place CSL variable.
   *
   * A array where the key repersents the full string, and the value repersents the abbreviation.
   *
   * @var array
   */
  private $publisherPlace;

  /**
   * Not really sure... No CSL variable exists for this.
   *
   * A array where the key repersents the full string, and the value repersents the abbreviation.
   *
   * @var array
   */
  private $hereInAfter;

  /**
   * Creates a CiteProcAbbreviation instance.
   */
  public function __construct() {
    $this->id = uniqid();
  }

  /**
   * Clone this CiteProcAbbreviation instance.
   */
  public function __clone() {
    $this->id = uniqid(); // Preserve uniqueness.
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
   * Adds an abbreviation for the container-title CSL variable field.
   *
   * @param string $full
   *   The full value of this CSL variable field.
   * @param string $abbreviation
   *   The abbreviated value of this CSL variable field, it will be
   *   used to replace the full string value in some citations.
   */
  public function addContainerTitle($full, $abbreviation) {
    $this->containerTitle[$full] = $abbreviation;
  }

  /**
   * Adds an abbreviation for the collection-title CSL variable field.
   *
   * @param string $full
   *   The full value of this CSL variable field.
   * @param string $abbreviation
   *   The abbreviated value of this CSL variable field, it will be
   *   used to replace the full string value in some citations.
   */
  public function addCollectionTitle($full, $abbreviation) {
    $this->collectionTitle[$full] = $abbreviation;
  }

  /**
   * Adds an abbreviation for the authority CSL variable field.
   *
   * @param string $full
   *   The full value of this CSL variable field.
   * @param string $abbreviation
   *   The abbreviated value of this CSL variable field, it will be
   *   used to replace the full string value in some citations.
   */
  public function addAuthority($full, $abbreviation) {
    $this->authority[$full] = $abbreviation;
  }

  /**
   * Adds an abbreviation for the institution CSL variable field.
   *
   * @param string $full
   *   The full value of this CSL variable field.
   * @param string $abbreviation
   *   The abbreviated value of this CSL variable field, it will be
   *   used to replace the full string value in some citations.
   */
  public function addInstitution($full, $abbreviation) {
    $this->institution[$full] = $abbreviation;
  }

  /**
   * Adds an abbreviation for the title CSL variable field.
   *
   * @param string $full
   *   The full value of this CSL variable field.
   * @param string $abbreviation
   *   The abbreviated value of this CSL variable field, it will be
   *   used to replace the full string value in some citations.
   */
  public function addTitle($full, $abbreviation) {
    $this->title[$full] = $abbreviation;
  }

  /**
   * Adds an abbreviation for the publisher CSL variable field.
   *
   * @param string $full
   *   The full value of this CSL variable field.
   * @param string $abbreviation
   *   The abbreviated value of this CSL variable field, it will be
   *   used to replace the full string value in some citations.
   */
  public function addPublisher($full, $abbreviation) {
    $this->publisher[$full] = $abbreviation;
  }

  /**
   * Adds an abbreviation for the publisher-place CSL variable field.
   *
   * @param string $full
   *   The full value of this CSL variable field.
   * @param string $abbreviation
   *   The abbreviated value of this CSL variable field, it will be
   *   used to replace the full string value in some citations.
   */
  public function addPublisherPlace($full, $abbreviation) {
    $this->publisherPlace[$full] = $abbreviation;
  }

  /**
   * Not sure what this does...
   *
   * @param string $full
   *   The full value of this CSL variable field.
   * @param string $abbreviation
   *   The abbreviated value of this CSL variable field, it will be
   *   used to replace the full string value in some citations.
   */
  public function addHereInAfter($full, $abbreviation) {
    $this->hereInAfter[$full] = $abbreviation;
  }

  /**
   * Adds an abbreviation for the container-title CSL variable field.
   *
   * @param string $full
   *   The full value of this CSL variable field.
   * @param string $abbreviation
   *   The abbreviated value of this CSL variable field, it will be
   *   used to replace the full string value in some citations.
   */
  public function toJSON() {
    return array(
      $this->id => array(
        'container-title' => $this->containerTitle,
        'collection-title' => $this->collectionTitle,
        'authority' => $this->authority,
        'institution' => $this->institution,
        'title' => $this->title,
        'publisher' => $this->publisher,
        'publisher-place' => $this->publisherPlace,
        'hereinafter' => $this->hereInAfter,
      )
    );
  }

}