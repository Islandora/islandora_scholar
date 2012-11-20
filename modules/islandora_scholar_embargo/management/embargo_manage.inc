<?php

/**
 * Builds management form for embargoed citations
 * @param array $form_state
 * @return array
 */

function embargo_manage_form(&$form_state) {

  $table = embargo_build_table();
  $form['table'] = array(
    '#type' => 'markup',
    '#value' => $table,
  );
  return $form;
}

/**
 * Builds table to pass to theming function
 * @return array
 */

function embargo_build_table() {
  module_load_include('inc', 'fedora_repository', 'api/fedora_utils');

  $query = <<<'XML'
select $pid $object $label from <#ri>
              where
              $pid <info:islandora/islandora-system:def/scholar#embargo-until> $object
              and
              $pid <info:fedora/fedora-system:def/model#label> $label
              order by $label
XML;

  $query = htmlentities(urlencode($query));
  $content = '';

  $url = variable_get('fedora_repository_url', 'http://localhost:8080/fedora/risearch');
  $url .= "?type=tuples&flush=TRUE&format=csv&limit=$limit&offset=$offset&lang=itql&stream=on&query=" . $query;
  $content .= do_curl($url);
  $results = explode("\n", $content);
  array_shift($results);
  $lines = array_filter($results);
  if (empty($lines)) {
    return;
  }
  foreach ($lines as $line) {
    $parts = explode(',', $line);
    $pid = preg_replace('/info:fedora\//', '', $parts[0]);
    $objects[$pid]['value'] = $parts[1];
    $objects[$pid]['label'] = $parts[2];
    $keys[] = $pid;
  }
//checkboxes are not used in this iteration
  
  $table = array(
    '#header' => array(theme('table_select_header_cell'), 'PID', t('Label'), t('Embargoed until')),
    '#theme' => 'islandora_embargo_table',
    '#tree' => TRUE,
    'rows' => array(),
    'selections' => array(
      '#type' => 'checkboxes',
      '#options' => array_fill_keys($keys, ''),
    ),
  );

  $rows = &$table['rows'];
  if (empty($objects)) {
    return;
  }

  foreach ($objects as $key => $object) {
    $textOptions = array(
      'attributes' => array('target' => '_blank'),
    );

    $rows[] = array(
      '#pid' => $key,
      'pid' => array('#value' => l($key, 'fedora/repository/' . $key, $textOptions)),
      'label' => array('#value' => l($object['label'], 'fedora/repository/' . $key, $textOptions)),
      'date' => array('#value' => $object['value']),
    );
  }

  return $table;
}

/**
 * Theming function for table
 * @param array $element
 * @return array
 */
function theme_embargo_manage_form(array $element) {
  $rows = array();
  foreach (element_children($element['table']['#value']['rows']) as $child) {
    $setting = $element['table']['#value']['rows'][$child];
    $pid = $setting['#pid'];
    $fields = array(
      drupal_render($element['table']['#value']['selections'][$pid]) // First field is a checkbox (unused in this iteration)
    );
    foreach (element_children($setting) as $property) {
      $field = $setting[$property];
      $fields[] = drupal_render($field);
    }
    $rows[] = array(
      'data' => $fields,

    );
  }
  $attributes = isset($element['#id']) ? array('id' => $element['#id']) : NULL;
  return theme_table($element['table']['#value']['#header'], $rows, $attributes);
}