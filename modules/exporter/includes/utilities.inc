<?php

/**
 * @file
 * Utility functions.
 */

/**
 * Gets the count of exporting objects.
 *
 * @param object $query_processor
 *   The query processor object.
 *
 * @return int
 *   The count of pids that are apart of the results.
 */
function get_count_of_exporting_objects($query_processor) {
  $qp = $query_processor;
  $old_params = $qp->solrParams;
  $qp->solrParams['fl'] = 'PID';
  $qp->solrStart = 0;
  $qp->solrLimit = 10000;
  $qp->executeQuery();
  $solr_results = $qp->islandoraSolrResult;
  $result_count = $solr_results['response']['numFound'];
  return $result_count;
}
