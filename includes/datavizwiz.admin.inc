<?php

function datavizwiz_tab_manage_fields($nid) {
  return drupal_get_form('datavizwiz_fields_form');
}

function datavizwiz_fields_form() {
  $nid = 14;

  $headers = array('Field', 'Display', 'Sortable', 'Visible Summary', 'Visible Detail', 'Visible Data');
  $tablerows = _datavizwiz_get_dataset_metadata($nid);

  $headerrows = array();

  if (count($tablerows) > 0) {
    foreach ($tablerows[0] as $key => $value) {
      if ($key != 'fid' && $key != 'nid' && $key != NULL && $key != 'order_summary' & $key != 'order_detail') {
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

function datavizwiz_fields_form_submit($form, &$form_state) {
  $form_values = $form_state['values'];
  foreach ($form_values as $form_key => $form_value) {
    if (substr($form_key, 0, 6) == 'check-') {
          //dsm($form_value . $form_key);
          
      $second_dash = strpos($form_key, '-', 6);
      $fid = substr($form_key, 6,  $second_dash-6);
      $field = substr($form_key, $second_dash+1, strlen($form_key) - $second_dash -1);
      $row = array ('fid' => $fid, $field => $form_value);
      drupal_write_record('dvw_field_meta',$row,'fid');
    }    
  }
  drupal_set_message(t('Dataset fields updated.'));
}

function datavizwiz_edit_two($nid) {
  return 'hello!';
}

function datavizwiz_summary_sort() {
  
}