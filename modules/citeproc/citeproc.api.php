<?php

/**
 * @file
 * Document interfaces/hooks exposed by this module.
 */

/**
 * Allow other modules to alter variables being output into citations.
 *
 * @param array $output
 *   The associative array of variables being passed to citeproc.
 * @param SimpleXMLElement $mods
 *   The MODS from which the original output was produced.
 */
function hook_convert_mods_to_citeproc_jsons_alter(array &$output, SimpleXMLElement $mods) {

}
