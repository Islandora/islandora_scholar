<?php

/**
 * @file
 *
 * Functions for including the Manage Bibliography form.
 */

/**
 * This form is called from the menu callback for the 'My Bibliography' page.
 *
 * From here the user can add/remove and export citations.
 *
 * @param array $form_state
 *   The drupal form state.
 *
 * @return array
 *   The drupal form.
 */
function islandora_bibliography_form(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_bibliography', 'Bibliography');
  $pids = Bibliography::GetCitations();
  if (empty($pids)) {
    islandora_bibilgraphy_form_empty_page();
  }
  else {
    module_load_include('inc', 'csl', 'CSL');
    drupal_add_css(drupal_get_path('module', 'islandora_bibliography') . '/css/bibliography.css');
    $styles = CSL::GetNames();
    return array(
      'fieldset' => array(
        '#type' => 'fieldset',
        '#title' => t('Citations'),
        'step_1' => array(
          '#markup' => t('Select or remove your citations'),
          '#prefix' => '<h3>',
          '#suffix' => '</h3>'
        ),
        'table' => islandora_bibilgraphy_form_table($pids),
        'remove' => array(
          '#type' => 'submit',
          '#value' => t('Remove Selected'),
          '#submit' => array('islandora_bibliography_form_remove')
        ),
        'step_2' => array(
          '#markup' => t('Create bibliography or export references'),
          '#prefix' => '<h3>',
          '#suffix' => '</h3>'
        ),
        'format' => array(
          '#title' => t('Format'),
          '#type' => 'select',
          '#length' => 50,
          '#options' => array(
            t('Bibliography') => array(
              'HTML' => t('Preview/HTML'),
            ),
          ),
        ),
        'style' => array(
          '#title' => t('Style'),
          '#type' => 'select',
          '#options' => $styles
        ),
        'export' => array(
          '#type' => 'submit',
          '#action' => 'export',
          '#export' => 'selected',
          '#value' => t('Process Selected'),
          '#submit' => array('islandora_bibliography_form_export')
        )
      )
    );
  }
}

/**
 * The form to render when no citations are in the bibliography.
 *
 * @return array
 *   The drupal form
 */
function islandora_bibilgraphy_form_empty_page() {
  drupal_set_message(t('You have no citations in your bibliography.'));
  return array(
    'redirect' => array(
      '#prefix' => '<p>',
      '#value' => l(t('Return to the Home Page.'), ''),
      '#suffix' => '</p>'
    )
  );
}

/**
 * Generates the table for the bibliography form.
 *
 * @param array $pids
 *   A collection of citations within the bibliography.
 *
 * @return array
 *   The drupal form table definition.
 */
function islandora_bibilgraphy_form_table(array $pids) {
  $table = array(
    '#header' => array(theme('table_select_header_cell'), t('Title'), t('Author(s)'), ''),
    '#theme' => THEME_BIBLIOGRAPHY_FORM_TABLE,
    '#tree' => TRUE,
    'rows' => array(),
    'selections' => array(
      '#type' => 'checkboxes',
      '#options' => array_fill_keys($pids, ''),
    ),
  );
  $row_count = 0;
  $rows = &$table['rows'];
  foreach ($pids as $pid) {
    list($title, $authors) = islandora_bibilgraphy_form_get_title_authors($pid);
    $rows[] = array(
      '#pid' => $pid,
      'title' => array('#markup' => l(check_plain($title), "islandora/object/$pid")),
      'author' => array('#markup' => check_plain($authors)),
      'remove' => array(
        '#type' => 'submit',
        '#value' => t('Remove'),
        '#name' => 'remove-citation' . $row_count, // Give a unique name to each button
        '#pid' => $pid,
        '#submit' => array('islandora_bibliography_form_remove_citation_submit')
      )
    );
    $row_count++;
  }
  return $table;
}

/**
 * Get the title and author of a given citation.
 *
 * @param string $pid
 *   A citations within the bibliography.
 *
 * @return array
 *   Where the first key is the title and the second is the authors.
 */
function islandora_bibilgraphy_form_get_title_authors($pid) {
  $title = $authors = 'Missing';
  $mods = islandora_bibilgraphy_form_get_mods($pid);
  if ($mods) {
    $mods = simplexml_import_dom($mods);
    $mods->registerXPathNamespace('mods', 'http://www.loc.gov/mods/v3');
    $title = implode(' ', $mods->xpath('/mods:mods/mods:titleInfo/mods:title'));
    $authors_xpath = $mods->xpath('/mods:mods/mods:name/mods:role[mods:roleTerm = "author"]/../mods:namePart[@type="family"]');
    $authors = is_array($authors_xpath) ? implode(' ', $authors_xpath) : null;
  }
  return array($title, $authors);
}

/**
 * Removes one citation from the bibliography.
 *
 * @param array $form
 *   The drupal form.
 * @param array $form_state
 *   The drupal form state.
 */
function islandora_bibliography_form_remove_citation_submit(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_bibliography', 'Bibliography');
  $pid = $form_state['clicked_button']['#pid'];
  Bibliography::RemoveCitation($pid);
  drupal_set_message(t('Removed citation'));
}

/**
 * Previews the selected citations from the bibliography.
 *
 * @param array $form
 *   The drupal form.
 * @param array $form_state
 *   The drupal form state.
 */
function islandora_bibliography_form_preview(array $form, array &$form_state) {
  module_load_include('inc', 'citeproc', 'CiteProcJSBibliography');
  drupal_add_css(drupal_get_path('module', 'islandora_bibliography') . '/css/export.css');
  $selections = $form_state['values']['table']['selections'];
  $style = citeproc_style($form_state['values']['style']);
  $bibliography = new CiteProcJSBibliography($style);
  $selections = array_filter($selections);
  $selections = array_keys($selections);
  foreach ($selections as $pid) {
    $mods = islandora_bibilgraphy_form_get_mods($pid);
    if ($mods) {
      $citation = citeproc_citation_from_mods($mods);
      $bibliography->addCitation($citation);
    }
  }
  if (isset($selections[0])) {
    $output = $bibliography->render();
  }
  else {
    drupal_set_message(t('No citations were selected!'), 'warning');
  }
  $output .= l(t('Return to the Bibliography.'), 'bibliography');
  //print theme('page', $output, FALSE);
  //exit();
  return $output;
}

/**
 * Removes the selected citations from the bibliography.
 *
 * @param array $form
 * @param array $form_state
 */
function islandora_bibliography_form_remove(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_bibliography', 'Bibliography');
  $selections = array_filter($form_state['values']['table']['selections']);
  $pids = array_keys($selections);
  Bibliography::RemoveCitations($pids);
  drupal_set_message(format_plural(count($pids), 'Removed 1 citation', 'Removed @count citations'));
}

/**
 * Exports the selected pids to file.
 *
 * @param array $form
 * @param array $form_state
 */
function islandora_bibliography_form_export(array &$form, array &$form_state) {
  $selections = array_filter($form_state['values']['table']['selections']);
  if ($selections != NULL) {
    $pids = array_keys($selections);
    if ($form_state['values']['format'] == 'HTML') {
      $form = array(
        '#markup' => islandora_bibliography_form_preview($form, $form_state),
      );
      $form_state['redirect'] = FALSE;
    }
  }
  else {
    drupal_set_message(t('No citations were selected!'), 'warning');
  }
}

/**
 * Gets the MOD from an object.
 *
 * Helper function for getting the MODS metadata.
 *
 * @param string $pid
 *   The fedora object to get the MODS from.
 *
 * @return DOMDocument
 *   An MODS document if found and non-empty, FALSE otherwise.
 */
function islandora_bibilgraphy_form_get_mods($pid) {
  $object = islandora_object_load($pid);

  if (!isset($object)) {
    return FALSE;
  }

  $mods_str = trim($object['MODS']->content);
  if ($mods_str) {
    $doc = new DOMDocument();
    $doc->loadXML($mods_str);
    return $doc;
  }
  else {
    return FALSE;
  }
}

/**
 * Theme's a form table for this module.
 *
 * @param array $args
 * @return sting
 *   HTML that renders a table of settings for datastreams.
 */
function theme_islandora_bibliography_form_table(array $args) {
  $element = $args['element'];

  $rows = array();
  foreach (element_children($element['rows']) as $child) {
    $setting = $element['rows'][$child];
    $pid = $setting['#pid'];
    $fields = array(
      drupal_render($element['selections'][$pid]) // First field is a checkbox
    );
    foreach (element_children($setting) as $property) {
      $field = $setting[$property];
      $fields[] = drupal_render($field);
    }
    $rows[] = array(
      'data' => $fields,
      'class' => isset($setting['#attributes']['class']) ? $setting['#attributes']['class'] : NULL
    );
  }
  $attributes = isset($element['#id']) ? array('id' => $element['#id']) : NULL;
  return theme('table', 
    array(
      'header' =>$element['#header'],
      'rows' => $rows,
      'attributes' => $attributes,
      //'caption' => $caption,
   ));
}
