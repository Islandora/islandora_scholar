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
  module_load_include('inc', 'fedora_repository', 'ObjectHelper');

  $query = <<<'XML'
      select $pid $object $label from <#ri>
              where $pid <info:islandora/islandora-system:def/scholar#embargo-until> $object
              and $pid <info:fedora/fedora-system:def/model#label> $label
              order by $label
XML;

  $results = ObjectHelper::performItqlQuery($query);

  if (empty($results)) {
    //return;
  }

  $table = array(
    '#header' => array('PID', t('Label'), t('Embargoed until')),
    '#theme' => 'islandora_embargo_table',
    '#tree' => TRUE,
    'rows' => array(),
  );
  $rows = &$table['rows'];
  $textOptions = array(
    'attributes' => array('target' => '_blank'),
  );

  foreach ($results as $result) {
    $rows[] = array(
      '#pid' => $result['pid'],
      'pid' => array('#value' => l($result['pid'], 'fedora/repository/' . $key, $textOptions)),
      'label' => array('#value' => l($result['label'], 'fedora/repository/' . $key, $textOptions)),
      'date' => array('#value' => $result['object']),
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
    $fields = array();

    foreach (element_children($setting) as $property) {
      $field = $setting[$property];
      $fields[] = drupal_render($field);
    }
    $rows[] = array(
      'data' => $fields,
    );
  }
  if(empty($rows)){
    $rows[] = array(t('There are no embargoed items in this repository'));
  }
 
  return theme_table($element['table']['#value']['#header'], $rows);
}