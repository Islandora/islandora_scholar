<?php

/**
 * @file
 * Utitlity functions.
 */

/**
 * Values to implement hook_islandora_bookmark_object_markup().
 *
 * @param AbstractObject $object
 *   Object representing Fedora Object
 *
 * @return string
 *   bookmark output
 */
function islandora_bibiliography_get_bookmark_output(AbstractObject $object) {
  $style = citeproc_default_style();
  $mods = islandora_bibliography_get_mods($object->id);
  if ($mods) {
    $bibliography_entry = citeproc_bibliography_from_mods($style, $mods);
  }
  $output = l($object->label, 'islandora/object/' . $object->id) . '<br />';
  $output .= $bibliography_entry;
  return $output;
}

/**
 * Values to Implement hook_islandora_bookmark_rss_item().
 *
 * @param AbstractObject $object
 *   Object representing Fedora Object
 *
 * @return array
 *   rss feed
 */
function islandora_bibliography_get_rss(AbstractObject $object) {

  // Global variables.
  global $base_url;

  // Create an associative array for the required elements
  // for a valid bookmark RSS item.
  $rss_item = array();

  // Get the citeproc default style.
  $style = citeproc_default_style();
  // Get the mods datastream.
  $mods = islandora_bibliography_get_mods($object->id);
  // If mods exists then create bibliography from mods.
  if ($mods) {
    $bibliography_entry = citeproc_bibliography_from_mods($style, $mods);

    // The title of the item will be the objects label.
    $rss_item['title'] = $object->label;
    // The link will be to the objects page.
    $rss_item['link'] = url('islandora/object/' . htmlspecialchars($object->id, ENT_QUOTES, 'utf-8'), array('absolute' => TRUE));
    // The description will be the bibliography entry.
    $rss_item['description'] = $bibliography_entry;

    // Get the site name to be the value of the source.
    $rss_source = variable_get('site_name', "Default site name");

    // Set the default source attribute to be to be the base url.
    $rss_item['items'] = array(
      array(
        'key' => 'source',
        'value' => $rss_source, 'attributes' => array('url' => $base_url)),
    );
  }

  // Returns the the rss default item.
  return $rss_item;
}
