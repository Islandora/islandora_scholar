<?php
/**
 * @file
 * Implements function to display embargoed list.
 */

/**
 * Builds management form for embargoed citations.
 * 
 * @param array $form
 *   The form.
 * @param array $form_state
 *   The form state.
 * 
 * @return array
 *   Returns the form.
 */
function islandora_scholar_embargo_manage_form(array $form, array &$form_state) {

  $table = islandora_scholar_embargo_build_table();

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
 *   Returns the table.
 */
function islandora_scholar_embargo_build_table() {

  $repository = islandora_get_tuque_connection();

  $query = 'select $pid $object $label from <#ri>
              where $pid <info:islandora/islandora-system:def/scholar#embargo-until> $object
              and $pid <info:fedora/fedora-system:def/model#label> $label
              order by $label';

  $results = $repository->repository->ri->itqlQuery($query, 'unlimited', '0');
 dsm($results);
  $header = array(t('PID'), t('Label'), t('Embargoed until'), t('Manage'));

  $rows = array();
  foreach ($results as $result) {
    $rows[] = array(
      $result['pid']['value'],
      l($result['label']['value'], 'islandora/object/' . $result['pid']['value']),
      $result['object']['value'],
      l(t('Manage'), 'islandora/object/' . $result['pid']['value'] . '/manage/embargo'),
    );
  }

  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'empty' => "There are no embargoed citations"));
}
