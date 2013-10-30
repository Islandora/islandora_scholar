<?php
/**
 * @file
 * Defines ScholarBatchImporter.
 */

abstract class ScholarBatchImporter extends IslandoraBatchImporter {

  /**
   * Get an identifier by checking for a Collection Policy defined namespace.
   *
   * If we can't find one we use the $namespace passed to this function.
   *
   * @see IslandoraBatchImporter::getIdentifier()
   */
  protected function getIdentifier(IslandoraTuque $tuque, $namespace) {
    static $cm_namespace;
    if (!isset($cm_namespace)) {
      $cm_namespace = $this->getNamespaceFromCollectionPolicy($namespace);
    }
    return parent::getIdentifier($tuque, $cm_namespace);
  }

  /**
   * Returns the namespace configured in the collection policy if it exists.
   *
   * If the namespace is not defined for the ir:citationCModel it will return
   * the default namespace provided.
   *
   * @param string $default_namespace
   *   will return the default namespace if it can't find one in the collection
   * policy or the collection solution pack is not installed.
   *
   * @return string
   *   the namespace to use
   */
  protected function getNamespaceFromCollectionPolicy($default_namespace) {
    if (!module_exists('islandora_basic_collection')) {
      return $default_namespace;
    }
    $parent = islandora_object_load($this->parentPid);
    if (isset($parent['COLLECTION_POLICY'])) {
      $policy_stream = $parent['COLLECTION_POLICY'];
      $policy = new CollectionPolicy($policy_stream->content);
      $cmodels = $policy->getContentModels();
      $namespace = (empty($cmodels['ir:citationCModel']['namespace'])) ?
        NULL : $cmodels['ir:citationCModel']['namespace'];
    }
    return (empty($namespace)) ? $default_namespace : $namespace;
  }
}
