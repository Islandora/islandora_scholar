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
  $form['citation_export_max_objects_for_pdf_export'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximium number of objects allowed per PDF citation export.'),
    '#description' => t('The number of objects that can be exported to PDF,
                        this number should be something that is achievable within the server\'s
                        max execution time which is currently set to @execution_limit. For
                        reference 30 seconds can handle up to 1200 objects.',
      array('execution_time' => $max_execution_time)),
    '#element_validate' => array('element_validate_integer_positive'),
    '#default_value' => variable_get('citation_export_max_objects_for_pdf_export', 1200),
    '#size' => 5,
  );
  return system_settings_form($form);
}
