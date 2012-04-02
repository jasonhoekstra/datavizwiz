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

function datavizwiz_tab_summary_pane_edit($nid, $op, $svid) {
    return drupal_get_form('datavizwiz_summary_pane_edit_form', $nid, $op, $svid);
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
  if ($path == '') {
    $path = '/node/' . $nid;
  }

  $output = '<a href="' . $path . '">View Dataset</a><br/><br/>';
  $output .= 'TODO:  Add content and link back to CCK type.';

  $form['message'] = array(
    '#value' => $output,
  );

  return $form;
}

// <editor-fold desc="Summary Pane">

function datavizwiz_summary_panes_form($frm, $nid) {
  $sql = sprintf("SELECT svid, nid, type, name, pane_order, options FROM {dvw_summary_views} WHERE nid = %s ORDER BY pane_order", $nid);
  $result = db_query($sql);

  $form['delete']['#tree'] = TRUE;
  $form['edit']['#tree'] = TRUE;

  while ($tablerow = db_fetch_array($result)) {

    $svid = $tablerow['svid'];

    $form[$svid]['type'] = array(
      '#type' => 'markup',
      '#value' => _datavizwiz_get_pane_type_name($tablerow['type']),
    );
    $form[$svid]['name'] = array(
      '#type' => 'markup',
      '#value' => $tablerow['name'],
    );
    $form['weights'][$svid] = array(
      '#type' => 'weight',
      '#delta' => 10,
      '#default_value' => $tablerow['pane_order'],
      '#attributes' => array('class' => 'weight'),
    );
    $form['edit'][$svid] = array(
      '#type' => 'image_button',
      '#name' => 'edit-' . $svid,
      '#src' => drupal_get_path('module', 'datavizwiz') . '/edit.png',
      '#return_value' => 'edit-' . $svid,
    );
    $form['delete'][$svid] = array(
      '#type' => 'image_button',
      '#name' => 'delete-' . $svid,
      '#src' => drupal_get_path('module', 'datavizwiz') . '/delete.png',
      '#return_value' => 'delete-' . $svid,
    );
    $form['weights'][$svid]['#attributes']['class'] = 'order-weight';
  }

  $form['nid'] = array('#type' => 'hidden', '#value' => $nid);

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save Order'));
  $form['add'] = array('#type' => 'submit', '#value' => t('Add Pane'));

  return $form;
}

function theme_datavizwiz_summary_panes_form($form) {
  $header = array(t('Type'), t('Pane Name'), t('Weight'), t('Edit'), t('Delete'));

  $rows = array();

  foreach (element_children($form) as $key) {
    $row = array();
    if (is_numeric($key)) {
      $row[] = array('data' => drupal_render($form[$key]['type']));
      $row[] = array('data' => drupal_render($form[$key]['name']));
      $row[] = array('data' => drupal_render($form['weights'][$key]));
      $row[] = array('data' => drupal_render($form['edit'][$key]));
      $row[] = array('data' => drupal_render($form['delete'][$key]));

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

function datavizwiz_summary_panes_form_submit($form, &$form_state) {
  dsm($form_state);
  //dsm($form_state['clicked_button']['#value']);

  $nid = $form_state['values']['nid'];
  $node =  node_load($nid);
  
        dsm($node);
  $submit_value = drupal_substr($form_state['clicked_button']['#value'], 0, 3);
  
  switch ($submit_value) {
    case 'Sav':
      foreach($form_state['values'] as $key => $value) {
        if (is_numeric($key)) {
        $sql = sprintf("UPDATE {dvw_summary_views} SET pane_order = %s WHERE svid = %s", $value, $key);
        db_query($sql);        
        }
      }
      
      drupal_set_message(t('Pane order updated.'));
      break;
    case 'Add':
      dsm('Add');
      $url = sprintf('admin/datavizwiz/edit/%s/summary_pane/%s/%s', $nid, 'add', 0);
      drupal_goto($url);
      break;
    case 'edi':
      dsm('Edit');
      $svid = drupal_substr($form_state['clicked_button']['#value'], 5);
      $url = sprintf('admin/datavizwiz/edit/%s/summary_pane/%s/%s', $nid, 'edit', $svid);
      drupal_goto($url);
      break;
    case 'del':
      dsm('Delete');
      $svid = drupal_substr($form_state['clicked_button']['#value'], 7);
      $url = sprintf('admin/datavizwiz/edit/%s/summary_pane/%s/%s', $nid, 'delete', $svid);
      drupal_goto($url);
      break;
  }
}

function datavizwiz_summary_pane_edit_form($frm, $nid, $op, $svid) {
  $form['#attributes'] = array('id' => 'summary-pane-edit');
  
  switch ($op) {
    case 'add':
      drupal_add_js('function submit_edit_form(id) {
        //alert(id);
        document.forms["datavizwiz-summary-pane-edit-form"].operation = id;
        document.forms["datavizwiz-summary-pane-edit-form"].submit();
        }','inline');
      $form['type'] = array(
        '#title' => t('Pane type'),
        '#type' => 'select',
        '#options' => array(
          DVW_SUMMARY_MAP_OPENLAYERS => t('Leaflet Map'), 
          DVW_SUMMARY_FLOT_PIE => t('Flot Pie Chart'),
          DVW_SUMMARY_FLOT_BAR=> t('Flot Bar Graph'),
          DVW_SUMMARY_HTML => t('HTML Block'),       
          ),
        '#default_value' => $frm['post']['type'],
        '#attributes' => array('onchange' => 'submit_edit_form(this.selectedIndex)')
      );  
      $form['operation'] = array('#type' => 'hidden', '#value' => 0);
      $form['submit'] = array('#type' => 'submit', '#value' => 0);
      break;
    case 'delete':
      $form['question'] = array('#value' => t("Are you sure you'd like to delete this summary view?<br/>"));
      $form['submit_yes'] = array('#type' => 'submit', '#value' => 'Yes');
      $form['submit_no'] = array('#type' => 'submit', '#value' => 'No');
      $form['delete'] = array('#type' => 'hidden', '#value' => 'delete');                  
      break;
  }
  return $form;
}

function datavizwiz_summary_pane_edit_form_submit($form, &$form_state) {
        dsm($form_state['values']['type']);
}

// </editor-fold>

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
      db_query("UPDATE {dvw_field_meta} SET weight_summary=%d WHERE fid=%d", $item, $id);
    }
    else {
      db_query("UPDATE {dvw_field_meta} SET weight_detail=%d WHERE fid=%d", $item, $id);
    }
  }
}

function _datavizwiz_get_pane_type_name($pane_type) {
  switch ($pane_type) {
    case DVW_SUMMARY_MAP_OPENLAYERS:
      return 'OpenLayers Map';
      break;
    case DVW_SUMMARY_FLOT_PIE:
      return 'Flot Graph (Pie Chart)';
      break;
    case DVW_SUMMARY_FLOT_BAR:
      return 'Flot Graph (Bar)';
      break;
    case DVW_SUMMARY_HTML:
      return 'HTML Pane';
      break;
  }
}

// </editor-fold>