<?php

function datavizwiz_tab_manage_fields($nid) {
  return drupal_get_form('datavizwiz_fields_form');
}

function datavizwiz_tab_summary_sort($nid) {
  return drupal_get_form('datavizwiz_summary_sort_form');
}

function datavizwiz_fields_form() {
  $nid = 14;

  $headers = array('Field', 'Display', 'Sortable', 'Visible Summary', 'Visible Detail', 'Visible Data');
  $tablerows = _datavizwiz_get_dataset_metadata($nid, array('*'));

  $headerrows = array();

  if (count($tablerows) > 0) {
    foreach ($tablerows[0] as $key => $value) {
      if ($key != 'fid' && $key != 'nid' && $key != NULL && $key != 'weight_summary' & $key != 'weight_detail') {
        array_push($headerrows, $key);
      }
    }
  }

  $displayrows = array();

  foreach ($tablerows as $tablerow) {
    if ($tablerow['field_name'] != 'ImporterID') {
      $displayrow = array();

      foreach ($headerrows as $headerrow) {
        $value = $tablerow[$headerrow];
        if ($value == 1) {
          $checked = 'CHECKED';
        }
        else {
          $checked = '';
        }
        switch ($headerrow) {
          case 'sortable' :
          case 'visible_summary' :
          case 'visible_detail' :
          case 'visible_data' :
            $form[$tablerow['fid']]['check-' . $tablerow['fid'] . '-' . $headerrow] = array(
              '#type' => 'checkbox',
              '#default_value' => $checked,
            );
            break;
          default :
            $form[$tablerow['fid']][$tablerow['fid'] . '-' . $headerrow] = array(
              '#value' => $value,
            );
        }
      }
    }
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function theme_datavizwiz_fields_form($form) {
  $rows = array();

  foreach (element_children($form) as $key) {
    $row = array();

    if (is_numeric($key)) {

      //$row[] = '' . drupal_render($form[$key]['name']) . '';
      $row[] = array('data' => drupal_render($form[$key][$key . '-field_name']));
      $row[] = array('data' => drupal_render($form[$key][$key . '-display_name']));
      $row[] = array('data' => drupal_render($form[$key]['check-' . $key . '-sortable']));
      $row[] = array('data' => drupal_render($form[$key]['check-' . $key . '-visible_summary']));
      $row[] = array('data' => drupal_render($form[$key]['check-' . $key . '-visible_detail']));
      $row[] = array('data' => drupal_render($form[$key]['check-' . $key . '-visible_data']));

      $rows[] = $row;
    }
  }

  // Individual table headers.
  $header = array();
  $header[] = array('data' => t('Featured'), 'class' => 'checkbox');
  $header[] = t('Name');
  $header[] = t('Category');
  $header[] = t('Discount');
  $header[] = t('Created on');

  $headers = array('Field', 'Display', 'Sortable', 'Include Summary', 'Include Detail', 'Include Data');

  $output = theme('table', $headers, $rows);
  $output .= drupal_render($form);
  return $output;
}

function theme_datavizwiz_summary_sort_form($form) {
  $header = array(t('Field Name'), t('Display Name'), t('Weight'));

  $rows = array();

  foreach (element_children($form) as $key) {
    $row = array();

    if (is_numeric($key)) {
      $row[] = array('data' => drupal_render($form[$key]['field_name']));
      $row[] = array('data' => drupal_render($form[$key]['display_name']));
      $row[] = array('data' => drupal_render($form['weight_summary'][$key]));

      $rows[] = array(
        'data' => $row,
        'class' => 'draggable',
      );
    }
  }


  $output = theme('table', $header, $rows, array('id' => 'summary-order'));
  $output .= drupal_render($form);

  drupal_add_tabledrag('summary-order', 'order', 'sibling', 'summary-order-weight', NULL, NULL, FALSE);

  return $output;
}

function datavizwiz_fields_form_submit($form, &$form_state) {
  $form_values = $form_state['values'];
  foreach ($form_values as $form_key => $form_value) {
    if (substr($form_key, 0, 6) == 'check-') {
      //dsm($form_value . $form_key);

      $second_dash = strpos($form_key, '-', 6);
      $fid = substr($form_key, 6, $second_dash - 6);
      $field = substr($form_key, $second_dash + 1, strlen($form_key) - $second_dash - 1);
      $row = array('fid' => $fid, $field => $form_value);
      drupal_write_record('dvw_field_meta', $row, 'fid');
    }
  }
  drupal_set_message(t('Dataset fields updated.'));
}

function datavizwiz_edit_two($nid) {
  return 'hello!';
}

function datavizwiz_summary_sort_form() {
  $nid = 14;
  $tablerows = _datavizwiz_get_dataset_metadata($nid, array('fid', 'field_name', 'display_name', 'weight_summary'));

  $header = array(t('Field Name'), t('Display Name'), t('Weight'));
  $rows = array();
  $form['weight_summary']['#tree'] = TRUE;
  
    $sql = ' SELECT fid, field_name, display_name, weight_summary FROM {dvw_field_meta} WHERE nid = %s AND visible_summary = TRUE ORDER BY weight_summary';
  $result = db_query($sql, $nid);
  
  while ($tablerow = db_fetch_array($result)) {
  
  //foreach ($tablerows as $tablerow) {
    $fid = $tablerow['fid'];

    $form[$fid]['weights']['#attributes']['class'] = 'filter-order-weight';
    $form[$fid]['field_name'] = array('#type' => 'markup', '#value' => $tablerow['field_name']);
    $form[$fid]['display_name'] = array('#type' => 'markup', '#value' => $tablerow['display_name']);
    $form['weight_summary'][$fid] = array('#type' => 'weight', 
      '#delta' => 30, 
      '#default_value' => $tablerow['weight_summary'], 
      '#attributes' => array('class' => 'weight'),);
    $form['weight_summary'][$fid]['#attributes']['class'] = 'summary-order-weight';
  }

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save Order'));
  //$output = theme('table', $header, $rows, array('id' => 'filter-order'));
  //$output .= drupal_render($form);
  //drupal_add_tabledrag('filter-order', 'order', 'sibling', 'filter-order-weight', NULL, NULL, FALSE);

  return $form;
}

function datavizwiz_summary_sort_form_submit($form, &$form_state) {
  //dsm($form_state);
  foreach ($form_state['values']['weight_summary'] as $id => $item) {
    db_query("UPDATE {dvw_field_meta} SET weight_summary=%d WHERE fid=%d",$item, $id);
    //dsm($id . ' -> ' . $item);
  }
}