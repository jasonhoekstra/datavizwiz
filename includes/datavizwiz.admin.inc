<?php

// <editor-fold desc="Tabs">

function datavizwiz_redirect_admin($nid) {
  $path = urldecode(drupal_get_destination());
  if (substr($path, 12, 4) == 'node') {
    $bits = explode('/', $path);
    if (count($bits) == 3 && is_numeric($bits[1])) {
      drupal_goto(sprintf('admin/datavizwiz/edit/%s', $bits[1]));
    }
  }
}

function datavizwiz_redirect_view($nid) {
  $path = urldecode(drupal_get_destination());
  $bits = explode('/', substr($path, 34));
  if (count($bits) == 2 && is_numeric($bits[0])) {
    drupal_goto(sprintf('node/%s', $bits[0]));
  }
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
  return drupal_get_form('datavizwiz_sort_summary_form', $nid);
}

function datavizwiz_tab_sort_detail($nid) {
  return drupal_get_form('datavizwiz_sort_detail_form', $nid);
}

function datavizwiz_settings_map_layers() {
  return drupal_get_form('datavizwiz_map_layers_detail_form');
}

// </editor-fold>

// <editor-fold desc="Summary Pane">

function datavizwiz_summary_panes_form($frm, $nid) {
  $sql = sprintf("SELECT svid, nid, type, name, pane_order, options FROM {dvw_summary_views} WHERE nid = %s ORDER BY pane_order", $nid);
  $result = db_query($sql);

  $form['delete']['#tree'] = TRUE;
  
  $form['nid'] = array('#type' => 'hidden', '#value' => $nid);
 
  
  $item_count=0;
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
    $form['delete'][$svid] = array(
      '#type' => 'image_button',
      '#name' => 'delete-' . $svid,
      '#src' => drupal_get_path('module', 'datavizwiz') . '/images/delete.png',
      '#return_value' => 'delete-' . $svid,
    );
    $form['weights'][$svid]['#attributes']['class'] = 'order-weight';
    $item_count++;
  }

  if ($item_count > 0) {
    $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));    
  }

  $form['spacer'] = array(
    '#type' => 'markup',
    '#value' => '<br/><br/><br/>',
  );
  
  $form['add_panel'] = array(
  '#type' => 'fieldset', 
  '#title' => t('Add new pane'),
  '#collapsible' => FALSE,
  );
  
  $form['add_panel']['type'] = array(
  '#title' => t('Pane type'),
  '#type' => 'select',
  '#options' => array(
    DVW_TYPE_MAP_OPENLAYERS => t('Map'),
    DVW_TYPE_PIE_FLOT => t('Pie Graph'),
    DVW_TYPE_BAR_FLOT=> t('Bar Chart'),
    DVW_TYPE_HTML => t('Freeform HTML'),

    ),
  '#attributes' => array('onchange' => 'submit_edit_form(this.selectedIndex)')
  );
  
  $form['add_panel']['add'] = array('#type' => 'submit', '#value' => t('Add Pane'));

  return $form;
}

function theme_datavizwiz_summary_panes_form($form) {
  $header = array(t('Type'), t('Pane Name'), t('Weight'), t('Delete'));

  $rows = array();

  $item_count = 0;
  foreach (element_children($form) as $key) {
    $row = array();
    if (is_numeric($key)) {
      $row[] = array('data' => drupal_render($form[$key]['type']));
      $row[] = array('data' => drupal_render($form[$key]['name']));
      $row[] = array('data' => drupal_render($form['weights'][$key]));
      $row[] = array('data' => drupal_render($form['delete'][$key]));

      $rows[] = array(
        'data' => $row,
        'class' => 'draggable',
      );
      $item_count++;
    }
  }

  if ($item_count > 0) {
    $output = theme('table', $header, $rows, array('id' => 'field-order'));
    $output .= drupal_render($form);
    
    drupal_add_tabledrag('field-order', 'order', 'sibling', 'order-weight', NULL, NULL, FALSE);
  }

  return $output;
}

function datavizwiz_summary_panes_form_submit($form, &$form_state) {
  $nid = $form_state['values']['nid'];
  $node =  node_load($nid);
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
      $url = sprintf('admin/datavizwiz/edit/%s/summary_pane/%s/%s', $nid, 'add', $form_state['values']['type']);
      drupal_goto($url);
      break;
    case 'del':
      $svid = drupal_substr($form_state['clicked_button']['#value'], 7);
      $url = sprintf('admin/datavizwiz/edit/%s/summary_pane/%s/%s', $nid, 'delete', $svid);
      drupal_goto($url);
      break;
  }
}

function datavizwiz_summary_pane_edit_form($frm, $nid, $op, $svid) {
  $node = node_load($nid);
  $form['#attributes'] = array('id' => 'summary-pane-edit');
  $form['nid'] = array('#type' => 'hidden', '#value' => $nid);
  
  switch ($op) {
    case 'add':      
      $form['pane_name'] = array('#type' => 'textfield',
        '#title' => t('Pane Name'),
        '#required' => TRUE,
        );
      switch (arg(6)) {
        case DVW_TYPE_MAP_OPENLAYERS:
          drupal_set_title(t('Add Pane - Map'));
          $form = _datavizwiz_summary_pane_edit_map_openlayers_form($form, $node);
          break;       
        case DVW_TYPE_PIE_FLOT:
          drupal_set_title(t('Add Pane - Pie Chart'));
          $form = _datavizwiz_summary_pane_edit_flot_form($form, $node, DVW_TYPE_PIE_FLOT);
          break;
        case DVW_TYPE_BAR_FLOT:
          drupal_set_title(t('Add Pane - Bar Graph'));
          $form = _datavizwiz_summary_pane_edit_flot_form($form, $node, DVW_TYPE_BAR_FLOT);
          break;
        case DVW_TYPE_HTML:
          drupal_set_title(t('Add Pane - Freeform HTML'));
          $form = _datavizwiz_summary_pane_edit_html_form($form);
          break;
      }
      $form['operation'] = array('#type' => 'hidden', '#value' => DVW_OP_ADD);
      $form['submit'] = array('#type' => 'submit', '#value' => t('Save Pane'));
      $form['cancel'] = array(
        '#type' => 'button',
        '#value' => t('Cancel'),
        '#attributes' => array('onClick' => 'history.go(-1); return true;'),
        '#post_render' => array('_datavizwiz_change_button_type'),
      );
      
      break;
    case 'delete':
      $form['operation'] = array('#type' => 'hidden', '#value' => DVW_OP_DELETE);
      $form['svid'] = array('#type' => 'hidden', '#value' => $svid);
      $form['question'] = array('#value' => t("Are you sure you'd like to delete this summary view?<br/>"));
      $form['submit_yes'] = array('#type' => 'submit', '#value' => 'Yes');
      $form['submit_no'] = array('#type' => 'submit', '#value' => 'No');
      $form['delete'] = array('#type' => 'hidden', '#value' => 'delete');                  
      break;
  }
  return $form;
}

function _datavizwiz_summary_pane_edit_map_openlayers_form(&$form, $node) {
  $field_array = _datavizwiz_get_field_array($node->field_dvw_datatable[0]['value'], TRUE);
  $map_layers = _datavizwiz_get_map_layers();
  
  $form['type'] = array('#type' => 'hidden',
    '#value' => DVW_TYPE_MAP_OPENLAYERS);
  
  $form['title_field'] = array('#type' => 'select',
    '#title' => t('Title field'),
    '#options' => $field_array,
  );
  
  $form['map_type_id'] = array('#type' => 'select',
    '#title' => t('Map layer type'),
    '#options' => $map_layers,
  );
  
  $form['tos_notice'] = array('#type' => 'markup',
    '#title' => t('Terms of Service'),
    '#value' => t('<i>Please read and ensure your web site meets the Terms of Service for the selected map provider:</i><br/><br/>
      <ul>
        <li><a href="https://developers.google.com/maps/terms" target="_blank">Google Terms of Service</a> (also if a federal agency, see <a href="http://www.google.com/intl/en-us/help/legalnotices_maps.html">U.S. Government Restricted Rights</a>)</li>
        <li><a href="http://mapbox.com/tos/" target="_blank">MapBox</a> (also if a federal agency, <a href="http://mapbox.com/government/" target="_blank">free usage with this Terms of Service)</a></li>
        <li><a href="http://www.openstreetmap.org/copyright" target="_blank">OpenStreetMap</a></li>
      </ul>
      <br/>')
    );
  
  return $form;
}

function _datavizwiz_summary_pane_edit_flot_form(&$form, $node, $chart_type) {
  $field_array = _datavizwiz_get_field_array($node->field_dvw_datatable[0]['value']);

  $form['type'] = array('#type' => 'hidden',
    '#value' => $chart_type);
  
  $form['variable'] = array('#type' => 'select',
  '#title' => t('Chart variable'),
  '#options' => $field_array,
  );
  
  return $form;
}

function _datavizwiz_summary_pane_edit_html_form(&$form) {
  $form['html'] = array('#type' => 'textarea',
    '#title' => t('HTML'),
    '#rows' => 15);
  
  $form['type'] = array('#type' => 'hidden',
    '#value' => DVW_TYPE_HTML);
  return $form;
}

function datavizwiz_summary_pane_edit_form_submit(&$form, &$form_state) {
  $nid = $form_state['values']['nid'];
  $op = $form_state['values']['operation'];

  switch ($op) {
    case DVW_OP_ADD:
      $pane_type = $form_state['values']['type'];
      $form['type'] = array('#value' => $pane_type);

      $record = new stdClass();
      $record->nid = $nid;
      $record->name = $form_state['values']['pane_name'];
      $record->type = $pane_type;

      switch ($pane_type) {
        case DVW_TYPE_MAP_OPENLAYERS:
          $options = array('title_field' => $form_state['values']['title_field'],
                           'map_type_id' => $form_state['values']['map_type_id']);
          $record->options = json_encode($options);
          break;
        case DVW_TYPE_PIE_FLOT:
          $options = array('variable' => $form_state['values']['variable']);
          $record->options = json_encode($options);
          break;
        case DVW_TYPE_BAR_FLOT:
          $options = array('variable' => $form_state['values']['variable']);
          $record->options = json_encode($options);
          break;
        case DVW_TYPE_HTML:
          $record->options = $form_state['values']['html'];
          break;
      }
      $result = drupal_write_record('dvw_summary_views', $record);            
      break;
    case DVW_OP_DELETE:
      $svid = $form_state['values']['svid'];
      db_query('DELETE FROM {dvw_summary_views} WHERE nid=%s and svid=%s', $nid, $svid);
      drupal_set_message(t('Summary pane successfully deleted.'));
      break;
  }

  drupal_goto(sprintf('admin/datavizwiz/edit/%s/summary_pane', $nid));
}

// </editor-fold>

// <editor-fold desc="Manage Fields">

function datavizwiz_fields_form($frm, $nid) {
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
          case 'visible_summary' :
          case 'visible_detail' :
          case 'visible_data' :
            $form[$tablerow['fid']]['check-' . $tablerow['fid'] . '-' . $headerrow] = array(
              '#type' => 'checkbox',
              '#default_value' => $checked,
            );
            break;
          case 'flag' :
            $form[$tablerow['fid']]['select-' . $tablerow['fid'] . '-' . $headerrow] = array(
              '#type' => 'select',
              '#options' => array(DVW_FLAG_NONE => '', 
                DVW_FLAG_CURRENCY => 'currency',
                DVW_FLAG_LATITUDE => 'latitude', 
                DVW_FLAG_LONGITUDE => 'longitude',
                ),
              '#default_value' => $value,
            );
            break;
          case 'filter' :
            $form[$tablerow['fid']]['select-' . $tablerow['fid'] . '-' . $headerrow] = array(
              '#type' => 'select',
              '#default_value' => $tablerow['filter'],
              '#options' => array(
                DVW_FILTER_NONE => t(''), 
                DVW_FILTER_TEXT => t('Text Search'), 
                DVW_FILTER_DROPDOWN => t('Drop Down')),
            );
            break;
          case 'field_name' :
            $form[$tablerow['fid']][$tablerow['fid'] . '-' . $headerrow] = array(
              '#value' => $value,
            );
            break;
          case 'display_name' :
            $form[$tablerow['fid']]['text-' . $tablerow['fid'] . '-' . $headerrow] = array(
              '#type' => 'textfield',
              '#default_value' => $value,
              '#size' => 15,
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
  _datavizwiz_add_form_warning_javascript();
  
  $rows = array();
  
  $row = array(array('header' => true ), 
    array('header' => true ), 
    array('data' => '<input type="checkbox" onClick="$(
      function() {
      var $checkbox = $(this).find(\'input[name*=visible_summary]\'); 
      $checkbox.attr(\'checked\', !$checkbox.attr(\'checked\')); 
      });" />', 'header' => true ), 
    array('data' => '<input type="checkbox" onClick="$(
      function() { 
      var $checkbox = $(this).find(\'input[name*=visible_detail]\'); 
      $checkbox.attr(\'checked\', !$checkbox.attr(\'checked\')); 
      });" />', 'header' => true ),  
    array('data' => '<input type="checkbox" onClick="$(
      function() { 
      var $checkbox = $(this).find(\'input[name*=visible_data]\'); 
      $checkbox.attr(\'checked\', !$checkbox.attr(\'checked\')); 
      });" />', 'header' => true ),   
    array('header' => true ),
    array('header' => true ));
  $rows[] = $row;

  foreach (element_children($form) as $key) {
    $row = array();

    if (is_numeric($key)) {

      //$row[] = '' . drupal_render($form[$key]['name']) . '';
      $row[] = array('data' => drupal_render($form[$key][$key . '-field_name']));
      $row[] = array('data' => drupal_render($form[$key]['text-' . $key . '-display_name']));
      $row[] = array('data' => drupal_render($form[$key]['check-' . $key . '-visible_summary']));
      $row[] = array('data' => drupal_render($form[$key]['check-' . $key . '-visible_detail']));
      $row[] = array('data' => drupal_render($form[$key]['check-' . $key . '-visible_data']));
      $row[] = array('data' => drupal_render($form[$key]['select-' . $key . '-filter']));
      $row[] = array('data' => drupal_render($form[$key]['select-' . $key . '-flag']));

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

  $headers = array('Field', 'Display Name', 'Include Summary', 'Include Detail', 'Include Data', 'Filter', 'Flag');
 
  $output = theme('table', $headers, $rows);
  theme_table_select_header_cell();
  $output .= drupal_render($form);
  return $output;
}

function datavizwiz_fields_form_submit($form, &$form_state) {
  $form_values = $form_state['values'];
  foreach ($form_values as $form_key => $form_value) {
    if (substr($form_key, 0, 6) == 'check-') {
      $second_dash = strpos($form_key, '-', 6);
      $fid = substr($form_key, 6, $second_dash - 6);
      $field = substr($form_key, $second_dash + 1, strlen($form_key) - $second_dash - 1);
      $row = array('fid' => $fid, $field => $form_value);
      drupal_write_record('dvw_field_meta', $row, 'fid');
    }
    if (substr($form_key, 0, 7) == 'select-') {
      $second_dash = strpos($form_key, '-', 7);
      $fid = substr($form_key, 7, $second_dash - 7);
      $field = substr($form_key, $second_dash + 1, strlen($form_key) - $second_dash - 1);
      $row = array('fid' => $fid, $field => $form_value);
      drupal_write_record('dvw_field_meta', $row, 'fid');
    }
    if (substr($form_key, 0, 5) == 'text-') { 
      $second_dash = strpos($form_key, '-', 5);
      $fid = substr($form_key, 5, $second_dash - 5);
      $field = substr($form_key, $second_dash + 1, strlen($form_key) - $second_dash - 1);
      $row = array('fid' => $fid, $field => $form_value);
      drupal_write_record('dvw_field_meta', $row, 'fid');
    }  
  }
  drupal_set_message(t('Dataset fields updated.'));
}

// </editor-fold>

// <editor-fold desc="Manage Summary Page">

function datavizwiz_sort_summary_form($frm, $nid) {
  $rows = array();
  $form['weights']['#tree'] = TRUE;
  $form['sortable']['#tree'] = TRUE;
  $form['detail_link']['#tree'] = TRUE;
  
  $sql = 'SELECT fid, field_name, display_name, weight_summary, sortable, detail_link FROM {dvw_field_meta} WHERE nid = %s AND visible_summary = TRUE ORDER BY weight_summary';
  $form['update_type'] = array(
    '#type' => 'hidden',
    '#value' => DVW_SORT_SUMMARY,
  );
  $field_name = 'weight_summary'; // used below

  $result = db_query($sql, $nid);

  while ($tablerow = db_fetch_array($result)) {
    $fid = $tablerow['fid'];
    $form[$fid] = '';

    $form['field_name'][$fid] = array(
      '#type' => 'markup',
      '#value' => $tablerow['field_name'],
    );
    $form['display_name'][$fid] = array(
      '#type' => 'markup',
      '#value' => $tablerow['display_name'],
    );
    $form['sortable'][$fid] = array(
      '#type' => 'checkbox',
      '#default_value' => $tablerow['sortable'],
    );
    $form['detail_link'][$fid] = array(
      '#type' => 'checkbox',
      '#default_value' => $tablerow['detail_link'],
    );
    $form['weights'][$fid] = array(
      '#type' => 'weight',
      '#delta' => 30,
      '#default_value' => $tablerow[$field_name],
      '#attributes' => array('class' => 'weight'),
    );
    $form['weights'][$fid]['#attributes']['class'] = 'order-weight';
  }

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  return $form;
}

function theme_datavizwiz_sort_summary_form($form) {
  _datavizwiz_add_form_warning_javascript();
  $header = array(t('Field Name'), t('Display Name'), t('Sortable'), t('Link to Detail'), t('Weight'));

  $rows = array();
  foreach (element_children($form) as $key) {
    $row = array();

    if (is_numeric($key)) {
      $row[] = array('data' => drupal_render($form['field_name'][$key]));
      $row[] = array('data' => drupal_render($form['display_name'][$key]));
      $row[] = array('data' => drupal_render($form['sortable'][$key]));
      $row[] = array('data' => drupal_render($form['detail_link'][$key]));
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

function datavizwiz_sort_summary_form_submit($form, &$form_state) {
  foreach ($form_state['values']['weights'] as $id => $item) {
    $record = new stdClass();
    $record->weight_summary = $item;
    $record->fid = $id;
    drupal_write_record('dvw_field_meta', $record, 'fid');
  }
  
  foreach ($form_state['values']['sortable'] as $id => $item) {
    $record = new stdClass();
    $record->sortable = $item;
    $record->fid = $id;
    drupal_write_record('dvw_field_meta', $record, 'fid');
  }
  
  foreach ($form_state['values']['detail_link'] as $id => $item) {
    $record = new stdClass();
    $record->detail_link = $item;
    $record->fid = $id;
    drupal_write_record('dvw_field_meta', $record, 'fid');
  }
  drupal_set_message(t('Summary page updated.'));
  
}

// </editor-fold>

// <editor-fold desc="Manage Detail Page">

function datavizwiz_sort_detail_form($frm, $nid) {
  $rows = array();
  $form['weights']['#tree'] = TRUE;
  $form['detail_wide']['#tree'] = TRUE;
  
  $sql = 'SELECT fid, field_name, display_name, detail_title, detail_wide, weight_detail FROM {dvw_field_meta} WHERE nid = %s AND visible_detail = TRUE ORDER BY weight_detail';
  $form['update_type'] = array(
    '#type' => 'hidden',
    '#value' => DVW_SORT_DETAIL,
  );
  $field_name = 'weight_detail'; // used below

  $result = db_query($sql, $nid);

  $checked_title = false;
  $checked_subtitle = false;
  
  while ($tablerow = db_fetch_array($result)) {
    $fid = $tablerow['fid'];
    $form[$fid] = '';
    if ($tablerow['detail_title']) { $checked_title = true; }
    
    $form[$fid]['field_name'] = array(
      '#type' => 'markup',
      '#value' => $tablerow['field_name'],
    );
    $form[$fid]['display_name'] = array(
      '#type' => 'markup',
      '#value' => $tablerow['display_name'],
    );
    $form[$fid]['detail_title']= array(
      '#type' => 'radio',
      '#name' => 'detail_title',
      '#return_value' => $fid,
      '#attributes' => $tablerow['detail_title'] ? array('checked' => 'false') : '',
    );
    $form['detail_wide'][$fid] = array(
      '#type' => 'checkbox',
      '#default_value' => $tablerow['detail_wide'],
    );
    $form['weights'][$fid] = array(
      '#type' => 'weight',
      '#delta' => 30,
      '#default_value' => $tablerow['weight_detail'],
      '#attributes' => array('class' => 'weight'),
    );
    $form['weights'][$fid]['#attributes']['class'] = 'order-weight';
  }
  
  $form[-1] = '';
  $form[-1]['detail_title'] = array(
    '#type' => 'radio',
    '#name' => 'detail_title',
    '#return_value' => -1,
    '#description' => 'none',
    '#attributes' => !$checked_title ? array('checked' => 'false') : '',
  );

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  return $form;
}

function theme_datavizwiz_sort_detail_form($form) {
  _datavizwiz_add_form_warning_javascript();
  $header = array(t('Field Name'), t('Display Name'), t('Title'), t('Wide Display'), t('Weight'));

  $rows = array();

  foreach (element_children($form) as $key) {
    $row = array();

    if (is_numeric($key)) {
      $row[] = array('data' => drupal_render($form[$key]['field_name']));
      $row[] = array('data' => drupal_render($form[$key]['display_name']));
      $row[] = array('data' => drupal_render($form[$key]['detail_title']));
      $row[] = array('data' => drupal_render($form['detail_wide'][$key]));
      $row[] = array('data' => drupal_render($form['weights'][$key]));

      if ($key > 0) {
        $class = 'draggable';
      } else {
        $class = '';
      }
      
      $rows[] = array(
        'data' => $row,
        'class' => $class,
      );
    }
  }


  $output = theme('table', $header, $rows, array('id' => 'field-order'));
  $output .= drupal_render($form);

  drupal_add_tabledrag('field-order', 'order', 'sibling', 'order-weight', NULL, NULL, FALSE);

  return $output;
}

function datavizwiz_sort_detail_form_submit($form, &$form_state) {
  foreach ($form_state['values']['weights'] as $id => $item) {
    $wide_checked = false;
    foreach($form_state['values']['detail_wide'] as $fid => $wide) {
      if ($fid == $id && $wide) { $wide_checked = true; }
    }
     
    $record = new stdClass();
    $record->weight_detail = $item;
    $record->detail_title = $form_state['values']['detail_title'] == $id ? 1 : 0;
    $record->detail_wide = $wide_checked;
    $record->fid = $id;
    drupal_write_record('dvw_field_meta', $record, 'fid');
  }
}

// </editor-fold>

// <editor-fold desc="Manage Map Layers">

function datavizwiz_map_layers_detail_form() {
  $sql = sprintf("SELECT mlid, type, name, options, enabled FROM {dvw_map_layers} ORDER BY enabled DESC, name");
  $result = db_query($sql);

  $form['delete']['#tree'] = TRUE;
  
  $item_count=0;
  while ($tablerow = db_fetch_array($result)) {

    $mlid = $tablerow['mlid'];

    $form[$mlid]['type'] = array(
      '#type' => 'markup',
      '#value' => _datavizwiz_get_map_type_name($tablerow['type']),
    );
    $form[$mlid]['name'] = array(
      '#type' => 'markup',
      '#value' => $tablerow['name'],
    );
    $form['enabled'][$mlid] = array(
      '#type' => 'checkbox',
      '#default_value' => $tablerow['enabled'],
    );
    $form['delete'][$mlid] = array(
      '#type' => 'image_button',
      '#name' => 'delete-' . $svid,
      '#src' => drupal_get_path('module', 'datavizwiz') . '/images/delete.png',
      '#return_value' => 'delete-' . $svid,
    );
    $item_count++;
  }

  if ($item_count > 0) {
    $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));    
  }

  $form['spacer'] = array(
    '#type' => 'markup',
    '#value' => '<br/><br/><br/>',
  );
  
  $form['add_map'] = array(
  '#type' => 'fieldset', 
  '#title' => t('Add new map layer'),
  '#collapsible' => FALSE,
  );
  
  $form['add_map']['type'] = array(
  '#title' => t('Layer type'),
  '#type' => 'select',
  '#options' => array(
    DVW_MAP_BING => t('Bing'),
    DVW_MAP_GOOGLE => t('Google'),
    DVW_MAP_OPENSTREETMAPS=> t('Open Street Maps'),
    DVW_MAP_TMS => t('TMS'),
    DVW_MAP_WMS => t('WMS'),
    DVW_MAP_XYZ => t('XYZ'),
    DVW_MAP_YAHOO => t('Yahoo')
    ),
  );
  
  $form['add_panel']['add'] = array('#type' => 'submit', '#value' => t('Add Layer'));

  return $form;

}

function theme_datavizwiz_map_layers_detail_form($form) {
  _datavizwiz_add_form_warning_javascript();
  
  $header = array(t('Type'), t('Pane Name'), t('Enabled'), t('Delete'));

  $rows = array();

  $item_count = 0;
  foreach (element_children($form) as $key) {
    $row = array();
    if (is_numeric($key)) {
      $row[] = array('data' => drupal_render($form[$key]['type']));
      $row[] = array('data' => drupal_render($form[$key]['name']));
      $row[] = array('data' => drupal_render($form['enabled'][$key]));
      $row[] = array('data' => drupal_render($form['delete'][$key]));

      $rows[] = array(
        'data' => $row,
        'class' => 'draggable',
      );
      $item_count++;
    }
  }

  if ($item_count > 0) {
    $output = theme('table', $header, $rows, array('id' => 'field-order'));
    $output .= drupal_render($form);
  }
  
  
  $output .= drupal_render($form);

  return $output;
}

function _datavizwiz_get_map_type_name($type) {
  switch ($type) {
    case DVW_MAP_BING :
      return t('Bing');
    case DVW_MAP_GOOGLE :
      return t('Google');
    case DVW_MAP_OPENSTREETMAPS :
      return t('Open Street Maps');
    case DVW_MAP_TMS :
      return t('TMS');
    case DVW_MAP_XYZ :
      return t('XYZ');
    case DVW_MAP_YAHOO :
      return t('Yahoo');
    default :
      return t('Unknown');
  }
}

/** 
 * Get an array of active mapping layers
 *  
 */
function _datavizwiz_get_map_layers() {
  $columns = array();
  $results = db_query('SELECT mlid, name FROM {dvw_map_layers} WHERE enabled=1 ORDER BY name');
  
  while ($column = db_fetch_array($results)) {
    $columns[$column['mlid']] = $column['name'];
  }
  
  return $columns;
}

// </editor-fold>

function _datavizwiz_get_pane_type_name($pane_type) {
  switch ($pane_type) {
    case DVW_TYPE_MAP_OPENLAYERS:
      return t('Map');
      break;
    case DVW_TYPE_PIE_FLOT:
      return t('Pie Chart');
      break;
    case DVW_TYPE_BAR_FLOT:
      return t('Bar Graph');
      break;
    case DVW_TYPE_HTML:
      return t('Freeform HTML');
      break;
  }
}

function _datavizwiz_get_field_array($table_name, $none_option = FALSE) {
  $columns = array();
  
  $sql = sprintf('SHOW COLUMNS FROM %s', $table_name);
  $results = db_query($sql);
  
  if ($none_option) {
    $columns["(none)"] = "(none)";
  }
  
  while ($column = db_fetch_array($results)) {
    if ($column['Field'] != 'ImporterID') {
      $columns[$column['Field']] = $column['Field'];
    }
  }
  
  return $columns;
}

/**
 * Adds Javascript to prevent a user from navigating away from a panel without
 * first saving. 
 */
function _datavizwiz_add_form_warning_javascript() {
  drupal_add_js('$(document).ready(function() {

      $(":submit").click(function(){ window.onbeforeunload = null; }); // Turn off warning on submit
      $("input:text,input:checkbox,input:radio,textarea,select",document.forms[0]).bind("change", function() { setConfirmUnload(true); }); // Prevent accidental navigation away    
    });

    function setConfirmUnload(on) {
      window.onbeforeunload = (on) ? unloadMessage : null;
    }

    function unloadMessage() {
      return "You have entered new data on this page.  If you navigate away from this page without first saving your data, the changes will be lost.";
    }', 
   'inline'); 
}

function _datavizwiz_change_button_type($markup, $element) {
    $markup = str_replace('type="submit', 'type="button', $markup);
    return $markup;
}