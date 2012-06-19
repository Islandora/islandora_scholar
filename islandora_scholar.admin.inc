<?php

function islandora_scholar_admin_form() {
  $form = array(
    '#tree' => TRUE,
    ''
  );

  return system_settings_form($form);
}
