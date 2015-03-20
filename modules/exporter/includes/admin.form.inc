<?php

/**
 * @file
 * Admin form for the citation_exporter.module
 */

/**
 * Defines the admin settings form.
 *
 * @return array
 *   Drupal form definition.
 */
function citation_exporter_admin_form() {
  $max_execution_time = (ini_get('max_execution_time') ? ini_get('max_execution_time') : "Unable to get max execution time.");
  $form['citation_exporter_max_objects_for_pdf_export'] = array(
    '#type' => 'textfield',
    '#title' => t('The maximium number of objects that can be exported during a single citation PDF export.'),
    '#description' => t('You may be able to increase this number. It is very important that the change should be
                        tested to make sure the pdf generation still functions up to the increased object limit.',
      array('execution_time' => $max_execution_time)),
    '#element_validate' => array('element_validate_integer_positive'),
    '#default_value' => variable_get('citation_exporter_max_objects_for_pdf_export', 1200),
    '#size' => 5,
  );
  return system_settings_form($form);
}
