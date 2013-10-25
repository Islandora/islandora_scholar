<?php
/**
 * @file
 * Defines ScholarBatchImporter.
 */

abstract class ScholarBatchImporter extends IslandoraBatchImporter {

  /**
   * Get an identifier.
   *
   * @param IslandoraTuque $tuque
   *   a tuque object
   * @param string $namespace
   *   a default namespace.  We use this namespace if we can't find a Collection
   * Policy or the collection solution pack is not installed.
   *
   * @return string
   *   An identifier for the repo.
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
      module_load_include('inc', 'islandora_basic_collection', 'collection_policy');
      $policy_stream = $parent['COLLECTION_POLICY'];
      $policy = new CollectionPolicy($policy_stream->content);

      foreach ($policy->getContentModels() as $pid => $info) {
        if ($pid == 'ir:citationCModel' && isset($info['namespace'])) {
          return $info['namespace'];
        }
      }
    }
    return $default_namespace;
  }
}