<?php

define('DVW_SORT_SUMMARY', 0);
define('DVW_SORT_DETAIL', 1);

// <editor-fold desc="Tabs">

function datavizwiz_tab_dataset($nid) {
  $path = urldecode(drupal_get_destination());
  if (substr($path, 12, 4) == 'node') {
    $bits = explode('/', $path);
    if (count($bits) == 3 && is_numeric($bits[1])) {
      drupal_goto(sprintf('admin/datavizwiz/edit/%s', $bits[1]));
    }
  }
  
  return drupal_get_form('datavizwiz_dataset_form', $nid);
}

function datavizwiz_tab_summary_panes($nid) {
    return drupal_get_form('datavizwiz_summary_panes_form', $nid);
}

function datavizwiz_tab_manage_fields($nid) {
  return drupal_get_form('datavizwiz_fields_form', $nid);
}

function datavizwiz_tab_sort_summary($nid) {
  return drupal_get_form('datavizwiz_sort_form', DVW_SORT_SUMMARY, $nid);
}

function datavizwiz_tab_sort_detail($nid) {
  return drupal_get_form('datavizwiz_sort_form', DVW_SORT_DETAIL, $nid);
}

// </editor-fold>

function datavizwiz_dataset_form($frm, $nid) {
  
  $node = node_load($nid);
  dsm($node);
  $path = drupal_lookup_path('alias', '/node/' . $nid);
  if ($path == '') { $path =  '/node/' . $nid; }
  
  $output = '<a href="' . $path . '">View Dataset</a><br/><br/>';
  $output .= 'TODO:  Add content and link back to CCK type.';
  
  $form['message'] = array (
    '#value' => $output,
    );
  
  return $form;
}

// <editor-fold desc="Manage Fields">

function datavizwiz_fields_form($frm, $nid) {
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

function datavizwiz_summary_panes_form($frm, $nid) {
  $sql =  sprintf("SELECT svid, nid, type, name, pane_order, options FROM {dvw_summary_views} WHERE nid = %s", $nid);
  $result = db_query($sql);
  
    while ($tablerow = db_fetch_array($result)) {
                  $form[$tablerow['svid']] = array(
              '#value' => $tablerow['type'] . ' ' . $tablerow['name'] . '<br/>',
                    );
      //dsm($tablerow);
      //$output .= $tablerow['type'] . ' ' . $tablerow['name'];
    }
  
  //$output = 'test';
//  $headers = array('Field', 'Display', 'Sortable', 'Visible Summary', 'Visible Detail', 'Visible Data');
//  $tablerows = _datavizwiz_get_dataset_metadata($nid, array('*'));
//
//  $headerrows = array();
//
//  if (count($tablerows) > 0) {
//    foreach ($tablerows[0] as $key => $value) {
//      if ($key != 'fid' && $key != 'nid' && $key != NULL && $key != 'weight_summary' & $key != 'weight_detail') {
//        array_push($headerrows, $key);
//      }
//    }
//  }
//
//  $displayrows = array();
//
//  foreach ($tablerows as $tablerow) {
//    if ($tablerow['field_name'] != 'ImporterID') {
//      $displayrow = array();
//
//      foreach ($headerrows as $headerrow) {
//        $value = $tablerow[$headerrow];
//        if ($value == 1) {
//          $checked = 'CHECKED';
//        }
//        else {
//          $checked = '';
//        }
//        switch ($headerrow) {
//          case 'sortable' :
//          case 'visible_summary' :
//          case 'visible_detail' :
//          case 'visible_data' :
//            $form[$tablerow['fid']]['check-' . $tablerow['fid'] . '-' . $headerrow] = array(
//              '#type' => 'checkbox',
//              '#default_value' => $checked,
//            );
//            break;
//          default :
//            $form[$tablerow['fid']][$tablerow['fid'] . '-' . $headerrow] = array(
//              '#value' => $value,
//            );
//        }
//      }
//    }
//  }
//
//  $form['submit'] = array(
//    '#type' => 'submit',
//    '#value' => t('Save'),
//  );

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
      $fid = substr($form_key, 6, $second_dash - 6);
      $field = substr($form_key, $second_dash + 1, strlen($form_key) - $second_dash - 1);
      $row = array('fid' => $fid, $field => $form_value);
      drupal_write_record('dvw_field_meta', $row, 'fid');
    }
  }
  drupal_set_message(t('Dataset fields updated.'));
}

// </editor-fold>

// <editor-fold desc="Manage Sorts">

function datavizwiz_sort_form($frm, $type, $nid) {
  $tablerows = _datavizwiz_get_dataset_metadata($nid, array('fid', 'field_name', 'display_name', 'weight_summary'));

  $header = array(t('Field Name'), t('Display Name'), t('Weight'));
  $rows = array();
  $form['weights']['#tree'] = TRUE;
  
  if ($type == DVW_SORT_SUMMARY) {
    $sql = 'SELECT fid, field_name, display_name, weight_summary FROM {dvw_field_meta} WHERE nid = %s AND visible_summary = TRUE ORDER BY weight_summary';
    $form['update_type'] = array(
      '#type' => 'hidden',
      '#value' => DVW_SORT_SUMMARY,
      );
    $field_name = 'weight_summary'; // used below
  } 
  else {
    $sql = 'SELECT fid, field_name, display_name, weight_detail FROM {dvw_field_meta} WHERE nid = %s AND visible_detail = TRUE ORDER BY weight_detail';
    $form['update_type'] = array(
      '#type' => 'hidden',
      '#value' => DVW_SORT_DETAIL,
      );
    $field_name = 'weight_detail'; // used below
  }
  $result = db_query($sql, $nid);
  
  while ($tablerow = db_fetch_array($result)) {
    $fid = $tablerow['fid'];

    $form[$fid]['field_name'] = array(
      '#type' => 'markup',
      '#value' => $tablerow['field_name'],
      );
    $form[$fid]['display_name'] = array(
      '#type' => 'markup',
      '#value' => $tablerow['display_name'],
      );
    $form['weights'][$fid] = array(
      '#type' => 'weight', 
      '#delta' => 30, 
      '#default_value' => $tablerow[$field_name],
      '#attributes' => array('class' => 'weight'),
      );
    $form['weights'][$fid]['#attributes']['class'] = 'order-weight';
  }

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save Order'));
  return $form;
}

function theme_datavizwiz_sort_form($form) {
  $header = array(t('Field Name'), t('Display Name'), t('Weight'));

  $rows = array();

  foreach (element_children($form) as $key) {
    $row = array();

    if (is_numeric($key)) {
      $row[] = array('data' => drupal_render($form[$key]['field_name']));
      $row[] = array('data' => drupal_render($form[$key]['display_name']));
      $row[] = array('data' => drupal_render($form['weights'][$key]));

      $rows[] = array(
        'data' => $row,
        'class' => 'draggable',
      );
    }
  }


  $output = theme('table', $header, $rows, array('id' => 'field-order'));
  $output .= drupal_render($form);

  drupal_add_tabledrag('field-order', 'order', 'sibling', 'order-weight', NULL, NULL, FALSE);

  return $output;
}

function datavizwiz_sort_form_submit($form, &$form_state) {
  foreach ($form_state['values']['weights'] as $id => $item) {
    if ($form_state['values']['update_type'] == DVW_SORT_SUMMARY) {
      db_query("UPDATE {dvw_field_meta} SET weight_summary=%d WHERE fid=%d",$item, $id);
    }
    else {
      db_query("UPDATE {dvw_field_meta} SET weight_detail=%d WHERE fid=%d",$item, $id);
    }

  }
}

// </editor-fold>