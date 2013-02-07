<?php

/**
 * @file
 * Administration form.
 */

function bibutils_admin_form($form, &$form_state) {
  $form['bibutils_endx2xml_path'] = array(
    '#type' => 'textarea',
    '#title' => t('Endnote XML to MODS executable'),
    '#description' => t('The path to endx2xml.'),
    '#default_value' => variable_get('bibutils_endx2xml_path', 'endx2xml'),
  );
  $form['bibutils_ris2xml_path'] = array(
    '#type' => 'textarea',
    '#title' => t('RIS to MODS executable'),
    '#description' => t('The path to ris2xml.'),
    '#default_value' => variable_get('bibutils_ris2xml_path', 'ris2xml'),
  );

  return system_settings_form($form);
}

