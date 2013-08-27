<?php
/**
 * @file
 * Implements function to display embargoed list.
 */

/**
 * Builds management form for embargoed citations.
 * 
 * @param array $form
 *   The form
 * @param array $form_state
 *   The form state
 * 
 * @return array
 *   Returns the form
 */
function embargo_manage_form(array $form, array &$form_state) {

  $table = embargo_build_table();

  $form['table'] = array(
    '#type' => 'markup',
    '#markup' => $table,
  );

  return $form;
}

/**
 * Builds table to pass to theming function.
 * 
 * @return array
 *   Returns the table
 */
function embargo_build_table() {

  module_load_include('inc', 'islandora', 'includes/tuque');
  $repository = new IslandoraTuque();

  $query = <<<'XML'
      select $pid $object $label from <#ri>
              where $pid <info:islandora/islandora-system:def/scholar#embargo-until> $object
              and $pid <info:fedora/fedora-system:def/model#label> $label
              order by $label
XML;

  $results = $repository->repository->ri->itqlQuery($query, 'unlimited', '0');

  if (empty($results)) {
    return;
  }
  $header = array('PID', t('Label'), t('Embargoed until'), t('Manage'));

  $text_options = array(
    'attributes' => array('target' => '_blank'),
  );

  $rows = array();
  foreach ($results as $result) {
    $rows[] = array(
      $result['pid']['value'],
      l($result['label']['value'], 'islandora/object/' . $result['pid']['value']),
      $result['object']['value'],
      l(t('Manage'), 'islandora/object/' . $result['pid']['value'] . '/manage/embargo'),
    );
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}
