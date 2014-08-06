<?php

/**
 * @file
 * Menu callbacks.
 */

/**
 * Retrieve a citation for an object's MODS DS.
 *
 * @param AbstractObject $object
 *   The object to get the citation for.
 * @param string $style
 *   The style to get the citation in.
 */
function islandora_scholar_get_citation(AbstractObject $object, $style) {
  drupal_json_output(
    array(
      'citation' => citeproc_bibliography_from_mods(
        citeproc_style($style),
        $object['MODS']->content
      ),
    )
  );
}
