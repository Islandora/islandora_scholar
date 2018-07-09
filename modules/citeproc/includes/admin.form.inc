<?php

/**
 * @file
 * Admin form for the citeproc.module
 */

/**
 * Defines the admin settings form.
 *
 * @return array
 *   Drupal form definition.
 */
function citeproc_admin_form() {
  module_load_include('inc', 'citeproc', 'includes/utilities');

  $form = array();
  $options = array();
  $options[CITEPROC_BACKEND_V1] = 'citeproc-php v1';
  if (citeproc_v2_supported()) {
    $options[CITEPROC_BACKEND_V2] = 'citeproc-php v2';
  }
  $form['citeproc_backend'] = array(
    '#type' => 'radios',
    '#title' => t('What version of citeproc library should be used?'),
    '#default_value' => variable_get('citeproc_backend', citeproc_v2_supported() ? CITEPROC_BACKEND_V2 : CITEPROC_BACKEND_V1),
    '#options' => $options,
    '#description' => t('V2 of citeproc-php is recommended when your PHP version supports it.'),
  );
  return system_settings_form($form);
}
