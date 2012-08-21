<?php

class EmbargoView {
  /**
   * Get a tabset to merge into the set of Islandora tabs.
   *
   * @param $pid string
   *   A string containing the PID for which we're to render a view.
   * @return array
   *   A tabset (to be merged with existing tabs).
   */
  public static function get_tabs($pid) {
    $tab_set = array();
    if (user_access('ingest new fedora objects') || $is_owner) {
      $tab_set['embargo'] = array(
        '#title' => t('Overview'),
        '#type' => 'tabpage',
        '#tab_name' => 'embargo',
        'embargo' => array(
          '#title' => t('Embargo Information'),
          '#type' => 'fieldset',
          '#value' => drupal_get_form('islandora_scholar_embargo_form', $pid),
        ),
      );
    }

    return $tab_set;
  }
}

function islandora_scholar_embargo_form(&$form_state, $pid) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  $item = new Fedora_Item($pid);
  dsm($item->datastreams, 'DSs');
  $form = array(
    '#tree' => TRUE,
    'pid' => array(
      '#type' => 'value',
      '#value' => $pid,
    ),
    'dsid' => array(
      '#type' => 'select',
      '#title' => t('Items to be embargoed'),
      '#options' => array(
        '!object/level!'  => t('Object'),
        'PDF' => t('PDF datastream'),
      ),
      '#multiple' => TRUE
    ),
    'embargo_select' => array(
      '#type' => 'radios',
      '#title' => t('Duration of Embargo'),
      '#required' => TRUE,
      '#default_value' => 'no_embargo',
      '#options' => array(
        'no_embargo' => t('Do not embargo'),
        'indefinite' => t('Embargo indefinitely (must be lifted manually)'),
        'date' => t('Embargo until date specified'),
      ),
    ),
    'embargo_date' => array(
      '#type' => 'datepicker',
      '#title' => t('Embargo date'),
      '#description' => t('A date until which this item should be embargoed.'),
    ),
  );

  return $form;
}

function islandora_scholar_embargo_form_submit($form, $form_state) {

}